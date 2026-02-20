#include "ExtraSettings.hpp"

#include "core/Pointer.hpp"


ExtraSettings::ExtraSettings(const Core::Logger& logger)
    :
    m_Logger(logger),
    m_PatchMetricUnits(Core::Pointer(0x062178C4).GetAddress(), 2)
{
}

void ExtraSettings::Load()
{
    // Apply metric units patch.
    {
        m_Logger.Info("Applying metric units patch...");

        m_PatchMetricUnits.Apply("\xEB\x63");

        m_Logger.Info("Applied metric units patch.");
    }
}

void ExtraSettings::Unload()
{
    // Remove metric units patch.
    {
        m_Logger.Info("Removing metric units patch...");

        m_PatchMetricUnits.Remove();

        m_Logger.Info("Removed metric units patch.");
    }
}
