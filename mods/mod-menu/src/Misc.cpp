#include "Misc.hpp"

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"


void Misc::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Misc"))
    {
        Core::Pointer gameModule = Core::Pointer(0x013FC8E0).deref(); // BrnGame::BrnGameModule*

        ImGui::Checkbox("Fatal Crashes", &gameModule.at(0x1E1364).as<bool>());
    }
}
