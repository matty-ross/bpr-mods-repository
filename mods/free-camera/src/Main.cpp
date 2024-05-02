#include <Windows.h>

#include "FreeCamera.hpp"


BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD     fdwReason,
    _In_ LPVOID    lpvReserved
)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        FreeCamera::Get().OnProcessAttach();
        break;

    case DLL_PROCESS_DETACH:
        FreeCamera::Get().OnProcessDetach();
        break;
    }

    return TRUE;
}
