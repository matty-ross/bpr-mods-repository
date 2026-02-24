#include "ExtraSettings.hpp"

#include "core/Pointer.hpp"


ExtraSettings::ExtraSettings(const Core::Logger& logger)
    :
    m_Logger(logger),
    m_Patch24HourTimeFormat(Core::Pointer(0x0621C64D).GetAddress(), 2),
    m_PatchMetricUnits(Core::Pointer(0x062178C4).GetAddress(), 2),
    m_PatchDisableCursorClipping(Core::Pointer(0x008FB91E).GetAddress(), 3)
{
}

void ExtraSettings::Load()
{
    // Apply 24 hour time format patch.
    {
        m_Logger.Info("Applying 24 hour time format patch...");

        m_Patch24HourTimeFormat.Apply("\xEB\x1F");

        m_Logger.Info("Applied 24 hour time format patch.");
    }

    // Apply metric units patch.
    {
        m_Logger.Info("Applying metric units patch...");

        m_PatchMetricUnits.Apply("\xEB\x63");

        m_Logger.Info("Applied metric units patch.");
    }

    // Apply disable cursor clipping patch.
    {
        m_Logger.Info("Applying disable cursor clipping patch...");

        m_PatchDisableCursorClipping.Apply("\xEB\x6A\x90");

        m_Logger.Info("Applied disable cursor clipping patch.");
    }
}

void ExtraSettings::Unload()
{
    // Remove disable cursor clipping patch.
    {
        m_Logger.Info("Removing disable cursor clipping patch...");

        m_PatchDisableCursorClipping.Remove();

        m_Logger.Info("Removed disable cursor clipping patch.");
    }

    // Remove metric units patch.
    {
        m_Logger.Info("Removing metric units patch...");

        m_PatchMetricUnits.Remove();

        m_Logger.Info("Removed metric units patch.");
    }

    // Remove 24 hour time format patch.
    {
        m_Logger.Info("Removing 24 hour time format patch...");

        m_Patch24HourTimeFormat.Remove();

        m_Logger.Info("Removed 24 hour time format patch.");
    }
}
