#include "GameplayExternalCamera.hpp"

#include "vendor/imgui.hpp"


GameplayExternalCamera::GameplayExternalCamera(CustomParametersFile& customParametersFile)
    :
    m_CustomParametersFile(customParametersFile),
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

void GameplayExternalCamera::OnArbitratorUpdate(
    Core::Pointer camera, // BrnDirector::Camera::Camera*
    Core::Pointer arbStateSharedInfo // BrnDirector::ArbStateSharedInfo*
)
{
    Core::Pointer behaviorParameterBank = Core::Pointer(0x013FC8E0).deref().at(0x714140); // BrnDirector::Camera::BehaviourParameterBank*
    uint64_t attribKey = behaviorParameterBank.at(0x2480).as<uint64_t>();

    {
        auto updateParameter = [&](Parameter& parameter) -> void
        {
            Core::Pointer address = behaviorParameterBank.at(0x2488 + parameter.Offset);

            if (attribKey != m_PreviousAttribKey)
            {
                parameter.DefaultValue = address.as<float>();
            }

            if (parameter.Reset || m_Parameters.ResetAll)
            {
                address.as<float>() = parameter.DefaultValue;
                parameter.Reset = false;
            }
        };
        
        updateParameter(m_Parameters.PitchSpring);
        updateParameter(m_Parameters.YawSpring);
        updateParameter(m_Parameters.PivotY);
        updateParameter(m_Parameters.PivotZ);
        updateParameter(m_Parameters.PivotZOffset);
        updateParameter(m_Parameters.FOV);
        updateParameter(m_Parameters.InFrontFOVMax);
        updateParameter(m_Parameters.FrontInAmount);
        updateParameter(m_Parameters.DriftYawSpring);
        updateParameter(m_Parameters.BoostFOVZoomCompensation);
        updateParameter(m_Parameters.DownAngle);
        updateParameter(m_Parameters.DropFactor);
        
        m_Parameters.ResetAll = false;
    }

    m_PreviousAttribKey = attribKey;
}

void GameplayExternalCamera::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Gameplay External Camera"))
    {
        if (ImGui::BeginTabBar("##gameplay-external-camera"))
        {
            if (ImGui::BeginTabItem("Behavior"))
            {
                int32_t gameplayExternalCameraIndex = Core::Pointer(0x013FC8E0).deref().at(0x6F5D38).as<int32_t>();
                Core::Pointer behavior = Core::Pointer(0x013FC8E0).deref().at(0x711340 + gameplayExternalCameraIndex * 0x1A0).as<void*>(); // BrnDirector::Camera::BehaviourGameplayExternal*

                {
                    ImGui::SeparatorText("Misc");
                    
                    if (ImGui::Button("Snap to Vehicle"))
                    {
                        behavior.at(0xBC1).as<bool>() = true;
                    }
                    
                    ImGui::Checkbox("Disable Automatic Rotation", &behavior.at(0xBC3).as<bool>());
                    
                    static bool disableCollision = false;
                    if (ImGui::Checkbox("Disable Collision", &disableCollision))
                    {
                        behavior.at(0x1B0).as<float>() = disableCollision ? -FLT_MAX : 0.01f;
                    }
                }

                {
                    ImGui::SeparatorText("Rotation Controller");
                    
                    ImGui::SliderFloat("Yaw", &behavior.at(0x20).as<float>(), -180.0f, 180.0f);
                    ImGui::SliderFloat("Pitch", &behavior.at(0x40).as<float>(), -10.0f, 10.0f);
                }

                {
                    ImGui::SeparatorText("Collision Policy");
                    
                    ImGui::SliderFloat("Desired Near Clip", &behavior.at(0x28C).as<float>(), 0.0f, 0.2f);
                    ImGui::Checkbox("Auto Elevate", &behavior.at(0x298).as<bool>());
                    ImGui::Checkbox("Do Vehicle Collision", &behavior.at(0x29F).as<bool>());
                }

                {
                    ImGui::SeparatorText("Frustum Collision Resolver");
                    
                    ImGui::Checkbox("Use Frustum Resolver", &behavior.at(0x29D).as<bool>());
                    ImGui::DragFloat3("Vehicle Resolve Vector", behavior.at(0x1A0).as<float[3]>(), 1.0f, 0.0f, 500.0f);
                }

                {
                    ImGui::SeparatorText("Ground Constraint");
                    
                    ImGui::Checkbox("Use Ground Constraint", &behavior.at(0x29B).as<bool>());
                    ImGui::SliderFloat("Desired Height", &behavior.at(0x260).as<float>(), -10.0f, 10.0f);
                }
                
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Parameters"))
            {
                {
                    auto renderParameter = []<typename Fn>(Parameter& parameter, Fn widget) -> void
                    {
                        float& value = Core::Pointer(0x013FC8E0).deref().at(0x7165C8 + parameter.Offset).as<float>();

                        ImGui::PushID(&parameter);
                        
                        if (ImGui::Button("Reset"))
                        {
                            parameter.Reset = true;
                        }
                    
                        ImGui::SameLine();
                    
                        widget(value);
                    
                        ImGui::PopID();
                    };

                    ImGui::SeparatorText("Current Parameters");

                    {
                        if (ImGui::Button("Reset All##parameters"))
                        {
                            m_Parameters.ResetAll = true;
                        }
                    }

                    ImGui::Separator();

                    {
                        renderParameter(m_Parameters.PitchSpring,              [](float& value) { ImGui::SliderFloat("Pitch Spring", &value, 0.0f, 1.0f); });
                        renderParameter(m_Parameters.YawSpring,                [](float& value) { ImGui::SliderFloat("Yaw Spring", &value, 0.0f, 1.0f); });
                        renderParameter(m_Parameters.DriftYawSpring,           [](float& value) { ImGui::SliderFloat("Drift Yaw Spring", &value, 0.0f, 1.0f); });
                        renderParameter(m_Parameters.PivotY,                   [](float& value) { ImGui::SliderFloat("Pivot Y", &value, 0.0f, 50.0f); });
                        renderParameter(m_Parameters.PivotZ,                   [](float& value) { ImGui::SliderFloat("Pivot Z", &value, 0.0f, 50.0f); });
                        renderParameter(m_Parameters.PivotZOffset,             [](float& value) { ImGui::SliderFloat("Pivot Z Offset", &value, 0.0f, 50.0f); });
                        renderParameter(m_Parameters.FOV,                      [](float& value) { ImGui::SliderFloat("FOV", &value, 1.0f, 179.0f); });
                        renderParameter(m_Parameters.InFrontFOVMax,            [](float& value) { ImGui::SliderFloat("In Front FOV Max", &value, 1.0f, 179.0f); });
                        renderParameter(m_Parameters.FrontInAmount,            [](float& value) { ImGui::SliderFloat("Front In Amount", &value, -2.0f, 2.0f); });
                        renderParameter(m_Parameters.BoostFOVZoomCompensation, [](float& value) { ImGui::SliderFloat("Boost FOV Zoom Compensation", &value, -5.0f, 5.0f); });
                        renderParameter(m_Parameters.DownAngle,                [](float& value) { ImGui::SliderFloat("Down Angle", &value, 0.0f, 90.0f); });
                        renderParameter(m_Parameters.DropFactor,               [](float& value) { ImGui::SliderFloat("Drop Factor", &value, 0.0f, 1.0f); });
                    }
                }

                {
                    ImGui::SeparatorText("Custom Parameters");

                    {
                        if (ImGui::Button("Save##custom-prameters-file"))
                        {
                            m_CustomParametersFile.Save();
                        }
                        
                        ImGui::SameLine();
                        
                        if (ImGui::Button("Load##custom-prameters-file"))
                        {
                            m_CustomParametersFile.Load();
                        }
                    }

                    ImGui::Separator();

                    {
                        static char name[64] = {};
                        if (ImGui::Button("Add Current Parameters"))
                        {
                            if (name[0] != '\0')
                            {
                                AddCurrentParametersToCustomParametersFile(name);
                                name[0] = '\0';
                            }
                        }
                    
                        ImGui::SameLine();
                        
                        ImGui::InputText("Name", name, IM_ARRAYSIZE(name));
                    }
                    
                    ImGui::Separator();

                    {
                        static ImGuiTextFilter customParametersFilter;
                        customParametersFilter.Draw("Filter##custom-parameters");
                    
                        if (ImGui::BeginListBox("##custom-parameters", ImVec2(-FLT_MIN, 0.0f)))
                        {
                            for (const CustomParameters& customParameters : m_CustomParametersFile.GetCustomParameters())
                            {
                                if (customParametersFilter.PassFilter(customParameters.Name.c_str()))
                                {
                                    ImGui::PushID(&customParameters);
                                    
                                    if (ImGui::Selectable(customParameters.Name.c_str()))
                                    {
                                        SetCurrentParametersFromCustomParameters(customParameters);
                                    }
                                    
                                    ImGui::PopID();
                                }
                            }

                            ImGui::EndListBox();
                        }
                    }
                }
                
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
    }
}

void GameplayExternalCamera::SetCurrentParametersFromCustomParameters(const CustomParameters& customParameters)
{
    Core::Pointer parameters = Core::Pointer(0x013FC8E0).deref().at(0x7165C8); // BrnDirector::Camera::BehaviourGameplayExternal::Parameters*

    parameters.at(0x3C).as<float>() = customParameters.PitchSpring;
    parameters.at(0x40).as<float>() = customParameters.YawSpring;
    parameters.at(0x4C).as<float>() = customParameters.PivotY;
    parameters.at(0x50).as<float>() = customParameters.PivotZ;
    parameters.at(0x54).as<float>() = customParameters.PivotZOffset;
    parameters.at(0x6C).as<float>() = customParameters.FOV;
    parameters.at(0x70).as<float>() = customParameters.InFrontFOVMax;
    parameters.at(0x74).as<float>() = customParameters.FrontInAmount;
    parameters.at(0x8C).as<float>() = customParameters.DriftYawSpring;
    parameters.at(0x90).as<float>() = customParameters.BoostFOVZoomCompensation;
    parameters.at(0x94).as<float>() = customParameters.DownAngle;
    parameters.at(0xA8).as<float>() = customParameters.DropFactor;
}

void GameplayExternalCamera::AddCurrentParametersToCustomParametersFile(const char* name)
{
    Core::Pointer parameters = Core::Pointer(0x013FC8E0).deref().at(0x7165C8); // BrnDirector::Camera::BehaviourGameplayExternal::Parameters*
    
    m_CustomParametersFile.GetCustomParameters().push_back(
        CustomParameters
        {
            .Name                     = name,
            .PitchSpring              = parameters.at(0x3C).as<float>(),
            .YawSpring                = parameters.at(0x40).as<float>(),
            .PivotY                   = parameters.at(0x4C).as<float>(),
            .PivotZ                   = parameters.at(0x50).as<float>(),
            .PivotZOffset             = parameters.at(0x54).as<float>(),
            .FOV                      = parameters.at(0x6C).as<float>(),
            .InFrontFOVMax            = parameters.at(0x70).as<float>(),
            .FrontInAmount            = parameters.at(0x74).as<float>(),
            .DriftYawSpring           = parameters.at(0x8C).as<float>(),
            .BoostFOVZoomCompensation = parameters.at(0x90).as<float>(),
            .DownAngle                = parameters.at(0x94).as<float>(),
            .DropFactor               = parameters.at(0xA8).as<float>(),
        }
    );
}
