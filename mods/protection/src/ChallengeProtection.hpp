#pragma once


#include <cstdint>

#include "core/Pointer.hpp"
#include "core/Logger.hpp"

#include "ChallengesFile.hpp"


class ChallengeProtection
{
public:
    ChallengeProtection(ChallengesFile& challengesFile, const Core::Logger& logger);

public:
    void Load();

    void RenderMenu();
    void AddNonVanillaChallengesToChallengesFile();

private:
    uint64_t HandleChallengeID(uint64_t challengeID) const;

    void CheckFreeburnChallengeMessageBeforePacking(Core::Pointer freeburnChallengeMessage);
    void CheckFreeburnChallengeMessageAfterUnpacking(Core::Pointer freeburnChallengeMessage);

private:
    static void Hook_CheckFreeburnChallengeMessageBeforePacking();
    static void Hook_CheckFreeburnChallengeMessageAfterUnpacking();

private:
    ChallengesFile& m_ChallengesFile;
    bool m_ChallengeProtectionEnabled = true;

    const Core::Logger& m_Logger;
};
