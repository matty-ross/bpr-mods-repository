#include <Windows.h>

#include "ModManager.h"


ModManager* g_ModManager = nullptr;


BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD     fdwReason,
    _In_ LPVOID    lpvReserved
)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        {
            g_ModManager = new ModManager();
            g_ModManager->OnProcessAttach();
        }
        break;

    case DLL_PROCESS_DETACH:
        {
            g_ModManager->OnProcessDetach();
            delete g_ModManager;
        }
        break;

    case DLL_THREAD_ATTACH:
        {
            g_ModManager->OnThreadAttach();
        }
        break;

    case DLL_THREAD_DETACH:
        {
            g_ModManager->OnThreadDetach();
        }
        break;
    }

    return TRUE;
}