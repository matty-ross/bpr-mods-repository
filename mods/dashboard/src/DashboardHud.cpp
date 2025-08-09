#include "DashboardHud.hpp"

#include <vector>
#include <algorithm>
#include <DirectXMath.h>

#include "core/File.hpp"


static ImVec2 GetAbsolutePosition(const ImVec2& position, float scale)
{
    ImGuiViewport* mainViewport = ImGui::GetMainViewport();

    float x = mainViewport->Pos.x + mainViewport->Size.x / 2.0f + position.x * scale;
    float y = mainViewport->Pos.y + mainViewport->Size.y - position.y * scale;

    return ImVec2(x, y);
}

static ImColor ApplyOpacityToColor(ImColor color, float opacity)
{
    color.Value.w = opacity / 100.0f;
    return color;
}


DashboardHud::DashboardHud(DashboardConfigFile& dashboardConfigFile, const Core::Logger& logger)
    :
    m_DashboardConfigFile(dashboardConfigFile),
    m_Logger(logger)
{
}

void DashboardHud::LoadTexture(const char* filePath)
{
    m_Logger.Info("Loading texture from file '%s' ...", filePath);
    
    Core::File file(filePath, Core::File::Mode::Read);
    std::vector<BYTE> textureData = file.Read<std::vector<BYTE>>();
    m_DashboardTexture.CreateTexture(textureData.data());

    m_Logger.Info("Loaded texture.");
}

void DashboardHud::LoadFont(const char* filePath)
{
    m_Logger.Info("Loading font from file '%s' ...", filePath);
    
    ImGuiIO& io = ImGui::GetIO();
    m_Font = io.Fonts->AddFontFromFileTTF(filePath);

    m_Logger.Info("Loaded font.");
}

void DashboardHud::OnProgressionAddDistanceDriven(
    Core::Pointer progressionManager, // BrnProgression::ProgressionManager*
    float distance, // In meters.
    int32_t vehicleType
)
{
    static constexpr ptrdiff_t offsets[] =
    {
        0x1EC,   // Distance driven online.
        0x1F8,   // Distance driven offline.
        0x29314, // Distance driven offline on BSI.
        0x29320, // Distance driven online on BSI.
    };
    
    float odometer = 0.0f;
    for (ptrdiff_t offset : offsets)
    {
        for (int i = 0; i < 3; ++i)
        {
            odometer += progressionManager.at(offset).as<float[3]>()[i];
        }
    }

    m_TripMeter += distance;
    m_Odometer = odometer + distance;
}

void DashboardHud::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Config"))
    {
        {
            if (ImGui::Button("Save##dashboard-config-file"))
            {
                m_DashboardConfigFile.Save();
            }

            ImGui::SameLine();

            if (ImGui::Button("Load##dashboard-config-file"))
            {
                m_DashboardConfigFile.Load();
            }
        }

        ImGui::Separator();

        {
            DashboardConfig& config = m_DashboardConfigFile.GetDashboardConfig();
            
            ImGui::Checkbox("Always Visible", &config.AlwaysVisible);
            ImGui::Checkbox("Metric Units", &config.MetricUnits);
            ImGui::SliderFloat("Opacity", &config.Opacity, 0.0f, 100.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat("Scale", &config.Scale, 0.1f, 5.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);

            {
                auto renderColorEdit = [](const char* label, uint32_t& configColor)
                {
                    ImColor color = configColor;
                    if (ImGui::ColorEdit3(label, reinterpret_cast<float*>(&color.Value)))
                    {
                        configColor = color;
                    }
                };
                
                ImGui::SeparatorText("Colors");

                renderColorEdit("Dial", config.Colors.Dial);
                renderColorEdit("Text", config.Colors.Text);
                renderColorEdit("Needle", config.Colors.Needle);
            }
        }
    }
}

void DashboardHud::OnRenderOverlay()
{
    const DashboardConfig& config = m_DashboardConfigFile.GetDashboardConfig();
    
    Core::Pointer guiPlayerInfo = Core::Pointer(0x013FC8E0).deref().at(0x8EFEC0); // BrnGui::GuiPlayerInfo*
    Core::Pointer raceMainHudState = Core::Pointer(0x013FC8E0).deref().at(0x7FABBC).as<void*>(); // BrnGui::RaceMainHudState*
    
    if (!config.AlwaysVisible)
    {
        bool inRaceHud = raceMainHudState.at(0x14C).as<bool>();
        int32_t engineState = guiPlayerInfo.at(0x48).as<int32_t>();
        if (!inRaceHud || engineState == 0) // BrnGui::GuiPlayerEngineEvent::EEngineState::E_ENGINE_OFF
        {
            return;
        }
    }

    // Speed
    {
        int32_t speed = abs(guiPlayerInfo.at(0x30).as<int32_t>());
        if (config.MetricUnits)
        {
            speed = static_cast<int32_t>(roundf(speed * 1.609f));
        }
        
        char speedText[8] = {};
        sprintf_s(speedText, "%d", speed);

        RenderTextureSegment(ImVec2(-128.0f, 72.0f), DashboardTexture::TextureSegment::Background, false);
        RenderTextureSegment(ImVec2(-128.0f, 72.0f), config.MetricUnits ? DashboardTexture::TextureSegment::KMH : DashboardTexture::TextureSegment::MPH);
        RenderText(ImVec2(-128.0f, 102.0f), speedText, 29.0f);
        RenderNeedle(ImVec2(-128.0f, 72.0f), static_cast<float>(speed), 0.0f, config.MetricUnits ? 360.0f : 240.0f);
    }

    // Trip meter
    {
        float tripMeter = config.MetricUnits ? (m_TripMeter / 1000.0f) : (m_TripMeter / 1609.0f);

        char tripMeterText[16] = {};
        sprintf_s(tripMeterText, "%.1f", tripMeter);

        RenderText(ImVec2(-128.0f, 69.0f), tripMeterText, 24.0f);
    }

    // Odometer
    {
        int32_t odometer = static_cast<int32_t>(config.MetricUnits ? (m_Odometer / 1000.0f) : (m_Odometer / 1609.0f));

        char odometerText[16] = {};
        sprintf_s(odometerText, "%06d", odometer);

        RenderText(ImVec2(-128.0f, 42.0f), odometerText, 29.0f);
    }

    // RPM
    {
        int32_t rpm = guiPlayerInfo.at(0x34).as<int32_t>();

        char rpmText[8] = {};
        sprintf_s(rpmText, "%d", rpm);

        RenderTextureSegment(ImVec2(128.0f, 72.0f), DashboardTexture::TextureSegment::Background, false);
        RenderTextureSegment(ImVec2(128.0f, 72.0f), DashboardTexture::TextureSegment::RPM);
        RenderText(ImVec2(128.0f, 102.0f), rpmText, 29.0f);
        RenderNeedle(ImVec2(128.0f, 72.0f), static_cast<float>(rpm), 0.0f, 12000.0f);
    }

    // Gear
    {
        int32_t gear = guiPlayerInfo.at(0x38).as<int32_t>();

        static constexpr char gears[][2] = { "R", "1", "2", "3", "4", "5" };

        RenderText(ImVec2(128.0f, 70.0f), gears[gear], 37.0f);
    }

    // Local Time
    {
        SYSTEMTIME localTime = {};
        GetLocalTime(&localTime);

        char localTimeText[8] = {};
        sprintf_s(localTimeText, "%02d:%02d", localTime.wHour, localTime.wMinute);

        RenderText(ImVec2(128.0f, 42.0f), localTimeText, 29.0f);
    }
}

void DashboardHud::RenderTextureSegment(const ImVec2& position, DashboardTexture::TextureSegment textureSegment, bool useColor) const
{
    const DashboardConfig& config = m_DashboardConfigFile.GetDashboardConfig();
    
    ImVec2 segmentSize = ImVec2(256.0f * config.Scale, 256.0f * config.Scale);
    ImColor color = ApplyOpacityToColor(useColor ? config.Colors.Dial : IM_COL32_WHITE, config.Opacity);

    DashboardTexture::TextureSegmentUVs uv = m_DashboardTexture.GetTextureSegmentUVs(textureSegment);

    ImVec2 absolutePosition = GetAbsolutePosition(position, config.Scale);
    float l = absolutePosition.x - segmentSize.x / 2.0f;
    float r = absolutePosition.x + segmentSize.x / 2.0f;
    float t = absolutePosition.y - segmentSize.y / 2.0f;
    float b = absolutePosition.y + segmentSize.y / 2.0f;
    
    ImDrawList* foregroundDrawList = ImGui::GetForegroundDrawList();
    foregroundDrawList->AddImage(reinterpret_cast<ImTextureID>(m_DashboardTexture.GetTextureView()), ImVec2(l, t), ImVec2(r, b), ImVec2(uv.Left, uv.Top), ImVec2(uv.Right, uv.Bottom), color);
}

void DashboardHud::RenderText(const ImVec2& position, const char* text, float size) const
{
    const DashboardConfig& config = m_DashboardConfigFile.GetDashboardConfig();
    
    float fontSize = size * config.Scale;
    ImColor color = ApplyOpacityToColor(config.Colors.Text, config.Opacity);

    ImVec2 textSize = m_Font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text);
    
    ImVec2 absolutePosition = GetAbsolutePosition(position, config.Scale);
    float x = absolutePosition.x - textSize.x / 2.0f;
    float y = absolutePosition.y - textSize.y / 2.0f;

    ImDrawList* foregroundDrawList = ImGui::GetForegroundDrawList();
    foregroundDrawList->AddText(m_Font, fontSize, ImVec2(x, y), color, text);
}

void DashboardHud::RenderNeedle(const ImVec2& position, float value, float minValue, float maxValue) const
{
    const DashboardConfig& config = m_DashboardConfigFile.GetDashboardConfig();
    
    constexpr float minAngle = DirectX::XMConvertToRadians(-225.0f);
    constexpr float maxAngle = DirectX::XMConvertToRadians(45.0f);
    float thickness = 3.0f * config.Scale;
    float innerRadius = 68.0f * config.Scale;
    float outerRadius = 103.0f * config.Scale;
    ImColor color = ApplyOpacityToColor(config.Colors.Needle, config.Opacity);

    value = std::clamp(value, minValue, maxValue);
    float angle = minAngle + (maxAngle - minAngle) * ((value - minValue) / (maxValue - minValue));
    float angleSin = 0.0f, angleCos = 0.0f;
    DirectX::XMScalarSinCos(&angleSin, &angleCos, angle);

    ImVec2 absolutePosition = GetAbsolutePosition(position, config.Scale);
    float x1 = absolutePosition.x + angleCos * innerRadius;
    float y1 = absolutePosition.y + angleSin * innerRadius;
    float x2 = absolutePosition.x + angleCos * outerRadius;
    float y2 = absolutePosition.y + angleSin * outerRadius;

    ImDrawList* foregroundDrawList = ImGui::GetForegroundDrawList();
    foregroundDrawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color, thickness);
}
