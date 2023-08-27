#include "DetourHook.h"

#include <Windows.h>

#include "detours/detours.h"

#include "ModManager.h"
#include "DetourHookManager.h"


static constexpr size_t k_StubSize = 12;


DetourHook::DetourHook(Core::Pointer hookAddress, const void* detourFunction)
    :
    m_HookAddress(hookAddress.GetAddress()),
    m_DetourFunction(detourFunction)
{
    m_Stub = VirtualAlloc(nullptr, k_StubSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    {
        // call dword ptr ds:[m_DetourFunction]
        m_Stub.at(0x0).as<uint8_t>() = 0xFF;
        m_Stub.at(0x1).as<uint8_t>() = 0x15;
        m_Stub.at(0x2).as<void*>() = &m_DetourFunction;

        // jmp dword ptr ds:[m_HookAddress]
        m_Stub.at(0x6).as<uint8_t>() = 0xFF;
        m_Stub.at(0x7).as<uint8_t>() = 0x25;
        m_Stub.at(0x8).as<void*>() = &m_HookAddress;
    }

    DWORD oldProtection = 0;
    VirtualProtect(m_Stub.GetAddress(), k_StubSize, PAGE_EXECUTE_READ, &oldProtection);
}

DetourHook::~DetourHook()
{
    VirtualFree(m_Stub.GetAddress(), 0, MEM_RELEASE);
}

void DetourHook::Attach()
{
    DetourHookManager& manager = ModManager::Get().GetDetourHookManager();

    manager.BeginTransaction();
    DetourAttach(&m_HookAddress, m_Stub.GetAddress());
    manager.EndTransaction();
    
    m_Attached = true;
}

void DetourHook::Detach()
{
    DetourHookManager& manager = ModManager::Get().GetDetourHookManager();
    
    manager.BeginTransaction();
    DetourDetach(&m_HookAddress, m_Stub.GetAddress());
    manager.EndTransaction();

    m_Attached = false;
}

bool DetourHook::IsAttached() const
{
    return m_Attached;
}