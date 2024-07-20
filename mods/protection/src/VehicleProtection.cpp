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

void VehicleProtection::OnPlayerParamsSerialize(
    Core::Pointer playerParams // BrnNetwork::PlayerParams*
)
{
    if (!m_VehicleProtectionEnabled)
    {
        return;
    }

    uint64_t vehicleID = 0;
    BPR::PlayerParamsBase_GetFreeburnVehicleID(playerParams.GetAddress(), &vehicleID);
    vehicleID = HandleVehicleID(vehicleID);
    BPR::PlayerParamsBase_SetFreeburnVehicleID(playerParams.GetAddress(), vehicleID);
}

void VehicleProtection::OnPlayerParamsDeserialize(
    Core::Pointer playerParams // BrnNetwork::PlayerParams*
)
{
    if (!m_VehicleProtectionEnabled)
    {
        return;
    }

    uint64_t vehicleID = 0;
    BPR::PlayerParamsBase_GetFreeburnVehicleID(playerParams.GetAddress(), &vehicleID);
    vehicleID = HandleVehicleID(vehicleID);
    BPR::PlayerParamsBase_SetFreeburnVehicleID(playerParams.GetAddress(), vehicleID);
}

void VehicleProtection::OnVehicleSelectMessagePack(
    Core::Pointer vehicleSelectMessage // BrnNetwork::CarSelectMessage*
)
{
    if (!m_VehicleProtectionEnabled)
    {
        return;
    }

    uint64_t vehicleID = vehicleSelectMessage.at(0x38).as<uint64_t>();
    vehicleID = HandleVehicleID(vehicleID);
    vehicleSelectMessage.at(0x38).as<uint64_t>() = vehicleID;
}

void VehicleProtection::OnVehicleSelectMessageUnpack(
    Core::Pointer vehicleSelectMessage // BrnNetwork::CarSelectMessage*
)
{
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
        auto renderVanillaVehiclesPopup = []<typename Fn>(const char* title, uint64_t selectedVehicleID, Fn onSelected) -> void
        {
            ImGui::SetNextWindowSize(ImVec2(0.0f, 500.0f));
            if (ImGui::BeginPopup("vanilla-vehicles-popup"))
            {
                ImGui::SeparatorText(title);

                static ImGuiTextFilter vanillaVehicleFilter;
                vanillaVehicleFilter.Draw("Filter##vanilla-vehicle-filter");

                if (ImGui::BeginListBox("##vanilla-vehicles-list", ImVec2(-FLT_MIN, -FLT_MIN)))
                {
                    for (const VanillaVehicle& vanillaVehicle : k_VanillaVehicles)
                    {
                        if (vanillaVehicleFilter.PassFilter(vanillaVehicle.Name))
                        {
                            bool selected = vanillaVehicle.ID == selectedVehicleID;
                            if (ImGui::Selectable(vanillaVehicle.Name, selected))
                            {
                                onSelected(vanillaVehicle);
                                vanillaVehicleFilter.Clear();
                                ImGui::CloseCurrentPopup();
                            }
                            if (selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                    }

                    ImGui::EndListBox();
                }

                ImGui::EndPopup();
            }
        };

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
        }

        {
            ImGui::SeparatorText("Fallback Vehicle");

            ImGui::TextUnformatted(m_VehiclesFile.GetFallbackVehicle()->Name);
            ImGui::SameLine(0.0f, 20.0f);
            if (ImGui::Button("Change...##fallback-vehicle-button", ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing())))
            {
                ImGui::OpenPopup("vanilla-vehicles-popup");
            }
            renderVanillaVehiclesPopup(
                "Fallback Vehicle",
                m_VehiclesFile.GetFallbackVehicle()->ID,
                [&](const VanillaVehicle& vanillaVehicle) -> void
                {
                    m_VehiclesFile.SetFallbackVehicle(&vanillaVehicle);
                }
            );
        }

        {
            ImGui::SeparatorText("Vehicles");

            static ImGuiTextFilter vehicleFilter;
            vehicleFilter.Draw("Filter##vehicle-filter");
            
            if (ImGui::BeginTable("##vehicles-table", 3, ImGuiTableFlags_ScrollY, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 20.0f)))
            {
                ImGui::TableSetupColumn("Vehicle", ImGuiTableColumnFlags_WidthStretch, 0.3f);
                ImGui::TableSetupColumn("Replacement Vehicle", ImGuiTableColumnFlags_WidthStretch, 0.5f);
                ImGui::TableSetupColumn("##change-vehicle-column", ImGuiTableColumnFlags_WidthStretch, 0.2f);
                ImGui::TableHeadersRow();

                for (Vehicle& vehicle : m_VehiclesFile.GetVehicles())
                {
                    if (vehicleFilter.PassFilter(vehicle.Name.c_str()))
                    {
                        ImGui::PushID(&vehicle);
                        ImGui::TableNextRow();
                        {
                            ImGui::TableNextColumn();
                            ImGui::TextUnformatted(vehicle.Name.c_str());

                            ImGui::TableNextColumn();
                            ImGui::TextUnformatted(vehicle.Replacement->Name);

                            ImGui::TableNextColumn();
                            if (ImGui::Button("Change...##replacement-vehicle-button", ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing())))
                            {
                                ImGui::OpenPopup("vanilla-vehicles-popup");
                            }
                            renderVanillaVehiclesPopup(
                                "Replacement Vehicle",
                                vehicle.Replacement->ID,
                                [&](const VanillaVehicle& vanillaVehicle) -> void
                                {
                                    vehicle.Replacement = &vanillaVehicle;
                                }
                            );
                        }
                        ImGui::PopID();
                    }
                }

                ImGui::EndTable();
            }
        }
    }
}

void VehicleProtection::AddNonVanillaVehiclesToVehiclesFile()
{
    Core::Pointer vehicleList = Core::Pointer(0x013FC8E0).deref().at(0x68C350); // BrnResource::VehicleList*

    int32_t vehiclesCount = vehicleList.at(0x3400).as<int32_t>();
    for (int32_t i = 0; i < vehiclesCount; ++i)
    {
        Core::Pointer vehicleSlot = vehicleList.at(0x400 + i * 0xC); // BrnResource::VehicleSlot*
        int32_t listIndex = vehicleSlot.at(0x4).as<int32_t>();
        int32_t entryIndex = vehicleSlot.at(0x8).as<int32_t>();

        Core::Pointer list = vehicleList.at(0x0 + listIndex * 0x20).as<void*>(); // BrnResource::VehicleListResource*
        Core::Pointer entry = list.at(0x4).deref().at(entryIndex * 0x108); // BrnResource::VehicleListEntry*

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
