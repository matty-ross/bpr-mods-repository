#include <Windows.h>

#include "ModManager.hpp"


BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD     fdwReason,
    _In_ LPVOID    lpvReserved
)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        ModManager::Get().OnProcessAttach();
        break;

    case DLL_PROCESS_DETACH:
        ModManager::Get().OnProcessDetach();
        break;
    }

    return TRUE;
}
