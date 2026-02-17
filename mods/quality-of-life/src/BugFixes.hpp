#pragma once


#include "core/Logger.hpp"
#include "core/Patch.hpp"
#include "mod-manager/DetourHookManager.hpp"


class BugFixes
{
public:
    BugFixes(const Core::Logger& logger);

public:
    void Load();
    void Unload();

private:
    static void DetourHudTextSpacing();

private:
    const Core::Logger& m_Logger;
    
    Core::Patch m_PatchUnknownLiveryVehicleIDs;
    Core::Patch m_PatchReloadingVehicleAfterLeavingJunkyard[2];
    DetourHook m_DetourHudTextSpacing;
    Core::Patch m_PatchIncorrectBikeRoadRules;
    Core::Patch m_PatchIncorrectLobbyDeletedPopup;
    Core::Patch m_PatchDisabledWindow;
};
