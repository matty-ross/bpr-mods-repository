#include "BugFixes.hpp"

#include "core/Pointer.hpp"


BugFixes::BugFixes(const Core::Logger& logger)
    :
    m_Logger(logger),
    m_PatchUnknownLiveryVehicleIDs(Core::Pointer(0x06E5C0AF).GetAddress(), 2),
    m_PatchDisabledWindow(Core::Pointer(0x008FB6A3).GetAddress(), 7)
{
}

void BugFixes::Load()
{
    // Apply unknown livery vehicle IDs patch.
    {
        m_Logger.Info("Applying unknown livery vehicle IDs patch...");

        m_PatchUnknownLiveryVehicleIDs.Apply("\x78\x70");

        m_Logger.Info("Applied unknown livery vehicle IDs patch.");
    }

    // Apply disabled window patch.
    {
        m_Logger.Info("Applying disabled window patch...");

        m_PatchDisabledWindow.Apply("\xEB\x19\x90\x90\x90\x90\x90");

        m_Logger.Info("Applied disabled window patch.");
    }
}

void BugFixes::Unload()
{
    // Remove disabled window patch.
    {
        m_Logger.Info("Removing disabled window patch...");

        m_PatchDisabledWindow.Remove();

        m_Logger.Info("Removed disabled window patch.");
    }
    
    // Remove unknown livery vehicle IDs patch.
    {
        m_Logger.Info("Removing unknown livery vehicle IDs patch...");

        m_PatchUnknownLiveryVehicleIDs.Remove();

        m_Logger.Info("Removed unknown livery vehicle IDs patch.");
    }
}
