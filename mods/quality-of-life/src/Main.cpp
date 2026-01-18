#include <Windows.h>

#include "QualityOfLife.hpp"


BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD     fdwReason,
    _In_ LPVOID    lpvReserved
)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        QualityOfLife::Get().OnProcessAttach();
        break;

    case DLL_PROCESS_DETACH:
        QualityOfLife::Get().OnProcessDetach();
        break;
    }

    return TRUE;
}
