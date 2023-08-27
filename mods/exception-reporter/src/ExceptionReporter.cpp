#include "ExceptionReporter.h"

#include "core/Pointer.h"

#include "../resource.h"
#include "ExceptionInformation.h"


extern ExceptionReporter* g_Mod;


ExceptionReporter::ExceptionReporter(HMODULE module)
    :
    Mod(module)
{
}

void ExceptionReporter::OnProcessAttach()
{
    PTHREAD_START_ROUTINE loadThreadProc = [](LPVOID lpParameter) -> DWORD
    {
        static_cast<ExceptionReporter*>(lpParameter)->Load();
        return 0;
    };
    m_LoadThread = CreateThread(nullptr, 0, loadThreadProc, this, 0, nullptr);
}

void ExceptionReporter::OnProcessDetach()
{
    WaitForSingleObject(m_LoadThread, 5000);
    CloseHandle(m_LoadThread);

    Unload();
}

void ExceptionReporter::OnThreadAttach(HANDLE thread)
{
}

void ExceptionReporter::OnThreadDetach(HANDLE thread)
{
}

void ExceptionReporter::Load()
{
    auto hasEnteredWinMain = []() -> bool
    {
        HANDLE mutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, "BurnoutParadiseexe");
        return mutex != nullptr;
    };


    while (!hasEnteredWinMain())
    {
        Sleep(1000);
    }

    PTOP_LEVEL_EXCEPTION_FILTER topLevelExceptionFilter = [](EXCEPTION_POINTERS* ExceptionInfo) -> LONG
    {
        return g_Mod->OnException(ExceptionInfo);
    };
    m_PreviousTopLevelExceptionFilter = SetUnhandledExceptionFilter(topLevelExceptionFilter);
}

void ExceptionReporter::Unload()
{
    SetUnhandledExceptionFilter(m_PreviousTopLevelExceptionFilter);
}

LONG ExceptionReporter::OnException(EXCEPTION_POINTERS* ExceptionInfo) const
{
    DLGPROC dialogProc = [](HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam) -> INT_PTR
    {
        switch (Msg)
        {
        case WM_INITDIALOG:
            {
                const ExceptionInformation* exceptionInformation = reinterpret_cast<ExceptionInformation*>(lParam);
                SetDlgItemTextA(hDlg, IDC_VALUE_CODE, exceptionInformation->GetCode().c_str());
                SetDlgItemTextA(hDlg, IDC_VALUE_ADDRESS, exceptionInformation->GetAddress().c_str());
                SetDlgItemTextA(hDlg, IDC_VALUE_FLAGS, exceptionInformation->GetFlags().c_str());
                SetDlgItemTextA(hDlg, IDC_VALUE_PARAMETERS, exceptionInformation->GetParameters().c_str());
                SetDlgItemTextA(hDlg, IDC_VALUE_REGISTERS, exceptionInformation->GetRegisters().c_str());
                SetDlgItemTextA(hDlg, IDC_VALUE_STACK_TRACE, exceptionInformation->GetStackTrace().c_str());
                return TRUE;
            }

        case WM_CLOSE:
            {
                EndDialog(hDlg, 0);
                return TRUE;
            }
        }

        return FALSE;
    };


    ExceptionInformation exceptionInformation(ExceptionInfo->ExceptionRecord, ExceptionInfo->ContextRecord);

    HWND parentWindowHandle = Core::Pointer(0x0139815C).as<HWND>();
    DialogBoxParamA(
        GetModule(),
        MAKEINTRESOURCEA(IDD_DIALOG_EXCEPTION_REPORT),
        parentWindowHandle,
        dialogProc,
        reinterpret_cast<LPARAM>(&exceptionInformation)
    );

    return m_PreviousTopLevelExceptionFilter != nullptr ? m_PreviousTopLevelExceptionFilter(ExceptionInfo) : EXCEPTION_CONTINUE_SEARCH;
}