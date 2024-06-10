#pragma once


#include <Windows.h>

#include "core/Logger.hpp"
#include "mod-manager/DetourHookManager.hpp"
#include "mod-manager/ImGuiManager.hpp"

#include "VehiclesFile.hpp"
#include "ChallengesFile.hpp"
#include "VehicleProtection.hpp"
#include "ChallengeProtection.hpp"


class Protection
{
private:
    Protection();

public:
    static Protection& Get();

public:
    void OnProcessAttach();
    void OnProcessDetach();

private:
    void Load();
    void Unload();

    void OnRenderMenu();

    void OnFreeburnChallengeMessagePack(void* freeburnChallengeMessage);
    void OnFreeburnChallengeMessageUnpack(void* freeburnChallengeMessage);

private:
    static void DetourPlayerParamsSerialize();
    static void DetourPlayerParamsDeserialize();
    static void DetourVehicleSelectMessagePack();
    static void DetourVehicleSelectMessageUnpack();
    static void DetourOnFreeburnChallengeMessagePack();
    static void DetourOnFreeburnChallengeMessageUnpack();

private:
    static Protection s_Instance;

private:
    Core::Logger m_Logger;

    VehiclesFile m_VehiclesFile;
    ChallengesFile m_ChallengesFile;

    VehicleProtection m_VehicleProtection;
    ChallengeProtection m_ChallengeProtection;
    
    ImGuiMenu m_Menu;

    DetourHook m_DetourPlayerParamsSerialize;
    DetourHook m_DetourPlayerParamsDeserialize;
    DetourHook m_DetourVehicleSelectMessagePack;
    DetourHook m_DetourVehicleSelectMessageUnpack;
    DetourHook m_DetourOnFreeburnChallengeMessagePack;
    DetourHook m_DetourOnFreeburnChallengeMessageUnpack;

    HANDLE m_LoadThread = NULL;
};
