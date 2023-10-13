#pragma once


#include <Windows.h>

#include "core/Mod.h"


class Protection : public Core::Mod
{
public:
    Protection(HMODULE module);

private:
    void Load() override;
    void Unload() override;
};