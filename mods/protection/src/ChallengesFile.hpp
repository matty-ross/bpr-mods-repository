#pragma once


#include <cstdint>
#include <span>
#include <string>
#include <vector>

#include "core/Path.hpp"
#include "core/Logger.hpp"

#include "Challenges.hpp"


class ChallengesFile
{
public:
    struct Challenge
    {
        uint64_t ID = 0;
        std::string Title;
        const VanillaChallenge* Replacement = nullptr;
    };

public:
    ChallengesFile(Core::Path configDirectoryPath, const Core::Logger& logger);

public:
    std::span<Challenge> GetChallenges();
    Challenge* FindChallengeByID(uint64_t challengeID);
    void AddChallenge(const Challenge& challenge);

    const VanillaChallenge* GetFallbackChallenge() const;
    void SetFallbackChallenge(const VanillaChallenge* fallbackChallenge);

    void Load();
    void Save() const;

private:
    static constexpr char k_Name[] = "challenges";

private:
    Core::Path m_FilePath;

    std::vector<Challenge> m_Challenges;
    const VanillaChallenge* m_FallbackChallenge = k_LastResortFallbackChallenge;

    const Core::Logger& m_Logger;
};
