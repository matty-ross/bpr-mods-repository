#pragma once


#include <vector>
#include <Windows.h>

#include "core/Pointer.hpp"
#include "core/Logger.hpp"
#include "mod-manager/ModManagerApi.hpp"


class HookManager
{
public:
    using UpdateHook = void(*)();
    using AddGameEventsHook = void(*)(Core::Pointer);
    using AddGameActionsHook = void(*)(Core::Pointer);

public:
    HookManager(const Core::Logger& logger);
    HookManager(const HookManager&) = delete;
    HookManager(HookManager&&) = delete;
    ~HookManager();

    HookManager& operator =(const HookManager&) = delete;
    HookManager& operator =(HookManager&&) = delete;

public:
    MOD_MANAGER_API void AddUpdateHook(UpdateHook updateHook);
    MOD_MANAGER_API void AddAddGameEventsHook(AddGameEventsHook addGameEventsHook);
    MOD_MANAGER_API void AddAddGameActionsHook(AddGameActionsHook addGameActionsHook);

    void Load();

private:
    void ExecuteUpdateHooks();
    void ExecuteAddGameEventsHooks(Core::Pointer gameEventQueue);
    void ExecuteAddGameActionsHooks(Core::Pointer gameActionQueue);

private:
    static void Hook_ExecuteUpdateHooks();
    //static void Hook_ExecuteAddGameEventsHooks();
    //static void Hook_ExecuteAddGameActionsHooks();

private:
    CRITICAL_SECTION m_CriticalSection = {};

    std::vector<UpdateHook> m_UpdateHooks;
    std::vector<AddGameEventsHook> m_AddGameEventsHooks;
    std::vector<AddGameActionsHook> m_AddGameActionsHooks;

    const Core::Logger& m_Logger;
};
