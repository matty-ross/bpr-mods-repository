#pragma once


#include <Windows.h>

#include "core/Logger.hpp"
#include "mod-manager/ModManagerApi.hpp"
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

public:
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
    Core::Logger m_Logger;
    
    DetourHookManager m_DetourHookManager;
    ImGuiManager m_ImGuiManager;

    DetourHook m_DetourPresent;
    DetourHook m_DetourWindowProc;
    DetourHook m_DetourUpdateKeyboardState;

    HANDLE m_LoadThreadHandle = NULL;
};
