#include "ImGuiManager.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "core/Pointer.h"


// https://github.com/ocornut/imgui/blob/docking/examples/example_win32_directx11/main.cpp


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


static constexpr int k_MenuToggleHotkey = VK_F7;
static constexpr int k_OverlayToggleHotkey = VK_F8;


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

void ImGuiManager::AddOverlay(ImGuiOverlay* overlay)
{
    EnterCriticalSection(&m_CriticalSection);
    m_Overlays.push_back(overlay);
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

void ImGuiManager::Initialize() const
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

void ImGuiManager::Shutdown() const
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

    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = false;

    EnterCriticalSection(&m_CriticalSection);
    for (const ImGuiMenu* menu : m_Menus)
    {
        if (menu->Visible)
        {
            menu->OnRenderMenuFunction();
            io.MouseDrawCursor = true;
        }
    }
    for (const ImGuiOverlay* overlay : m_Overlays)
    {
        if (overlay->Visible)
        {
            overlay->OnRenderOverlayFunction();
        }
    }
    LeaveCriticalSection(&m_CriticalSection);

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool ImGuiManager::OnWindowMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);

    // Custom events.
    switch (Msg)
    {
    case WM_KEYDOWN:
        {
            bool repeat = HIWORD(lParam) & KF_REPEAT;
            if (!repeat)
            {
                EnterCriticalSection(&m_CriticalSection);
                switch (wParam)
                {
                case k_MenuToggleHotkey:
                    for (ImGuiMenu* menu : m_Menus)
                    {
                        menu->Visible = !(menu->Visible);
                    }
                    break;

                case k_OverlayToggleHotkey:
                    for (ImGuiOverlay* overlay : m_Overlays)
                    {
                        overlay->Visible = !(overlay->Visible);
                    }
                    break;
                }
                LeaveCriticalSection(&m_CriticalSection);
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
        case WM_CHAR:
        case WM_KEYDOWN:
        //case WM_SYSKEYDOWN:
        //case WM_COMMAND:
        //case WM_MENUCHAR:
            return true;
        }
    }
    
    return false;
}