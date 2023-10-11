#include "BlacklistedPlayersFile.h"

#include <Windows.h>

#include "yaml-cpp/yaml.h"

#include "core/File.h"


BlacklistedPlayersFile::BlacklistedPlayersFile(const Core::Logger& logger, const std::string& filePath)
    :
    m_Logger(logger),
    m_FilePath(filePath)
{
}

void BlacklistedPlayersFile::Load()
{
    try
    {
        m_Logger.Info("Loading blacklisted players from file '%s' ...", m_FilePath.c_str());

        auto readFile = [this]() -> std::string
        {
            try
            {
                Core::File file(m_FilePath, GENERIC_READ, FILE_SHARE_READ, OPEN_ALWAYS);
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
        m_BlacklistedPlayerIDs.clear();

        for (const YAML::Node& blacklistedPlayerNode : yaml)
        {
            uint64_t blacklistedPlayerID = blacklistedPlayerNode["ID"].as<uint64_t>();
            BlacklistedPlayer blacklistedPlayer =
            {
                .Name     = blacklistedPlayerNode["Name"].as<std::string>(),
                .Autokick = blacklistedPlayerNode["Autokick"].as<bool>(),
                .Automute = blacklistedPlayerNode["Automute"].as<bool>(),
            };

            m_BlacklistedPlayers[blacklistedPlayerID] = blacklistedPlayer;
            m_BlacklistedPlayerIDs.push_back(blacklistedPlayerID);
        }

        m_Logger.Info("Loaded blacklisted players.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to load blacklisted players. %s", e.what());
    }
}

void BlacklistedPlayersFile::Save()
{
    try
    {
        m_Logger.Info("Saving blacklisted players to file '%s' ...", m_FilePath.c_str());

        auto writeFile = [this](const std::string& content) -> void
        {
            try
            {
                Core::File file(m_FilePath, GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS);
                file.Write(content);
            }
            catch (const std::runtime_error& e)
            {
                m_Logger.Warning("%s. Last error: 0x%08X.", e.what(), GetLastError());
            }
        };

        YAML::Node yaml;

        for (uint64_t blacklistedPlayerID : m_BlacklistedPlayerIDs)
        {
            const BlacklistedPlayer& blacklistedPlayer = m_BlacklistedPlayers.at(blacklistedPlayerID);
            YAML::Node blacklistedPlayerNode;
            {
                blacklistedPlayerNode["ID"]       = blacklistedPlayerID;
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

std::map<uint64_t, BlacklistedPlayer>& BlacklistedPlayersFile::GetBlacklistedPlayers()
{
    return m_BlacklistedPlayers;
}

const std::vector<uint64_t>& BlacklistedPlayersFile::GetBlacklistedPlayerIDs() const
{
    return m_BlacklistedPlayerIDs;
}

void BlacklistedPlayersFile::AddBlacklistedPlayer(uint64_t blacklistedPlayerID, const BlacklistedPlayer& blacklistedPlayer)
{
    m_BlacklistedPlayers[blacklistedPlayerID] = blacklistedPlayer;
    m_BlacklistedPlayerIDs.push_back(blacklistedPlayerID);
}