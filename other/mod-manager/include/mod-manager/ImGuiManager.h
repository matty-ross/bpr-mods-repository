#pragma once


#include <vector>
#include <functional>

#include <Windows.h>

#include "mod-manager/ModManagerApi.h"


struct ImGuiMenu
{
    std::function<void()> OnRenderMenuFunction;
    bool Visible;
};

struct ImGuiOverlay
{
    std::function<void()> OnRenderOverlayFunction;
    bool Visible;
};


class ImGuiManager
{
    friend class ModManager;

private:
    ImGuiManager();
    ~ImGuiManager();

public:
    ImGuiManager(const ImGuiManager&) = delete;
    ImGuiManager(ImGuiManager&&) = delete;
    ImGuiManager& operator =(const ImGuiManager&) = delete;
    ImGuiManager& operator =(ImGuiManager&&) = delete;

public:
    MOD_MANAGER_API void AddMenu(ImGuiMenu* menu);
    MOD_MANAGER_API void AddOverlay(ImGuiOverlay* overlay);

    MOD_MANAGER_API bool WantCaptureMouse() const;
    MOD_MANAGER_API bool WantCaptureKeyboard() const;

private:
    void Initialize() const;
    void Shutdown() const;

    void OnRenderFrame();
    bool OnWindowMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

private:
    std::vector<ImGuiMenu*> m_Menus;
    std::vector<ImGuiOverlay*> m_Overlays;
    CRITICAL_SECTION m_CriticalSection = {};
};