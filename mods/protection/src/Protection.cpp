#include "Protection.h"

#include "imgui/imgui.h"


using namespace std::string_literals;


static constexpr char k_ModName[]      = "Protection";
static constexpr char k_ModVersion[]   = "1.0.0";
static constexpr char k_ModAuthor[]    = "PISros0724 (Matty)";
static constexpr char k_ModDirectory[] = ".\\mods\\protection\\";


Protection::Protection(HMODULE module)
    :
    Mod(module),
    m_Logger(k_ModName),
    m_VehiclesFile(m_Logger, k_ModDirectory + "vehicles.yaml"s),
    m_VehicleProtection(m_VehiclesFile),
    m_Menu
    {
        .OnRenderMenuFunction   = [this]() { OnRenderMenu(); },
        .ToggleVisibilityHotkey = VK_F7,
        .Visible                = true,
    }
{
}

void Protection::Load()
{
}

void Protection::Unload()
{
}

void Protection::OnRenderMenu()
{
    if (ImGui::Begin(k_ModName))
    {
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Version     %s", k_ModVersion);
        ImGui::Text("Author      %s", k_ModAuthor);
        ImGui::Text("Framerate   %.3f", io.Framerate);

        m_VehicleProtection.OnRenderMenu();

        ImGui::PopItemWidth();
    }
    ImGui::End();
}