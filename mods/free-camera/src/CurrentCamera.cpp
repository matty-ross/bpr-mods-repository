#include "CurrentCamera.hpp"

#include <DirectXMath.h>

#include "vendor/imgui.hpp"


CurrentCamera::CurrentCamera()
    :
    m_Misc
    {
        .Fov = 0x58,
    },
    m_Effects
    {
        .SimulationTimeScale = 0x100,
        .CameraLag           = 0x108,
        .BlackBarAmount      = 0x10C
    },
    m_MotionBlur
    {
        .Vehicle = 0xA8,
        .World   = 0xAC
    },
    m_DepthOfField
    {
        .FocusStartDistance        = 0x120,
        .PerfectFocusStartDistance = 0x124,
        .PerfectFocusEndDistance   = 0x128,
        .FocusEndDistance          = 0x12C,
        .Blurriness                = 0x130,
    }
{
}

void CurrentCamera::OnArbitratorUpdate(Core::Pointer camera, Core::Pointer arbStateSharedInfo)
{
    auto updateProperty = [&]<typename T>(Property<T>& property) -> void
    {
        Core::Pointer address = camera.at(property.Offset);

        if (property.Override)
        {
            address.as<T>() = property.Value;
        }
        else
        {
            property.Value = address.as<T>();
        }
    };

    {
        if (m_Transformation.Override)
        {
            DirectX::XMFLOAT4X4& transformation = camera.at(0x0).as<DirectX::XMFLOAT4X4>();
            
            if (m_Transformation.Init)
            {
                DirectX::XMVECTOR scaleVector = {};
                DirectX::XMVECTOR rotationVector = {};
                DirectX::XMVECTOR translationVector = {};
                DirectX::XMMatrixDecompose(&scaleVector, &rotationVector, &translationVector, DirectX::XMLoadFloat4x4(&transformation));

                DirectX::XMFLOAT4 rotation = {};
                DirectX::XMStoreFloat4(&rotation, rotationVector);
                m_Transformation.Rotation[0] = DirectX::XMConvertToDegrees(asinf(2.0f * (rotation.x * rotation.z - rotation.w * rotation.y)));
                m_Transformation.Rotation[1] = DirectX::XMConvertToDegrees(atan2f(2.0f * (rotation.x * rotation.w + rotation.y * rotation.z), 1.0f - 2.0f * (rotation.y * rotation.y + rotation.z * rotation.z)));
                m_Transformation.Rotation[2] = DirectX::XMConvertToDegrees(atan2f(2.0f * (rotation.x * rotation.y + rotation.z * rotation.w), 1.0f - 2.0f * (rotation.y * rotation.y + rotation.z * rotation.z)));
                
                DirectX::XMFLOAT4 translation = {};
                DirectX::XMStoreFloat4(&translation, translationVector);
                m_Transformation.Translation[0] = translation.x;
                m_Transformation.Translation[1] = translation.y;
                m_Transformation.Translation[2] = translation.z;
                
                m_Misc.Fov.Value = 90.0f;
                
                m_Transformation.Init = false;
            }

            m_Transformation.Rotation[0] = fmodf(m_Transformation.Rotation[0] + m_Transformation.RotationDelta[0], 360.f);
            m_Transformation.Rotation[1] = fmodf(m_Transformation.Rotation[1] + m_Transformation.RotationDelta[1], 360.f);
            m_Transformation.Rotation[2] = fmodf(m_Transformation.Rotation[2] + m_Transformation.RotationDelta[2], 360.f);
            DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(
                DirectX::XMConvertToRadians(m_Transformation.Rotation[0]),
                DirectX::XMConvertToRadians(m_Transformation.Rotation[1]),
                DirectX::XMConvertToRadians(m_Transformation.Rotation[2])
            );
            
            DirectX::XMFLOAT3 translationDelta = {};
            DirectX::XMStoreFloat3(
                &translationDelta,
                DirectX::XMVector3Transform(
                    DirectX::XMVectorSet(
                        m_Transformation.TranslationDelta[0],
                        m_Transformation.TranslationDelta[1],
                        m_Transformation.TranslationDelta[2],
                        0.0f
                    ),
                    rotationMatrix
                )
            );
            m_Transformation.Translation[0] += translationDelta.x;
            m_Transformation.Translation[1] += translationDelta.y;
            m_Transformation.Translation[2] += translationDelta.z;
            DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(
                m_Transformation.Translation[0],
                m_Transformation.Translation[1],
                m_Transformation.Translation[2]
            );
            
            DirectX::XMStoreFloat4x4(&transformation, rotationMatrix * translationMatrix);

            for (int i = 0; i < 3; ++i)
            {
                m_Transformation.RotationDelta[i] = 0.0f;
                m_Transformation.TranslationDelta[i] = 0.0f;
            }
        }
    }
    
    {
        updateProperty(m_Misc.Fov);
    }

    {
        updateProperty(m_Effects.SimulationTimeScale);
        updateProperty(m_Effects.CameraLag);
        updateProperty(m_Effects.BlackBarAmount);
    }

    {
        updateProperty(m_MotionBlur.Vehicle);
        updateProperty(m_MotionBlur.World);
    }

    {
        updateProperty(m_DepthOfField.FocusStartDistance);
        updateProperty(m_DepthOfField.PerfectFocusStartDistance);
        updateProperty(m_DepthOfField.PerfectFocusEndDistance);
        updateProperty(m_DepthOfField.FocusEndDistance);
        updateProperty(m_DepthOfField.Blurriness);
    }

    {
        Core::Pointer backgroundEffectRequest = camera.at(0xB4);
        
        if (m_BackgroundEffect.Active)
        {
            strcpy_s(backgroundEffectRequest.at(0x0).GetAddress<char*>(), 32, m_BackgroundEffect.HookName);
            backgroundEffectRequest.at(0x24).as<float>() = m_BackgroundEffect.BlendAmount;
            backgroundEffectRequest.at(0x28).as<bool>() = true;
        }
        
        if (m_BackgroundEffect.Stop)
        {
            backgroundEffectRequest.at(0x29).as<bool>() = true;
            m_BackgroundEffect.HookName = nullptr;
            m_BackgroundEffect.BlendAmount = 0.0f;
            m_BackgroundEffect.Active = false;
            m_BackgroundEffect.Stop = false;
        }
    }
}

void CurrentCamera::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Current Camera"))
    {
        using WidgetFn = bool(*)(Core::Pointer);
        auto renderProperty = []<typename T>(Property<T>& property, WidgetFn widget) -> void
        {
            ImGui::PushID(&property);
            if (ImGui::Button("Reset"))
            {
                property.Override = false;
            }
            ImGui::SameLine();
            if (widget(&property.Value))
            {
                property.Override = true;
            }
            ImGui::PopID();
        };
        
        ImGui::SeparatorText("Transformation");
        if (ImGui::Checkbox("Override", &m_Transformation.Override))
        {
            m_Transformation.Init = true;
            m_Misc.Fov.Override = m_Transformation.Override;
        }
        ImGui::DragFloat3("Rotation", m_Transformation.Rotation);
        ImGui::DragFloat3("Translation", m_Transformation.Translation);

        ImGui::SeparatorText("Misc");
        renderProperty(m_Misc.Fov,        [](Core::Pointer address) -> bool { return ImGui::SliderFloat("FOV", &address.as<float>(), 1.0f, 179.0f); });
        
        ImGui::SeparatorText("Effects");
        renderProperty(m_Effects.SimulationTimeScale, [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Simulation Time Scale", &address.as<float>(), 0.0f, 2.0f); });
        renderProperty(m_Effects.CameraLag,           [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Camera Lag", &address.as<float>(), 0.0f, 1.0f); });
        renderProperty(m_Effects.BlackBarAmount,      [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Black Bar Amount", &address.as<float>(), 0.0f, 0.5f); });

        ImGui::SeparatorText("Motion Blur");
        renderProperty(m_MotionBlur.Vehicle, [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Vehicle", &address.as<float>(), 0.0f, 1.0f); });
        renderProperty(m_MotionBlur.World,   [](Core::Pointer address) -> bool { return ImGui::SliderFloat("World", &address.as<float>(), 0.0f, 1.0f); });

        ImGui::SeparatorText("Depth of Field");
        renderProperty(m_DepthOfField.FocusStartDistance,        [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Focus Start Distance", &address.as<float>(), 0.0f, 150.0f); });
        renderProperty(m_DepthOfField.PerfectFocusStartDistance, [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Perfect Focus Start Distance", &address.as<float>(), 0.0f, 150.0f); });
        renderProperty(m_DepthOfField.PerfectFocusEndDistance,   [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Perfect Focus End Distance", &address.as<float>(), 150.0f, 300.0f); });
        renderProperty(m_DepthOfField.FocusEndDistance,          [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Focus End Distance", &address.as<float>(), 150.0f, 300.0f); });
        renderProperty(m_DepthOfField.Blurriness,                [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Blurriness", &address.as<float>(), 0.0f, 1.0f); });

        ImGui::SeparatorText("Background Effect");
        if (ImGui::Button("Stop"))
        {
            if (m_BackgroundEffect.Active)
            {
                m_BackgroundEffect.Stop = true;
            }
        }
        if (ImGui::BeginListBox("##hook-names-list", ImVec2(-FLT_MIN, 0.0f)))
        {
            Core::Pointer effectInterface = Core::Pointer(0x013FC8E0).deref().at(0x7179D0);

            uint32_t hookNamesCount = effectInterface.at(0xCE4).as<uint32_t>();
            for (uint32_t i = 0; i < hookNamesCount; ++i)
            {
                const char* hookName = effectInterface.at(0x0 + i * 0x21).GetAddress<const char*>();
                if (ImGui::Selectable(hookName, m_BackgroundEffect.HookName == hookName))
                {
                    if (!m_BackgroundEffect.Active)
                    {
                        m_BackgroundEffect.HookName = hookName;
                        m_BackgroundEffect.BlendAmount = 0.75f;
                        m_BackgroundEffect.Active = true;
                    }
                }
            }
            ImGui::EndListBox();
        }
        ImGui::SliderFloat("Blend Amount", &m_BackgroundEffect.BlendAmount, 0.0f, 1.0f);
    }
}

void CurrentCamera::OnMouseInput(const RAWMOUSE& mouse)
{
    if (!m_Transformation.Override || ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    /*
    * mouse movement while the left button is presed rotates the camera along the Y and X axis
    * mouse movement while the right button is pressed translates the camera sidewards and upwards
    * mouse wheel translates the camera forwards and backwards
    * shift key increases the mouse input sensitivity
    */

    bool isShiftKeyDown = GetKeyState(VK_SHIFT) & 0x8000;

    if (mouse.usFlags == MOUSE_MOVE_RELATIVE)
    {
        if (GetKeyState(VK_LBUTTON) & 0x8000)
        {
            m_Transformation.RotationDelta[1] -= isShiftKeyDown ? mouse.lLastX : (mouse.lLastX / 2.0f);
            m_Transformation.RotationDelta[0] += isShiftKeyDown ? mouse.lLastY : (mouse.lLastY / 2.0f);
        }
        if (GetKeyState(VK_RBUTTON) & 0x8000)
        {
            m_Transformation.TranslationDelta[0] -= isShiftKeyDown ? mouse.lLastX : (mouse.lLastX / 2.0f);
            m_Transformation.TranslationDelta[1] -= isShiftKeyDown ? mouse.lLastY : (mouse.lLastY / 2.0f);
        }
    }
    if (mouse.usButtonFlags & RI_MOUSE_WHEEL)
    {
        short scrolls = static_cast<short>(mouse.usButtonData) / WHEEL_DELTA;
        m_Transformation.TranslationDelta[2] += isShiftKeyDown ? (scrolls * 4.0f) : scrolls;
    }
}
