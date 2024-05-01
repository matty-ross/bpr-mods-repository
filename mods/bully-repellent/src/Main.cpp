#include <Windows.h>

#include "BullyRepellent.hpp"


BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD     fdwReason,
    _In_ LPVOID    lpvReserved
)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        BullyRepellent::Get().OnProcessAttach();
        break;

    case DLL_PROCESS_DETACH:
        BullyRepellent::Get().OnProcessDetach();
        break;
    }

    return TRUE;
}
