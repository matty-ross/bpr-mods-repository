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

    void OnRenderMenu();

private:
    static void DetourOnGuiEventNetworkPlayerStatus();

private:
    static BullyRepellent s_Instance;

private:
    Core::Logger m_Logger;
    
    BlacklistedPlayersFile m_BlacklistedPlayersFile;
    
    OnlinePlayers m_OnlinePlayers;
    
    ImGuiMenu m_Menu;

    DetourHook m_DetourOnGuiEventNetworkPlayerStatus;
    
    HANDLE m_LoadThread = NULL;
};
