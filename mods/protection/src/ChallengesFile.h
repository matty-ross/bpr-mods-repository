#pragma once


#include <string>
#include <map>
#include <vector>

#include "core/Logger.h"

#include "Challenges.h"


struct Challenge
{
    ChallengeID NewID;
    const ChallengeID* ReplacementID;
};


class ChallengesFile
{
public:
    ChallengesFile(const Core::Logger& logger, const std::string& filePath);

public:
    void Load();
    void Save() const;

    const std::vector<uint64_t>& GetChallengeIDs() const;
    Challenge* GetChallenge(uint64_t challengeID);
    void AddChallenge(const Challenge& challenge);

    const ChallengeID* GetFallbackChallengeID() const;
    void SetFallbackChallengeID(const ChallengeID* fallbackChallengeID);

private:
    const Core::Logger& m_Logger;

    std::string m_FilePath;

    std::map<uint64_t, Challenge> m_Challenges;
    std::vector<uint64_t> m_ChallengeIDs;

    const ChallengeID* m_FallbackChallengeID = k_LastResortFallbackChallengeID;

    bool m_Valid = false;
};