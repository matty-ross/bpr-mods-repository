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
    void Load(HINSTANCE instanceHandle);
    void Unload();

private:
    void DeferredLoad();

    LONG TopLevelExceptionFilter(EXCEPTION_POINTERS* ExceptionInfo) const;

private:
    static constexpr char k_Name[] = "Exception Reporter";
    static constexpr char k_Version[] = "2.0.0";

    static ExceptionReporter s_Instance;

private:
    Core::Logger m_Logger;

    HINSTANCE m_InstanceHandle = NULL;
    HANDLE m_DeferredLoadThreadHandle = NULL;
    PTOP_LEVEL_EXCEPTION_FILTER m_PreviousTopLevelExceptionFilter = nullptr;
};
