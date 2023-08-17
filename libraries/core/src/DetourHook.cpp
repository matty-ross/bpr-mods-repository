#include "DetourHook.h"

#include <Windows.h>

#include "vendor/detours/src/detours.h"


namespace Core
{
    DetourHook::DetourHook(Pointer hookAddress, const void* detourFunction)
        :
        m_HookAddress(hookAddress.GetAddress()),
        m_DetourFunction(detourFunction)
    {
        constexpr size_t stubSize = 12;

        m_Stub = VirtualAlloc(nullptr, stubSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

        {
            // call dword ptr ds:[m_Function]
            m_Stub.at(0x0).as<uint8_t>() = 0xFF;
            m_Stub.at(0x1).as<uint8_t>() = 0x15;
            m_Stub.at(0x2).as<void*>() = &m_DetourFunction;

            // jmp dword ptr ds:[m_Address]
            m_Stub.at(0x6).as<uint8_t>() = 0xFF;
            m_Stub.at(0x7).as<uint8_t>() = 0x25;
            m_Stub.at(0x8).as<void*>() = &m_HookAddress;
        }

        DWORD oldProtection = 0;
        VirtualProtect(m_Stub.GetAddress(), stubSize, PAGE_EXECUTE_READ, &oldProtection);
    }

    DetourHook::~DetourHook()
    {
        VirtualFree(m_Stub.GetAddress(), 0, MEM_RELEASE);
    }

    void DetourHook::Attach()
    {
        DetourTransactionBegin();
        DetourAttach(&m_HookAddress, m_Stub.GetAddress());
        DetourTransactionCommit();
        
        m_Attached = true;
    }

    void DetourHook::Detach()
    {
        DetourTransactionBegin();
        DetourDetach(&m_HookAddress, m_Stub.GetAddress());
        DetourTransactionCommit();

        m_Attached = false;
    }

    bool DetourHook::IsAttached() const
    {
        return m_Attached;
    }
}