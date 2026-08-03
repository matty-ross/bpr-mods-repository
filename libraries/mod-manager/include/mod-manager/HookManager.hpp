#pragma once


#include <vector>
#include <Windows.h>

#include "core/Pointer.hpp"
#include "mod-manager/ModManagerApi.hpp"


class HookManager
{
public:
    using UpdateHook = void(*)(Core::Pointer gameModule);
    using RenderMenuHook = void(*)(Core::Pointer gameModule);
    using RenderOverlayHook = void(*)(Core::Pointer gameModule);
    using AddGameEventsHook = void(*)(Core::Pointer gameModule, Core::Pointer gameEventQueue);
    using AddGameActionsHook = void(*)(Core::Pointer gameModule, Core::Pointer gameActionQueue);

private:
    HookManager();
    ~HookManager();

public:
    HookManager(const HookManager&) = delete;
    HookManager(HookManager&&) = delete;

    HookManager& operator =(const HookManager&) = delete;
    HookManager& operator =(HookManager&&) = delete;

public:
    MOD_MANAGER_API void AddUpdateHook(UpdateHook updateHook);
    MOD_MANAGER_API void AddRenderMenuHook(RenderMenuHook renderMenuHook);
    MOD_MANAGER_API void AddRenderOverlayHook(RenderOverlayHook renderOverlayHook);
    MOD_MANAGER_API void AddAddGameEventsHook(AddGameEventsHook addGameEventsHook);
    MOD_MANAGER_API void AddAddGameActionsHook(AddGameActionsHook addGameActionsHook);

    void ExecuteUpdateHooks(Core::Pointer gameModule);
    void ExecuteRenderMenuHooks(Core::Pointer gameModule);
    void ExecuteRenderOverlayHooks(Core::Pointer gameModule);
    void ExecuteAddGameEventsHooks(Core::Pointer gameModule, Core::Pointer gameEventQueue);
    void ExecuteAddGameActionsHooks(Core::Pointer gameModule, Core::Pointer gameActionQueue);

private:
    CRITICAL_SECTION m_CriticalSection = {};

    std::vector<UpdateHook> m_UpdateHooks;
    std::vector<RenderMenuHook> m_RenderMenuHooks;
    std::vector<RenderOverlayHook> m_RenderOverlayHooks;
    std::vector<AddGameEventsHook> m_AddGameEventsHooks;
    std::vector<AddGameActionsHook> m_AddGameActionsHooks;
};
