#include "CurrentLobby.h"

#include <Windows.h>

#include "imgui/imgui.h"

#include "yaml-cpp/yaml.h"

#include "core/File.h"


namespace BPR
{
    enum PlayerOption
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


CurrentLobby::CurrentLobby(Core::Logger& logger, const std::string& blacklistedPlayersFilePath)
    :
    m_Logger(logger),
    m_BlacklistedPlayersFilePath(blacklistedPlayersFilePath)
{
}

void CurrentLobby::OnUpdate(Core::Pointer guiEventNetworkPlayerStatus)
{
    int32_t playersCount = guiEventNetworkPlayerStatus.at(0x9C0).as<int32_t>();
    for (int32_t i = 0; i < playersCount; ++i)
    {
        Core::Pointer playerStatusData = guiEventNetworkPlayerStatus.at(0x0 + i * 0x138);

        bool isLocalPlayer = playerStatusData.at(0x12D).as<bool>();
        if (isLocalPlayer)
        {
            continue;
        }

        uint64_t playerId = playerStatusData.at(0x110).as<uint64_t>();

        auto it = std::find_if(m_BlacklistedPlayers.begin(), m_BlacklistedPlayers.end(), [=](const BlacklistedPlayer& blacklistedPlayer)
            {
                return blacklistedPlayer.ID == playerId;
            }
        );
        if (it == m_BlacklistedPlayers.end())
        {
            continue;
        }

        BlacklistedPlayer& blacklistedPlayer = *it;
        if (blacklistedPlayer.Autokick)
        {
            bool isLocalPlayerHost = guiEventNetworkPlayerStatus.at(0xA05).as<bool>();
            if (isLocalPlayerHost)
            {
                BPR::SelectedPlayerOptionEvent selectedPlayerOptionEvent =
                {
                    .PlayerOption    = BPR::PlayerOption::Kick,
                    .NetworkPlayerID = playerId,
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
                    .NetworkPlayerID = playerId,
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

        ImGui::Text("Lobby Name             %s", guiCache.at(0xEA00).as<char[65]>());
        ImGui::Text("Local Player is Host   %s", guiCache.at(0xEA59).as<bool>() ? "Yes" : "No");

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
                Core::Pointer playerStatusData = playerInfo.at(i * 0x138);

                ImGui::PushID(playerStatusData.GetAddress());
                
                ImGui::TableNextRow();
                
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(playerStatusData.at(0xF0).as<char[25]>());

                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(playerStatusData.at(0x12C).as<bool>() ? "Yes" : "No");

                ImGui::TableSetColumnIndex(2);
                if (ImGui::Button("Add"))
                {
                    AddPlayerIntoBlacklistedPlayers(playerStatusData);
                }

                ImGui::PopID();
            }

            ImGui::EndTable();
        }

        ImGui::SeparatorText("Blacklisted Players");
        if (ImGui::BeginTable("##blacklisted-players-table", 3))
        {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Autokick");
            ImGui::TableSetupColumn("Automute");
            ImGui::TableHeadersRow();

            for (BlacklistedPlayer& blacklistedPlayer : m_BlacklistedPlayers)
            {
                ImGui::PushID(&blacklistedPlayer);

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(blacklistedPlayer.Name.c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::Checkbox("##autokick-checkbox", &blacklistedPlayer.Autokick);

                ImGui::TableSetColumnIndex(2);
                ImGui::Checkbox("##automute-checkbox", &blacklistedPlayer.Automute);

                ImGui::PopID();
            }
            
            ImGui::EndTable();
        }
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
    try
    {
        m_Logger.Info("Loading blacklisted players from file '%s' ...", m_BlacklistedPlayersFilePath.c_str());

        auto readFile = [this]() -> std::string
        {
            try
            {
                Core::File file(m_BlacklistedPlayersFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_ALWAYS);
                return file.Read();
            }
            catch (const std::runtime_error& e)
            {
                m_Logger.Warning("%s. Last error: 0x%08X.", e.what(), GetLastError());
            }

            return std::string();
        };

        YAML::Node yaml = YAML::Load(readFile());

        m_BlacklistedPlayers.clear();
        for (const YAML::Node& blacklistedPlayerNode : yaml)
        {
            BlacklistedPlayer blacklistedPlayer =
            {
                .ID       = blacklistedPlayerNode["ID"].as<uint64_t>(),
                .Name     = blacklistedPlayerNode["Name"].as<std::string>(),
                .Autokick = blacklistedPlayerNode["Autokick"].as<bool>(),
                .Automute = blacklistedPlayerNode["Automute"].as<bool>(),
            };
            m_BlacklistedPlayers.push_back(blacklistedPlayer);
        }

        m_Logger.Info("Loaded blacklisted players.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to load blacklisted players. %s", e.what());
    }
}

void CurrentLobby::SaveBlacklistedPlayers()
{
    try
    {
        m_Logger.Info("Saving blacklisted players to file '%s' ...", m_BlacklistedPlayersFilePath.c_str());

        auto writeFile = [this](const std::string& content) -> void
        {
            try
            {
                Core::File file(m_BlacklistedPlayersFilePath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS);
                file.Write(content);
            }
            catch (const std::runtime_error& e)
            {
                m_Logger.Warning("%s. Last error: 0x%08X.", e.what(), GetLastError());
            }
        };

        YAML::Node yaml;
        for (const BlacklistedPlayer& blacklistedPlayer : m_BlacklistedPlayers)
        {
            YAML::Node blacklistedPlayerNode;
            {
                blacklistedPlayerNode["ID"]       = blacklistedPlayer.ID;
                blacklistedPlayerNode["Name"]     = blacklistedPlayer.Name;
                blacklistedPlayerNode["Autokick"] = blacklistedPlayer.Autokick;
                blacklistedPlayerNode["Automute"] = blacklistedPlayer.Automute;   
            }
            yaml.push_back(blacklistedPlayerNode);
        }

        writeFile(YAML::Dump(yaml));

        m_Logger.Info("Saved blacklisted players.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to save blacklisted players. %s", e.what());
    }
}

void CurrentLobby::AddPlayerIntoBlacklistedPlayers(Core::Pointer playerStatusData)
{
    BlacklistedPlayer blacklistedPlayer =
    {
        .ID       = playerStatusData.at(0x110).as<uint64_t>(),
        .Name     = playerStatusData.at(0xF0).as<char[25]>(),
        .Autokick = false,
        .Automute = false,
    };
    m_BlacklistedPlayers.push_back(blacklistedPlayer);
}