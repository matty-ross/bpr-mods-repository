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

void HookManager::AddRenderMenuHook(RenderMenuHook renderMenuHook)
{
    EnterCriticalSection(&m_CriticalSection);

    m_RenderMenuHooks.push_back(renderMenuHook);

    LeaveCriticalSection(&m_CriticalSection);
}

void HookManager::AddRenderOverlayHook(RenderOverlayHook renderOverlayHook)
{
    EnterCriticalSection(&m_CriticalSection);

    m_RenderOverlayHooks.push_back(renderOverlayHook);

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

void HookManager::ExecuteUpdateHooks(Core::Pointer gameModule)
{
    EnterCriticalSection(&m_CriticalSection);

    for (UpdateHook updateHook : m_UpdateHooks)
    {
        updateHook(gameModule);
    }

    LeaveCriticalSection(&m_CriticalSection);
}

void HookManager::ExecuteRenderMenuHooks(Core::Pointer gameModule)
{
    EnterCriticalSection(&m_CriticalSection);

    for (RenderMenuHook renderMenuHook : m_RenderMenuHooks)
    {
        renderMenuHook(gameModule);
    }

    LeaveCriticalSection(&m_CriticalSection);
}

void HookManager::ExecuteRenderOverlayHooks(Core::Pointer gameModule)
{
    EnterCriticalSection(&m_CriticalSection);

    for (RenderOverlayHook renderOverlayHook : m_RenderOverlayHooks)
    {
        renderOverlayHook(gameModule);
    }

    LeaveCriticalSection(&m_CriticalSection);
}

void HookManager::ExecuteAddGameEventsHooks(Core::Pointer gameModule, Core::Pointer gameEventQueue)
{
    EnterCriticalSection(&m_CriticalSection);

    for (AddGameEventsHook addGameEventsHook : m_AddGameEventsHooks)
    {
        addGameEventsHook(gameModule, gameEventQueue);
    }

    LeaveCriticalSection(&m_CriticalSection);
}

void HookManager::ExecuteAddGameActionsHooks(Core::Pointer gameModule, Core::Pointer gameActionQueue)
{
    EnterCriticalSection(&m_CriticalSection);

    for (AddGameActionsHook addGameActionsHook : m_AddGameActionsHooks)
    {
        addGameActionsHook(gameModule, gameActionQueue);
    }

    LeaveCriticalSection(&m_CriticalSection);
}
