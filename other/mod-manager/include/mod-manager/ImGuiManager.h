#pragma once


#include <vector>
#include <functional>

#include <Windows.h>


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
public:
    ImGuiManager();
    ~ImGuiManager();

    ImGuiManager(const ImGuiManager&) = delete;
    ImGuiManager(ImGuiManager&&) = delete;
    ImGuiManager& operator =(const ImGuiManager&) = delete;
    ImGuiManager& operator =(ImGuiManager&&) = delete;

public:
    void AddMenu(ImGuiMenu* menu);
    void AddOverlay(ImGuiOverlay* overlay);

    void Initialize() const;
    void Shutdown() const;

    void OnRenderFrame();
    bool OnWindowMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

private:
    std::vector<ImGuiMenu*> m_Menus;
    std::vector<ImGuiOverlay*> m_Overlays;
    CRITICAL_SECTION m_CriticalSection = {};
};