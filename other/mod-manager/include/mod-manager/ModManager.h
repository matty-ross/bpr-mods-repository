#pragma once


#include "mod-manager/DetourHookManager.h"
#include "mod-manager/ImGuiManager.h"


class ModManager
{
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
    DetourHookManager m_DetourHookManager;
    ImGuiManager m_ImGuiManager;
};