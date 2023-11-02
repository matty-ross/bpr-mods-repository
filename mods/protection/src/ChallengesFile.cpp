#include "ChallengesFile.h"

#include <Windows.h>

#include "yaml-cpp/yaml.h"

#include "core/File.h"


ChallengesFile::ChallengesFile(const Core::Logger& logger, const std::string& filePath)
    :
    m_Logger(logger),
    m_FilePath(filePath)
{
}

void ChallengesFile::Load()
{
    try
    {
        m_Logger.Info("Loading challenges from file '%s' ...", m_FilePath.c_str());

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

        m_Challenges.clear();
        m_ChallengeIDs.clear();

        for (const YAML::Node& challengeNode : yaml)
        {
            uint64_t newID = challengeNode["NewID"].as<uint64_t>();
            uint64_t replacementID = challengeNode["ReplacementID"].as<uint64_t>();

            const ChallengeID* replacementChallengeID = GetVanillaChallengeID(newID);

            Challenge challenge = {};
            challenge.NewID.Number = newID;
            sprintf_s(challenge.NewID.String, "%llu", newID);
            challenge.ReplacementID = replacementChallengeID != nullptr ? replacementChallengeID : &k_FallbackChallengeID;

            m_Challenges[challenge.NewID.Number] = challenge;
            m_ChallengeIDs.push_back(challenge.NewID.Number);
        }

        m_Logger.Info("Loaded challenges.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to load challenges. %s", e.what());
    }
}

void ChallengesFile::Save()
{
    try
    {
        m_Logger.Info("Saving challenges to file '%s' ...", m_FilePath.c_str());

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

        for (uint64_t challengeID : m_ChallengeIDs)
        {
            const Challenge& challenge = m_Challenges.at(challengeID);
            YAML::Node challengeNode;
            {
                challengeNode["NewID"]         = challenge.NewID.Number;
                challengeNode["ReplacementID"] = challenge.ReplacementID->Number;
            }

            yaml.push_back(challengeNode);
        }

        writeFile(YAML::Dump(yaml));

        m_Logger.Info("Saved challenges.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to save challenges. %s", e.what());
    }
}

std::map<uint64_t, Challenge>& ChallengesFile::GetChallenges()
{
    return m_Challenges;
}

const std::vector<uint64_t>& ChallengesFile::GetChallengeIDs() const
{
    return m_ChallengeIDs;
}

void ChallengesFile::AddChallenge(const Challenge& challenge)
{
    uint64_t challengeID = challenge.NewID.Number;
    m_Challenges[challengeID] = challenge;
    m_ChallengeIDs.push_back(challengeID);
}