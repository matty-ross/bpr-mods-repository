#include <cstdint>
#include <exception>
#include <span>
#include <string>
#include <vector>

#include "vendor/yaml-cpp.hpp"

#include "core/Path.hpp"
#include "core/Logger.hpp"
#include "core/File.hpp"

#include "BlacklistedPlayersFile.hpp"


BlacklistedPlayersFile::BlacklistedPlayersFile(Core::Path configDirectoryPath, const Core::Logger& logger)
    :
    m_FilePath(configDirectoryPath.Append("blacklisted-players.yaml")),
    m_Logger(logger)
{
}

std::span<BlacklistedPlayersFile::BlacklistedPlayer> BlacklistedPlayersFile::GetBlacklistedPlayers()
{
    return m_BlacklistedPlayers;
}

BlacklistedPlayersFile::BlacklistedPlayer* BlacklistedPlayersFile::FindBlacklistedPlayerByID(uint64_t blacklistedPlayerID)
{
    for (BlacklistedPlayer& blacklistedPlayer : m_BlacklistedPlayers)
    {
        if (blacklistedPlayer.ID == blacklistedPlayerID)
        {
            return &blacklistedPlayer;
        }
    }

    return nullptr;
}

void BlacklistedPlayersFile::AddBlacklistedPlayer(const BlacklistedPlayer& blacklistedPlayer)
{
    m_BlacklistedPlayers.push_back(blacklistedPlayer);
}

void BlacklistedPlayersFile::RemoveBlacklistedPlayerByID(uint64_t blacklistedPlayerID)
{
    std::erase_if(m_BlacklistedPlayers, [=](BlacklistedPlayer& blacklistedPlayer)
    {
        return blacklistedPlayer.ID == blacklistedPlayerID;
    });
}

void BlacklistedPlayersFile::Load()
{
    try
    {
        YAML::Node yaml = YAML::Load(
            Core::File(m_FilePath, Core::File::Mode::Read, m_Logger).ReadAsText()
        );

        m_BlacklistedPlayers.clear();
        for (YAML::Node blacklistedPlayerNode : yaml["BlacklistedPlayers"])
        {
            BlacklistedPlayer blacklistedPlayer =
            {
                .ID = blacklistedPlayerNode["ID"].as<uint64_t>(-1),
                .Name = blacklistedPlayerNode["Name"].as<std::string>(""),
                .Autokick = blacklistedPlayerNode["Autokick"].as<bool>(false),
                .Automute = blacklistedPlayerNode["Automute"].as<bool>(false),
            };
            m_BlacklistedPlayers.push_back(blacklistedPlayer);
        }

        m_Logger.Info("Loaded %s.", k_Name);
    }
    catch (const std::exception& ex)
    {
        m_Logger.Warning("Failed to load %s. exception: %s", k_Name, ex.what());
    }
}

void BlacklistedPlayersFile::Save() const
{
    try
    {
        YAML::Node yaml;

        for (const BlacklistedPlayer& blacklistedPlayer : m_BlacklistedPlayers)
        {
            YAML::Node blacklistedPlayerNode;
            blacklistedPlayerNode["ID"] = blacklistedPlayer.ID;
            blacklistedPlayerNode["Name"] = blacklistedPlayer.Name;
            blacklistedPlayerNode["Autokick"] = blacklistedPlayer.Autokick;
            blacklistedPlayerNode["Automute"] = blacklistedPlayer.Automute;
            yaml["BlacklistedPlayers"].push_back(blacklistedPlayerNode);
        }

        Core::File(m_FilePath, Core::File::Mode::Write, m_Logger).WriteAsText(
            YAML::Dump(yaml)
        );

        m_Logger.Info("Saved %s.", k_Name);
    }
    catch (const std::exception& ex)
    {
        m_Logger.Warning("Failed to save %s. exception: %s", k_Name, ex.what());
    }
}
