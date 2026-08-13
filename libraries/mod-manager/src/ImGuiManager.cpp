#include <vector>
#include <Windows.h>
#include <d3d11.h>

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"
#include "core/Path.hpp"
#include "core/Logger.hpp"
#include "core/Patch.hpp"
#include "mod-manager/ModManager.hpp"
#include "mod-manager/ModManagerConfigFile.hpp"
#include "mod-manager/ImGuiManager.hpp"


// https://github.com/ocornut/imgui/blob/docking/examples/example_win32_directx11/main.cpp


IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


ImGuiManager::ImGuiManager(ModManagerConfigFile::ImGuiConfig& imguiConfig, Core::Path configDirectory, const Core::Logger& logger)
    :
    m_ImGuiConfig(imguiConfig),
    m_IniFilePath(configDirectory.Append("imgui.ini")),
    m_Logger(logger)
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
    m_Logger.Info("Added ImGui menu. address: 0x%p", menu);

    LeaveCriticalSection(&m_CriticalSection);
}

void ImGuiManager::AddOverlay(RenderImGuiOverlay overlay)
{
    EnterCriticalSection(&m_CriticalSection);

    m_Overlays.push_back(overlay);
    m_Logger.Info("Added ImGui overlay. address: 0x%p", overlay);

    LeaveCriticalSection(&m_CriticalSection);
}

void ImGuiManager::Load()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = m_IniFilePath.GetPath();
    io.ConfigViewportsNoTaskBarIcon = true;

    HWND windowHandle = Core::Pointer(0x0139815C).as<HWND>();
    ID3D11Device* d3d11Device = Core::Pointer(0x01485BF8).as<ID3D11Device*>();
    ID3D11DeviceContext* d3d11DeviceContext = Core::Pointer(0x01485ECC).as<ID3D11DeviceContext*>();
    ImGui_ImplWin32_Init(windowHandle);
    ImGui_ImplDX11_Init(d3d11Device, d3d11DeviceContext);

    // ImGui updates the cursor itself.
    SetClassLongPtrA(windowHandle, GCLP_HCURSOR, NULL);

    Core::Patch(0x0817E440, 6, m_Logger).WriteJMP(Hook_Render);
    Core::Patch(0x008FB9D9, 5, m_Logger).WriteJMP(Hook_WindowProc);

    m_Logger.Info(
        "Loaded ImGui manager. window handle: 0x%08X, D3D11 device: 0x%p, D3D11 device context: 0x%p",
        windowHandle,
        d3d11Device,
        d3d11DeviceContext
    );
}

void ImGuiManager::Unload()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();

    m_Logger.Info("Unloaded ImGui manager.");
}

void ImGuiManager::RenderMenu()
{
    if (ImGui::CollapsingHeader("ImGui Config"))
    {
        static constexpr const char* styleColors[] =
        {
            "Classic",
            "Dark",
            "Light",
        };
        ImGui::Combo("Style Colors", reinterpret_cast<int*>(&m_ImGuiConfig.StyleColors), styleColors, IM_ARRAYSIZE(styleColors));

        ImGui::Checkbox("Enable Docking", &m_ImGuiConfig.EnableDocking);
        ImGui::Checkbox("Enable Viewports", &m_ImGuiConfig.EnableViewports);
    }
}

void ImGuiManager::Render()
{
    EnterCriticalSection(&m_CriticalSection);

    ImGuiIO& io = ImGui::GetIO();
    
    if (m_ImGuiConfig.EnableDocking)
    {
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }
    else
    {
        io.ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
    }

    if (m_ImGuiConfig.EnableViewports)
    {
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    }
    else
    {
        io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
    }

    switch (m_ImGuiConfig.StyleColors)
    {
    case ModManagerConfigFile::ImGuiConfig::StyleColors::Dark:
        ImGui::StyleColorsDark();
        break;

    case ModManagerConfigFile::ImGuiConfig::StyleColors::Light:
        ImGui::StyleColorsLight();
        break;

    case ModManagerConfigFile::ImGuiConfig::StyleColors::Classic:
    default:
        ImGui::StyleColorsClassic();
        break;
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

__declspec(naked) void ImGuiManager::Hook_Render()
{
    __asm
    {
        pushfd
        pushad

        mov ecx, offset ModManager::s_Instance.m_ImGuiManager
        call ImGuiManager::Render

        popad
        popfd

        // Original code.
        mov edx, dword ptr [ecx]
        push esi
        mov eax, dword ptr [edx + 0x8]

        // Jump back.
        push 0x0817E446
        ret
    }
}

__declspec(naked) void ImGuiManager::Hook_WindowProc()
{
    __asm
    {
        pushfd
        pushad

        push dword ptr [ebp + 0x14]
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xC]
        push dword ptr [ebp + 0x8]
        mov ecx, offset ModManager::s_Instance.m_ImGuiManager
        call ImGuiManager::WindowProc

        popad
        popfd

        // Original code.
        push ebx
        mov ebx, dword ptr [ebp + 0xC]
        push esi

        // Jump back.
        push 0x008FB9DE
        ret
    }
}
