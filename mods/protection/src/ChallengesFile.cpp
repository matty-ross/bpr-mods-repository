#include <cstdint>
#include <exception>
#include <span>
#include <string>

#include "vendor/yaml-cpp.hpp"

#include "core/Path.hpp"
#include "core/Logger.hpp"
#include "core/File.hpp"

#include "Challenges.hpp"
#include "ChallengesFile.hpp"


ChallengesFile::ChallengesFile(Core::Path configDirectoryPath, const Core::Logger& logger)
    :
    m_FilePath(configDirectoryPath.Append("challenges.yaml")),
    m_Logger(logger)
{
}

std::span<ChallengesFile::Challenge> ChallengesFile::GetChallenges()
{
    return m_Challenges;
}

ChallengesFile::Challenge* ChallengesFile::FindChallengeByID(uint64_t challengeID)
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

void ChallengesFile::AddChallenge(const Challenge& challenge)
{
    m_Challenges.push_back(challenge);
}

const VanillaChallenge* ChallengesFile::GetFallbackChallenge() const
{
    return m_FallbackChallenge;
}

void ChallengesFile::SetFallbackChallenge(const VanillaChallenge* fallbackChallenge)
{
    m_FallbackChallenge = fallbackChallenge;
}

void ChallengesFile::Load()
{
    try
    {
        YAML::Node yaml = YAML::Load(
            Core::File(m_FilePath, Core::File::Mode::Read, m_Logger).ReadAsText()
        );

        uint64_t fallbackChallengeID = yaml["FallbackID"].as<uint64_t>();
        const VanillaChallenge* fallbackChallenge = FindVanillaChallengeByID(fallbackChallengeID);
        m_FallbackChallenge = fallbackChallenge != nullptr ? fallbackChallenge : k_LastResortFallbackChallenge;

        m_Challenges.clear();
        for (YAML::Node challengeNode : yaml["Challenges"])
        {
            uint64_t replacementChallengeID = challengeNode["ReplacementID"].as<uint64_t>();
            const VanillaChallenge* replacementChallenge = FindVanillaChallengeByID(replacementChallengeID);

            Challenge challenge =
            {
                .ID = challengeNode["ID"].as<uint64_t>(),
                .Title = challengeNode["Title"].as<std::string>(),
                .Replacement = replacementChallenge != nullptr ? replacementChallenge : m_FallbackChallenge,
            };
            m_Challenges.push_back(challenge);
        }

        m_Logger.Info("Loaded %s.", k_Name);
    }
    catch (const std::exception& ex)
    {
        m_Logger.Warning("Failed to load %s. exception: %s", k_Name, ex.what());
    }
}

void ChallengesFile::Save() const
{
    try
    {
        YAML::Node yaml;

        yaml["FallbackID"] = m_FallbackChallenge->ID;

        for (const Challenge& challenge : m_Challenges)
        {
            YAML::Node challengeNode;
            challengeNode["ID"] = challenge.ID;
            challengeNode["Title"] = challenge.Title;
            challengeNode["ReplacementID"] = challenge.Replacement->ID;
            yaml["Challenges"].push_back(challengeNode);
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
