#include "Environment.hpp"

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"


void Environment::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Environment"))
    {
        Core::Pointer gameModule = Core::Pointer(0x013FC8E0).deref(); // BrnGame::BrnGameModule*
        
        {
            ImGui::SeparatorText("Traffic Density");

            if (ImGui::Checkbox("Override##traffic-density", &m_OverrideTrafficDensity))
            {
                gameModule.at(0x6E4768).as<bool>() = m_OverrideTrafficDensity;
            }

            if (!m_OverrideTrafficDensity)
            {
                ImGui::BeginDisabled();
            }
            
            ImGui::SliderFloat("Traffic Density", &gameModule.at(0x6E4764).as<float>(), 0.0f, 5.0f);

            if (!m_OverrideTrafficDensity)
            {
                ImGui::EndDisabled();
            }
        }

        {
            ImGui::SeparatorText("Time of Day");

            // TODO
        }

        {
            ImGui::SeparatorText("Weather");

            if (ImGui::Checkbox("Override##weather", &m_OverrideWeather))
            {
                if (!m_OverrideWeather)
                {
                    gameModule.at(0x6E4828).as<int32_t>() = 3;
                }
            }

            if (!m_OverrideWeather)
            {
                ImGui::BeginDisabled();
            }

            static constexpr const char* weatherTypes[] =
            {
                "Sunny",
                "Overcast",
                "Foggy",
            };
            if (ImGui::Combo("Weather Type", &gameModule.at(0x6E4838).as<int32_t>(), weatherTypes, IM_ARRAYSIZE(weatherTypes)))
            {
                gameModule.at(0x6E4834).as<float>() = 5.0f;
                gameModule.at(0x6E4828).as<int32_t>() = 4;
            }

            if (!m_OverrideWeather)
            {
                ImGui::EndDisabled();
            }
        }
    }
}
