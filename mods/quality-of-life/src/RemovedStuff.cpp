#include "RemovedStuff.hpp"

#include "core/Pointer.hpp"
#include "mod-manager/ModManager.hpp"

#include "QualityOfLife.hpp"


RemovedStuff::RemovedStuff(const Core::Logger& logger)
    :
    m_Logger(logger),
    m_DetourCopsAndIslandPlayerIcons
    {
        .Target = Core::Pointer(0x00A843AE).GetAddress(),
        .Detour = &RemovedStuff::DetourCopsAndIslandPlayerIcons,
    },
    m_PatchCarAchievements
    {
        Core::Patch(Core::Pointer(0x076ACCA3).GetAddress(), 9),
        Core::Patch(Core::Pointer(0x032EB8FD).GetAddress(), 2),
    }
{
}

void RemovedStuff::Load()
{
    // Attach Cops and Island player icons detour.
    {
        m_Logger.Info("Attaching Cops and Island player icons detour...");

        ModManager::Get().GetDetourHookManager().Attach(m_DetourCopsAndIslandPlayerIcons);

        m_Logger.Info("Attached Cops and Island player icons detour.");
    }

    // Apply car achievements patch.
    {
        m_Logger.Info("Applying car achievements patch...");

        m_PatchCarAchievements[0].Apply("\x90\x90\x90\x90\x90\x90\x90\x90\x90");
        m_PatchCarAchievements[1].Apply("\xEB\x0A");

        m_Logger.Info("Applied car achievements patch.");
    }
}

void RemovedStuff::Unload()
{
    // Remove car achievements patch.
    {
        m_Logger.Info("Removing car achievements patch...");

        m_PatchCarAchievements[0].Remove();
        m_PatchCarAchievements[1].Remove();

        m_Logger.Info("Removed car achievements patch.");
    }

    // Detach Cops and Island player icons detour.
    {
        m_Logger.Info("Detaching Cops and Island player icons detour...");

        ModManager::Get().GetDetourHookManager().Detach(m_DetourCopsAndIslandPlayerIcons);

        m_Logger.Info("Detached Cops and Island player icons detour.");
    }
}

__declspec(naked) void RemovedStuff::DetourCopsAndIslandPlayerIcons()
{
    __asm
    {
        mov eax, edi
        shl eax, 6
        mov dword ptr [ebx + 0xE68 + eax], 4

        jmp dword ptr [QualityOfLife::s_Instance.m_RemovedStuff.m_DetourCopsAndIslandPlayerIcons.Target]
    }
}
