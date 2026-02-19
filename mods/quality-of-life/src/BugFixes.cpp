#include "BugFixes.hpp"

#include "core/Pointer.hpp"
#include "mod-manager/ModManager.hpp"

#include "QualityOfLife.hpp"


BugFixes::BugFixes(const Core::Logger& logger, BugFixesFeatures& bugFixesFeatures)
    :
    m_Logger(logger),
    m_BugFixesFeatures(bugFixesFeatures),
    m_PatchUnknownLiveryVehicleIDs(Core::Pointer(0x06E5C0AF).GetAddress(), 2),
    m_PatchReloadingVehicleAfterLeavingJunkyard
    {
        Core::Patch(Core::Pointer(0x06F4B673).GetAddress(), 3),
        Core::Patch(Core::Pointer(0x06F4B693).GetAddress(), 3),
    },
    m_DetourHudTextSpacing
    {
        .Target = Core::Pointer(0x0088FA1C).GetAddress(),
        .Detour = &BugFixes::DetourHudTextSpacing,
    },
    m_PatchIncorrectBikeRoadRules(Core::Pointer(0x009E193A).GetAddress(), 2),
    m_PatchIncorrectLobbyDeletedPopup(Core::Pointer(0x00B27BE0).GetAddress(), 4),
    m_PatchDisabledWindow(Core::Pointer(0x008FB6A3).GetAddress(), 7)
{
}

void BugFixes::Load()
{
    if (m_BugFixesFeatures.UnknownLiveryVehicleIDs)
    {
        // Apply unknown livery vehicle IDs patch.
        {
            m_Logger.Info("Applying unknown livery vehicle IDs patch...");

            m_PatchUnknownLiveryVehicleIDs.Apply("\x78\x70");

            m_Logger.Info("Applied unknown livery vehicle IDs patch.");
        }
    }

    if (m_BugFixesFeatures.ReloadingVehicleAfterLeavingJunkyard)
    {
        // Apply reloading vehicle after leaving Junkyard patch.
        {
            m_Logger.Info("Applying reloading vehicle after leaving Junkyard patch...");

            m_PatchReloadingVehicleAfterLeavingJunkyard[0].Apply("\x31\xC9\x90");
            m_PatchReloadingVehicleAfterLeavingJunkyard[1].Apply("\x31\xC9\x90");

            m_Logger.Info("Applied reloading vehicle after leaving Junkyard patch.");
        }
    }

    if (m_BugFixesFeatures.HudTextSpacing)
    {
        // Attach HUD text spacing detour.
        {
            m_Logger.Info("Attaching HUD text spacing detour...");

            ModManager::Get().GetDetourHookManager().Attach(m_DetourHudTextSpacing);

            m_Logger.Info("Attached HUD text spacing detour.");
        }
    }

    if (m_BugFixesFeatures.IncorrectBikeRoadRules)
    {
        // Apply incorrect bike Road Rules patch.
        {
            m_Logger.Info("Applying incorrect bike Road Rules patch...");

            m_PatchIncorrectBikeRoadRules.Apply("\xEB\x16");

            m_Logger.Info("Applied incorrect bike Road Rules patch.");
        }
    }

    if (m_BugFixesFeatures.IncorrectLobbyDeletedPopup)
    {
        // Apply incorrect lobby deleted popup patch.
        {
            m_Logger.Info("Applying incorrect lobby deleted popup patch...");

            m_PatchIncorrectLobbyDeletedPopup.Apply("\x04\x78\xB2\x00");

            m_Logger.Info("Applied incorrect lobby deleted popup patch.");
        }
    }

    if (m_BugFixesFeatures.DisabledWindow)
    {
        // Apply disabled window patch.
        {
            m_Logger.Info("Applying disabled window patch...");

            m_PatchDisabledWindow.Apply("\xEB\x19\x90\x90\x90\x90\x90");

            m_Logger.Info("Applied disabled window patch.");
        }
    }
}

void BugFixes::Unload()
{
    if (m_BugFixesFeatures.DisabledWindow)
    {
        // Remove disabled window patch.
        {
            m_Logger.Info("Removing disabled window patch...");

            m_PatchDisabledWindow.Remove();

            m_Logger.Info("Removed disabled window patch.");
        }
    }

    if (m_BugFixesFeatures.IncorrectLobbyDeletedPopup)
    {
        // Remove incorrect lobby deleted popup patch.
        {
            m_Logger.Info("Removing incorrect lobby deleted popup patch...");

            m_PatchIncorrectLobbyDeletedPopup.Remove();

            m_Logger.Info("Removed incorrect lobby deleted popup patch.");
        }
    }

    if (m_BugFixesFeatures.IncorrectBikeRoadRules)
    {
        // Remove incorrect bike Road Rules patch.
        {
            m_Logger.Info("Removing incorrect bike Road Rules patch...");

            m_PatchIncorrectBikeRoadRules.Remove();

            m_Logger.Info("Removed incorrect bike Road Rules patch.");
        }
    }

    if (m_BugFixesFeatures.HudTextSpacing)
    {
        // Detach HUD text spacing detour.
        {
            m_Logger.Info("Detaching HUD text spacing detour...");

            ModManager::Get().GetDetourHookManager().Detach(m_DetourHudTextSpacing);

            m_Logger.Info("Detached HUD text spacing detour.");
        }
    }

    if (m_BugFixesFeatures.ReloadingVehicleAfterLeavingJunkyard)
    {
        // Remove reloading vehicle after leaving Junkyard patch.
        {
            m_Logger.Info("Removing reloading vehicle after leaving Junkyard patch...");

            m_PatchReloadingVehicleAfterLeavingJunkyard[0].Remove();
            m_PatchReloadingVehicleAfterLeavingJunkyard[1].Remove();

            m_Logger.Info("Removed reloading vehicle after leaving Junkyard patch.");
        }
    }
    
    if (m_BugFixesFeatures.UnknownLiveryVehicleIDs)
    {
        // Remove unknown livery vehicle IDs patch.
        {
            m_Logger.Info("Removing unknown livery vehicle IDs patch...");

            m_PatchUnknownLiveryVehicleIDs.Remove();

            m_Logger.Info("Removed unknown livery vehicle IDs patch.");
        }
    }
}

__declspec(naked) void BugFixes::DetourHudTextSpacing()
{
    __asm
    {
        mov eax, dword ptr [edi + 0x28]
        mov dword ptr [edi + 0x30], eax

        jmp dword ptr [QualityOfLife::s_Instance.m_BugFixes.m_DetourHudTextSpacing.Target]
    }
}
