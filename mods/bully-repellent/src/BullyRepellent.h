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

    void OnUpdate(void* guiCache);
    void OnRenderMenu();

private:
    static void DetourUpatePlayerStatus();

private:
    Core::Logger m_Logger;
    
    CurrentLobby m_CurrentLobby;

    DetourHook m_DetourUpatePlayerStatus;
    ImGuiMenu m_Menu;
};