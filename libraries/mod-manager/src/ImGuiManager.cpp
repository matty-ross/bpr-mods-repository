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
}

bool ImGuiManager::AreMenusVisible() const
{
    return m_MenusVisible;
}

bool ImGuiManager::AreOverlaysVisible() const
{
    return m_OverlaysVisible;
}

void ImGuiManager::Load()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = m_IniFilePath.GetPath();
    if (m_ImGuiConfig.EnableDocking)
    {
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }
    if (m_ImGuiConfig.EnableViewports)
    {
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigViewportsNoTaskBarIcon = true;
    }

    ImGui::StyleColorsClassic();

    HWND windowHandle = Core::Pointer(0x0139815C).as<HWND>();
    ImGui_ImplWin32_Init(windowHandle);

    ID3D11Device* d3d11Device = Core::Pointer(0x01485BF8).as<ID3D11Device*>();
    ID3D11DeviceContext* d3d11DeviceContext = Core::Pointer(0x01485ECC).as<ID3D11DeviceContext*>();
    ImGui_ImplDX11_Init(d3d11Device, d3d11DeviceContext);
}

void ImGuiManager::Unload()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();
}

void ImGuiManager::NewFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    if (m_ImGuiConfig.EnableViewports)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void ImGuiManager::OnWindowMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
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
