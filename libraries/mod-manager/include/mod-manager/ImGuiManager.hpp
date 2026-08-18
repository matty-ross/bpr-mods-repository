#pragma once


#include <vector>
#include <Windows.h>

#include "core/Path.hpp"
#include "core/Logger.hpp"
#include "mod-manager/ModManagerApi.hpp"
#include "mod-manager/ModManagerConfigFile.hpp"


class ImGuiManager
{
public:
    using RenderImGuiMenu = void(*)();
    using RenderImGuiOverlay = void(*)();

public:
    ImGuiManager(ModManagerConfigFile::ImGuiConfig& imguiConfig, Core::Path configDirectoryPath, const Core::Logger& logger);
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

    void RenderMenu();

private:
    void Render();
    void WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

    void ApplyConfig();

private:
    static void Hook_Render();
    static void Hook_WindowProc();

private:
    ModManagerConfigFile::ImGuiConfig& m_ImGuiConfig;

    Core::Path m_IniFilePath;

    CRITICAL_SECTION m_CriticalSection = {};

    bool m_MenusVisible = false;
    std::vector<RenderImGuiMenu> m_Menus;
    bool m_OverlaysVisible = true;
    std::vector<RenderImGuiOverlay> m_Overlays;

    const Core::Logger& m_Logger;
};
