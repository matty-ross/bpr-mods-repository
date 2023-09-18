#pragma once


#include <Windows.h>

#include "core/Mod.h"
#include "core/Logger.h"

#include "mod-manager/DetourHookManager.h"
#include "mod-manager/ImGuiManager.h"

#include "CurrentLobby.h"


class BullyRepellent : public Core::Mod
{
public:
    BullyRepellent(HMODULE module);

private:
    void Load() override;
    void Unload() override;

    void OnRenderMenu();

private:
    CurrentLobby m_CurrentLobby;

    Core::Logger m_Logger;

    ImGuiMenu m_Menu;
};