#pragma once


#include <vector>
#include <functional>
#include <Windows.h>

#include "mod-manager/ModManagerApi.hpp"
#include "mod-manager/ModManagerConfigFile.hpp"


struct ImGuiMenu
{
    std::function<void()> OnRenderFunction;
};

struct ImGuiOverlay
{
    std::function<void()> OnRenderFunction;
};


class ImGuiManager
{
    friend class ModManager;

private:
    ImGuiManager(const ImGuiConfig& imguiConfig);
    ~ImGuiManager();

public:
    ImGuiManager(const ImGuiManager&) = delete;
    ImGuiManager(ImGuiManager&&) = delete;
    ImGuiManager& operator =(const ImGuiManager&) = delete;
    ImGuiManager& operator =(ImGuiManager&&) = delete;

public:
    MOD_MANAGER_API CRITICAL_SECTION& GetCriticalSection();

    MOD_MANAGER_API void AddMenu(ImGuiMenu* menu);
    MOD_MANAGER_API void RemoveMenu(ImGuiMenu* menu);
    MOD_MANAGER_API void AddOverlay(ImGuiOverlay* overlay);
    MOD_MANAGER_API void RemoveOverlay(ImGuiOverlay* overlay);

private:
    void Load() const;
    void Unload() const;

    void OnRenderFrame();
    bool OnWindowMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

    bool WantCaptureMouse() const;
    bool WantCaptureKeyboard() const;

private:
    const ImGuiConfig& m_ImGuiConfig;

    std::vector<ImGuiMenu*> m_Menus;
    bool m_MenusVisible = false;
    
    std::vector<ImGuiOverlay*> m_Overlays;
    bool m_OverlaysVisible = true;
    
    CRITICAL_SECTION m_CriticalSection = {};
};
