#include "CurrentLobby.h"

#include "imgui/imgui.h"

#include "core/Pointer.h"


void CurrentLobby::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Current Lobby"))
    {
        Core::Pointer guiCache = Core::Pointer(0x013FC8E0).deref().at(0x8E8430);

        ImGui::Text("Lobby Name: %s", guiCache.at(0xEA00).as<char[64]>());
        ImGui::Checkbox("Local Player Host", &guiCache.at(0xEA59).as<bool>());

        ImGui::SeparatorText("Player Info");
        {
            Core::Pointer playerInfo = guiCache.at(0xDE38);

            int32_t onlinePlayersCount = guiCache.at(0xDE2C).as<int32_t>();
            for (int32_t i = 0; i < onlinePlayersCount; ++i)
            {
                Core::Pointer playerStatus = playerInfo.at(i * 0x138);

                ImGui::PushID(playerStatus.GetAddress());
                
                ImGui::Button("Autokick");
                ImGui::Button("Automute");

                ImGui::Text("Player Name %s", playerStatus.at(0xF0).as<char[25]>());
                ImGui::Text("Player ID %016llX", playerStatus.at(0x110).as<uint64_t>());

                ImGui::PopID();
            }
        }
    }
}