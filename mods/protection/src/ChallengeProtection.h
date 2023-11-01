#pragma once


#include "ChallengesFile.h"


class ChallengeProtection
{
public:
    ChallengeProtection(ChallengesFile& challengesFile);

public:
    void OnRenderMenu();

    void AddNonVanillaChallengeIDsToChallengesFile();

private:
    uint64_t HandleChallengeID(uint64_t challengeID);

private:
    ChallengesFile& m_ChallengesFile;

    bool m_ChallengeProtectionEnabled = true;
};