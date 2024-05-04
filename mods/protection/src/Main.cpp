#include <Windows.h>

#include "Protection.hpp"


BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD     fdwReason,
    _In_ LPVOID    lpvReserved
)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        Protection::Get().OnProcessAttach();
        break;

    case DLL_PROCESS_DETACH:
        Protection::Get().OnProcessDetach();
        break;
    }

    return TRUE;
}
