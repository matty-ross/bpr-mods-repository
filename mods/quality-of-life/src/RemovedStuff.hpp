#pragma once


#include "core/Logger.hpp"
#include "core/Patch.hpp"
#include "mod-manager/DetourHookManager.hpp"

#include "FeaturesFile.hpp"


class RemovedStuff
{
public:
    RemovedStuff(const Core::Logger& logger, RemovedStuffFeatures& removedStuffFeatures);

public:
    void Load();
    void Unload();

private:
    static void DetourCopsAndIslandPlayerIcons();
    static void DetourCrashCameraEffect();

private:
    const Core::Logger& m_Logger;
    RemovedStuffFeatures& m_RemovedStuffFeatures;

    DetourHook m_DetourCopsAndIslandPlayerIcons;
    Core::Patch m_PatchCarAchievements[2];
    DetourHook m_DetourCrashCameraEffect;
};
