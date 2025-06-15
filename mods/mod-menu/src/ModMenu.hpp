#pragma once


#include <Windows.h>

#include "core/Logger.hpp"
#include "mod-manager/ImGuiManager.hpp"


class ModMenu
{
private:
    ModMenu();

public:
    static ModMenu& Get();

public:
    void OnProcessAttach();
    void OnProcessDetach();

private:
    void Load();
    void Unload();

    void OnRenderMenu();

private:
    static ModMenu s_Instance;

private:
    Core::Logger m_Logger;

    ImGuiMenu m_Menu;

    HANDLE m_LoadThreadHandle = NULL;
};
