#include "ExceptionReporter.hpp"

#include "mod-manager/ModManager.hpp"

#include "../resource.h"
#include "ExceptionInformation.hpp"


static constexpr char k_ModName[]    = "Exception Reporter";
static constexpr char k_ModVersion[] = "1.2.0";


ExceptionReporter ExceptionReporter::s_Instance;


ExceptionReporter::ExceptionReporter()
    :
    m_Logger(k_ModName)
{
}

ExceptionReporter& ExceptionReporter::Get()
{
    return s_Instance;
}

void ExceptionReporter::OnProcessAttach(HINSTANCE instanceHandle)
{
    m_InstanceHandle = instanceHandle;

    PTHREAD_START_ROUTINE loadThreadProc = [](LPVOID lpThreadParameter) -> DWORD
    {
        s_Instance.Load();
        return 0;
    };
    m_LoadThreadHandle = CreateThread(nullptr, 0, loadThreadProc, nullptr, 0, nullptr);
}

void ExceptionReporter::OnProcessDetach()
{
    Unload();
    CloseHandle(m_LoadThreadHandle);
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
            }
            return TRUE;

        case WM_CLOSE:
            EndDialog(hDlg, 0);
            return TRUE;
        }

        return FALSE;
    };

    ExceptionInformation exceptionInformation(ExceptionInfo->ExceptionRecord, ExceptionInfo->ContextRecord);

    DialogBoxParamA(
        m_InstanceHandle,
        MAKEINTRESOURCEA(IDD_DIALOG_EXCEPTION_REPORT),
        NULL,
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

        // Check versions.
        {
            m_Logger.Info("Checking versions...");

            if (strcmp(ModManager::GetVersion(), k_ModVersion) != 0)
            {
                throw std::exception("Mod Manager and Mod versions mismatch.");
            }

            m_Logger.Info("Checked versions.");
        }

        // Wait to enter WinMain.
        {
            m_Logger.Info("Waiting to enter WinMain...");
            
            while (true)
            {
                HANDLE exeMutexHandle = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, "BurnoutParadiseexe");
                if (exeMutexHandle != NULL)
                {
                    break;
                }

                Sleep(100);
            }

            m_Logger.Info("Entered WinMain.");
        }

        // Set top level exception filter.
        {
            m_Logger.Info("Setting top level exception filter...");
            
            PTOP_LEVEL_EXCEPTION_FILTER topLevelExceptionFilter = [](EXCEPTION_POINTERS* ExceptionInfo) -> LONG
            {
                return s_Instance.OnException(ExceptionInfo);
            };
            m_PreviousTopLevelExceptionFilter = SetUnhandledExceptionFilter(topLevelExceptionFilter);
            
            m_Logger.Info("Set top level exception filter. Previous filter: 0x%p.", m_PreviousTopLevelExceptionFilter);
        }

        m_Logger.Info("Loaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(NULL, e.what(), k_ModName, MB_ICONERROR);
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
        MessageBoxA(NULL, e.what(), k_ModName, MB_ICONERROR);
    }
}
