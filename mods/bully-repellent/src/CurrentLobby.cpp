#include "CurrentLobby.h"

#include <Windows.h>

#include "imgui/imgui.h"

#include "yaml-cpp/yaml.h"


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
                Core::Pointer playerStatus = playerInfo.at(i * 0x138);

                ImGui::PushID(playerStatus.GetAddress());
                
                ImGui::TableNextRow();
                
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(playerStatus.at(0xF0).as<char[25]>());

                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(playerStatus.at(0x12C).as<bool>() ? "Yes" : "No");

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
        auto readBlacklistedPlayersFile = [this]() -> std::string
        {
            HANDLE file = CreateFileA(
                m_BlacklistedPlayersFilePath.c_str(),
                GENERIC_READ,
                FILE_SHARE_READ,
                nullptr,
                OPEN_ALWAYS,
                0,
                nullptr
            );
            if (file == INVALID_HANDLE_VALUE)
            {
                m_Logger.Warning("Cannot open '%s'. Last error: 0x%08X.", m_BlacklistedPlayersFilePath.c_str(), GetLastError());
                return std::string();
            }

            size_t size = GetFileSize(file, nullptr);
            std::string content(size, ' ');

            DWORD bytesRead = 0;
            if (ReadFile(file, content.data(), content.size(), &bytesRead, nullptr) == FALSE)
            {
                m_Logger.Warning("Cannot read '%s'. Last error: 0x%08X.", m_BlacklistedPlayersFilePath.c_str(), GetLastError());
                content.clear();
            }

            CloseHandle(file);

            return content;
        };

        m_Logger.Info("Loading blacklisted players from file '%s' ...", m_BlacklistedPlayersFilePath.c_str());

        YAML::Node yaml = YAML::Load(readBlacklistedPlayersFile());

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
        m_Logger.Warning("Failed to load '%s'. %s.", m_BlacklistedPlayersFilePath.c_str(), e.what());
    }
}

void CurrentLobby::SaveBlacklistedPlayers()
{
    try
    {
        auto writeBlacklistedPlayersFile = [this](const std::string& content) -> void
        {
            HANDLE file = CreateFileA(
                m_BlacklistedPlayersFilePath.c_str(),
                GENERIC_WRITE,
                FILE_SHARE_READ,
                nullptr,
                CREATE_ALWAYS,
                0,
                nullptr
            );
            if (file == INVALID_HANDLE_VALUE)
            {
                m_Logger.Warning("Cannot open '%s'. Last error: 0x%08X.", m_BlacklistedPlayersFilePath.c_str(), GetLastError());
                return;
            }

            DWORD bytesWritten = 0;
            if (WriteFile(file, content.data(), content.size(), &bytesWritten, nullptr) == FALSE)
            {
                m_Logger.Warning("Cannot write '%s'. Last error: 0x%08X.", m_BlacklistedPlayersFilePath.c_str(), GetLastError());
            }

            CloseHandle(file);
        };

        m_Logger.Info("Saving blacklisted players into file '%s' ...", m_BlacklistedPlayersFilePath.c_str());

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

        writeBlacklistedPlayersFile(YAML::Dump(yaml));

        m_Logger.Info("Saved blacklisted players.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to save '%s'. %s", m_BlacklistedPlayersFilePath.c_str(), e.what());
    }
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