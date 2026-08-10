#include <vector>
#include <Windows.h>

#include "core/Pointer.hpp"
#include "core/Logger.hpp"
#include "core/Patch.hpp"
#include "mod-manager/ModManager.hpp"
#include "mod-manager/HookManager.hpp"


HookManager::HookManager(const Core::Logger& logger)
    :
    m_Logger(logger)
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
    m_Logger.Info("Added 'update' hook. address: 0x%p", updateHook);

    LeaveCriticalSection(&m_CriticalSection);
}

void HookManager::AddAddGameEventsHook(AddGameEventsHook addGameEventsHook)
{
    EnterCriticalSection(&m_CriticalSection);

    m_AddGameEventsHooks.push_back(addGameEventsHook);
    m_Logger.Info("Added 'add game events' hook. address: 0x%p", addGameEventsHook);

    LeaveCriticalSection(&m_CriticalSection);
}

void HookManager::AddAddGameActionsHook(AddGameActionsHook addGameActionsHook)
{
    EnterCriticalSection(&m_CriticalSection);

    m_AddGameActionsHooks.push_back(addGameActionsHook);
    m_Logger.Info("Added 'add game actions' hook. address: 0x%p", addGameActionsHook);

    LeaveCriticalSection(&m_CriticalSection);
}

void HookManager::Load()
{
    Core::Patch(0x00A49235, 5, m_Logger).WriteJMP(Hook_ExecuteUpdateHooks);

    m_Logger.Info("Loaded hook manager.");
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

__declspec(naked) void HookManager::Hook_ExecuteUpdateHooks()
{
    __asm
    {
        pushfd
        pushad

        mov ecx, offset ModManager::s_Instance.m_HookManager
        call HookManager::ExecuteUpdateHooks

        popad
        popfd

        // Original code.
        mov eax, dword ptr ds:[0x013FC8E0]

        // Jump back.
        push 0x00A4923A
        ret
    }
}
