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

void HookManager::AddGameMainHook(GameMainHook gameMainHook)
{
    EnterCriticalSection(&m_CriticalSection);

    m_GameMainHooks.push_back(gameMainHook);
    m_Logger.Info("Added 'game main' hook. address: 0x%p", gameMainHook);

    LeaveCriticalSection(&m_CriticalSection);
}

void HookManager::AddGameStatePreWorldUpdateHook(GameStatePreWorldUpdateHook gameStatePreWorldUpdateHook)
{
    EnterCriticalSection(&m_CriticalSection);

    m_GameStatePreWorldUpdateHooks.push_back(gameStatePreWorldUpdateHook);
    m_Logger.Info("Added 'game state pre world update' hook. address: 0x%p", gameStatePreWorldUpdateHook);

    LeaveCriticalSection(&m_CriticalSection);
}

void HookManager::Load()
{
    Core::Patch(0x070533C4, 7, m_Logger).WriteJMP(Hook_ExecuteGameMainHooks);
    Core::Patch(0x00A2A509, 9, m_Logger).WriteJMP(Hook_ExecuteGameStatePreWorldUpdateHooks);

    m_Logger.Info("Loaded hook manager.");
}

void HookManager::ExecuteGameMainHooks()
{
    EnterCriticalSection(&m_CriticalSection);

    for (GameMainHook gameMainHook : m_GameMainHooks)
    {
        gameMainHook();
    }

    LeaveCriticalSection(&m_CriticalSection);
}

void HookManager::ExecuteGameStatePreWorldUpdateHooks(
    Core::Pointer gameEventQueue, // BrnGameState::GameStateModuleIO::GameEventQueue*
    Core::Pointer gameActionQueue // BrnGameState::GameStateModuleIO::BaseGameActionQueue<13312>*
)
{
    EnterCriticalSection(&m_CriticalSection);

    for (GameStatePreWorldUpdateHook gameStatePreWorldUpdateHook : m_GameStatePreWorldUpdateHooks)
    {
        gameStatePreWorldUpdateHook(gameEventQueue, gameActionQueue);
    }

    LeaveCriticalSection(&m_CriticalSection);
}

__declspec(naked) void HookManager::Hook_ExecuteGameMainHooks()
{
    /*
        bool __thiscall BrnGame::BrnGameModule::GameMain()
    */

    __asm
    {
        pushfd
        pushad

        mov ecx, offset ModManager::s_Instance.m_HookManager
        call HookManager::ExecuteGameMainHooks

        popad
        popfd

        // Original code.
        cmp byte ptr [esi + 0xB6D9B8], 0

        // Jump back.
        push 0x070533CB
        ret
    }
}

__declspec(naked) void HookManager::Hook_ExecuteGameStatePreWorldUpdateHooks()
{
    /*
        void __thiscall BrnGameState::GameStateModule::PreWorldUpdate(
            CgsModule::IOBufferStack* lpInputBufferStack,
            CgsModule::IOBufferStack* lpOutputBufferStack,
            const BrnGameState::GameStateModuleIO::PreWorldInputBuffer* lpInput,
            BrnGameState::GameStateModuleIO::OutputBuffer* lpOutput,
            BrnUpdateSet lUpdateSet
        )
    */

    __asm
    {
        // ebp - 0x1588: BrnGameState::GameStateModuleIO::GameEventQueue lGameEventQueue
        // ebp - 0x14: BrnGameState::GameStateModuleIO::BaseGameActionQueue<13312>* lpActionQueue

        pushfd
        pushad

        // &lGameEventQueue
        lea eax, [ebp - 0x1588]

        push dword ptr [ebp - 0x14]
        push eax
        mov ecx, offset ModManager::s_Instance.m_HookManager
        call HookManager::ExecuteGameStatePreWorldUpdateHooks

        popad
        popfd

        // Original code.
        mov esi, dword ptr [ebp - 0x14]
        lea eax, [ebp - 0x1588]

        // Jump back.
        push 0x00A2A512
        ret
    }
}
