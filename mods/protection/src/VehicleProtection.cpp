#include "VehicleProtection.hpp"

#include "vendor/imgui.hpp"


namespace BPR
{
    // void CgsIDConvertToString(CgsID lID, char* lpcString)
    static void CgsID_ConvertToString(uint64_t id, char* string)
    {
        __asm
        {
            mov ecx, dword ptr [string]
            push dword ptr [id + 0x4]
            push dword ptr [id + 0x0]

            mov eax, 0x0086CE00
            call eax
            add esp, 0x8
        }
    }
    
    // void __thiscall BrnNetwork::PlayerParamsBase::GetFreeBurnCarID(CgsID* lpCarId)
    static void PlayerParamsBase_GetFreeburnVehicleID(void* playerParams, uint64_t* vehicleID)
    {
        __asm
        {
            push dword ptr [vehicleID]
            mov ecx, dword ptr [playerParams]

            mov eax, 0x004FF0D0
            call eax
        }
    }

    // void __thiscall BrnNetwork::PlayerParamsBase::SetFreeBurnCarID(CgsID lCarId)
    static void PlayerParamsBase_SetFreeburnVehicleID(void* playerParams, uint64_t vehicleID)
    {
        __asm
        {
            push dword ptr [vehicleID + 0x4]
            push dword ptr [vehicleID + 0x0]
            mov ecx, dword ptr [playerParams]

            mov eax, 0x004FF090
            call eax
        }
    }
}


VehicleProtection::VehicleProtection(VehiclesFile& vehiclesFile)
    :
    m_VehiclesFile(vehiclesFile)
{
}

void VehicleProtection::OnPlayerParamsSerialize(Core::Pointer playerParams)
{
    // BrnNetwork::PlayerParams* playerParams
    
    if (!m_VehicleProtectionEnabled)
    {
        return;
    }

    uint64_t vehicleID = 0x0000000000000000;
 
    BPR::PlayerParamsBase_GetFreeburnVehicleID(playerParams.GetAddress(), &vehicleID);
    vehicleID = HandleVehicleID(vehicleID);
    BPR::PlayerParamsBase_SetFreeburnVehicleID(playerParams.GetAddress(), vehicleID);
}

void VehicleProtection::OnPlayerParamsDeserialize(Core::Pointer playerParams)
{
    // BrnNetwork::PlayerParams* playerParams
    
    if (!m_VehicleProtectionEnabled)
    {
        return;
    }

    uint64_t vehicleID = 0x0000000000000000;
    
    BPR::PlayerParamsBase_GetFreeburnVehicleID(playerParams.GetAddress(), &vehicleID);
    vehicleID = HandleVehicleID(vehicleID);
    BPR::PlayerParamsBase_SetFreeburnVehicleID(playerParams.GetAddress(), vehicleID);
}

void VehicleProtection::OnVehicleSelectMessagePack(Core::Pointer vehicleSelectMessage)
{
    // BrnNetwork::CarSelectMessage* vehicleSelectMessage
    
    if (!m_VehicleProtectionEnabled)
    {
        return;
    }

    uint64_t vehicleID = vehicleSelectMessage.at(0x38).as<uint64_t>();
    vehicleID = HandleVehicleID(vehicleID);
    vehicleSelectMessage.at(0x38).as<uint64_t>() = vehicleID;
}

void VehicleProtection::OnVehicleSelectMessageUnpack(Core::Pointer vehicleSelectMessage)
{
    // BrnNetwork::CarSelectMessage* vehicleSelectMessage
    
    if (!m_VehicleProtectionEnabled)
    {
        return;
    }
    
    uint64_t vehicleID = vehicleSelectMessage.at(0x38).as<uint64_t>();
    vehicleID = HandleVehicleID(vehicleID);
    vehicleSelectMessage.at(0x38).as<uint64_t>() = vehicleID;
}

void VehicleProtection::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Vehicle Protection"))
    {
        ImGui::Checkbox("Vehicle Protection Enabled", &m_VehicleProtectionEnabled);

        if (ImGui::Button("Save"))
        {
            m_VehiclesFile.Save();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load"))
        {
            m_VehiclesFile.Load();
        }

        static ImGuiTextFilter vehicleNameComboFilter;
        vehicleNameComboFilter.Draw("Vehicle Combo Filter");

        if (ImGui::BeginCombo("Fallback Vehicle", m_VehiclesFile.GetFallbackVehicle()->Name))
        {
            for (const VanillaVehicle& vanillaVehicle : k_VanillaVehicles)
            {
                if (vehicleNameComboFilter.PassFilter(vanillaVehicle.Name))
                {
                    bool selected = m_VehiclesFile.GetFallbackVehicle()->ID == vanillaVehicle.ID;
                    if (ImGui::Selectable(vanillaVehicle.Name, selected))
                    {
                        m_VehiclesFile.SetFallbackVehicle(&vanillaVehicle);
                    }
                    if (selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
            }
            
            ImGui::EndCombo();
        }

        if (ImGui::BeginTable("##vehicles-table", 2))
        {
            ImGui::TableSetupColumn("Vehicle", ImGuiTableColumnFlags_WidthStretch, 0.4f);
            ImGui::TableSetupColumn("Replacement Vehicle", ImGuiTableColumnFlags_WidthStretch, 0.6f);
            ImGui::TableHeadersRow();
            
            for (Vehicle& vehicle : m_VehiclesFile.GetVehicles())
            {
                ImGui::PushID(&vehicle);
                ImGui::TableNextRow();
                {
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(vehicle.Name.c_str());
                
                    ImGui::TableNextColumn();
                    ImGui::SetNextItemWidth(-35.0f);
                    if (ImGui::BeginCombo("##replacement-vehicle-combo", vehicle.Replacement->Name))
                    {
                        for (const VanillaVehicle& vanillaVehicle : k_VanillaVehicles)
                        {
                            if (vehicleNameComboFilter.PassFilter(vanillaVehicle.Name))
                            {
                                bool selected = vehicle.Replacement->ID == vanillaVehicle.ID;
                                if (ImGui::Selectable(vanillaVehicle.Name, selected))
                                {
                                    vehicle.Replacement = &vanillaVehicle;
                                }
                                if (selected)
                                {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                        }
                        
                        ImGui::EndCombo();
                    }
                }
                ImGui::PopID();
            }
            
            ImGui::EndTable();
        }
    }
}

void VehicleProtection::AddNonVanillaVehiclesToVehiclesFile()
{
    Core::Pointer vehicleList = Core::Pointer(0x013FC8E0).deref().at(0x68C350);

    int32_t vehiclesCount = vehicleList.at(0x3400).as<int32_t>();
    for (int32_t i = 0; i < vehiclesCount; ++i)
    {
        Core::Pointer vehicleSlot = vehicleList.at(0x400 + i * 0xC);
        int32_t listIndex = vehicleSlot.at(0x4).as<int32_t>();
        int32_t entryIndex = vehicleSlot.at(0x8).as<int32_t>();

        Core::Pointer list = vehicleList.at(0x0 + listIndex * 0x20).as<void*>();
        Core::Pointer entry = list.at(0x4).deref().at(entryIndex * 0x108);

        uint64_t vehicleID = entry.at(0x0).as<uint64_t>();
        bool isVanilla = GetVanillaVehicle(vehicleID) != nullptr;
        bool isInFile = m_VehiclesFile.GetVehicle(vehicleID) != nullptr;
        if (!isVanilla && !isInFile)
        {
            char stringVehicleID[13] = {};
            BPR::CgsID_ConvertToString(vehicleID, stringVehicleID);

            m_VehiclesFile.GetVehicles().push_back(
                Vehicle
                {
                    .ID          = vehicleID,
                    .Name        = stringVehicleID,
                    .Replacement = m_VehiclesFile.GetFallbackVehicle(),
                }
            );
        }
    }
}

uint64_t VehicleProtection::HandleVehicleID(uint64_t vehicleID) const
{
    bool isVanilla = GetVanillaVehicle(vehicleID) != nullptr;
    if (isVanilla)
    {
        return vehicleID;
    }

    Vehicle* vehicle = m_VehiclesFile.GetVehicle(vehicleID);
    if (vehicle != nullptr)
    {
        return vehicle->Replacement->ID;
    }

    return m_VehiclesFile.GetFallbackVehicle()->ID;
}
