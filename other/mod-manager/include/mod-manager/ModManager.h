#pragma once


#include <Windows.h>

#include "mod-manager/DetourHookManager.h"
#include "mod-manager/ImGuiManager.h"


class ModManager
{
public:
    ModManager() = default;
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
    void OnThreadAttach();
    void OnThreadDetach();

private:
    void Load();

private:
    DetourHookManager m_DetourHookManager;
    ImGuiManager m_ImGuiManager;

    HANDLE m_LoadThread = nullptr;
};