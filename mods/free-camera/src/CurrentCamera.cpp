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
    m_Properties
    {
        .FOV                 = 0x58,
        .SimulationTimeScale = 0x100,
        .CameraLag           = 0x108,
        .BlackBarAmount      = 0x10C,
        .MotionBlurVehicle   = 0xA8,
        .MotionBlurWorld     = 0xAC,
        .Blurriness          = 0x130,
    }
{
}

void CurrentCamera::OnArbitratorUpdate(
    Core::Pointer camera, // BrnDirector::Camera::Camera*
    Core::Pointer arbStateSharedInfo // BrnDirector::ArbStateSharedInfo*
)
{
    {
        auto updateProperty = [&]<typename T>(Property<T>& property) -> void
        {
            Core::Pointer address = camera.at(property.Offset);

            if (m_Properties.ResetAll)
            {
                property.Override = false;
            }
            
            if (property.Override)
            {
                address.as<T>() = property.Value;
            }
            else
            {
                property.Value = address.as<T>();
            }
        };

        updateProperty(m_Properties.FOV);
        updateProperty(m_Properties.SimulationTimeScale);
        updateProperty(m_Properties.CameraLag);
        updateProperty(m_Properties.BlackBarAmount);
        updateProperty(m_Properties.MotionBlurVehicle);
        updateProperty(m_Properties.MotionBlurWorld);
        updateProperty(m_Properties.Blurriness);

        m_Properties.ResetAll = false;
    }

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
            DirectX::XMStoreFloat3A(&m_Transformation.RotationDelta, DirectX::XMVectorZero());
            DirectX::XMStoreFloat3A(&m_Transformation.Translation, translation);
            DirectX::XMStoreFloat3A(&m_Transformation.TranslationDelta, DirectX::XMVectorZero());
        }
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
        {
            auto renderProperty = []<typename T, typename Fn>(Property<T>& property, Fn widget) -> void
            {
                ImGui::PushID(&property);
            
                if (ImGui::Button("Reset"))
                {
                    property.Override = false;
                }
            
                ImGui::SameLine();
            
                if (widget(property.Value))
                {
                    property.Override = true;
                }
            
                ImGui::PopID();
            };

            ImGui::SeparatorText("Properties");

            {
                if (ImGui::Button("Reset All##properties"))
                {
                    m_Properties.ResetAll = true;
                }
            }

            ImGui::Separator();

            {
                renderProperty(m_Properties.FOV,                 [](float& value) { return ImGui::SliderFloat("FOV", &value, 1.0f, 179.0f); });
                renderProperty(m_Properties.SimulationTimeScale, [](float& value) { return ImGui::SliderFloat("Simulation Time Scale", &value, 0.0f, 2.0f); });
                renderProperty(m_Properties.CameraLag,           [](float& value) { return ImGui::SliderFloat("Camera Lag", &value, 0.0f, 1.0f); });
                renderProperty(m_Properties.BlackBarAmount,      [](float& value) { return ImGui::SliderFloat("Black Bar Amount", &value, 0.0f, 0.5f); });
                renderProperty(m_Properties.MotionBlurVehicle,   [](float& value) { return ImGui::SliderFloat("Vehicle", &value, 0.0f, 1.0f); });
                renderProperty(m_Properties.MotionBlurWorld,     [](float& value) { return ImGui::SliderFloat("World", &value, 0.0f, 1.0f); });
                renderProperty(m_Properties.Blurriness,          [](float& value) { return ImGui::SliderFloat("Blurriness", &value, 0.0f, 1.0f); });
            }
        }

        {
            ImGui::SeparatorText("Transformation");
            
            if (ImGui::Checkbox("Override##transformation", &m_Transformation.Override))
            {
                m_Transformation.Init = true;
            }
            
            ImGui::DragFloat3("Rotate", reinterpret_cast<float*>(&m_Transformation.RotationDelta));
            ImGui::DragFloat3("Translate", reinterpret_cast<float*>(&m_Transformation.TranslationDelta));
        }

        {
            ImGui::SeparatorText("Effect");

            {
                if (ImGui::Button("Stop##effect"))
                {
                    if (m_Effect.Play)
                    {
                        m_Effect.Stop = true;
                    }
                }

                ImGui::SameLine();

                ImGui::SliderFloat("Blend Amount", &m_Effect.BlendAmount, 0.0f, 1.0f);
            }

            ImGui::Separator();
            
            {
                static ImGuiTextFilter effectFilter;
                effectFilter.Draw("Filter##effect");
                
                if (ImGui::BeginListBox("##effects", ImVec2(-FLT_MIN, 0.0f)))
                {
                    Core::Pointer effectInterface = Core::Pointer(0x013FC8E0).deref().at(0x7179D0); // BrnDirector::EffectInterface*

                    uint32_t effectsCount = effectInterface.at(0xCE4).as<uint32_t>();
                    for (uint32_t i = 0; i < effectsCount; ++i)
                    {
                        const char* effectName = effectInterface.at(0x0 + i * 0x21).GetAddress<const char*>();
                        
                        if (effectFilter.PassFilter(effectName))
                        {
                            bool selected = m_Effect.Name == effectName;
                            if (ImGui::Selectable(effectName, selected))
                            {
                                if (!m_Effect.Play)
                                {
                                    m_Effect.Name = effectName;
                                    m_Effect.BlendAmount = 0.75f;
                                    m_Effect.Play = true;
                                }
                            }
                            if (selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                    }
                
                    ImGui::EndListBox();
                }
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
