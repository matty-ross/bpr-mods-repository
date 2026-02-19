#pragma once


#include "core/Logger.hpp"
#include "core/Patch.hpp"
#include "mod-manager/DetourHookManager.hpp"

#include "FeaturesFile.hpp"


class BugFixes
{
public:
    BugFixes(const Core::Logger& logger, BugFixesFeatures& bugFixesFeatures);

public:
    void Load();
    void Unload();

private:
    static void DetourHudTextSpacing();

private:
    const Core::Logger& m_Logger;
    const BugFixesFeatures& m_BugFixesFeatures;
    
    Core::Patch m_PatchUnknownLiveryVehicleIDs;
    Core::Patch m_PatchReloadingVehicleAfterLeavingJunkyard[2];
    DetourHook m_DetourHudTextSpacing;
    Core::Patch m_PatchIncorrectBikeRoadRules;
    Core::Patch m_PatchIncorrectLobbyDeletedPopup;
    Core::Patch m_PatchDisabledWindow;
};
