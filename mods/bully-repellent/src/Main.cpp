#include <Windows.h>

#include "BullyRepellent.h"


BullyRepellent* g_Mod = nullptr;


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
            g_Mod = new BullyRepellent(hinstDLL);
            g_Mod->OnProcessAttach();
        }
        break;

    case DLL_PROCESS_DETACH:
        {
            g_Mod->OnProcessDetach();
            delete g_Mod;
        }
        break;

    case DLL_THREAD_ATTACH:
        {
            g_Mod->OnThreadAttach();
        }
        break;

    case DLL_THREAD_DETACH:
        {
            g_Mod->OnThreadDetach();
        }
        break;
    }

    return TRUE;
}