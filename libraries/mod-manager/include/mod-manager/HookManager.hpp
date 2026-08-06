#pragma once


#include <vector>
#include <Windows.h>

#include "core/Pointer.hpp"
#include "mod-manager/ModManagerApi.hpp"


class HookManager
{
public:
    using UpdateHook = void(*)();
    using AddGameEventsHook = void(*)(Core::Pointer);
    using AddGameActionsHook = void(*)(Core::Pointer);

public:
    HookManager();
    HookManager(const HookManager&) = delete;
    HookManager(HookManager&&) = delete;
    ~HookManager();

    HookManager& operator =(const HookManager&) = delete;
    HookManager& operator =(HookManager&&) = delete;

public:
    MOD_MANAGER_API void AddUpdateHook(UpdateHook updateHook);
    MOD_MANAGER_API void AddAddGameEventsHook(AddGameEventsHook addGameEventsHook);
    MOD_MANAGER_API void AddAddGameActionsHook(AddGameActionsHook addGameActionsHook);

    void ExecuteUpdateHooks();
    void ExecuteAddGameEventsHooks(Core::Pointer gameEventQueue);
    void ExecuteAddGameActionsHooks(Core::Pointer gameActionQueue);

private:
    CRITICAL_SECTION m_CriticalSection = {};

    std::vector<UpdateHook> m_UpdateHooks;
    std::vector<AddGameEventsHook> m_AddGameEventsHooks;
    std::vector<AddGameActionsHook> m_AddGameActionsHooks;
};
