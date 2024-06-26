#include "OnlinePlayers.hpp"

#include "vendor/imgui.hpp"


namespace BPR
{
    // BrnGui::GuiEventNetworkSelectedPlayerOption
    struct SelectedPlayerOptionEvent
    {
        int32_t Option;
        uint64_t PlayerID;
    };
    

    // void __thiscall CgsGui::StateInterface::OutputGuiEvent<>(BrnGui::GuiEventNetworkSelectedPlayerOption* lOutEvent)
    static void OutputSelectedPlayerOptionEvent(const SelectedPlayerOptionEvent* selectedPlayerOptionEvent)
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

void OnlinePlayers::OnGuiEventNetworkPlayerStatus(Core::Pointer guiEventNetworkPlayerStatus, Core::Pointer guiCache)
{
    // BrnGui::GuiEventNetworkPlayerStatus* guiEventNetworkPlayerStatus
    // BrnGui::GuiCache* guiCache

    if (!m_BlacklistEnabled)
    {
        return;
    }

    int32_t currentGameMode = guiCache.at(0xCF38).as<int32_t>();
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
                BPR::OutputSelectedPlayerOptionEvent(&selectedPlayerOptionEvent);
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
                BPR::OutputSelectedPlayerOptionEvent(&selectedPlayerOptionEvent);
            }
        }
    }
}

void OnlinePlayers::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Current Players"))
    {
        // BrnGui::GuiCache* guiCache
        
        Core::Pointer guiCache = Core::Pointer(0x013FC8E0).deref().at(0x8E8430);

        bool isOnline = guiCache.at(0x7B00).as<bool>();
        if (isOnline)
        {        
            {
                ImGui::Text("Host Name      %s", guiCache.at(0xEA00).as<char[65]>());
                ImGui::Text("You are Host   %s", guiCache.at(0xEA59).as<bool>() ? "Yes" : "No");
            }

            {
                ImGui::SeparatorText("Current Players");
                
                if (ImGui::BeginTable("##player-info-table", 2, ImGuiTableFlags_None, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 12.0f)))
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Blacklist");
                    ImGui::TableHeadersRow();
                
                    int32_t playersCount = guiCache.at(0xDE2C).as<int32_t>();
                    for (int32_t i = 0; i < playersCount; ++i)
                    {
                        Core::Pointer playerStatusData = guiCache.at(0xDE38 + i * 0x138);
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
        }
        else
        {
            ImGui::TextUnformatted("Currently Offline");
        }
    }

    if (ImGui::CollapsingHeader("Blacklisted Players"))
    {
        {
            ImGui::Checkbox("Blacklist Enabled", &m_BlacklistEnabled);
        
            if (ImGui::Button("Save"))
            {
                m_BlacklistedPlayersFile.Save();
            }
            ImGui::SameLine();
            if (ImGui::Button("Load"))
            {
                m_BlacklistedPlayersFile.Load();
            }
        }

        {
            ImGui::SeparatorText("Blacklisted Players");

            static ImGuiTextFilter blacklistedPlayerFilter;
            blacklistedPlayerFilter.Draw("Filter##blacklisted-player");
            
            if (ImGui::BeginTable("##blacklisted-players-table", 3, ImGuiTableFlags_ScrollY, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 20.0f)))
            {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Autokick");
                ImGui::TableSetupColumn("Automute");
                ImGui::TableHeadersRow();
            
                for (BlacklistedPlayer& blacklistedPlayer : m_BlacklistedPlayersFile.GetBlacklistedPlayers())
                {
                    if (blacklistedPlayerFilter.PassFilter(blacklistedPlayer.Name.c_str()))
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
                }

                ImGui::EndTable();
            }
        }
    }
}
