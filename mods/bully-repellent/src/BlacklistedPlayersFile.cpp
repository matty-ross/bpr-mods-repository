#include "BlacklistedPlayersFile.hpp"

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
    try
    {
        m_Logger.Info("Loading blacklisted players from file '%s' ...", m_FilePath.c_str());

        Core::File file(m_FilePath, Core::File::Mode::Read);
        
        YAML::Node yaml = YAML::Load(file.Read());
        {
            m_BlacklistedPlayers.clear();
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
        }

        m_Logger.Info("Loaded blacklisted players.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to load blacklisted players - %s", e.what());
    }
}

void BlacklistedPlayersFile::Save() const
{
    try
    {
        m_Logger.Info("Saving blacklisted players to file '%s' ...", m_FilePath.c_str());

        Core::File file(m_FilePath, Core::File::Mode::Write);
        
        YAML::Node yaml;
        {
            for (const BlacklistedPlayer& blacklistedPlayer : m_BlacklistedPlayers)
            {
                YAML::Node blacklistedPlayerNode;
                blacklistedPlayerNode["ID"]       = blacklistedPlayer.ID;
                blacklistedPlayerNode["Name"]     = blacklistedPlayer.Name;
                blacklistedPlayerNode["Autokick"] = blacklistedPlayer.Autokick;
                blacklistedPlayerNode["Automute"] = blacklistedPlayer.Automute;

                yaml["BlacklistedPlayers"].push_back(blacklistedPlayerNode);
            }
        }
        file.Write(YAML::Dump(yaml));

        m_Logger.Info("Saved blacklisted players.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to save blacklisted players - %s", e.what());
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
