#include "DetourHook.h"

#include <Windows.h>

#include "detours/detours.h"

#include "core/Pointer.h"

#include "ModManager.h"
#include "DetourHookManager.h"


static constexpr size_t k_StubSize = 12;


DetourHook::DetourHook(void* hookAddress, const void* detourFunction)
    :
    m_HookAddress(hookAddress),
    m_DetourFunction(detourFunction)
{
    Core::Pointer stub = VirtualAlloc(nullptr, k_StubSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    {
        // call dword ptr ds:[m_DetourFunction]
        stub.at(0x0).as<uint8_t>() = 0xFF;
        stub.at(0x1).as<uint8_t>() = 0x15;
        stub.at(0x2).as<void*>() = &m_DetourFunction;

        // jmp dword ptr ds:[m_HookAddress]
        stub.at(0x6).as<uint8_t>() = 0xFF;
        stub.at(0x7).as<uint8_t>() = 0x25;
        stub.at(0x8).as<void*>() = &m_HookAddress;
    }
    m_Stub = stub.GetAddress();

    DWORD oldProtection = 0;
    VirtualProtect(m_Stub, k_StubSize, PAGE_EXECUTE_READ, &oldProtection);
}

DetourHook::~DetourHook()
{
    VirtualFree(m_Stub, 0, MEM_RELEASE);
}

void DetourHook::Attach()
{
    DetourHookManager& manager = ModManager::Get().GetDetourHookManager();

    manager.BeginTransaction();
    DetourAttach(&m_HookAddress, m_Stub);
    manager.EndTransaction();
    
    m_Attached = true;
}

void DetourHook::Detach()
{
    DetourHookManager& manager = ModManager::Get().GetDetourHookManager();
    
    manager.BeginTransaction();
    DetourDetach(&m_HookAddress, m_Stub);
    manager.EndTransaction();

    m_Attached = false;
}

bool DetourHook::IsAttached() const
{
    return m_Attached;
}