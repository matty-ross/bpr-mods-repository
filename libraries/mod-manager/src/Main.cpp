#include <Windows.h>

#include "mod-manager/ModManager.hpp"


BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD     fdwReason,
    _In_ LPVOID    lpvReserved
)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        ModManager::Get().Load();
        break;

    case DLL_PROCESS_DETACH:
        ModManager::Get().Unload();
        break;
    }

    return TRUE;
}
