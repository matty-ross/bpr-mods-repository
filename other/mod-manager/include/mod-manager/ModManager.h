#pragma once


#include <Windows.h>

#include "core/Logger.h"

#include "mod-manager/ModManagerApi.h"
#include "mod-manager/DetourHookManager.h"
#include "mod-manager/ImGuiManager.h"


class ModManager
{
public:
    ModManager();

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
    void OnThreadAttach();
    void OnThreadDetach();

private:
    void Load();
    void Unload();
    
private:
    static void DetourPresent();
    static void DetourWindowProc();
    static void DetourUpdateKeyboardState();

private:
    Core::Logger m_Logger;
    
    DetourHookManager m_DetourHookManager;
    ImGuiManager m_ImGuiManager;

    DetourHook m_DetourPresent;
    DetourHook m_DetourWindowProc;
    DetourHook m_DetourUpdateKeyboardState;

    HANDLE m_LoadThread = nullptr;
};