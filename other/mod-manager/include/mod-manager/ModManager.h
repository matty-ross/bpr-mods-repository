#pragma once


#include <Windows.h>

#include "mod-manager/DetourHookManager.h"
#include "mod-manager/ImGuiManager.h"
#include "mod-manager/DetourHook.h"


class ModManager
{
public:
    ModManager();
    ~ModManager() = default;

    ModManager(const ModManager&) = delete;
    ModManager(ModManager&&) = delete;
    ModManager& operator =(const ModManager&) = delete;
    ModManager& operator =(ModManager&&) = delete;

public:
    static ModManager& Get();

public:
    DetourHookManager& GetDetourHookManager();
    ImGuiManager& GetImGuiManager();

    void OnProcessAttach();
    void OnProcessDetach();
    void OnThreadAttach(HANDLE thread);
    void OnThreadDetach(HANDLE thread);

private:
    void Load();
    void Unload();
    
private:
    static void DetourPresent();
    static void DetourWindowProc();

private:
    DetourHookManager m_DetourHookManager;
    ImGuiManager m_ImGuiManager;

    DetourHook m_DetourPresent;
    DetourHook m_DetourWindowProc;

    HANDLE m_LoadThread = nullptr;
};