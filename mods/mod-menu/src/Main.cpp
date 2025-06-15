#include <Windows.h>

#include "ModMenu.hpp"


BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD     fdwReason,
    _In_ LPVOID    lpvReserved
)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        ModMenu::Get().OnProcessAttach();
        break;

    case DLL_PROCESS_DETACH:
        ModMenu::Get().OnProcessDetach();
        break;
    }

    return TRUE;
}
