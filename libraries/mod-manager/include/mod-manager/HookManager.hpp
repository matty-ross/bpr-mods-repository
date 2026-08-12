#pragma once


#include <vector>
#include <Windows.h>

#include "core/Pointer.hpp"
#include "core/Logger.hpp"
#include "mod-manager/ModManagerApi.hpp"


class HookManager
{
public:
    using GameStatePreWorldUpdateHook = void(*)(Core::Pointer gameEventQueue, Core::Pointer gameActionQueue);

public:
    HookManager(const Core::Logger& logger);
    HookManager(const HookManager&) = delete;
    HookManager(HookManager&&) = delete;
    ~HookManager();

    HookManager& operator =(const HookManager&) = delete;
    HookManager& operator =(HookManager&&) = delete;

public:
    MOD_MANAGER_API void AddGameStatePreWorldUpdateHook(GameStatePreWorldUpdateHook gameStatePreWorldUpdateHook);

    void Load();

private:
    void ExecuteGameStatePreWorldUpdateHooks(Core::Pointer gameEventQueue, Core::Pointer gameActionQueue);

private:
    static void Hook_ExecuteGameStatePreWorldUpdateHooks();

private:
    CRITICAL_SECTION m_CriticalSection = {};

    std::vector<GameStatePreWorldUpdateHook> m_GameStatePreWorldUpdateHooks;

    const Core::Logger& m_Logger;
};
