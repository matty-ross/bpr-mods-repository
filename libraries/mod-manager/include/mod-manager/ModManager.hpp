#pragma once


#include <Windows.h>

#include "core/Path.hpp"
#include "core/Logger.hpp"
#include "mod-manager/ModManagerApi.hpp"
#include "mod-manager/ModManagerConfigFile.hpp"
#include "mod-manager/DetourHookManager.hpp"
#include "mod-manager/ImGuiManager.hpp"


class ModManager
{
private:
    ModManager();

public:
    ModManager(const ModManager&) = delete;
    ModManager(ModManager&&) = delete;
    ModManager& operator =(const ModManager&) = delete;
    ModManager& operator =(ModManager&&) = delete;

public:
    MOD_MANAGER_API static ModManager& Get();
    MOD_MANAGER_API static const char* GetVersion();

public:
    MOD_MANAGER_API Core::Path GetAssetsDirectory() const;
    MOD_MANAGER_API Core::Path GetConfigDirectory() const;
    MOD_MANAGER_API DetourHookManager& GetDetourHookManager();
    MOD_MANAGER_API ImGuiManager& GetImGuiManager();

    void OnProcessAttach();
    void OnProcessDetach();

private:
    void Load();
    void Unload();
    
private:
    static void DetourPresent();
    static void DetourWindowProc();
    static void DetourUpdateKeyboardState();

private:
    static ModManager s_Instance;

private:
    Core::Path m_AssetsDirectory;
    Core::Path m_ConfigDirectory;
    Core::Logger m_Logger;

    ModManagerConfigFile m_ModManagerConfigFile;
    
    DetourHookManager m_DetourHookManager;
    ImGuiManager m_ImGuiManager;

    DetourHook m_DetourPresent;
    DetourHook m_DetourWindowProc;
    DetourHook m_DetourUpdateKeyboardState;

    HANDLE m_LoadThreadHandle = NULL;
    HCURSOR m_PreviousCursorHandle = NULL;
};
