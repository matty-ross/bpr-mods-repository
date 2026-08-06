#include <vector>
#include <Windows.h>

#include "core/Pointer.hpp"
#include "mod-manager/HookManager.hpp"


HookManager::HookManager()
{
    InitializeCriticalSection(&m_CriticalSection);
}

HookManager::~HookManager()
{
    DeleteCriticalSection(&m_CriticalSection);
}

void HookManager::AddUpdateHook(UpdateHook updateHook)
{
    EnterCriticalSection(&m_CriticalSection);

    m_UpdateHooks.push_back(updateHook);

    LeaveCriticalSection(&m_CriticalSection);
}

void HookManager::AddAddGameEventsHook(AddGameEventsHook addGameEventsHook)
{
    EnterCriticalSection(&m_CriticalSection);

    m_AddGameEventsHooks.push_back(addGameEventsHook);

    LeaveCriticalSection(&m_CriticalSection);
}

void HookManager::AddAddGameActionsHook(AddGameActionsHook addGameActionsHook)
{
    EnterCriticalSection(&m_CriticalSection);

    m_AddGameActionsHooks.push_back(addGameActionsHook);

    LeaveCriticalSection(&m_CriticalSection);
}

void HookManager::ExecuteUpdateHooks()
{
    EnterCriticalSection(&m_CriticalSection);

    for (UpdateHook updateHook : m_UpdateHooks)
    {
        updateHook();
    }

    LeaveCriticalSection(&m_CriticalSection);
}

void HookManager::ExecuteAddGameEventsHooks(Core::Pointer gameEventQueue)
{
    EnterCriticalSection(&m_CriticalSection);

    for (AddGameEventsHook addGameEventsHook : m_AddGameEventsHooks)
    {
        addGameEventsHook(gameEventQueue);
    }

    LeaveCriticalSection(&m_CriticalSection);
}

void HookManager::ExecuteAddGameActionsHooks(Core::Pointer gameActionQueue)
{
    EnterCriticalSection(&m_CriticalSection);

    for (AddGameActionsHook addGameActionsHook : m_AddGameActionsHooks)
    {
        addGameActionsHook(gameActionQueue);
    }

    LeaveCriticalSection(&m_CriticalSection);
}
