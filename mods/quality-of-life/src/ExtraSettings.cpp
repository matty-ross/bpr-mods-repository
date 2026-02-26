#include "ExtraSettings.hpp"

#include "core/Pointer.hpp"


ExtraSettings::ExtraSettings(const Core::Logger& logger, ExtraSettingsFeatures& extraSettingsFeatures)
    :
    m_Logger(logger),
    m_ExtraSettingsFeatures(extraSettingsFeatures),
    m_PatchUse24HourTimeFormat(Core::Pointer(0x0621C64D).GetAddress(), 2),
    m_PatchUseMetricUnits(Core::Pointer(0x062178C4).GetAddress(), 2),
    m_PatchDisableWebcam(Core::Pointer(0x00A3CD84).GetAddress(), 2),
    m_PatchDisableCursorClipping(Core::Pointer(0x008FB91E).GetAddress(), 3)
{
}

void ExtraSettings::Load()
{
    if (m_ExtraSettingsFeatures.Use24HourTimeFormat)
    {
        // Apply use 24 hour time format patch.
        {
            m_Logger.Info("Applying use 24 hour time format patch...");

            m_PatchUse24HourTimeFormat.Apply("\xEB\x1F");

            m_Logger.Info("Applied use 24 hour time format patch.");
        }
    }

    if (m_ExtraSettingsFeatures.UseMetricUnits)
    {
        // Apply use metric units patch.
        {
            m_Logger.Info("Applying use metric units patch...");

            m_PatchUseMetricUnits.Apply("\xEB\x63");

            m_Logger.Info("Applied use metric units patch.");
        }
    }

    if (m_ExtraSettingsFeatures.DisableWebcam)
    {
        // Apply disable webcam patch.
        {
            m_Logger.Info("Applying disable webcam patch...");

            m_PatchDisableWebcam.Apply("\x31\xC0");

            m_Logger.Info("Applied disable webcam patch.");
        }
    }

    if (m_ExtraSettingsFeatures.DisableCursorClipping)
    {
        // Apply disable cursor clipping patch.
        {
            m_Logger.Info("Applying disable cursor clipping patch...");

            m_PatchDisableCursorClipping.Apply("\xEB\x6A\x90");

            m_Logger.Info("Applied disable cursor clipping patch.");
        }
    }
}

void ExtraSettings::Unload()
{
    if (m_ExtraSettingsFeatures.DisableCursorClipping)
    {
        // Remove disable cursor clipping patch.
        {
            m_Logger.Info("Removing disable cursor clipping patch...");

            m_PatchDisableCursorClipping.Remove();

            m_Logger.Info("Removed disable cursor clipping patch.");
        }
    }

    if (m_ExtraSettingsFeatures.DisableWebcam)
    {
        // Remove disable webcam patch.
        {
            m_Logger.Info("Removing disable webcam patch...");

            m_PatchDisableWebcam.Remove();

            m_Logger.Info("Removed disable webcam patch.");
        }
    }

    if (m_ExtraSettingsFeatures.UseMetricUnits)
    {
        // Remove use metric units patch.
        {
            m_Logger.Info("Removing use metric units patch...");

            m_PatchUseMetricUnits.Remove();

            m_Logger.Info("Removed use metric units patch.");
        }
    }

    if (m_ExtraSettingsFeatures.Use24HourTimeFormat)
    {
        // Remove use 24 hour time format patch.
        {
            m_Logger.Info("Removing use 24 hour time format patch...");

            m_PatchUse24HourTimeFormat.Remove();

            m_Logger.Info("Removed use 24 hour time format patch.");
        }
    }
}
