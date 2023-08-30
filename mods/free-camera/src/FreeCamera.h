#pragma once


#include <Windows.h>

#include "core/Mod.h"

#include "mod-manager/ImGuiManager.h"


class FreeCamera : public Core::Mod
{
public:
    FreeCamera(HMODULE module);

public:
    void OnProcessAttach() override;
    void OnProcessDetach() override;
    void OnThreadAttach() override;
    void OnThreadDetach() override;

private:
    void Load();
    void Unload();

    void OnRenderMenu();

private:
    HANDLE m_LoadThread = nullptr;

    ImGuiMenu m_Menu;
};