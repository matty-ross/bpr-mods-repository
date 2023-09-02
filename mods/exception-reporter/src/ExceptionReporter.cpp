#include "ExceptionReporter.h"

#include "../resource.h"
#include "ExceptionInformation.h"


extern ExceptionReporter* g_Mod;


static constexpr char k_ModName[] = "Exception Reporter";


ExceptionReporter::ExceptionReporter(HMODULE module)
    :
    Mod(module),
    m_Logger(k_ModName)
{
}

void ExceptionReporter::OnProcessAttach()
{
    PTHREAD_START_ROUTINE loadThreadProc = [](LPVOID lpThreadParameter) -> DWORD
    {
        static_cast<ExceptionReporter*>(lpThreadParameter)->Load();
        return 0;
    };
    m_LoadThread = CreateThread(nullptr, 0, loadThreadProc, this, 0, nullptr);
}

void ExceptionReporter::OnProcessDetach()
{
    CloseHandle(m_LoadThread);
    Unload();
}

void ExceptionReporter::OnThreadAttach()
{
}

void ExceptionReporter::OnThreadDetach()
{
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

    DialogBoxParamA(
        GetModule(),
        MAKEINTRESOURCEA(IDD_DIALOG_EXCEPTION_REPORT),
        nullptr,
        dialogProc,
        reinterpret_cast<LPARAM>(&exceptionInformation)
    );

    return m_PreviousTopLevelExceptionFilter != nullptr ? m_PreviousTopLevelExceptionFilter(ExceptionInfo) : EXCEPTION_CONTINUE_SEARCH;
}

void ExceptionReporter::Load()
{
    try
    {
        m_Logger.Info("Loading...");

        // Wait to enter WinMain.
        {
            m_Logger.Info("Waiting to enter WinMain...");
            
            auto hasEnteredWinMain = []() -> bool
            {
                HANDLE mutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, "BurnoutParadiseexe");
                return mutex != nullptr;
            };

            while (!hasEnteredWinMain())
            {
                Sleep(100);
            }

            m_Logger.Info("Entered WinMain.");
        }

        // Set top level exception filter.
        {
            m_Logger.Info("Setting top level exception filter...");
            
            PTOP_LEVEL_EXCEPTION_FILTER topLevelExceptionFilter = [](EXCEPTION_POINTERS* ExceptionInfo) -> LONG
            {
                return g_Mod->OnException(ExceptionInfo);
            };
            m_PreviousTopLevelExceptionFilter = SetUnhandledExceptionFilter(topLevelExceptionFilter);
            
            m_Logger.Info("Set top level exception filter. Previous filter: 0x%p.", m_PreviousTopLevelExceptionFilter);
        }

        m_Logger.Info("Loaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(nullptr, e.what(), k_ModName, MB_ICONERROR);
    }
}

void ExceptionReporter::Unload()
{
    try
    {
        m_Logger.Info("Unloading...");

        // Set previous top level exception filter.
        {
            m_Logger.Info("Setting previous top level exception filter...");
            
            SetUnhandledExceptionFilter(m_PreviousTopLevelExceptionFilter);
            
            m_Logger.Info("Set previous top level exception filter.");
        }

        m_Logger.Info("Unloaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(nullptr, e.what(), k_ModName, MB_ICONERROR);
    }
}