#include "ImGuiManager.hpp"

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"


// https://github.com/ocornut/imgui/blob/docking/examples/example_win32_directx11/main.cpp


IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


ImGuiManager::ImGuiManager(Core::Path directory, const ImGuiConfig& imguiConfig)
    :
    m_IniFilePath(directory.Append("imgui.ini")),
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

void ImGuiManager::AddMenu(ImGuiMenu* menu)
{
    EnterCriticalSection(&m_CriticalSection);
    
    m_Menus.push_back(menu);
    
    LeaveCriticalSection(&m_CriticalSection);
}

void ImGuiManager::RemoveMenu(ImGuiMenu* menu)
{
    EnterCriticalSection(&m_CriticalSection);
    
    auto it = std::find(m_Menus.begin(), m_Menus.end(), menu);
    if (it != m_Menus.end())
    {
        m_Menus.erase(it);
    }
    
    LeaveCriticalSection(&m_CriticalSection);
}

void ImGuiManager::AddOverlay(ImGuiOverlay* overlay)
{
    EnterCriticalSection(&m_CriticalSection);
    
    m_Overlays.push_back(overlay);
    
    LeaveCriticalSection(&m_CriticalSection);
}

void ImGuiManager::RemoveOverlay(ImGuiOverlay* overlay)
{
    EnterCriticalSection(&m_CriticalSection);
    
    auto it = std::find(m_Overlays.begin(), m_Overlays.end(), overlay);
    if (it != m_Overlays.end())
    {
        m_Overlays.erase(it);
    }
    
    LeaveCriticalSection(&m_CriticalSection);
}

void ImGuiManager::Load() const
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

void ImGuiManager::Unload() const
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();
}

void ImGuiManager::OnRenderFrame()
{
    EnterCriticalSection(&m_CriticalSection);
    
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (m_MenusVisible)
    {
        for (const ImGuiMenu* menu : m_Menus)
        {
            menu->OnRenderFunction();
        }
    }
    if (m_OverlaysVisible)
    {
        for (const ImGuiOverlay* overlay : m_Overlays)
        {
            overlay->OnRenderFunction();
        }
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    if (m_ImGuiConfig.EnableViewports)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
    
    LeaveCriticalSection(&m_CriticalSection);
}

bool ImGuiManager::OnWindowMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
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

    // Prevent forwarding messages meant for ImGui.
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        switch (Msg)
        {
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MOUSEWHEEL:
            return true;
        }
    }
    if (io.WantCaptureKeyboard)
    {
        switch (Msg)
        {
        case WM_KEYDOWN:
        case WM_CHAR:
        case WM_SYSKEYDOWN:
        case WM_MENUCHAR:
        case WM_COMMAND:
            return true;
        }
    }
    
    return false;
}
