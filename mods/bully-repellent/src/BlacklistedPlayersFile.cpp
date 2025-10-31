#include "BlacklistedPlayersFile.hpp"

#include "vendor/yaml-cpp.hpp"

#include "core/File.hpp"


static constexpr char k_Name[] = "blacklisted players";


BlacklistedPlayersFile::BlacklistedPlayersFile(Core::Path directory, const Core::Logger& logger)
    :
    m_FilePath(directory.Append("blacklisted-players.yaml")),
    m_Logger(logger)
{
}

void BlacklistedPlayersFile::Load()
{
    try
    {
        m_Logger.Info("Loading %s from file '%s' ...", k_Name, m_FilePath.GetPath());

        Core::File file(m_FilePath.GetPath(), Core::File::Mode::Read);
        YAML::Node yaml = YAML::Load(file.Read<std::string>());
        {
            m_BlacklistedPlayers.clear();
            for (YAML::Node blacklistedPlayerNode : yaml["BlacklistedPlayers"])
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

        m_Logger.Info("Loaded %s.", k_Name);
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to load %s - %s", k_Name, e.what());
    }
}

void BlacklistedPlayersFile::Save() const
{
    try
    {
        m_Logger.Info("Saving %s to file '%s' ...", k_Name, m_FilePath.GetPath());

        Core::File file(m_FilePath.GetPath(), Core::File::Mode::Write);
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

        m_Logger.Info("Saved %s.", k_Name);
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to save %s - %s", k_Name, e.what());
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
