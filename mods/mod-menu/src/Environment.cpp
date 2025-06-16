#include "Environment.hpp"

#include <algorithm>

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"


void Environment::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Environment"))
    {
        Core::Pointer gameModule = Core::Pointer(0x013FC8E0).deref(); // BrnGame::BrnGameModule*
        
        {
            ImGui::SeparatorText("Traffic Density");

            ImGui::Checkbox("Override##traffic-density", &gameModule.at(0x6E4768).as<bool>());
            ImGui::SliderFloat("Traffic Density", &gameModule.at(0x6E4764).as<float>(), 0.0f, 5.0f);
        }

        {
            ImGui::SeparatorText("Time of Day");

            if (ImGui::Button("Change##time-of-day"))
            {
                // TODO
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset##time-of-day"))
            {
                // TODO
            }

            // TODO: better widget - spacing, no buttons, ...
            ImGui::PushItemWidth(68.f);
            if (ImGui::InputInt("##time-of-day-hours", &m_TimeOfDayHours))
            {
                m_TimeOfDayHours = std::clamp(m_TimeOfDayHours, 0, 23);
            }
            ImGui::SameLine();
            ImGui::TextUnformatted(":");
            ImGui::SameLine();
            if (ImGui::InputInt("##time-of-day-minutes", &m_TimeOfDayMinutes))
            {
                m_TimeOfDayMinutes = std::clamp(m_TimeOfDayMinutes, 0, 59);
            }
            ImGui::PopItemWidth();
        }

        {
            ImGui::SeparatorText("Weather");

            if (ImGui::Button("Change##weather"))
            {
                gameModule.at(0x6E4834).as<float>() = 5.0f;
                gameModule.at(0x6E4828).as<int32_t>() = 4;
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset##weather"))
            {
                gameModule.at(0x6E4828).as<int32_t>() = 2;
            }

            static constexpr const char* weatherTypes[] =
            {
                "Sunny",
                "Overcast",
                "Foggy",
            };
            ImGui::Combo("Weather Type", &gameModule.at(0x6E4838).as<int32_t>(), weatherTypes, IM_ARRAYSIZE(weatherTypes));
        }
    }
}
