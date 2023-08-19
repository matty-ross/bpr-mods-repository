#pragma once


#include "mod-manager/DetourHookManager.h"


class ModManager
{
public:
    void OnProcessAttach();
    void OnProcessDetach();
    void OnThreadAttach();
    void OnThreadDetach();

private:
    DetourHookManager m_DetourHookManager;
};