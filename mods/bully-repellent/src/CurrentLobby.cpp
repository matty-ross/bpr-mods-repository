#include "CurrentLobby.h"

#include "imgui/imgui.h"

#include "core/Pointer.h"


void CurrentLobby::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Current Lobby"))
    {
        Core::Pointer guiCache = Core::Pointer(0x013FC8E0).deref().at(0x8E8430);

        ImGui::Text("Lobby Name: %s", guiCache.at(0xEA00).as<char[65]>());
        ImGui::Checkbox("Local Player Host", &guiCache.at(0xEA59).as<bool>());

        if (ImGui::BeginTable("##player-info-table", 3))
        {
            Core::Pointer playerInfo = guiCache.at(0xDE38);

            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Is Host");
            ImGui::TableSetupColumn("Actions");
            ImGui::TableHeadersRow();

            int32_t onlinePlayersCount = guiCache.at(0xDE2C).as<int32_t>();
            for (int32_t i = 0; i < onlinePlayersCount; ++i)
            {
                Core::Pointer playerStatus = playerInfo.at(i * 0x138);

                ImGui::PushID(playerStatus.GetAddress());
                
                ImGui::TableNextRow();
                
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(playerStatus.at(0xF0).as<char[25]>());

                ImGui::TableSetColumnIndex(1);
                ImGui::Checkbox("##is-host-checkbox", &playerStatus.at(0x12C).as<bool>());

                ImGui::TableSetColumnIndex(2);
                ImGui::Button("Autokick");
                ImGui::SameLine();
                ImGui::Button("Automute");

                ImGui::PopID();
            }

            ImGui::EndTable();
        }
    }
}