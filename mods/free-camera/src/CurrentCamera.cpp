#include "CurrentCamera.hpp"

#include "vendor/imgui.hpp"


namespace BPR
{
    // rw::math::vpu::Vector3 BrnDirector::Camera::Utils::EulerAnglesZXYFromMatrix44Affine(rw::math::vpu::Matrix44Affine, rw::math::vpu::Vector3*)
    static void CameraUtils_EulerAnglesZXYFromMatrix44Affine(void* angles, const void* transformation)
    {
        __asm
        {
            push 0
            mov edx, dword ptr [transformation]
            mov ecx, dword ptr [angles]

            mov eax, 0x0094A650
            call eax
            add esp, 4
        }
    }
    
    // void BrnDirector::Camera::EnsureEffectIsPlaying(BrnDirector::Camera::Camera&, const BrnDirector::EffectInterface&, const char*, float)
    static void Camera_EnsureEffectIsPlaying(void* camera, const void* effectInterface, const char* name, float blendAmount)
    {
        __asm
        {
            movss xmm3, dword ptr [blendAmount]
            push dword ptr [name]
            mov edx, dword ptr [effectInterface]
            mov ecx, dword ptr [camera]
            
            mov eax, 0x004D2700
            call eax
            add esp, 4
        }
    }

    // void BrnDirector::Camera::StopCurrentEffect(BrnDirector::Camera::Camera&, const BrnDirector::EffectInterface&)
    static void Camera_StopCurrentEffect(void* camera, const void* effectInterface)
    {
        __asm
        {
            mov edx, dword ptr [effectInterface]
            mov ecx, dword ptr [camera]

            mov eax, 0x004D26A0
            call eax
        }
    }
}


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
    // BrnDirector::Camera::Camera* camera
    // BrnDirector::ArbStateSharedInfo* arbStateSharedInfo

    auto updateProperty = [&]<typename T>(Property<T>& property) -> void
    {
        if (property.Override)
        {
            camera.at(property.Offset).as<T>() = property.Value;
        }
        else
        {
            property.Value = camera.at(property.Offset).as<T>();
        }
    };

    {
        if (m_Transformation.Override)
        {
            DirectX::XMFLOAT4X4& transformation = camera.at(0x0).as<DirectX::XMFLOAT4X4>();

            if (m_Transformation.Init)
            {
                BPR::CameraUtils_EulerAnglesZXYFromMatrix44Affine(&m_Transformation.Rotation, &transformation);

                m_Transformation.Translation.x = transformation(3, 0);
                m_Transformation.Translation.y = transformation(3, 1);
                m_Transformation.Translation.z = transformation(3, 2);

                m_Transformation.Init = false;
            }

            DirectX::XMVECTOR rotation = DirectX::XMLoadFloat3A(&m_Transformation.Rotation);
            DirectX::XMVECTOR rotationDelta = DirectX::XMLoadFloat3A(&m_Transformation.RotationDelta);
            DirectX::XMVECTOR translation = DirectX::XMLoadFloat3A(&m_Transformation.Translation);
            DirectX::XMVECTOR translationDelta = DirectX::XMLoadFloat3A(&m_Transformation.TranslationDelta);

            rotation = DirectX::XMVectorAddAngles(rotation, rotationDelta);
            DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(rotation);

            translation = DirectX::XMVectorAdd(translation, DirectX::XMVector3Transform(translationDelta, rotationMatrix));
            DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(translation);

            DirectX::XMStoreFloat4x4(&transformation, rotationMatrix * translationMatrix);

            DirectX::XMStoreFloat3A(&m_Transformation.Rotation, rotation);
            m_Transformation.RotationDelta = { 0.0f, 0.0f, 0.0f };
            DirectX::XMStoreFloat3A(&m_Transformation.Translation, translation);
            m_Transformation.TranslationDelta = { 0.0f, 0.0f, 0.0f };
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
        if (m_Effect.Play)
        {
            BPR::Camera_EnsureEffectIsPlaying(camera.GetAddress(), arbStateSharedInfo.at(0x30).as<void*>(), m_Effect.Name, m_Effect.BlendAmount);
        }

        if (m_Effect.Stop)
        {
            BPR::Camera_StopCurrentEffect(camera.GetAddress(), arbStateSharedInfo.at(0x30).as<void*>());
            
            m_Effect.Name = nullptr;
            m_Effect.BlendAmount = 0.0f;
            m_Effect.Play = false;
            m_Effect.Stop = false;
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

        {
            ImGui::SeparatorText("Transformation");
            
            if (ImGui::Checkbox("Override", &m_Transformation.Override))
            {
                m_Transformation.Init = true;
            }
            ImGui::DragFloat3("Rotate", reinterpret_cast<float*>(&m_Transformation.RotationDelta));
            ImGui::DragFloat3("Translate", reinterpret_cast<float*>(&m_Transformation.TranslationDelta));
        }

        {
            ImGui::SeparatorText("Misc");
            
            renderProperty(m_Misc.Fov, [](Core::Pointer address) -> bool { return ImGui::SliderFloat("FOV", &address.as<float>(), 1.0f, 179.0f); });
        }

        {
            ImGui::SeparatorText("Effects");
            
            renderProperty(m_Effects.SimulationTimeScale, [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Simulation Time Scale", &address.as<float>(), 0.0f, 2.0f); });
            renderProperty(m_Effects.CameraLag,           [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Camera Lag", &address.as<float>(), 0.0f, 1.0f); });
            renderProperty(m_Effects.BlackBarAmount,      [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Black Bar Amount", &address.as<float>(), 0.0f, 0.5f); });
        }

        {
            ImGui::SeparatorText("Motion Blur");
            
            renderProperty(m_MotionBlur.Vehicle, [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Vehicle", &address.as<float>(), 0.0f, 1.0f); });
            renderProperty(m_MotionBlur.World,   [](Core::Pointer address) -> bool { return ImGui::SliderFloat("World", &address.as<float>(), 0.0f, 1.0f); });
        }

        {
            ImGui::SeparatorText("Depth of Field");
            
            renderProperty(m_DepthOfField.FocusStartDistance,        [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Focus Start Distance", &address.as<float>(), 0.0f, 150.0f); });
            renderProperty(m_DepthOfField.PerfectFocusStartDistance, [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Perfect Focus Start Distance", &address.as<float>(), 0.0f, 150.0f); });
            renderProperty(m_DepthOfField.PerfectFocusEndDistance,   [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Perfect Focus End Distance", &address.as<float>(), 150.0f, 300.0f); });
            renderProperty(m_DepthOfField.FocusEndDistance,          [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Focus End Distance", &address.as<float>(), 150.0f, 300.0f); });
            renderProperty(m_DepthOfField.Blurriness,                [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Blurriness", &address.as<float>(), 0.0f, 1.0f); });
        }

        {
            ImGui::SeparatorText("Effect");

            if (ImGui::Button("Stop"))
            {
                if (m_Effect.Play)
                {
                    m_Effect.Stop = true;
                }
            }

            ImGui::SliderFloat("Blend Amount", &m_Effect.BlendAmount, 0.0f, 1.0f);

            if (ImGui::BeginListBox("##hook-names-list", ImVec2(-FLT_MIN, 0.0f)))
            {
                // BrnDirector::EffectInterface* effectInterface
                
                Core::Pointer effectInterface = Core::Pointer(0x013FC8E0).deref().at(0x7179D0);

                uint32_t hookNamesCount = effectInterface.at(0xCE4).as<uint32_t>();
                for (uint32_t i = 0; i < hookNamesCount; ++i)
                {
                    const char* hookName = effectInterface.at(0x0 + i * 0x21).GetAddress<const char*>();
                    bool selected = m_Effect.Name == hookName;
                    if (ImGui::Selectable(hookName, selected))
                    {
                        if (!m_Effect.Play)
                        {
                            m_Effect.Name = hookName;
                            m_Effect.BlendAmount = 0.75f;
                            m_Effect.Play = true;
                        }
                    }
                    if (selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                
                ImGui::EndListBox();
            }
        }
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
    * mouse movement while the right button is pressed translates the camera sidewards and upwards/downwards
    * mouse wheel translates the camera forwards/backwards
    * shift key increases the mouse input sensitivity
    */

    bool isShiftKeyDown = GetKeyState(VK_SHIFT) & 0x8000;

    if (mouse.usFlags == MOUSE_MOVE_RELATIVE)
    {
        if (GetKeyState(VK_LBUTTON) & 0x8000)
        {
            m_Transformation.RotationDelta.y -= isShiftKeyDown ? (mouse.lLastX / 200.0f) : (mouse.lLastX / 400.0f);
            m_Transformation.RotationDelta.x += isShiftKeyDown ? (mouse.lLastY / 200.0f) : (mouse.lLastY / 400.0f);
        }
        if (GetKeyState(VK_RBUTTON) & 0x8000)
        {
            m_Transformation.TranslationDelta.x -= isShiftKeyDown ? (mouse.lLastX / 10.0f) : (mouse.lLastX / 20.0f);
            m_Transformation.TranslationDelta.y -= isShiftKeyDown ? (mouse.lLastY / 10.0f) : (mouse.lLastY / 20.0f);
        }
    }
    if (mouse.usButtonFlags & RI_MOUSE_WHEEL)
    {
        short scrolls = static_cast<short>(mouse.usButtonData) / WHEEL_DELTA;
        m_Transformation.TranslationDelta.z += isShiftKeyDown ? (scrolls * 4.0f) : (scrolls * 1.0f);
    }
}
