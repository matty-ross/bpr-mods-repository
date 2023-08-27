#include <Windows.h>

#include "ExceptionReporter.h"


ExceptionReporter* g_Mod = nullptr;


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
            g_Mod = new ExceptionReporter(hinstDLL);
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
            HANDLE currentThread = GetCurrentThread();
            g_Mod->OnThreadAttach(currentThread);
        }
        break;

    case DLL_THREAD_DETACH:
        {
            HANDLE currentThread = GetCurrentThread();
            g_Mod->OnThreadDetach(currentThread);
        }
        break;
    }

    return TRUE;
}