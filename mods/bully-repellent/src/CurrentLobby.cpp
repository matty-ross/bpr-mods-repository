#include "CurrentLobby.h"

#include "imgui/imgui.h"


CurrentLobby::CurrentLobby(Core::Logger& logger, const std::string& blacklistedPlayersFilePath)
    :
    m_Logger(logger),
    m_BlacklistedPlayersFilePath(blacklistedPlayersFilePath)
{
}

void CurrentLobby::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Current Lobby"))
    {
        Core::Pointer guiCache = Core::Pointer(0x013FC8E0).deref().at(0x8E8430);

        ImGui::Text("Lobby Name: %s", guiCache.at(0xEA00).as<char[65]>());
        ImGui::Checkbox("Local Player Host", &guiCache.at(0xEA59).as<bool>());

        ImGui::SeparatorText("Current Players");
        if (ImGui::BeginTable("##player-info-table", 3))
        {
            Core::Pointer playerInfo = guiCache.at(0xDE38);

            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Is Host");
            ImGui::TableSetupColumn("Blacklist");
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
                if (ImGui::Button("Add"))
                {
                    AddPlayerIntoBlacklistedPlayers(playerStatus);
                }

                ImGui::PopID();
            }

            ImGui::EndTable();
        }

        ImGui::SeparatorText("Blacklisted Players");
        // TODO: show blacklisted players
        if (ImGui::Button("Save"))
        {
            SaveBlacklistedPlayers();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load"))
        {
            LoadBlacklistedPlayers();
        }
    }
}

void CurrentLobby::LoadBlacklistedPlayers()
{
    // TODO: implement
}

void CurrentLobby::SaveBlacklistedPlayers()
{
    // TODO: implement
}

void CurrentLobby::AddPlayerIntoBlacklistedPlayers(Core::Pointer playerStatus)
{
    BlacklistedPlayer blacklistedPlayer =
    {
        .ID       = playerStatus.at(0x110).as<uint64_t>(),
        .Name     = playerStatus.at(0xF0).as<char[25]>(),
        .Autokick = false,
        .Automute = false,
    };
    m_BlacklistedPlayers.push_back(blacklistedPlayer);
}