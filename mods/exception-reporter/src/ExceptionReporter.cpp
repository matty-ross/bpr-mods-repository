#include <exception>
#include <Windows.h>

#include "mod-manager/ModManager.hpp"

#include "../resource.h"
#include "ExceptionReporter.hpp"
#include "ExceptionInformation.hpp"


ExceptionReporter ExceptionReporter::s_Instance;


ExceptionReporter::ExceptionReporter()
    :
    m_Logger(k_Name)
{
}

ExceptionReporter& ExceptionReporter::Get()
{
    return s_Instance;
}

void ExceptionReporter::Load(HINSTANCE instanceHandle)
{
    try
    {
        m_InstanceHandle = instanceHandle;

        if (!ModManager::Get().CheckVersion(k_Version))
        {
            throw std::exception("Mod Manager and Mod versions mismatch.");
        }

        auto deferredLoadThreadProc = [](LPVOID) -> DWORD
        {
            s_Instance.DeferredLoad();

            return 0;
        };
        m_DeferredLoadThreadHandle = CreateThread(nullptr, 0, deferredLoadThreadProc, nullptr, 0, nullptr);
    }
    catch (const std::exception& ex)
    {
        m_Logger.Error("%s", ex.what());
        MessageBoxA(NULL, ex.what(), k_Name, MB_ICONERROR);
    }
}

void ExceptionReporter::Unload()
{
    try
    {
        CloseHandle(m_DeferredLoadThreadHandle);
    }
    catch (const std::exception& ex)
    {
        m_Logger.Error("%s", ex.what());
        MessageBoxA(NULL, ex.what(), k_Name, MB_ICONERROR);
    }
}

void ExceptionReporter::DeferredLoad()
{
    try
    {
        while (true)
        {
            HANDLE exeMutexHandle = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, "BurnoutParadiseexe");
            if (exeMutexHandle != NULL)
            {
                break;
            }

            Sleep(100);
        }

        auto topLevelExceptionFilter = [](EXCEPTION_POINTERS* ExceptionInfo) -> LONG
        {
            return s_Instance.TopLevelExceptionFilter(ExceptionInfo);
        };
        m_PreviousTopLevelExceptionFilter = SetUnhandledExceptionFilter(topLevelExceptionFilter);
        m_Logger.Info("Set top level exception filter. previous top level exception filter: 0x%p.", m_PreviousTopLevelExceptionFilter);
    }
    catch (const std::exception& ex)
    {
        m_Logger.Error("%s", ex.what());
        MessageBoxA(NULL, ex.what(), k_Name, MB_ICONERROR);
    }
}

LONG ExceptionReporter::TopLevelExceptionFilter(EXCEPTION_POINTERS* ExceptionInfo) const
{
    ExceptionInformation exceptionInformation(ExceptionInfo->ExceptionRecord, ExceptionInfo->ContextRecord);

    auto dialogProc = [](HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam) -> INT_PTR
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
    DialogBoxParamA(
        m_InstanceHandle,
        MAKEINTRESOURCEA(IDD_DIALOG_EXCEPTION_REPORT),
        NULL,
        dialogProc,
        reinterpret_cast<LPARAM>(&exceptionInformation)
    );

    return m_PreviousTopLevelExceptionFilter != nullptr ? m_PreviousTopLevelExceptionFilter(ExceptionInfo) : EXCEPTION_CONTINUE_SEARCH;
}
