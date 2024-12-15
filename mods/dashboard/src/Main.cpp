#include <Windows.h>

#include "Dashboard.hpp"


BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD     fdwReason,
    _In_ LPVOID    lpvReserved
)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        Dashboard::Get().OnProcessAttach();
        break;

    case DLL_PROCESS_DETACH:
        Dashboard::Get().OnProcessDetach();
        break;
    }

    return TRUE;
}
