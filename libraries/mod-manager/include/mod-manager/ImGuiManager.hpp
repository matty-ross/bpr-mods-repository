#pragma once


#include <Windows.h>

#include "core/Path.hpp"
#include "mod-manager/ModManagerConfigFile.hpp"


class ImGuiManager
{
private:
    ImGuiManager(Core::Path configDirectory, const ImGuiConfig& imguiConfig);

public:
    ImGuiManager(const ImGuiManager&) = delete;
    ImGuiManager(ImGuiManager&&) = delete;

    ImGuiManager& operator =(const ImGuiManager&) = delete;
    ImGuiManager& operator =(ImGuiManager&&) = delete;

public:
    bool AreMenusVisible() const;
    bool AreOverlaysVisible() const;

    void Load();
    void Unload();

    void NewFrame();
    void Render();

    void OnWindowMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

private:
    Core::Path m_IniFilePath;
    const ImGuiConfig& m_ImGuiConfig;

    bool m_MenusVisible = false;
    bool m_OverlaysVisible = true;
};
