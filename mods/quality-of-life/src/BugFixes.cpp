#include "BugFixes.hpp"

#include "core/Pointer.hpp"


BugFixes::BugFixes(const Core::Logger& logger)
    :
    m_Logger(logger),
    m_PatchUnknownLiveryVehicleIDs(Core::Pointer(0x06E5C0AF).GetAddress(), 2)
{
}

void BugFixes::Load()
{
    // Apply unknown livery vehicle IDs patch.
    {
        m_Logger.Info("Applying unknown livery vehicle IDs patch...");

        m_PatchUnknownLiveryVehicleIDs.Apply("\x78\x70");

        m_Logger.Info("Applyed unknown livery vehicle IDs patch.");
    }
}

void BugFixes::Unload()
{
    // Remove unknown livery vehicle IDs patch.
    {
        m_Logger.Info("Removing unknown livery vehicle IDs patch...");

        m_PatchUnknownLiveryVehicleIDs.Remove();

        m_Logger.Info("Removed unknown livery vehicle IDs patch.");
    }
}
