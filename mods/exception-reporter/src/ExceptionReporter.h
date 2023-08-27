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
    void OnThreadAttach(HANDLE thread) override;
    void OnThreadDetach(HANDLE thread) override;
};