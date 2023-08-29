#pragma once


#include <Windows.h>

#include "core/Mod.h"


class ExceptionReporter : public Core::Mod
{
public:
    ExceptionReporter(HMODULE module);

public:
    void OnProcessAttach() override;
    void OnProcessDetach() override;
    void OnThreadAttach() override;
    void OnThreadDetach() override;

    LONG OnException(EXCEPTION_POINTERS* ExceptionInfo) const;

private:
    void Load();
    void Unload();

private:
    HANDLE m_LoadThread = nullptr;

    PTOP_LEVEL_EXCEPTION_FILTER m_PreviousTopLevelExceptionFilter = nullptr;
};