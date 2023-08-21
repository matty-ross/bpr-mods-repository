#pragma once


#include "mod-manager/DetourHookManager.h"


class ModManager
{
public:
    static ModManager& Get();

public:
    DetourHookManager& GetDetourHookManager();

    void OnProcessAttach();
    void OnProcessDetach();
    void OnThreadAttach();
    void OnThreadDetach();

private:
    DetourHookManager m_DetourHookManager;
};