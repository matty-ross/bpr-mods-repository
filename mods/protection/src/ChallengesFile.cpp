#include "ChallengesFile.h"

#include <Windows.h>

#include "yaml-cpp/yaml.h"

#include "core/File.h"


ChallengesFile::ChallengesFile(const std::string& filePath, const Core::Logger& logger)
    :
    m_FilePath(filePath),
    m_Logger(logger)
{
}

void ChallengesFile::Load()
{
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

    m_Valid = false;

    try
    {
        m_Logger.Info("Loading challenges from file '%s' ...", m_FilePath.c_str());

        m_Challenges.clear();
        m_ChallengeIDs.clear();
        
        YAML::Node yaml = YAML::Load(readFile());
        {
            uint64_t fallbackChallengeID = yaml["FallbackID"].as<uint64_t>();
            const VanillaChallenge* fallbackChallenge = GetVanillaChallenge(fallbackChallengeID);
            m_FallbackChallenge = fallbackChallenge != nullptr ? fallbackChallenge : k_LastResortFallbackChallenge;
        }
        for (const YAML::Node& challengeNode : yaml["Challenges"])
        {
            uint64_t challengeID = challengeNode["ID"].as<uint64_t>();
            uint64_t replacementChallengeID = challengeNode["ReplacementID"].as<uint64_t>();
            const VanillaChallenge* replacementChallenge = GetVanillaChallenge(replacementChallengeID);
            Challenge challenge =
            {
                .Title       = challengeNode["Title"].as<std::string>(),
                .Replacement = replacementChallenge != nullptr ? replacementChallenge : m_FallbackChallenge,
            };
            
            m_Challenges[challengeID] = challenge;
            m_ChallengeIDs.push_back(challengeID);
        }

        m_Logger.Info("Loaded challenges.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to load challenges. %s", e.what());
    }
    
    m_Valid = true;
}

void ChallengesFile::Save() const
{
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

    if (!m_Valid)
    {
        return;
    }

    try
    {
        m_Logger.Info("Saving challenges to file '%s' ...", m_FilePath.c_str());

        YAML::Node yaml;
        {
            yaml["FallbackID"] = m_FallbackChallenge->ID;
        }
        for (uint64_t challengeID : m_ChallengeIDs)
        {
            const Challenge& challenge = m_Challenges.at(challengeID);
            
            YAML::Node challengeNode;
            challengeNode["ID"]            = challengeID;
            challengeNode["Title"]         = challenge.Title;
            challengeNode["ReplacementID"] = challenge.Replacement->ID;

            yaml["Challenges"].push_back(challengeNode);
        }
        writeFile(YAML::Dump(yaml));

        m_Logger.Info("Saved challenges.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to save challenges. %s", e.what());
    }
}

const std::vector<uint64_t>& ChallengesFile::GetChallengeIDs() const
{
    return m_ChallengeIDs;
}

Challenge* ChallengesFile::GetChallenge(uint64_t challengeID)
{
    auto it = m_Challenges.find(challengeID);
    return it != m_Challenges.end() ? &(it->second) : nullptr;
}

void ChallengesFile::AddChallenge(uint64_t challengeID, const Challenge& challenge)
{
    m_Challenges[challengeID] = challenge;
    m_ChallengeIDs.push_back(challengeID);
}

const VanillaChallenge* ChallengesFile::GetFallbackChallenge() const
{
    return m_FallbackChallenge;
}

void ChallengesFile::SetFallbackChallenge(const VanillaChallenge* fallbackChallenge)
{
    m_FallbackChallenge = fallbackChallenge;
}