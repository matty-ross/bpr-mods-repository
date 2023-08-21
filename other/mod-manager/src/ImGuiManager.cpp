#include "ImGuiManager.h"

#include "imgui/imgui.h"


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