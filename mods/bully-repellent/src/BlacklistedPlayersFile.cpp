#include "BlacklistedPlayersFile.hpp"

#include <stdexcept>

#include "vendor/yaml-cpp.hpp"

#include "core/File.hpp"


BlacklistedPlayersFile::BlacklistedPlayersFile(const std::string& filePath, const Core::Logger& logger)
    :
    m_FilePath(filePath),
    m_Logger(logger)
{
}

void BlacklistedPlayersFile::Load()
{
    auto readFile = [this]() -> std::string
    {
        try
        {
            Core::File file(m_FilePath, Core::File::Operation::Read);
            return file.Read();
        }
        catch (const std::runtime_error& e)
        {
            m_Logger.Warning("%s. Last error: 0x%08X.", e.what(), GetLastError());
        }

        return std::string();
    };

    m_Valid = false;

    try
    {
        m_Logger.Info("Loading blacklisted players from file '%s' ...", m_FilePath.c_str());

        m_BlacklistedPlayers.clear();

        YAML::Node yaml = YAML::Load(readFile());
        for (const YAML::Node& blacklistedPlayerNode : yaml["BlacklistedPlayers"])
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

    m_Valid = true;
}

void BlacklistedPlayersFile::Save() const
{
    auto writeFile = [this](const std::string& content) -> void
    {
        try
        {
            Core::File file(m_FilePath, Core::File::Operation::Write);
            file.Write(content);
        }
        catch (const std::runtime_error& e)
        {
            m_Logger.Warning("%s. Last error: 0x%08X.", e.what(), GetLastError());
        }
    };

    if (!m_Valid)
    {
        return;
    }

    try
    {
        m_Logger.Info("Saving blacklisted players to file '%s' ...", m_FilePath.c_str());

        YAML::Node yaml;
        for (const BlacklistedPlayer& blacklistedPlayer : m_BlacklistedPlayers)
        {
            YAML::Node blacklistedPlayerNode;
            blacklistedPlayerNode["ID"]       = blacklistedPlayer.ID;
            blacklistedPlayerNode["Name"]     = blacklistedPlayer.Name;
            blacklistedPlayerNode["Autokick"] = blacklistedPlayer.Autokick;
            blacklistedPlayerNode["Automute"] = blacklistedPlayer.Automute;

            yaml["BlacklistedPlayers"].push_back(blacklistedPlayerNode);
        }
        writeFile(YAML::Dump(yaml));

        m_Logger.Info("Saved blacklisted players.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to save blacklisted players. %s", e.what());
    }
}

std::vector<BlacklistedPlayer>& BlacklistedPlayersFile::GetBlacklistedPlayers()
{
    return m_BlacklistedPlayers;
}

BlacklistedPlayer* BlacklistedPlayersFile::GetBlacklistedPlayer(uint64_t blacklistedPlayerID)
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
