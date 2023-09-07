#include "CurrentCamera.h"

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

        ImGui::SeparatorText("Misc");
        renderProperty(m_Misc.Fov,        [](Core::Pointer address) -> bool { return ImGui::SliderFloat("FOV", &address.as<float>(), 1.0f, 179.0f); });
        renderProperty(m_Misc.Blurriness, [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Blurriness", &address.as<float>(), 0.0f, 1.0f); });
        
        ImGui::SeparatorText("Effects");
        renderProperty(m_Effects.SimulationTimeScale, [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Simulation Time Scale", &address.as<float>(), 0.1f, 2.0f); });
        renderProperty(m_Effects.CameraLag,           [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Camera Lag", &address.as<float>(), 0.0f, 1.0f); });
        renderProperty(m_Effects.BlackBarAmount,      [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Black Bar Amount", &address.as<float>(), 0.0f, 0.5f); });

        ImGui::SeparatorText("Motion Blur");
        renderProperty(m_MotionBlur.Vehicle, [](Core::Pointer address) -> bool { return ImGui::SliderFloat("Vehicle", &address.as<float>(), 0.0f, 1.0f); });
        renderProperty(m_MotionBlur.World,   [](Core::Pointer address) -> bool { return ImGui::SliderFloat("World", &address.as<float>(), 0.0f, 1.0f); });
    }
}