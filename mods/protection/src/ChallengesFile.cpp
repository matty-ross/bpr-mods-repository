#include "ChallengesFile.hpp"

#include "vendor/yaml-cpp.hpp"

#include "core/File.hpp"


ChallengesFile::ChallengesFile(const std::string& filePath, const Core::Logger& logger)
    :
    m_FilePath(filePath),
    m_Logger(logger)
{
}

void ChallengesFile::Load()
{
    try
    {
        m_Logger.Info("Loading challenges from file '%s' ...", m_FilePath.c_str());

        Core::File file(m_FilePath, Core::File::Mode::Read);
        
        YAML::Node yaml = YAML::Load(file.Read());
        {
            uint64_t fallbackChallengeID = yaml["FallbackID"].as<uint64_t>();
            const VanillaChallenge* fallbackChallenge = GetVanillaChallenge(fallbackChallengeID);
            m_FallbackChallenge = fallbackChallenge != nullptr ? fallbackChallenge : k_LastResortFallbackChallenge;

            m_Challenges.clear();
            for (const YAML::Node& challengeNode : yaml["Challenges"])
            {
                uint64_t replacementChallengeID = challengeNode["ReplacementID"].as<uint64_t>();
                const VanillaChallenge* replacementChallenge = GetVanillaChallenge(replacementChallengeID);
                Challenge challenge =
                {
                    .ID          = challengeNode["ID"].as<uint64_t>(),
                    .Title       = challengeNode["Title"].as<std::string>(),
                    .Replacement = replacementChallenge != nullptr ? replacementChallenge : m_FallbackChallenge,
                };
            
                m_Challenges.push_back(challenge);
            }
        }

        m_Logger.Info("Loaded challenges.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to load challenges - %s", e.what());
    }
}

void ChallengesFile::Save() const
{
    try
    {
        m_Logger.Info("Saving challenges to file '%s' ...", m_FilePath.c_str());

        Core::File file(m_FilePath, Core::File::Mode::Write);

        YAML::Node yaml;
        {
            yaml["FallbackID"] = m_FallbackChallenge->ID;
            
            for (const Challenge& challenge : m_Challenges)
            {
                YAML::Node challengeNode;
                challengeNode["ID"]            = challenge.ID;
                challengeNode["Title"]         = challenge.Title;
                challengeNode["ReplacementID"] = challenge.Replacement->ID;

                yaml["Challenges"].push_back(challengeNode);
            }
        }
        file.Write(YAML::Dump(yaml));

        m_Logger.Info("Saved challenges.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to save challenges - %s", e.what());
    }
}

std::vector<Challenge>& ChallengesFile::GetChallenges()
{
    return m_Challenges;
}

Challenge* ChallengesFile::GetChallenge(uint64_t challengeID)
{
    for (Challenge& challenge : m_Challenges)
    {
        if (challenge.ID == challengeID)
        {
            return &challenge;
        }
    }

    return nullptr;
}

const VanillaChallenge* ChallengesFile::GetFallbackChallenge() const
{
    return m_FallbackChallenge;
}

void ChallengesFile::SetFallbackChallenge(const VanillaChallenge* fallbackChallenge)
{
    m_FallbackChallenge = fallbackChallenge;
}
