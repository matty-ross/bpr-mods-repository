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
        ExceptionReporter::Get().Load(hinstDLL);
        break;
    }

    return TRUE;
}
