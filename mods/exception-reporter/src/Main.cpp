#include <Windows.h>

#include "ExceptionReporter.hpp"


BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD     fdwReason,
    _In_ LPVOID    lpvReserved
)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        ExceptionReporter::Get().OnProcessAttach(hinstDLL);
        break;

    case DLL_PROCESS_DETACH:
        ExceptionReporter::Get().OnProcessDetach();
        break;
    }

    return TRUE;
}
