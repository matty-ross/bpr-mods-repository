#pragma once


#include "core/Path.hpp"
#include "core/Logger.hpp"


struct BugFixesFeatures
{
    bool UnknownLiveryVehicleIDs = true;
    bool ReloadingVehicleAfterLeavingJunkyard = true;
    bool HudTextSpacing = true;
    bool IncorrectBikeRoadRules = true;
    bool IncorrectLobbyDeletedPopup = true;
    bool DisabledWindow = true;
};

struct ExtraSettingsFeatures
{
    bool Use24HourTimeFormat = true;
    bool UseMetricUnits = true;
    bool DisableWebcam = true;
    bool DisableCursorClipping = true;
};


class FeaturesFile
{
public:
    FeaturesFile(Core::Path directory, const Core::Logger& logger);

public:
    void Load();
    void Save() const;

    BugFixesFeatures& GetBugFixesFeatures();
    ExtraSettingsFeatures& GetExtraSettingsFeatures();

private:
    Core::Path m_FilePath;
    const Core::Logger& m_Logger;

    BugFixesFeatures m_BugFixesFeatures;
    ExtraSettingsFeatures m_ExtraSettingsFeatures;
};
