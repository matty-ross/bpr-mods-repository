#pragma once


#include <Windows.h>

#include "core/Logger.hpp"
#include "mod-manager/DetourHookManager.hpp"
#include "mod-manager/ImGuiManager.hpp"

#include "BlacklistedPlayersFile.hpp"
#include "OnlinePlayers.hpp"


class BullyRepellent
{
private:
    BullyRepellent();

public:
    static BullyRepellent& Get();

public:
    void OnProcessAttach();
    void OnProcessDetach();

private:
    void Load();
    void Unload();

    void OnGuiEventNetworkPlayerStatus(void* guiEventNetworkPlayerStatus, void* guiCache);

    void OnRenderMenu();

private:
    static void DetourOnGuiEventNetworkPlayerStatus();

private:
    static BullyRepellent s_Instance;

private:
    Core::Logger m_Logger;
    
    BlacklistedPlayersFile m_BlacklistedPlayersFile;
    
    OnlinePlayers m_OnlinePlayers;
    
    DetourHook m_DetourOnGuiEventNetworkPlayerStatus;

    ImGuiMenu m_Menu;
    
    HANDLE m_LoadThread = NULL;
};
