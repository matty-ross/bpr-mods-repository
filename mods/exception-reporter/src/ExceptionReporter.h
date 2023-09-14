#pragma once


#include <Windows.h>

#include "core/Mod.h"
#include "core/Logger.h"


class ExceptionReporter : public Core::Mod
{
public:
    ExceptionReporter(HMODULE module);

public:
    LONG OnException(EXCEPTION_POINTERS* ExceptionInfo) const;

private:
    void Load() override;
    void Unload() override;

private:
    Core::Logger m_Logger;

    PTOP_LEVEL_EXCEPTION_FILTER m_PreviousTopLevelExceptionFilter = nullptr;
};