#include "VehicleProtection.h"

#include "imgui/imgui.h"

#include "core/Pointer.h"

#include "bpr/CgsID.h"


namespace BPR
{
    void GetFreeburnVehicleID(void* playerParams, uint64_t* vehicleID)
    {
        __asm
        {
            push dword ptr [vehicleID]
            mov ecx, dword ptr [playerParams]

            mov eax, 0x004FF0D0
            call eax
        }
    }

    void SetFreeburnVehicleID(void* playerParams, uint64_t vehicleID)
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
        if (ImGui::BeginTable("##vehicles-table", 2))
        {
            ImGui::TableSetupColumn("New Vehicle");
            ImGui::TableSetupColumn("Replacement Vehicle");
            ImGui::TableHeadersRow();

            for (uint64_t vehicleID : m_VehiclesFile.GetVehicleIDs())
            {
                Vehicle& vehicle = m_VehiclesFile.GetVehicles().at(vehicleID);

                ImGui::PushID(&vehicle);

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(vehicle.NewID.Uncompressed);

                ImGui::TableSetColumnIndex(1);
                if (ImGui::BeginCombo("##replacement-vehicle-combo", vehicle.ReplacementID->Uncompressed))
                {
                    for (const VehicleID& vanillaVehicleID : k_VanillaVehicleIDs)
                    {
                        bool selected = vehicle.ReplacementID->Compressed == vanillaVehicleID.Compressed;
                        if (ImGui::Selectable(vanillaVehicleID.Uncompressed, selected))
                        {
                            vehicle.ReplacementID = &vanillaVehicleID;
                        }
                        if (selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                ImGui::PopID();
            }

            ImGui::EndTable();
        }
    }
}

void VehicleProtection::OnPlayerParamsSerialize(void* playerParams)
{
    uint64_t vehicleID = 0x0000000000000000;
 
    BPR::GetFreeburnVehicleID(playerParams, &vehicleID);
    vehicleID = HandleVehicleID(vehicleID);
    BPR::SetFreeburnVehicleID(playerParams, vehicleID);
}

void VehicleProtection::OnPlayerParamsDeserialize(void* playerParams)
{
    // TODO: skip self

    uint64_t vehicleID = 0x0000000000000000;
    
    BPR::GetFreeburnVehicleID(playerParams, &vehicleID);
    vehicleID = HandleVehicleID(vehicleID);
    BPR::SetFreeburnVehicleID(playerParams, vehicleID);
}

void VehicleProtection::AddNonVanillaVehicleIDsToVehiclesFile()
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
        bool isVanilla = GetVanillaVehicleID(vehicleID) != nullptr;
        bool isInFile = m_VehiclesFile.GetVehicles().contains(vehicleID);
        if (!isVanilla && !isInFile)
        {
            VehicleID nonVanillaVehicleID = {};
            nonVanillaVehicleID.Compressed = vehicleID;
            BPR::CgsID_ConvertToString(vehicleID, nonVanillaVehicleID.Uncompressed);
            
            m_VehiclesFile.AddVehicle(
                {
                    .NewID = nonVanillaVehicleID,
                    .ReplacementID = &k_FallbackVehicleID,
                }
            );
        }
    }
}

uint64_t VehicleProtection::HandleVehicleID(uint64_t vehicleID)
{
    bool isVanilla = GetVanillaVehicleID(vehicleID) != nullptr;
    if (isVanilla)
    {
        return vehicleID;
    }

    auto it = m_VehiclesFile.GetVehicles().find(vehicleID);
    if (it == m_VehiclesFile.GetVehicles().end())
    {
        return k_FallbackVehicleID.Compressed;
    }

    return it->second.ReplacementID->Compressed;
}