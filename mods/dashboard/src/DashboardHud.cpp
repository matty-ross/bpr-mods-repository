#include "DashboardHud.hpp"

#include <DirectXMath.h>

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"
#include "core/File.hpp"


DashboardHud::DashboardHud(DashboardConfigFile& dashboardConfigFile, const Core::Logger& logger)
    :
    m_DashboardConfigFile(dashboardConfigFile),
    m_Logger(logger)
{
}

void DashboardHud::LoadTexture(const std::string& filePath)
{
    m_Logger.Info("Loading texture from file '%s' ...", filePath.c_str());
    
    Core::File file(filePath, Core::File::Mode::Read);
    std::vector<BYTE> textureData = file.ReadBinary();
    m_DashboardTexture.CreateTexture(textureData.data());

    m_Logger.Info("Loaded texture.");
}

void DashboardHud::LoadFonts(const std::string& filePath)
{
    m_Logger.Info("Loading fonts from file '%s' ...", filePath.c_str());
    
    ImGuiIO& io = ImGui::GetIO();
    m_Font24 = io.Fonts->AddFontFromFileTTF(filePath.c_str(), 24.0f);
    m_Font29 = io.Fonts->AddFontFromFileTTF(filePath.c_str(), 29.0f);
    m_Font37 = io.Fonts->AddFontFromFileTTF(filePath.c_str(), 37.0f);

    m_Logger.Info("Loaded fonts.");
}

void DashboardHud::OnProgressionAddDistanceDriven(float distance, int32_t vehicleType)
{
    // Distance is in meters.

    DashboardConfig& config = m_DashboardConfigFile.GetDashboardConfig();
    
    if (vehicleType != -1)
    {
        m_TripMeter += distance;
        config.Odometer += distance;
    }
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
            
            ImGui::Checkbox("Metric Units", &config.MetricUnits);

            // TODO: other config
        }
    }
}

void DashboardHud::OnRenderOverlay()
{
    Core::Pointer raceMainHudState = Core::Pointer(0x013FC8E0).deref().at(0x7FABBC).as<void*>(); // BrnGui::RaceMainHudState*
    
    bool inRaceHud = raceMainHudState.at(0x14C).as<bool>();
    if (!inRaceHud)
    {
        return;
    }
    
    DashboardConfig& config = m_DashboardConfigFile.GetDashboardConfig();

    Core::Pointer guiPlayerInfo = Core::Pointer(0x013FC8E0).deref().at(0x8EFEC0); // BrnGui::GuiPlayerInfo*

    ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    ImDrawList* foregroundDrawList = ImGui::GetForegroundDrawList();

    auto relativePositionX = [=](float position) -> float { return mainViewport->Pos.x + mainViewport->Size.x / 2.0f + position; };
    auto relativePositionY = [=](float position) -> float { return mainViewport->Pos.y + mainViewport->Size.y - position; };

    auto drawTextureSegment = [=](const ImVec2& position, DashboardTexture::TextureSegment textureSegment, bool useColor = true) -> void
    {
        constexpr ImVec2 segmentSize = ImVec2(256.0f, 256.0f);
        
        DashboardTexture::TextureSegmentUVs uv = m_DashboardTexture.GetTextureSegmentUVs(textureSegment);

        float l = relativePositionX(position.x) - segmentSize.x / 2.0f;
        float r = relativePositionX(position.x) + segmentSize.x / 2.0f;
        float t = relativePositionY(position.y) - segmentSize.y / 2.0f;
        float b = relativePositionY(position.y) + segmentSize.y / 2.0f;

        ImColor color = useColor ? config.Colors.Dial : IM_COL32_WHITE;
        color.Value.w = config.Opacity / 100.0f;
        
        foregroundDrawList->AddImage(m_DashboardTexture.GetTextureView(), ImVec2(l, t), ImVec2(r, b), ImVec2(uv.Left, uv.Top), ImVec2(uv.Right, uv.Bottom), color);
    };

    auto drawText = [=](const ImVec2& position, const char* text, const ImFont* font) -> void
    {
        ImVec2 textSize = font->CalcTextSizeA(font->FontSize, FLT_MAX, 0.0f, text);

        float x = relativePositionX(position.x) - textSize.x / 2.0f;
        float y = relativePositionY(position.y) - textSize.y / 2.0f;

        ImColor color = config.Colors.Text;
        color.Value.w = config.Opacity / 100.0f;
        
        foregroundDrawList->AddText(font, font->FontSize, ImVec2(x, y), color, text);
    };

    auto drawNeedle = [=](const ImVec2& position, float value, float minValue, float maxValue) -> void
    {
        constexpr float thickness = 3.0f;
        constexpr float minAngle = DirectX::XMConvertToRadians(-225);
        constexpr float maxAngle = DirectX::XMConvertToRadians(45);
        constexpr float innerRadius = 68.0f;
        constexpr float outerRadius = 103.0f;

        float angle = minAngle + (maxAngle - minAngle) * ((value - minValue) / (maxValue - minValue));
        float angleSin = 0.0f, angleCos = 0.0f;
        DirectX::XMScalarSinCos(&angleSin, &angleCos, angle);

        float x1 = relativePositionX(position.x) + angleCos * innerRadius;
        float y1 = relativePositionY(position.y) + angleSin * innerRadius;
        float x2 = relativePositionX(position.x) + angleCos * outerRadius;
        float y2 = relativePositionY(position.y) + angleSin * outerRadius;
        
        ImColor color = config.Colors.Needle;
        color.Value.w = config.Opacity / 100.0f;
        
        foregroundDrawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color, thickness);
    };

    // Speed
    {
        int32_t speed = abs(guiPlayerInfo.at(0x30).as<int32_t>());
        if (config.MetricUnits)
        {
            speed = static_cast<int32_t>(roundf(speed * 1.609f));
        }
        
        char speedText[8] = {};
        sprintf_s(speedText, "%d", speed);

        drawTextureSegment(ImVec2(-128.0f, 72.0f), DashboardTexture::TextureSegment::Background, false);
        drawTextureSegment(ImVec2(-128.0f, 72.0f), config.MetricUnits ? DashboardTexture::TextureSegment::KMH : DashboardTexture::TextureSegment::MPH);
        drawText(ImVec2(-128.0f, 102.0f), speedText, m_Font29);
        drawNeedle(ImVec2(-128.0f, 72.0f), static_cast<float>(speed), 0.0f, config.MetricUnits ? 360.0f : 240.0f);
    }

    // Trip meter
    {
        float tripMeter = config.MetricUnits ? (m_TripMeter / 1000.0f) : (m_TripMeter / 1609.0f);

        char tripMeterText[16] = {};
        sprintf_s(tripMeterText, "%.1f", tripMeter);

        drawText(ImVec2(-128.0f, 69.0f), tripMeterText, m_Font24);
    }

    // Odometer
    {
        int32_t odometer = static_cast<int32_t>(config.MetricUnits ? (config.Odometer / 1000.0f) : (config.Odometer / 1609.0f));

        char odometerText[16] = {};
        sprintf_s(odometerText, "%06d", odometer);

        drawText(ImVec2(-128.0f, 42.0f), odometerText, m_Font29);
    }

    // RPM
    {
        int32_t rpm = guiPlayerInfo.at(0x34).as<int32_t>();

        char rpmText[8] = {};
        sprintf_s(rpmText, "%d", rpm);

        drawTextureSegment(ImVec2(128.0f, 72.0f), DashboardTexture::TextureSegment::Background, false);
        drawTextureSegment(ImVec2(128.0f, 72.0f), DashboardTexture::TextureSegment::RPM);
        drawText(ImVec2(128.0f, 102.0f), rpmText, m_Font29);
        drawNeedle(ImVec2(128.0f, 72.0f), static_cast<float>(rpm), 0.0f, 12000.0f);
    }

    // Gear
    {
        int32_t gear = guiPlayerInfo.at(0x38).as<int32_t>();

        static constexpr char gears[][2] = { "R", "1", "2", "3", "4", "5" };

        drawText(ImVec2(128.0f, 70.0f), gears[gear], m_Font37);
    }

    // Local Time
    {
        SYSTEMTIME localTime = {};
        GetLocalTime(&localTime);

        char localTimeText[8] = {};
        sprintf_s(localTimeText, "%02d:%02d", localTime.wHour, localTime.wMinute);

        drawText(ImVec2(128.0f, 42.0f), localTimeText, m_Font29);
    }
}
