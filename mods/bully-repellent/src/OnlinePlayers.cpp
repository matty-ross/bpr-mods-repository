#include "OnlinePlayers.hpp"

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"


namespace BPR
{
    // BrnGui::GuiEventNetworkSelectedPlayerOption
    struct SelectedPlayerOptionEvent
    {
        int32_t Option;
        uint64_t PlayerID;
    };
    
    static void AddSelectedPlayerOptionEvent(const SelectedPlayerOptionEvent* selectedPlayerOptionEvent)
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


OnlinePlayers::OnlinePlayers(BlacklistedPlayersFile& blacklistedPlayersFile)
    :
    m_BlacklistedPlayersFile(blacklistedPlayersFile)
{
}

void OnlinePlayers::OnGuiEventNetworkPlayerStatus(void* event)
{
    // BrnGui::GuiEventNetworkPlayerStatus*
    Core::Pointer guiEventNetworkPlayerStatus = event;

    if (!m_BlacklistEnabled)
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

        const BlacklistedPlayer* blacklistedPlayer = m_BlacklistedPlayersFile.GetBlacklistedPlayer(playerID);
        if (blacklistedPlayer == nullptr)
        {
            continue;
        }
        
        if (blacklistedPlayer->Autokick)
        {
            bool isLocalPlayerHost = guiEventNetworkPlayerStatus.at(0xA05).as<bool>();
            if (isLocalPlayerHost)
            {
                BPR::SelectedPlayerOptionEvent selectedPlayerOptionEvent =
                {
                    .Option   = 2,
                    .PlayerID = playerID,
                };
                BPR::AddSelectedPlayerOptionEvent(&selectedPlayerOptionEvent);
            }
        }
        if (blacklistedPlayer->Automute)
        {
            bool isMuted = playerStatusData.at(0x130).as<bool>();
            if (!isMuted)
            {
                BPR::SelectedPlayerOptionEvent selectedPlayerOptionEvent =
                {
                    .Option   = 3,
                    .PlayerID = playerID,
                };
                BPR::AddSelectedPlayerOptionEvent(&selectedPlayerOptionEvent);
            }
        }
    }
}

void OnlinePlayers::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Current Players"))
    {
        ImGui::Checkbox("Blacklist Enabled", &m_BlacklistEnabled);
        
        Core::Pointer guiCache = Core::Pointer(0x013FC8E0).deref().at(0x8E8430);

        bool isOnline = guiCache.at(0x7B00).as<bool>();
        if (isOnline)
        {        
            ImGui::Text("Host Name      %s", guiCache.at(0xEA00).as<char[65]>());
            ImGui::Text("You are Host   %s", guiCache.at(0xEA59).as<bool>() ? "Yes" : "No");

            if (ImGui::BeginTable("##player-info-table", 2))
            {
                Core::Pointer playerInfo = guiCache.at(0xDE38);

                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Blacklist");
                ImGui::TableHeadersRow();
                
                int32_t playersCount = guiCache.at(0xDE2C).as<int32_t>();
                for (int32_t i = 0; i < playersCount; ++i)
                {
                    Core::Pointer playerStatusData = playerInfo.at(i * 0x138);
                    uint64_t playerID = playerStatusData.at(0x110).as<uint64_t>();

                    ImGui::PushID(playerStatusData.GetAddress());
                    ImGui::TableNextRow();
                    {
                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted(playerStatusData.at(0xF0).as<char[25]>());
                    
                        ImGui::TableNextColumn();
                        bool isLocalPlayer = playerStatusData.at(0x12D).as<bool>();
                        bool alreadyOnBlacklist = m_BlacklistedPlayersFile.GetBlacklistedPlayer(playerID) != nullptr;
                        if (isLocalPlayer || alreadyOnBlacklist)
                        {
                            ImGui::BeginDisabled();
                        }
                        if (ImGui::Button("Add"))
                        {
                            m_BlacklistedPlayersFile.GetBlacklistedPlayers().push_back(
                                BlacklistedPlayer
                                {
                                    .ID       = playerID,
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
                    }
                    ImGui::PopID();
                }

                ImGui::EndTable();
            }
        }
        else
        {
            ImGui::TextUnformatted("Currently Offline");
        }
    }

    if (ImGui::CollapsingHeader("Blacklisted Players"))
    {
        if (ImGui::Button("Save"))
        {
            m_BlacklistedPlayersFile.Save();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load"))
        {
            m_BlacklistedPlayersFile.Load();
        }
        
        if (ImGui::BeginTable("##blacklisted-players-table", 3))
        {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Autokick");
            ImGui::TableSetupColumn("Automute");
            ImGui::TableHeadersRow();
            
            for (BlacklistedPlayer& blacklistedPlayer : m_BlacklistedPlayersFile.GetBlacklistedPlayers())
            {
                ImGui::PushID(&blacklistedPlayer);
                ImGui::TableNextRow();
                {
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(blacklistedPlayer.Name.c_str());
                
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##autokick-checkbox", &blacklistedPlayer.Autokick);
                
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##automute-checkbox", &blacklistedPlayer.Automute);
                }
                ImGui::PopID();
            }

            ImGui::EndTable();
        }
    }
}
