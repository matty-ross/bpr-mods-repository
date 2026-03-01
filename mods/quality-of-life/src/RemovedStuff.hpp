#pragma once


#include "core/Logger.hpp"
#include "mod-manager/DetourHookManager.hpp"


class RemovedStuff
{
public:
    RemovedStuff(const Core::Logger& logger);

public:
    void Load();
    void Unload();

private:
    static void DetourCopsAndIslandPlayerIcons();

private:
    const Core::Logger& m_Logger;

    DetourHook m_DetourCopsAndIslandPlayerIcons;
};
