#pragma once


#include <vector>
#include <Windows.h>

#include "core/Pointer.hpp"
#include "core/Logger.hpp"
#include "mod-manager/ModManagerApi.hpp"


class HookManager
{
public:
    using GameMainHook = void(*)();
    using GameStatePreWorldUpdateHook = void(*)(Core::Pointer gameEventQueue, Core::Pointer gameActionQueue);
    using GuiEventInterpreterUpdateHook = void(*)(Core::Pointer guiOutEventQueue);

public:
    HookManager(const Core::Logger& logger);
    HookManager(const HookManager&) = delete;
    HookManager(HookManager&&) = delete;
    ~HookManager();

    HookManager& operator =(const HookManager&) = delete;
    HookManager& operator =(HookManager&&) = delete;

public:
    MOD_MANAGER_API void AddGameMainHook(GameMainHook gameMainHook);
    MOD_MANAGER_API void AddGameStatePreWorldUpdateHook(GameStatePreWorldUpdateHook gameStatePreWorldUpdateHook);
    MOD_MANAGER_API void AddGuiEventInterpreterUpdateHook(GuiEventInterpreterUpdateHook guiEventInterpreterUpdateHook);

    void Load();

private:
    void ExecuteGameMainHooks();
    void ExecuteGameStatePreWorldUpdateHooks(Core::Pointer gameEventQueue, Core::Pointer gameActionQueue);
    void ExecuteGuiEventInterpreterUpdateHooks(Core::Pointer guiOutEventQueue);

private:
    static void Hook_ExecuteGameMainHooks();
    static void Hook_ExecuteGameStatePreWorldUpdateHooks();
    static void Hook_ExecuteGuiEventInterpreterUpdateHooks();

private:
    CRITICAL_SECTION m_CriticalSection = {};

    std::vector<GameMainHook> m_GameMainHooks;
    std::vector<GameStatePreWorldUpdateHook> m_GameStatePreWorldUpdateHooks;
    std::vector<GuiEventInterpreterUpdateHook> m_GuiEventInterpreterUpdateHooks;

    const Core::Logger& m_Logger;
};
