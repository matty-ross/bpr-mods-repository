#pragma once


#include "ChallengesFile.hpp"


class ChallengeProtection
{
public:
    ChallengeProtection(ChallengesFile& challengesFile);

public:
    void OnFreeburnChallengeMessagePack(void* freeburnChallengeMessage);
    void OnFreeburnChallengeMessageUnpack(void* freeburnChallengeMessage);
    void OnRenderMenu();

    void AddNonVanillaChallengesToChallengesFile();

private:
    uint64_t HandleChallengeID(uint64_t challengeID) const;

private:
    ChallengesFile& m_ChallengesFile;

    bool m_ChallengeProtectionEnabled = true;
};
