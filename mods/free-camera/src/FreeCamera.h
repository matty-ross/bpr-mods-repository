#pragma once


#include <Windows.h>

#include "core/Mod.h"


class FreeCamera : public Core::Mod
{
public:
    FreeCamera(HMODULE module);

public:
    void OnProcessAttach() override;
    void OnProcessDetach() override;
    void OnThreadAttach() override;
    void OnThreadDetach() override;
};