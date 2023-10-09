#pragma once


#include <Windows.h>

#include "core/Mod.h"
#include "core/Logger.h"

#include "mod-manager/DetourHookManager.h"
#include "mod-manager/ImGuiManager.h"

#include "BlacklistedPlayersFile.h"
#include "CurrentLobby.h"


class BullyRepellent : public Core::Mod
{
public:
    BullyRepellent(HMODULE module);

private:
    void Load() override;
    void Unload() override;

    void OnUpdate(void* guiEventNetworkPlayerStatus);
    void OnRenderMenu();

private:
    static void DetourOnGuiEventNetworkPlayerStatus();

private:
    Core::Logger m_Logger;
    
    BlacklistedPlayersFile m_BlacklistedPlayersFile;
    CurrentLobby m_CurrentLobby;

    DetourHook m_DetourOnGuiEventNetworkPlayerStatus;
    
    ImGuiMenu m_Menu;
};