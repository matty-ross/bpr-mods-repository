#pragma once


#include <string>
#include <vector>

#include "core/Logger.hpp"

#include "Challenges.hpp"


struct Challenge
{
    uint64_t ID = 0;
    std::string Title;
    const VanillaChallenge* Replacement = nullptr;
};


class ChallengesFile
{
public:
    ChallengesFile(const char* filePath, const Core::Logger& logger);

public:
    void Load();
    void Save() const;

    std::vector<Challenge>& GetChallenges();
    Challenge* GetChallenge(uint64_t challengeID);
    const VanillaChallenge* GetFallbackChallenge() const;
    void SetFallbackChallenge(const VanillaChallenge* fallbackChallenge);

private:
    const char* m_FilePath;
    const Core::Logger& m_Logger;

    std::vector<Challenge> m_Challenges;
    const VanillaChallenge* m_FallbackChallenge = k_LastResortFallbackChallenge;
};
