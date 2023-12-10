#include "GameplayExternalCamera.h"

#include "imgui/imgui.h"


GameplayExternalCamera::GameplayExternalCamera(CustomParametersFile& customParametersFile)
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
    },
    m_CustomParametersFile(customParametersFile)
{
}

void GameplayExternalCamera::OnUpdate(Core::Pointer camera, Core::Pointer sharedInfo)
{
    uint64_t attribKey = Core::Pointer(0x013FC8E0).deref().at(0x7165C0).as<uint64_t>();

    {
        bool updateDefaultValues = attribKey != m_PreviousAttribKey;
        
        auto updateParameter = [this, updateDefaultValues](Parameter& parameter) -> void
        {
            Core::Pointer address = Core::Pointer(0x013FC8E0).deref().at(0x7165C8 + parameter.Offset);

            if (updateDefaultValues)
            {
                parameter.DefaultValue = address.as<float>();
            }

            if (parameter.Reset || m_ResetAllParameters)
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
        
        m_ResetAllParameters = false;
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
                Core::Pointer behavior = Core::Pointer(0x013FC8E0).deref().at(0x711340 + gameplayExternalCameraIndex * 0x1A0).as<void*>();
                Core::Pointer rotationController = behavior.at(0x10);
                Core::Pointer collisionPolicy = behavior.at(0x50);
                Core::Pointer groundConstraint = collisionPolicy.at(0x1C0);
                Core::Pointer frustumCollisionResolver = collisionPolicy.at(0x10);

                ImGui::SeparatorText("Misc");
                if (ImGui::Button("Snap to Vehicle"))
                {
                    behavior.at(0xBC1).as<bool>() = true;
                }
                ImGui::Checkbox("Disable Automatic Rotation", &behavior.at(0xBC3).as<bool>());
                static bool disableCollision = false;
                if (ImGui::Checkbox("Disable Collision", &disableCollision))
                {
                    frustumCollisionResolver.at(0x150).as<float>() = disableCollision ? -FLT_MAX : 0.01f;
                }

                ImGui::SeparatorText("Rotation Controller");
                ImGui::SliderFloat("Yaw Degrees", &rotationController.at(0x10).as<float>(), -180.0f, 180.0f);
                ImGui::SliderFloat("Yaw Velocity", &rotationController.at(0x14).as<float>(), -100.0f, 100.0f);
                ImGui::SliderFloat("Pitch Degrees", &rotationController.at(0x30).as<float>(), -10.0f, 10.0f);
                ImGui::SliderFloat("Pitch Velocity", &rotationController.at(0x2C).as<float>(), -10.0f, 10.0f);

                ImGui::SeparatorText("Collision Policy");
                ImGui::SliderFloat("Desired Near Clip", &collisionPolicy.at(0x23C).as<float>(), 0.0f, 1.0f);
                ImGui::Checkbox("Auto Elevate", &collisionPolicy.at(0x248).as<bool>());
                ImGui::Checkbox("Do Vehicle Collision", &collisionPolicy.at(0x24F).as<bool>());

                ImGui::SeparatorText("Frustum Collision Resolver");
                ImGui::Checkbox("Use Frustum Resolver", &collisionPolicy.at(0x24D).as<bool>());
                ImGui::DragFloat3("Vehicle Resolve Vector", frustumCollisionResolver.at(0x140).as<float[3]>(), 1.0f, 0.0f, 500.0f);

                ImGui::SeparatorText("Ground Constraint");
                ImGui::Checkbox("Use Ground Constraint", &collisionPolicy.at(0x24B).as<bool>());
                ImGui::SliderFloat("Desired Height", &groundConstraint.at(0x50).as<float>(), -10.0f, 10.0f);
                
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Parameters"))
            {
                using WidgetFn = void(*)(Core::Pointer);
                auto renderParameter = [](Parameter& parameter, WidgetFn widget) -> void
                {
                    Core::Pointer address = Core::Pointer(0x013FC8E0).deref().at(0x7165C8 + parameter.Offset);

                    ImGui::PushID(&parameter);
                    if (ImGui::Button("Reset"))
                    {
                        parameter.Reset = true;
                    }
                    ImGui::SameLine();
                    widget(address);
                    ImGui::PopID();
                };

                if (ImGui::Button("Reset All"))
                {
                    m_ResetAllParameters = true;
                }

                ImGui::SeparatorText("Current Parameters");
                renderParameter(m_Parameters.PitchSpring,              [](Core::Pointer address) { ImGui::SliderFloat("Pitch Spring", &address.as<float>(), 0.0f, 1.0f); });
                renderParameter(m_Parameters.YawSpring,                [](Core::Pointer address) { ImGui::SliderFloat("Yaw Spring", &address.as<float>(), 0.0f, 1.0f); });
                renderParameter(m_Parameters.DriftYawSpring,           [](Core::Pointer address) { ImGui::SliderFloat("Drift Yaw Spring", &address.as<float>(), 0.0f, 1.0f); });
                renderParameter(m_Parameters.PivotY,                   [](Core::Pointer address) { ImGui::SliderFloat("Pivot Y", &address.as<float>(), 0.0f, 50.0f); });
                renderParameter(m_Parameters.PivotZ,                   [](Core::Pointer address) { ImGui::SliderFloat("Pivot Z", &address.as<float>(), 0.0f, 50.0f); });
                renderParameter(m_Parameters.PivotZOffset,             [](Core::Pointer address) { ImGui::SliderFloat("Pivot Z Offset", &address.as<float>(), 0.0f, 50.0f); });
                renderParameter(m_Parameters.FOV,                      [](Core::Pointer address) { ImGui::SliderFloat("FOV", &address.as<float>(), 1.0f, 179.0f); });
                renderParameter(m_Parameters.InFrontFOVMax,            [](Core::Pointer address) { ImGui::SliderFloat("In Front FOV Max", &address.as<float>(), 1.0f, 179.0f); });
                renderParameter(m_Parameters.FrontInAmount,            [](Core::Pointer address) { ImGui::SliderFloat("Front In Amount", &address.as<float>(), -2.0f, 2.0f); });
                renderParameter(m_Parameters.BoostFOVZoomCompensation, [](Core::Pointer address) { ImGui::SliderFloat("Boost FOV Zoom Compensation", &address.as<float>(), -5.0f, 5.0f); });
                renderParameter(m_Parameters.DownAngle,                [](Core::Pointer address) { ImGui::SliderFloat("Down Angle", &address.as<float>(), 0.0f, 90.0f); });
                renderParameter(m_Parameters.DropFactor,               [](Core::Pointer address) { ImGui::SliderFloat("Drop Factor", &address.as<float>(), 0.0f, 1.0f); });

                ImGui::SeparatorText("Custom Parameters");
                if (ImGui::Button("Save"))
                {
                    m_CustomParametersFile.Save();
                }
                ImGui::SameLine();
                if (ImGui::Button("Load"))
                {
                    m_CustomParametersFile.Load();
                }
                static char name[64] = {};
                if (ImGui::Button("Add Current Parameters"))
                {
                    if (name[0] != '\0')
                    {
                        AddCurrentParametersIntoCustomParameters(name);
                        name[0] = '\0';
                    }
                }
                ImGui::SameLine();
                ImGui::InputText("Name", name, IM_ARRAYSIZE(name));
                if (ImGui::BeginListBox("##custom-parameters-list", ImVec2(-FLT_MIN, 0.0f)))
                {
                    for (const CustomParameters& customParameters : m_CustomParametersFile.GetCustomParameters())
                    {
                        ImGui::PushID(&customParameters);
                        if (ImGui::Selectable(customParameters.Name.c_str()))
                        {
                            SetCurrentParametersFromCustomParameters(customParameters);
                        }
                        ImGui::PopID();
                    }
                    ImGui::EndListBox();
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
}

void GameplayExternalCamera::SetCurrentParametersFromCustomParameters(const CustomParameters& customParameters)
{
    Core::Pointer parameters = Core::Pointer(0x013FC8E0).deref().at(0x7165C8);

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

void GameplayExternalCamera::AddCurrentParametersIntoCustomParameters(const char* name)
{
    Core::Pointer parameters = Core::Pointer(0x013FC8E0).deref().at(0x7165C8);
    
    CustomParameters customParameters =
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
    };
    
    m_CustomParametersFile.AddCustomParameters(customParameters);
}