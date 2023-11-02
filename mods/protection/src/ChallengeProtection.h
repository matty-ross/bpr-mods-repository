#pragma once


#include "ChallengesFile.h"


class ChallengeProtection
{
public:
    ChallengeProtection(ChallengesFile& challengesFile);

public:
    void OnFreeburnChallengeMessagePack(void* freeburnChallengeMessage);
    void OnFreeburnChallengeMessageUnpack(void* freeburnChallengeMessage);
    void OnRenderMenu();

    void AddNonVanillaChallengeIDsToChallengesFile();

private:
    uint64_t HandleChallengeID(uint64_t challengeID);

private:
    ChallengesFile& m_ChallengesFile;

    bool m_ChallengeProtectionEnabled = true;
};