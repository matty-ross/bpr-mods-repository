#pragma once


#include "core/Logger.hpp"
#include "core/Patch.hpp"

#include "FeaturesFile.hpp"


class ExtraSettings
{
public:
    ExtraSettings(const Core::Logger& logger, ExtraSettingsFeatures& extraSettingsFeatures);

public:
    void Load();
    void Unload();

private:
    const Core::Logger& m_Logger;
    ExtraSettingsFeatures& m_ExtraSettingsFeatures;

    Core::Patch m_PatchUse24HourTimeFormat;
    Core::Patch m_PatchUseMetricUnits;
    Core::Patch m_PatchDisableWebcam;
    Core::Patch m_PatchDisableCursorClipping;
};
