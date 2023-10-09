#include "CurrentLobby.h"

#include "imgui/imgui.h"


namespace BPR
{
    enum class PlayerOption
    {
        Kick = 2,
        Mute = 3,
    };
    
    struct SelectedPlayerOptionEvent
    {
        PlayerOption PlayerOption;
        uint64_t NetworkPlayerID;
    };
    
    void AddSelectedPlayerOptionEvent(const SelectedPlayerOptionEvent* selectedPlayerOptionEvent)
    {
        __asm
        {
            push dword ptr [selectedPlayerOptionEvent]
            mov ecx, dword ptr ds:[0x013FC8E0]
            add ecx, 0x7FABDC
            
            mov eax, 0x004FB4C0
            call eax
        }
    }
}


CurrentLobby::CurrentLobby(BlacklistedPlayersFile& blacklistedPlayersFile)
    :
    m_BlacklistedPlayersFile(blacklistedPlayersFile)
{
}

void CurrentLobby::OnUpdate(Core::Pointer guiEventNetworkPlayerStatus)
{
    if (!m_BlacklistedPlayersFile.IsBlacklistEnabled())
    {
        return;
    }

    int32_t currentGameMode = Core::Pointer(0x013FC8E0).deref().at(0x8F5368).as<int32_t>();
    if (!(currentGameMode == 15 || currentGameMode == 16))
    {
        return;
    }
    
    int32_t playersCount = guiEventNetworkPlayerStatus.at(0x9C0).as<int32_t>();
    for (int32_t i = 0; i < playersCount; ++i)
    {
        Core::Pointer playerStatusData = guiEventNetworkPlayerStatus.at(0x0 + i * 0x138);
        uint64_t playerID = playerStatusData.at(0x110).as<uint64_t>();

        bool isLocalPlayer = playerStatusData.at(0x12D).as<bool>();
        if (isLocalPlayer)
        {
            continue;
        }

        auto it = m_BlacklistedPlayersFile.GetBlacklistedPlayers().find(playerID);
        if (it == m_BlacklistedPlayersFile.GetBlacklistedPlayers().end())
        {
            continue;
        }
        
        const BlacklistedPlayer& blacklistedPlayer = it->second;
        
        if (blacklistedPlayer.Autokick)
        {
            bool isLocalPlayerHost = guiEventNetworkPlayerStatus.at(0xA05).as<bool>();
            if (isLocalPlayerHost)
            {
                BPR::SelectedPlayerOptionEvent selectedPlayerOptionEvent =
                {
                    .PlayerOption    = BPR::PlayerOption::Kick,
                    .NetworkPlayerID = playerID,
                };
                BPR::AddSelectedPlayerOptionEvent(&selectedPlayerOptionEvent);
            }
        }
        if (blacklistedPlayer.Automute)
        {
            bool isMuted = playerStatusData.at(0x130).as<bool>();
            if (!isMuted)
            {
                BPR::SelectedPlayerOptionEvent selectedPlayerOptionEvent =
                {
                    .PlayerOption    = BPR::PlayerOption::Mute,
                    .NetworkPlayerID = playerID,
                };
                BPR::AddSelectedPlayerOptionEvent(&selectedPlayerOptionEvent);
            }
        }
    }
}

void CurrentLobby::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Current Lobby"))
    {
        Core::Pointer guiCache = Core::Pointer(0x013FC8E0).deref().at(0x8E8430);

        bool isOnline = guiCache.at(0x7B00).as<bool>();
        if (isOnline)
        {
            auto getGameModeName = [](int32_t gameMode) -> const char*
            {
                switch (gameMode)
                {
                case 10: return "Race";
                case 12: return "Stunt Run";
                case 14: return "Stunt Run Free-for-all";
                case 15: return "Freeburn";
                case 16: return "Showtime";
                case 17: return "Stunt Run Co-op";
                }
                return "???";
            };
                    
            ImGui::Text("Lobby Name     %s", guiCache.at(0xEA00).as<char[65]>());
            ImGui::Text("You are Host   %s", guiCache.at(0xEA59).as<bool>() ? "Yes" : "No");
            ImGui::Text("Game Mode      %s", getGameModeName(guiCache.at(0xCF38).as<int32_t>()));

            if (ImGui::BeginTable("##player-info-table", 3))
            {
                Core::Pointer playerInfo = guiCache.at(0xDE38);

                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Is Host");
                ImGui::TableSetupColumn("Blacklist");
                ImGui::TableHeadersRow();

                int32_t playersCount = guiCache.at(0xDE2C).as<int32_t>();
                for (int32_t i = 0; i < playersCount; ++i)
                {
                    Core::Pointer playerStatusData = playerInfo.at(i * 0x138);
                    uint64_t playerID = playerStatusData.at(0x110).as<uint64_t>();

                    ImGui::PushID(playerStatusData.GetAddress());
                
                    ImGui::TableNextRow();
                
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(playerStatusData.at(0xF0).as<char[25]>());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(playerStatusData.at(0x12C).as<bool>() ? "Yes" : "No");

                    ImGui::TableSetColumnIndex(2);
                    bool isLocalPlayer = playerStatusData.at(0x12D).as<bool>();
                    bool alreadyOnBlacklist = m_BlacklistedPlayersFile.GetBlacklistedPlayers().contains(playerID);
                    if (isLocalPlayer || alreadyOnBlacklist)
                    {
                        ImGui::BeginDisabled();
                    }
                    if (ImGui::Button("Add"))
                    {
                        m_BlacklistedPlayersFile.AddBlacklistedPlayer(
                            playerID,
                            {
                                .Name     = playerStatusData.at(0xF0).as<char[25]>(),
                                .Autokick = false,
                                .Automute = false,
                            }
                        );
                    }
                    if (isLocalPlayer || alreadyOnBlacklist)
                    {
                        ImGui::EndDisabled();
                    }

                    ImGui::PopID();
                }

                ImGui::EndTable();
            }
        }
    }
}