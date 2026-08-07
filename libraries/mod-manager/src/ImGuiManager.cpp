#include <vector>
#include <Windows.h>

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"
#include "core/Path.hpp"
#include "mod-manager/ModManagerConfigFile.hpp"
#include "mod-manager/ImGuiManager.hpp"


// https://github.com/ocornut/imgui/blob/docking/examples/example_win32_directx11/main.cpp


IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


ImGuiManager::ImGuiManager(Core::Path configDirectory, const ImGuiConfig& imguiConfig)
    :
    m_IniFilePath(configDirectory.Append("imgui.ini")),
    m_ImGuiConfig(imguiConfig)
{
    InitializeCriticalSection(&m_CriticalSection);
}

ImGuiManager::~ImGuiManager()
{
    DeleteCriticalSection(&m_CriticalSection);
}

CRITICAL_SECTION* ImGuiManager::GetCriticalSection()
{
    return &m_CriticalSection;
}

void ImGuiManager::AddMenu(RenderImGuiMenu menu)
{
    EnterCriticalSection(&m_CriticalSection);

    m_Menus.push_back(menu);

    LeaveCriticalSection(&m_CriticalSection);
}

void ImGuiManager::AddOverlay(RenderImGuiOverlay overlay)
{
    EnterCriticalSection(&m_CriticalSection);

    m_Overlays.push_back(overlay);

    LeaveCriticalSection(&m_CriticalSection);
}

void ImGuiManager::Load()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = m_IniFilePath.GetPath();

    HWND windowHandle = Core::Pointer(0x0139815C).as<HWND>();
    ID3D11Device* d3d11Device = Core::Pointer(0x01485BF8).as<ID3D11Device*>();
    ID3D11DeviceContext* d3d11DeviceContext = Core::Pointer(0x01485ECC).as<ID3D11DeviceContext*>();
    ImGui_ImplWin32_Init(windowHandle);
    ImGui_ImplDX11_Init(d3d11Device, d3d11DeviceContext);

    // ImGui updates the cursor itself.
    SetClassLongPtrA(windowHandle, GCLP_HCURSOR, NULL);

    m_Loaded = true;
}

void ImGuiManager::Unload()
{
    if (!m_Loaded)
    {
        return;
    }
    
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();
}

void ImGuiManager::Render()
{
    if (!m_Loaded)
    {
        return;
    }
    
    EnterCriticalSection(&m_CriticalSection);

    ImGuiIO& io = ImGui::GetIO();
    if (m_ImGuiConfig.EnableDocking)
    {
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }
    if (m_ImGuiConfig.EnableViewports)
    {
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigViewportsNoTaskBarIcon = true;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (m_MenusVisible)
    {
        for (RenderImGuiMenu menu : m_Menus)
        {
            menu();
        }
    }
    if (m_OverlaysVisible)
    {
        for (RenderImGuiOverlay overlay : m_Overlays)
        {
            overlay();
        }
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    LeaveCriticalSection(&m_CriticalSection);
}

void ImGuiManager::WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);

    switch (Msg)
    {
    case WM_KEYDOWN:
        if (!(HIWORD(lParam) & KF_REPEAT))
        {
            if (wParam == m_ImGuiConfig.ToggleMenusVK)
            {
                m_MenusVisible = !m_MenusVisible;

                Core::Pointer(0x01398242).as<bool>() = m_MenusVisible;
                Core::Pointer(0x0139813E).as<bool>() = true;
            }
            if (wParam == m_ImGuiConfig.ToggleOverlaysVK)
            {
                m_OverlaysVisible = !m_OverlaysVisible;
            }
        }
        break;
    }
}
