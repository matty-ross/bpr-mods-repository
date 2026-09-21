#include <cstdint>

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"
#include "core/Logger.hpp"
#include "core/Patch.hpp"
#include "bpr/CgsID.hpp"

#include "Protection.hpp"
#include "Vehicles.hpp"
#include "VehiclesFile.hpp"
#include "VehicleProtection.hpp"


namespace BPR
{
    /*
        void __thiscall BrnNetwork::PlayerParamsBase::GetFreeBurnCarID(
            CgsID* lpCarId
        )
    */
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

    /*
        void __thiscall BrnNetwork::PlayerParamsBase::SetFreeBurnCarID(
            CgsID lCarId
        )
    */
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


VehicleProtection::VehicleProtection(VehiclesFile& vehiclesFile, const Core::Logger& logger)
    :
    m_VehiclesFile(vehiclesFile),
    m_Logger(logger)
{
}

void VehicleProtection::Load()
{
    Core::Patch(0x00B7218A, 6, m_Logger).WriteJMP(Hook_CheckPlayerParamsBeforeSerializing);
    Core::Patch(0x00B72958, 5, m_Logger).WriteJMP(Hook_CheckPlayerParamsAfterDeserializing);
    Core::Patch(0x00B62095, 5, m_Logger).WriteJMP(Hook_CheckVehicleSelectMessageBeforePacking);
    Core::Patch(0x00B6209F, 5, m_Logger).WriteJMP(Hook_CheckVehicleSelectMessageAfterUnpacking);
}

void VehicleProtection::RenderMenu()
{
    if (ImGui::CollapsingHeader("Vehicle Protection"))
    {
        static constexpr char vanillaVehiclesPopupID[] = "vanilla-vehicles-popup";

        auto renderVanillaVehiclesPopup = []<typename Fn>(const char* title, uint64_t selectedVehicleID, Fn onSelected) -> void
        {
            ImGui::SetNextWindowSize(ImVec2(0.0f, 500.0f));

            if (ImGui::BeginPopup(vanillaVehiclesPopupID))
            {
                ImGui::SeparatorText(title);

                static ImGuiTextFilter vanillaVehicleFilter;
                vanillaVehicleFilter.Draw("Filter##vanilla-vehicle");

                if (ImGui::BeginListBox("##vanilla-vehicles", ImVec2(-FLT_MIN, -FLT_MIN)))
                {
                    for (const VanillaVehicle& vanillaVehicle : k_VanillaVehicles)
                    {
                        if (vanillaVehicleFilter.PassFilter(vanillaVehicle.Name))
                        {
                            ImGui::PushID(&vanillaVehicle);

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

                            ImGui::PopID();
                        }
                    }

                    ImGui::EndListBox();
                }

                ImGui::EndPopup();
            }
        };

        {
            ImGui::Checkbox("Vehicle Protection Enabled", &m_VehicleProtectionEnabled);

            if (ImGui::Button("Save##vehicles-file"))
            {
                m_VehiclesFile.Save();
            }

            ImGui::SameLine();

            if (ImGui::Button("Load##vehicles-file"))
            {
                m_VehiclesFile.Load();
            }
        }

        ImGui::Separator();

        {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Fallback Vehicle   %s", m_VehiclesFile.GetFallbackVehicle()->Name);

            ImGui::SameLine(0.0f, 20.0f);

            if (ImGui::Button("Change##fallback-vehicle"))
            {
                ImGui::OpenPopup(vanillaVehiclesPopupID);
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

        ImGui::Separator();

        {
            static ImGuiTextFilter vehicleFilter;
            vehicleFilter.Draw("Filter##vehicle");

            if (ImGui::BeginTable("##vehicles", 3, ImGuiTableFlags_ScrollY, ImVec2(0.0f, 400.0f)))
            {
                ImGui::TableSetupScrollFreeze(0, 1);
                ImGui::TableSetupColumn("Vehicle", ImGuiTableColumnFlags_WidthStretch, 0.3f);
                ImGui::TableSetupColumn("Replacement Vehicle", ImGuiTableColumnFlags_WidthStretch, 0.5f);
                ImGui::TableSetupColumn("##change-vehicle", ImGuiTableColumnFlags_WidthStretch, 0.2f);
                ImGui::TableHeadersRow();

                for (VehiclesFile::Vehicle& vehicle : m_VehiclesFile.GetVehicles())
                {
                    if (vehicleFilter.PassFilter(vehicle.Name.c_str()))
                    {
                        ImGui::PushID(&vehicle);

                        ImGui::TableNextRow();
                        {
                            ImGui::TableNextColumn();

                            ImGui::AlignTextToFramePadding();
                            ImGui::TextUnformatted(vehicle.Name.c_str());
                        }
                        {
                            ImGui::TableNextColumn();

                            ImGui::AlignTextToFramePadding();
                            ImGui::TextUnformatted(vehicle.Replacement->Name);
                        }
                        {
                            ImGui::TableNextColumn();

                            if (ImGui::Button("Change##replacement-vehicle"))
                            {
                                ImGui::OpenPopup(vanillaVehiclesPopupID);
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
    Core::Pointer vehicleListResource = Core::Pointer(0x013FC8E0).deref().at(0x68C350).as<void*>(); // BrnResource::VehicleListResource*

    uint32_t vehiclesCount = vehicleListResource.at(0x0).as<uint32_t>();
    for (uint32_t i = 0; i < vehiclesCount; ++i)
    {
        Core::Pointer vehicleListEntry = vehicleListResource.at(0x4).deref().at(i * 0x108); // BrnResource::VehicleListEntry*

        uint64_t vehicleID = vehicleListEntry.at(0x0).as<uint64_t>();
        bool isVanilla = FindVanillaVehicleByID(vehicleID) != nullptr;
        bool isInFile = m_VehiclesFile.FindVehicleByID(vehicleID) != nullptr;

        if (!isVanilla && !isInFile)
        {
            char stringVehicleID[13] = {};
            BPR::CgsID_Uncompress(vehicleID, stringVehicleID);

            m_VehiclesFile.AddVehicle(
                VehiclesFile::Vehicle
                {
                    .ID = vehicleID,
                    .Name = stringVehicleID,
                    .Replacement = m_VehiclesFile.GetFallbackVehicle(),
                }
            );
        }
    }
}

uint64_t VehicleProtection::HandleVehicleID(uint64_t vehicleID) const
{
    bool isVanilla = FindVanillaVehicleByID(vehicleID) != nullptr;
    if (isVanilla)
    {
        return vehicleID;
    }

    VehiclesFile::Vehicle* vehicle = m_VehiclesFile.FindVehicleByID(vehicleID);
    if (vehicle != nullptr)
    {
        return vehicle->Replacement->ID;
    }

    return m_VehiclesFile.GetFallbackVehicle()->ID;
}

void VehicleProtection::CheckPlayerParamsBeforeSerializing(
    Core::Pointer playerParams // BrnNetwork::PlayerParams*
)
{
    if (!m_VehicleProtectionEnabled)
    {
        return;
    }

    uint64_t vehicleID = 0;
    BPR::PlayerParamsBase_GetFreeburnVehicleID(playerParams.GetPointer(), &vehicleID);
    vehicleID = HandleVehicleID(vehicleID);
    BPR::PlayerParamsBase_SetFreeburnVehicleID(playerParams.GetPointer(), vehicleID);
}

void VehicleProtection::CheckPlayerParamsAfterDeserializing(
    Core::Pointer playerParams // BrnNetwork::PlayerParams*
)
{
    if (!m_VehicleProtectionEnabled)
    {
        return;
    }

    uint64_t vehicleID = 0;
    BPR::PlayerParamsBase_GetFreeburnVehicleID(playerParams.GetPointer(), &vehicleID);
    vehicleID = HandleVehicleID(vehicleID);
    BPR::PlayerParamsBase_SetFreeburnVehicleID(playerParams.GetPointer(), vehicleID);
}

void VehicleProtection::CheckVehicleSelectMessageBeforePacking(
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

void VehicleProtection::CheckVehicleSelectMessageAfterUnpacking(
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

__declspec(naked) void VehicleProtection::Hook_CheckPlayerParamsBeforeSerializing()
{
    __asm
    {
        // edi: BrnNetwork::PlayerParams* this

        pushfd
        pushad

        push edi
        mov ecx, offset Protection::s_Instance.m_VehicleProtection
        call VehicleProtection::CheckPlayerParamsBeforeSerializing

        popad
        popfd

        // Original code.
        lea ecx, [edi + 0x228]

        // Jump back.
        push 0x00B72190
        ret
    }
}

__declspec(naked) void VehicleProtection::Hook_CheckPlayerParamsAfterDeserializing()
{
    __asm
    {
        // edi: BrnNetwork::PlayerParams* this

        pushfd
        pushad

        push edi
        mov ecx, offset Protection::s_Instance.m_VehicleProtection
        call VehicleProtection::CheckPlayerParamsAfterDeserializing

        popad
        popfd

        // Original code.
        push 0x00ECF090

        // Jump back.
        push 0x00B7295D
        ret
    }
}

__declspec(naked) void VehicleProtection::Hook_CheckVehicleSelectMessageBeforePacking()
{
    /*
        BrnNetwork::BrnNetworkManager::PackOrUnpackResult __thiscall BrnNetwork::CarSelectMessage::PackOrUnpack()
    */

    __asm
    {
        // esi: BrnNetwork::CarSelectMessage* this

        pushfd
        pushad

        cmp dword ptr [esi + 0x4], 0 // CgsNetwork::Message::EPackOrUnpack::E_PACK_INTO_BITSTREAM
        jne _end

        push esi
        mov ecx, offset Protection::s_Instance.m_VehicleProtection
        call VehicleProtection::CheckVehicleSelectMessageBeforePacking

    _end:
        popad
        popfd

        // Original code.
        mov ecx, esi
        push edx
        mov bh, al

        // Jump back.
        push 0x00B6209A
        ret
    }
}

__declspec(naked) void VehicleProtection::Hook_CheckVehicleSelectMessageAfterUnpacking()
{
    /*
        BrnNetwork::BrnNetworkManager::PackOrUnpackResult __thiscall BrnNetwork::CarSelectMessage::PackOrUnpack()
    */

    __asm
    {
        // esi: BrnNetwork::CarSelectMessage* this

        pushfd
        pushad

        cmp dword ptr [esi + 0x4], 1 // CgsNetwork::Message::EPackOrUnpack::E_UNPACK_FROM_BITSTREAM
        jne _end

        push esi
        mov ecx, offset Protection::s_Instance.m_VehicleProtection
        call VehicleProtection::CheckVehicleSelectMessageAfterUnpacking

    _end:
        popad
        popfd

        // Original code.
        lea ecx, [esi + 0x40]
        or bh, al

        // Jump back.
        push 0x00B620A4
        ret
    }
}
