#include "Misc.hpp"

#include "vendor/imgui.hpp"


void Misc::OnSetPlayerVehicleIndex(
    Core::Pointer directorInputBuffer // BrnDirector::DirectorIO::InputBuffer*
)
{
    if (m_OverridePlayerVehicleIndex)
    {
        directorInputBuffer.at(0x8168).as<int32_t>() = m_PlayerVehicleIndex;
    }
}

void Misc::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Misc"))
    {
        Core::Pointer gameModule = Core::Pointer(0x013FC8E0).deref();
        Core::Pointer directorModule = gameModule.at(0x6EF240);
        Core::Pointer arbitrator = gameModule.at(0x6F2AF0);
        Core::Pointer arbStateRoaming = arbitrator.at(0x1340);

        {
            ImGui::Checkbox("Allow Jump Moment", &directorModule.at(0x29F5D).as<bool>());
            ImGui::Checkbox("Allow Slow Motion", &directorModule.at(0x283A8).as<bool>());
            ImGui::Checkbox("Disable Depth of Field", &directorModule.at(0x29F68).as<bool>());
        }

        ImGui::Separator();

        {
            ImGui::Checkbox("Do Attract Mode", &arbitrator.at(0x3DCD).as<bool>());
            ImGui::Checkbox("Do Render Metrics", &arbitrator.at(0x3DCE).as<bool>());
        }
        
        ImGui::Separator();

        {
            ImGui::Checkbox("Disable Picture Paradise", &arbStateRoaming.at(0x3BF).as<bool>());
            ImGui::Checkbox("Play Race End Effect", &arbStateRoaming.at(0x3C0).as<bool>());
        }

        ImGui::Separator();

        {
            if (ImGui::Button("Reset"))
            {
                m_PlayerVehicleIndex = 0;
                m_OverridePlayerVehicleIndex = false;
            }
            
            ImGui::SameLine();
            
            if (ImGui::SliderInt("Player Vehicle Index", &m_PlayerVehicleIndex, 0, 7, "%d", ImGuiSliderFlags_AlwaysClamp))
            {
                m_OverridePlayerVehicleIndex = true;
            }
        }
    }
}
