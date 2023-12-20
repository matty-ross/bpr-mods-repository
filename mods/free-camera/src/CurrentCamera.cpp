#include "CurrentCamera.h"

#include <Windowsx.h>
#include <DirectXMath.h>

#include "imgui/imgui.h"


CurrentCamera::CurrentCamera()
    :
    m_Misc
    {
        .Fov        = 0x58,
        .Blurriness = 0x130,
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
    }
{
}

void CurrentCamera::OnUpdate(Core::Pointer camera, Core::Pointer sharedInfo)
{
    auto updateProperty = [camera]<typename T>(Property<T>& property) -> void
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
            DirectX::XMFLOAT4X4& transform = camera.at(0x0).as<DirectX::XMFLOAT4X4>();
            
            if (m_Transformation.Init)
            {
                for (int i = 0; i < 3; ++i)
                {
                    m_Transformation.Rotation[i] = 0.0f;
                    m_Transformation.RotationDelta[i] = 0.0f;
                    m_Transformation.Translation[i] = 0.0f;
                    m_Transformation.TranslationDelta[i] = transform(3, i);
                }
                m_Transformation.Init = false;
                
                m_Misc.Fov.Value = 90.0f;
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
            
            DirectX::XMStoreFloat4x4(&transform, rotationMatrix * translationMatrix);

            for (int i = 0; i < 3; ++i)
            {
                m_Transformation.RotationDelta[i] = 0.0f;
                m_Transformation.TranslationDelta[i] = 0.0f;
            }
        }
    }
    
    {
        updateProperty(m_Misc.Fov);
        updateProperty(m_Misc.Blurriness);
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
        ImGui::SameLine();
        ImGui::Checkbox("Use Mouse", &m_Transformation.UseMouse);
        ImGui::DragFloat3("Rotation", m_Transformation.RotationDelta);
        ImGui::DragFloat3("Translation", m_Transformation.TranslationDelta);

        ImGui::SeparatorText("Misc");
        renderProperty(m_Misc.Fov,        [](Core::Pointer address) -> bool { return ImGui::SliderFloat("FOV", &address.as<float>(), 1.0f, 179.0f); });
        renderProperty(m_Misc.Blurriness, [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Blurriness", &address.as<float>(), 0.0f, 1.0f); });
        
        ImGui::SeparatorText("Effects");
        renderProperty(m_Effects.SimulationTimeScale, [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Simulation Time Scale", &address.as<float>(), 0.0f, 2.0f); });
        renderProperty(m_Effects.CameraLag,           [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Camera Lag", &address.as<float>(), 0.0f, 1.0f); });
        renderProperty(m_Effects.BlackBarAmount,      [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Black Bar Amount", &address.as<float>(), 0.0f, 0.5f); });

        ImGui::SeparatorText("Motion Blur");
        renderProperty(m_MotionBlur.Vehicle, [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Vehicle", &address.as<float>(), 0.0f, 1.0f); });
        renderProperty(m_MotionBlur.World,   [](Core::Pointer address) -> bool { return ImGui::SliderFloat("World", &address.as<float>(), 0.0f, 1.0f); });

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

void CurrentCamera::OnWindowMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (!m_Transformation.UseMouse || ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    switch (Msg)
    {
    case WM_MOUSEMOVE:
        {
            bool leftMouseButtonDown = GET_KEYSTATE_WPARAM(wParam) == MK_LBUTTON;
            float x = static_cast<float>(GET_X_LPARAM(lParam));
            float y = static_cast<float>(GET_Y_LPARAM(lParam));

            if (leftMouseButtonDown)
            {
                float deltaX = x - m_MouseData.PreviousPositionX;
                float deltaY = y - m_MouseData.PreviousPositionY;
                m_Transformation.RotationDelta[1] -= deltaX;
                m_Transformation.RotationDelta[0] += deltaY;
            }
            
            m_MouseData.PreviousPositionX = x;
            m_MouseData.PreviousPositionY = y;
        }
        break;

    case WM_MOUSEWHEEL:
        {
            bool ctrlKeyDown = GET_KEYSTATE_WPARAM(wParam) == MK_CONTROL;
            float delta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
            
            m_Transformation.TranslationDelta[2] += ctrlKeyDown ? delta * 4.0f : delta;
        }
        break;
    }
}