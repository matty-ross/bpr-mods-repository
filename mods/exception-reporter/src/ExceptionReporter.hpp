#pragma once


#include <Windows.h>

#include "core/Logger.hpp"


class ExceptionReporter
{
private:
    ExceptionReporter();

public:
    static ExceptionReporter& Get();

public:
    void OnProcessAttach(HINSTANCE moduleInstance);
    void OnProcessDetach();

    LONG OnException(EXCEPTION_POINTERS* ExceptionInfo) const;

private:
    void Load();
    void Unload();

private:
    static ExceptionReporter s_Instance;

private:
    Core::Logger m_Logger;

    HINSTANCE m_ModuleInstance = nullptr;
    PTOP_LEVEL_EXCEPTION_FILTER m_PreviousTopLevelExceptionFilter = nullptr;

    HANDLE m_LoadThread = nullptr;
};
