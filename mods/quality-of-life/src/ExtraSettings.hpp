#pragma once


#include "core/Logger.hpp"
#include "core/Patch.hpp"


class ExtraSettings
{
public:
    ExtraSettings(const Core::Logger& logger);

public:
    void Load();
    void Unload();

private:
    const Core::Logger& m_Logger;

    Core::Patch m_Patch24HourTimeFormat;
    Core::Patch m_PatchMetricUnits;
    Core::Patch m_PatchDisableWebcam;
    Core::Patch m_PatchDisableCursorClipping;
};
