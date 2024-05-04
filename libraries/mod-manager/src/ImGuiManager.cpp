#include "ImGuiManager.hpp"

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"


// https://github.com/ocornut/imgui/blob/docking/examples/example_win32_directx11/main.cpp


IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


ImGuiManager::ImGuiManager()
{
    InitializeCriticalSection(&m_CriticalSection);   
}

ImGuiManager::~ImGuiManager()
{
    DeleteCriticalSection(&m_CriticalSection);
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

bool ImGuiManager::WantCaptureMouse() const
{
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureMouse;
}

bool ImGuiManager::WantCaptureKeyboard() const
{
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureKeyboard;
}

void ImGuiManager::Load() const
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsClassic();
    
    HWND windowHandle = Core::Pointer(0x0139815C).as<HWND>();
    ImGui_ImplWin32_Init(windowHandle);

    ID3D11Device* device = Core::Pointer(0x01485BF8).as<ID3D11Device*>();
    ID3D11DeviceContext* deviceContext = Core::Pointer(0x01485ECC).as<ID3D11DeviceContext*>();
    ImGui_ImplDX11_Init(device, deviceContext);
}

void ImGuiManager::Unload() const
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();
}

void ImGuiManager::OnRenderFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // TODO: store the hotkeys in a config file
    if (ImGui::IsKeyPressed(ImGuiKey_F7, false))
    {
        m_MenusVisible = !m_MenusVisible;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_F8, false))
    {
        m_OverlaysVisible = !m_OverlaysVisible;
    }

    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = m_MenusVisible;

    EnterCriticalSection(&m_CriticalSection);
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
    LeaveCriticalSection(&m_CriticalSection);

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool ImGuiManager::OnWindowMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);

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
