#include "DashboardHud.hpp"

#include <DirectXMath.h>

#include "vendor/imgui.hpp"

#include "core/File.hpp"


static constexpr float k_TextureWidth = 488.0f;
static constexpr float k_TextureHeight = 188.0f;


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
    CreateTexture(textureData.data());

    m_Logger.Info("Loaded texture.");
}

void DashboardHud::LoadFonts(const std::string& filePath)
{
    m_Logger.Info("Loading fonts from file '%s' ...", filePath.c_str());
    
    ImGuiIO& io = ImGui::GetIO();
    m_Font11 = io.Fonts->AddFontFromFileTTF(filePath.c_str(), 11.0f);
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

    auto drawText = [=](const ImVec2& position, const char* text, const ImFont* font) -> void
    {
        constexpr ImU32 color = IM_COL32(0x24, 0xFF, 0xFC, 0xC8);

        ImVec2 textSize = font->CalcTextSizeA(font->FontSize, FLT_MAX, 0.0f, text);

        float x = relativePositionX(position.x) - textSize.x / 2.0f;
        float y = relativePositionY(position.y) - textSize.y / 2.0f;
        
        foregroundDrawList->AddText(font, font->FontSize, ImVec2(x, y), color, text);
    };

    auto drawNeedle = [=](const ImVec2& position, float value, float minValue, float maxValue) -> void
    {
        constexpr ImU32 color = IM_COL32(0xFF, 0x26, 0x34, 0xC8);
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
        
        foregroundDrawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color, thickness);
    };

    // Texture
    {
        float l = relativePositionX(-(k_TextureWidth / 2.0f));
        float r = relativePositionX(+(k_TextureWidth / 2.0f));
        float t = relativePositionY(k_TextureHeight);
        float b = relativePositionY(0.0f);

        foregroundDrawList->AddImage(m_TextureView.Get(), ImVec2(l, t), ImVec2(r, b));
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

        drawText(ImVec2(-128.0f, 125.0f), config.MetricUnits ? "km/h" : "mph", m_Font11);
        drawText(ImVec2(-128.0f, 102.0f), speedText, m_Font29);
        drawNeedle(ImVec2(-128.0f, 72.0f), static_cast<float>(speed), 0.0f, 360.0f);
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

        drawText(ImVec2(128.0f, 125.0f), "rpm", m_Font11);
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

void DashboardHud::CreateTexture(Core::Pointer ddsData)
{
    // https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dx-graphics-dds-pguide

    /*
    * DDS texture
    * no compression
    * no mipmaps
    */
    
    uint32_t magic = ddsData.at(0x0).as<uint32_t>();
    if (magic != MAKEFOURCC('D', 'D', 'S', ' '))
    {
        throw std::exception("DDS magic number mismatch.");
    }

    D3D11_TEXTURE2D_DESC textureDesc =
    {
        .Width      = ddsData.at(0x10).as<uint32_t>(),
        .Height     = ddsData.at(0xC).as<uint32_t>(),
        .MipLevels  = 1,
        .ArraySize  = 1,
        .Format     = DXGI_FORMAT_B8G8R8A8_UNORM,
        .SampleDesc =
        {
            .Count   = 1,
            .Quality = 0,
        },
        .Usage      = D3D11_USAGE_DEFAULT,
        .BindFlags  = D3D11_BIND_SHADER_RESOURCE,
    };

    D3D11_SUBRESOURCE_DATA initialData =
    {
        .pSysMem     = ddsData.at(0x80).GetAddress(),
        .SysMemPitch = ddsData.at(0x14).as<uint32_t>(),
    };

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc =
    {
        .Format        = DXGI_FORMAT_B8G8R8A8_UNORM,
        .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
        .Texture2D     =
        {
            .MostDetailedMip = 0,
            .MipLevels       = 1,
        },
    };

    ID3D11Device* device = Core::Pointer(0x01485BF8).as<ID3D11Device*>();

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture = nullptr;
    if (FAILED(device->CreateTexture2D(&textureDesc, &initialData, &texture)))
    {
        throw std::exception("Failed to create D3D11 texture.");
    }
    
    if (FAILED(device->CreateShaderResourceView(texture.Get(), &shaderResourceViewDesc, &m_TextureView)))
    {
        throw std::exception("Failed to create D3D11 shader resource view.");
    }
}
