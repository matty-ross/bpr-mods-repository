#include "BugFixes.hpp"

#include "core/Pointer.hpp"


BugFixes::BugFixes(const Core::Logger& logger)
    :
    m_Logger(logger),
    m_PatchUnknownLiveryVehicleIDs(Core::Pointer(0x06E5C0AF).GetAddress(), 2),
    m_PatchIncorrectBikeRoadRules(Core::Pointer(0x009E193A).GetAddress(), 2),
    m_PatchIncorrectLobbyDeletedPopup(Core::Pointer(0x00B27BE0).GetAddress(), 4),
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

    // Apply incorrect bike Road Rules patch.
    {
        m_Logger.Info("Applying incorrect bike Road Rules patch...");

        m_PatchIncorrectBikeRoadRules.Apply("\xEB\x16");

        m_Logger.Info("Applied incorrect bike Road Rules patch.");
    }

    // Apply incorrect lobby deleted popup patch.
    {
        m_Logger.Info("Applying incorrect lobby deleted popup patch...");

        m_PatchIncorrectLobbyDeletedPopup.Apply("\x04\x78\xB2\x00");

        m_Logger.Info("Applied incorrect lobby deleted popup patch.");
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

    // Remove incorrect lobby deleted popup patch.
    {
        m_Logger.Info("Removing incorrect lobby deleted popup patch...");

        m_PatchIncorrectLobbyDeletedPopup.Remove();

        m_Logger.Info("Removed incorrect lobby deleted popup patch.");
    }

    // Remove incorrect bike Road Rules patch.
    {
        m_Logger.Info("Removing incorrect bike Road Rules patch...");

        m_PatchIncorrectBikeRoadRules.Remove();

        m_Logger.Info("Removed incorrect bike Road Rules patch.");
    }
    
    // Remove unknown livery vehicle IDs patch.
    {
        m_Logger.Info("Removing unknown livery vehicle IDs patch...");

        m_PatchUnknownLiveryVehicleIDs.Remove();

        m_Logger.Info("Removed unknown livery vehicle IDs patch.");
    }
}
