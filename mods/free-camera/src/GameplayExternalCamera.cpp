#include "GameplayExternalCamera.h"

#include "imgui/imgui.h"


GameplayExternalCamera::GameplayExternalCamera()
    :
    m_Parameters
    {
        .PitchSpring              = 0x3C,
        .YawSpring                = 0x40,
        .PivotY                   = 0x4C,
        .PivotZ                   = 0x50,
        .PivotZOffset             = 0x54,
        .FOV                      = 0x6C,
        .InFrontFOVMax            = 0x70,
        .FrontInAmount            = 0x74,
        .DriftYawSpring           = 0x8C,
        .BoostFOVZoomCompensation = 0x90,
        .DownAngle                = 0x94,
        .DropFactor               = 0xA8,
    }
{
}

void GameplayExternalCamera::OnUpdate(Core::Pointer camera, Core::Pointer sharedInfo)
{
    uint64_t attribKey = Core::Pointer(0x0013FC8E0).deref().at(0x7165C0).as<uint64_t>();

    {
        auto updateParameter = [this](Parameter& parameter, Core::Pointer parameters, bool updateDefaultParameterValues) -> void
        {
            if (updateDefaultParameterValues)
            {
                parameter.DefaultValue = parameters.at(parameter.Offset).as<float>();
            }

            if (parameter.Reset || m_ResetAllParameters)
            {
                parameters.at(parameter.Offset).as<float>() = parameter.DefaultValue;
                parameter.Reset = false;
            }
        };

        Core::Pointer parameters = Core::Pointer(0x0013FC8E0).deref().at(0x7165C8);
        bool updateDefaultParameterValues = attribKey != m_PreviousAttribKey;
        
        updateParameter(m_Parameters.PitchSpring, parameters, updateDefaultParameterValues);
        updateParameter(m_Parameters.YawSpring, parameters, updateDefaultParameterValues);
        updateParameter(m_Parameters.PivotY, parameters, updateDefaultParameterValues);
        updateParameter(m_Parameters.PivotZ, parameters, updateDefaultParameterValues);
        updateParameter(m_Parameters.PivotZOffset, parameters, updateDefaultParameterValues);
        updateParameter(m_Parameters.FOV, parameters, updateDefaultParameterValues);
        updateParameter(m_Parameters.InFrontFOVMax, parameters, updateDefaultParameterValues);
        updateParameter(m_Parameters.FrontInAmount, parameters, updateDefaultParameterValues);
        updateParameter(m_Parameters.DriftYawSpring, parameters, updateDefaultParameterValues);
        updateParameter(m_Parameters.BoostFOVZoomCompensation, parameters, updateDefaultParameterValues);
        updateParameter(m_Parameters.DownAngle, parameters, updateDefaultParameterValues);
        updateParameter(m_Parameters.DropFactor, parameters, updateDefaultParameterValues);
        
        m_ResetAllParameters = false;
    }

    m_PreviousAttribKey = attribKey;
}

void GameplayExternalCamera::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Gameplay External Camera"))
    {
        if (ImGui::TreeNode("Parameters"))
        {
            using WidgetFn = void(*)(Core::Pointer);
            auto renderParameter = [](Parameter& parameter, Core::Pointer parameters, WidgetFn widget) -> void
            {
                ImGui::PushID(&parameter);
                if (ImGui::Button("Reset"))
                {
                    parameter.Reset = true;
                }
                ImGui::SameLine();
                widget(parameters.at(parameter.Offset));
                ImGui::PopID();
            };

            Core::Pointer parameters = Core::Pointer(0x0013FC8E0).deref().at(0x7165C8);

            if (ImGui::Button("Reset All"))
            {
                m_ResetAllParameters = true;
            }

            renderParameter(m_Parameters.PitchSpring, parameters,
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Pitch Spring", &address.as<float>(), 0.0f, 1.0f);
                }
            );
            renderParameter(m_Parameters.YawSpring, parameters,
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Yaw Spring", &address.as<float>(), 0.0f, 1.0f);
                }
            );
            renderParameter(m_Parameters.DriftYawSpring, parameters,
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Drift Yaw Spring", &address.as<float>(), 0.0f, 1.0f);
                }
            );
            renderParameter(m_Parameters.PivotY, parameters,
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Pivot Y", &address.as<float>(), 0.0f, 50.0f);
                }
            );
            renderParameter(m_Parameters.PivotZ, parameters,
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Pivot Z", &address.as<float>(), 0.0f, 50.0f);
                }
            );
            renderParameter(m_Parameters.PivotZOffset, parameters,
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Pivot Z Offset", &address.as<float>(), 0.0f, 50.0f);
                }
            );
            renderParameter(m_Parameters.FOV, parameters,
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("FOV", &address.as<float>(), 1.0f, 179.0f);
                }
            );
            renderParameter(m_Parameters.InFrontFOVMax, parameters,
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("In Front FOV Max", &address.as<float>(), 1.0f, 179.0f);
                }
            );
            renderParameter(m_Parameters.FrontInAmount, parameters,
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Front In Amount", &address.as<float>(), -2.0f, 2.0f);
                }
            );
            renderParameter(m_Parameters.BoostFOVZoomCompensation, parameters,
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Boost FOV Zoom Compensation", &address.as<float>(), -5.0f, 5.0f);
                }
            );
            renderParameter(m_Parameters.DownAngle, parameters,
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Down Angle", &address.as<float>(), 0.0f, 90.0f);
                }
            );
            renderParameter(m_Parameters.DropFactor, parameters,
                [](Core::Pointer address) -> void
                {
                    ImGui::SliderFloat("Drop Factor", &address.as<float>(), 0.0f, 1.0f);
                }
            );

            ImGui::TreePop();
        }
    }
}