#include "RemovedStuff.hpp"

#include "core/Pointer.hpp"
#include "mod-manager/ModManager.hpp"

#include "QualityOfLife.hpp"


RemovedStuff::RemovedStuff(const Core::Logger& logger, RemovedStuffFeatures& removedStuffFeatures)
    :
    m_Logger(logger),
    m_RemovedStuffFeatures(removedStuffFeatures),
    m_DetourCopsAndIslandPlayerIcons
    {
        .Target = Core::Pointer(0x00A843AE).GetAddress(),
        .Detour = &RemovedStuff::DetourCopsAndIslandPlayerIcons,
    },
    m_PatchCarAchievements
    {
        Core::Patch(Core::Pointer(0x076ACCA3).GetAddress(), 9),
        Core::Patch(Core::Pointer(0x032EB8FD).GetAddress(), 2),
    },
    m_DetourCrashCameraEffect
    {
        .Target = Core::Pointer(0x0096F880).GetAddress(),
        .Detour = &RemovedStuff::DetourCrashCameraEffect,
    }
{
}

void RemovedStuff::Load()
{
    if (m_RemovedStuffFeatures.CopsAndIslandPlayerIcons)
    {
        // Attach Cops and Island player icons detour.
        {
            m_Logger.Info("Attaching Cops and Island player icons detour...");

            ModManager::Get().GetDetourHookManager().Attach(m_DetourCopsAndIslandPlayerIcons);

            m_Logger.Info("Attached Cops and Island player icons detour.");
        }
    }

    if (m_RemovedStuffFeatures.CarAchievements)
    {
        // Apply car achievements patch.
        {
            m_Logger.Info("Applying car achievements patch...");

            m_PatchCarAchievements[0].Apply("\x90\x90\x90\x90\x90\x90\x90\x90\x90");
            m_PatchCarAchievements[1].Apply("\xEB\x0A");

            m_Logger.Info("Applied car achievements patch.");
        }
    }

    if (m_RemovedStuffFeatures.CrashCameraEffect)
    {
        // Attach crash camera effect detour.
        {
            m_Logger.Info("Attaching crash camera effect detour...");

            ModManager::Get().GetDetourHookManager().Attach(m_DetourCrashCameraEffect);

            m_Logger.Info("Attached crash camera effect detour.");
        }
    }
}

void RemovedStuff::Unload()
{
    if (m_RemovedStuffFeatures.CrashCameraEffect)
    {
        // Detach crash camera effect detour.
        {
            m_Logger.Info("Detaching crash camera effect detour...");

            ModManager::Get().GetDetourHookManager().Detach(m_DetourCrashCameraEffect);

            m_Logger.Info("Detached crash camera effect detour.");
        }
    }

    if (m_RemovedStuffFeatures.CarAchievements)
    {
        // Remove car achievements patch.
        {
            m_Logger.Info("Removing car achievements patch...");

            m_PatchCarAchievements[0].Remove();
            m_PatchCarAchievements[1].Remove();

            m_Logger.Info("Removed car achievements patch.");
        }
    }

    if (m_RemovedStuffFeatures.CopsAndIslandPlayerIcons)
    {
        // Detach Cops and Island player icons detour.
        {
            m_Logger.Info("Detaching Cops and Island player icons detour...");

            ModManager::Get().GetDetourHookManager().Detach(m_DetourCopsAndIslandPlayerIcons);

            m_Logger.Info("Detached Cops and Island player icons detour.");
        }
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

__declspec(naked) void RemovedStuff::DetourCrashCameraEffect()
{
    static constexpr char wrecked[] = "Wrecked";
    static constexpr char crash[] = "Crash";
    static constexpr float amount = 1.0f;

    __asm
    {
        pushfd
        pushad

        mov eax, offset wrecked
        mov edx, offset crash
        cmove eax, edx

        movss xmm3, [amount]
        mov edx, dword ptr [edi + 0x30]
        push eax
        lea ecx, [esi + 0x10]
        mov eax, 0x004D2700
        call eax // BrnDirector::Camera::EnsureEffectIsPlaying
        add esp, 4

        popad
        popfd

        jmp dword ptr [QualityOfLife::s_Instance.m_RemovedStuff.m_DetourCrashCameraEffect.Target]
    }
}
