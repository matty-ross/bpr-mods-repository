#include "DetourHook.h"

#include <Windows.h>


namespace Core
{
    DetourHook::DetourHook(Pointer hookAddress, const void* detourFunction)
        :
        m_HookAddress(hookAddress),
        m_DetourFunction(detourFunction)
    {
        // TODO: allocate the stub
    }

    DetourHook::~DetourHook()
    {
        // TODO: free the stub
    }

    void DetourHook::Attach()
    {
        // TODO: attach the detour function
        
        m_Attached = true;
    }

    void DetourHook::Detach()
    {
        // TODO: detach the detour function

        m_Attached = false;
    }

    bool DetourHook::IsAttached() const
    {
        return m_Attached;
    }
}