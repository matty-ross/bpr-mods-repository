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
    void Save();

    std::map<uint64_t, Challenge>& GetChallenges();
    const std::vector<uint64_t>& GetChallengeIDs() const;
    void AddChallenge(const Challenge& challenge);

private:
    const Core::Logger& m_Logger;

    std::string m_FilePath;

    std::map<uint64_t, Challenge> m_Challenges;
    std::vector<uint64_t> m_ChallengeIDs;
};