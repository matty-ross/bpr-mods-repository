#pragma once


#include <string>
#include <map>
#include <vector>

#include "core/Logger.h"

#include "Challenges.h"


struct Challenge
{
    std::string Title;
    const VanillaChallenge* Replacement = nullptr;
    bool Protect = true;
};


class ChallengesFile
{
public:
    ChallengesFile(const std::string& filePath, const Core::Logger& logger);

public:
    void Load();
    void Save() const;

    const std::vector<uint64_t>& GetChallengeIDs() const;
    Challenge* GetChallenge(uint64_t challengeID);
    void AddChallenge(uint64_t challengeID, const Challenge& challenge);

    const VanillaChallenge* GetFallbackChallenge() const;
    void SetFallbackChallenge(const VanillaChallenge* fallbackChallenge);

private:
    std::string m_FilePath;
    const Core::Logger& m_Logger;

    std::map<uint64_t, Challenge> m_Challenges;
    std::vector<uint64_t> m_ChallengeIDs;
    
    const VanillaChallenge* m_FallbackChallenge = nullptr;

    bool m_Valid = false;
};