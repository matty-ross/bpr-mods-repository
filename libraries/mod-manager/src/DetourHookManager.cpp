#include "DetourHookManager.h"

#include "detours.h"

#include "core/Pointer.h"


// https://github.com/microsoft/Detours/wiki/Using-Detours


static void CreateStubFunction(DetourHook& detourHook)
{
    constexpr size_t stubSize = 12;

    Core::Pointer stub = VirtualAlloc(nullptr, stubSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    {
        // call dword ptr ds:[DetourFunction]
        stub.at(0x0).as<uint8_t>() = 0xFF;
        stub.at(0x1).as<uint8_t>() = 0x15;
        stub.at(0x2).as<void*>() = &detourHook.DetourFunction;

        // jmp dword ptr ds:[HookAddress]
        stub.at(0x6).as<uint8_t>() = 0xFF;
        stub.at(0x7).as<uint8_t>() = 0x25;
        stub.at(0x8).as<void*>() = &detourHook.HookAddress;
    }

    DWORD oldProtection = 0;
    VirtualProtect(stub.GetAddress(), stubSize, PAGE_EXECUTE_READ, &oldProtection);

    detourHook.StubFunction = stub.GetAddress();
}

static void DestroyStubFunction(DetourHook& detourHook)
{
    VirtualFree(detourHook.StubFunction, 0, MEM_RELEASE);

    detourHook.StubFunction = nullptr;
}


DetourHookManager::DetourHookManager()
{
    InitializeCriticalSection(&m_TransactionCriticalSection);
}

DetourHookManager::~DetourHookManager()
{
    DeleteCriticalSection(&m_TransactionCriticalSection);
}

void DetourHookManager::AttachDetourHook(DetourHook& detourHook, bool createStubFunction)
{
    if (createStubFunction)
    {
        CreateStubFunction(detourHook);
    }

    BeginTransaction();
    DetourAttach(&detourHook.HookAddress, detourHook.StubFunction);
    EndTransaction();
}

void DetourHookManager::DetachDetourHook(DetourHook& detourHook, bool destroyStubFunction)
{
    BeginTransaction();
    DetourDetach(&detourHook.HookAddress, detourHook.StubFunction);
    EndTransaction();

    if (destroyStubFunction)
    {
        DestroyStubFunction(detourHook);
    }
}

void DetourHookManager::BeginTransaction()
{
    EnterCriticalSection(&m_TransactionCriticalSection);
    m_TransactionInProgress = true;

    DetourTransactionBegin();
}

void DetourHookManager::EndTransaction()
{
    DetourTransactionCommit();

    m_TransactionInProgress = false;
    LeaveCriticalSection(&m_TransactionCriticalSection);
}

bool DetourHookManager::IsTransactionInProgress() const
{
    return m_TransactionInProgress;
}