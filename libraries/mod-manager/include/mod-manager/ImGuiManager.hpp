#pragma once


#include <vector>
#include <Windows.h>

#include "core/Path.hpp"
#include "mod-manager/ModManagerApi.hpp"
#include "mod-manager/ModManagerConfigFile.hpp"


class ImGuiManager
{
public:
    using RenderImGuiMenu = void(*)();
    using RenderImGuiOverlay = void(*)();

public:
    ImGuiManager(Core::Path configDirectory, const ImGuiConfig& imguiConfig);
    ImGuiManager(const ImGuiManager&) = delete;
    ImGuiManager(ImGuiManager&&) = delete;
    ~ImGuiManager();

    ImGuiManager& operator =(const ImGuiManager&) = delete;
    ImGuiManager& operator =(ImGuiManager&&) = delete;

public:
    MOD_MANAGER_API CRITICAL_SECTION* GetCriticalSection();

    MOD_MANAGER_API void AddMenu(RenderImGuiMenu menu);
    MOD_MANAGER_API void AddOverlay(RenderImGuiOverlay overlay);

    void Load();
    void Unload();

    void Render();
    bool HandleWindowMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

private:
    Core::Path m_IniFilePath;
    const ImGuiConfig& m_ImGuiConfig;

    CRITICAL_SECTION m_CriticalSection = {};

    std::vector<RenderImGuiMenu> m_Menus;
    std::vector<RenderImGuiOverlay> m_Overlays;
    bool m_MenusVisible = false;
    bool m_OverlaysVisible = true;

    bool m_Loaded = false;
};
