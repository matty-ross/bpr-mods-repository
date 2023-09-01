#include "GameplayExternalCamera.h"

#include "imgui/imgui.h"


void GameplayExternalCamera::OnUpdate(Core::Pointer camera, Core::Pointer sharedInfo)
{
    auto updateParameter = [this](Parameter& parameter, Core::Pointer address) -> void
    {
        if (m_UpdateDefaultValues)
        {
            parameter.DefaultValue = address.as<float>();
        }

        if (parameter.Reset || m_ResetAllParameters)
        {
            address.as<float>() = parameter.DefaultValue;
            parameter.Reset = false;
        }
    };


    uint64_t attribKey = Core::Pointer(0x0013FC8E0).deref().at(0x7165C0).as<uint64_t>();
    m_UpdateDefaultValues = attribKey != m_PreviousAttribKey;

    {
        Core::Pointer parameters = Core::Pointer(0x0013FC8E0).deref().at(0x7165C8);

        updateParameter(m_PitchSpring, parameters.at(0x3C));
        updateParameter(m_YawSpring, parameters.at(0x40));
        updateParameter(m_PivotY, parameters.at(0x4C));
        updateParameter(m_PivotZ, parameters.at(0x50));
        updateParameter(m_PivotZOffset, parameters.at(0x54));
        updateParameter(m_FOV, parameters.at(0x6C));
        updateParameter(m_InFrontFOVMax, parameters.at(0x70));
        updateParameter(m_FrontInAmount, parameters.at(0x74));
        updateParameter(m_DriftYawSpring, parameters.at(0x8C));
        updateParameter(m_BoostFOVZoomCompensation, parameters.at(0x90));
        updateParameter(m_DownAngle, parameters.at(0x94));
        updateParameter(m_DropFactor, parameters.at(0xA8));
    }

    m_ResetAllParameters = false;
    m_PreviousAttribKey = attribKey;
}

void GameplayExternalCamera::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Gameplay External Camera"))
    {
        if (ImGui::TreeNode("Parameters"))
        {
            using WidgetFn = void(*)(Core::Pointer);
            auto renderParameter = [](Parameter& parameter, Core::Pointer address, WidgetFn widget) -> void
            {
                ImGui::PushID(&parameter);
                if (ImGui::Button("Reset"))
                {
                    parameter.Reset = true;
                }
                ImGui::SameLine();
                widget(address);
                ImGui::PopID();
            };


            Core::Pointer parameters = Core::Pointer(0x0013FC8E0).deref().at(0x7165C8);

            if (ImGui::Button("Reset All"))
            {
                m_ResetAllParameters = true;
            }

            renderParameter(m_PitchSpring, parameters.at(0x3C),
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Pitch Spring", &address.as<float>(), 0.0f, 1.0f);
                }
            );
            renderParameter(m_YawSpring, parameters.at(0x40),
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Yaw Spring", &address.as<float>(), 0.0f, 1.0f);
                }
            );
            renderParameter(m_DriftYawSpring, parameters.at(0x8C),
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Drift Yaw Spring", &address.as<float>(), 0.0f, 1.0f);
                }
            );
            renderParameter(m_PivotY, parameters.at(0x4C),
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Pivot Y", &address.as<float>(), 0.0f, 50.0f);
                }
            );
            renderParameter(m_PivotZ, parameters.at(0x50),
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Pivot Z", &address.as<float>(), 0.0f, 50.0f);
                }
            );
            renderParameter(m_PivotZOffset, parameters.at(0x54),
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Pivot Z Offset", &address.as<float>(), 0.0f, 50.0f);
                }
            );
            renderParameter(m_FOV, parameters.at(0x6C),
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("FOV", &address.as<float>(), 1.0f, 179.0f);
                }
            );
            renderParameter(m_InFrontFOVMax, parameters.at(0x70),
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("In Front FOV Max", &address.as<float>(), 1.0f, 179.0f);
                }
            );
            renderParameter(m_FrontInAmount, parameters.at(0x74),
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Front In Amount", &address.as<float>(), -2.0f, 2.0f);
                }
            );
            renderParameter(m_BoostFOVZoomCompensation, parameters.at(0x90),
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Boost FOV Zoom Compensation", &address.as<float>(), -5.0f, 5.0f);
                }
            );
            renderParameter(m_DownAngle, parameters.at(0x94),
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Down Angle", &address.as<float>(), 0.0f, 90.0f);
                }
            );
            renderParameter(m_DropFactor, parameters.at(0xA8),
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Drop Factor", &address.as<float>(), 0.0f, 1.0f);
                }
            );

            ImGui::TreePop();
        }
    }
}