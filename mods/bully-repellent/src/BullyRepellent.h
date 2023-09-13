#pragma once


#include <Windows.h>

#include "core/Mod.h"
#include "core/Logger.h"

#include "mod-manager/DetourHookManager.h"
#include "mod-manager/ImGuiManager.h"


class BullyRepellent : public Core::Mod
{
public:
    BullyRepellent(HMODULE module);

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
    Core::Logger m_Logger;

    ImGuiMenu m_Menu;

    HANDLE m_LoadThread = nullptr;
};