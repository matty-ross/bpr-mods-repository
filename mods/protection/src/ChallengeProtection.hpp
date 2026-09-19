#pragma once


#include <cstdint>

#include "core/Pointer.hpp"

#include "ChallengesFile.hpp"


class ChallengeProtection
{
public:
    ChallengeProtection(ChallengesFile& challengesFile);

public:
    void OnFreeburnChallengeMessagePack(Core::Pointer freeburnChallengeMessage);
    void OnFreeburnChallengeMessageUnpack(Core::Pointer freeburnChallengeMessage);

    void RenderMenu();

    void AddNonVanillaChallengesToChallengesFile();

private:
    uint64_t HandleChallengeID(uint64_t challengeID) const;

private:
    ChallengesFile& m_ChallengesFile;
    bool m_ChallengeProtectionEnabled = true;
};
