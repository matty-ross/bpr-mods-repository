#include "DashboardHud.hpp"

#include <DirectXMath.h>

#include "vendor/imgui.hpp"

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
    CreateTexture(textureData.data());

    m_Logger.Info("Loaded texture.");
}

void DashboardHud::LoadFonts(const std::string& filePath)
{
    m_Logger.Info("Loading fonts from file '%s' ...", filePath.c_str());
    
    ImGuiIO& io = ImGui::GetIO();
    m_Font24 = io.Fonts->AddFontFromFileTTF(filePath.c_str(), 24.0f);
    m_Font29 = io.Fonts->AddFontFromFileTTF(filePath.c_str(), 29.0f);
    m_Font37 = io.Fonts->AddFontFromFileTTF(filePath.c_str(), 37.0f);

    ImGui_ImplDX11_InvalidateDeviceObjects();

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
    // TODO: ImGui window is just here for debugging purposes
    ImGui::Begin("Dashboard debug");

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
        float l = relativePositionX(-(m_TextureWidth / 2.0f));
        float r = relativePositionX(+(m_TextureWidth / 2.0f));
        float t = relativePositionY(static_cast<float>(m_TextureHeight));
        float b = relativePositionY(0.0f);

        foregroundDrawList->AddImage(m_TextureView.Get(), ImVec2(l, t), ImVec2(r, b));
    }

    // Speed
    {
        ImGui::SeparatorText("Speed");
        
        int32_t speed = abs(guiPlayerInfo.at(0x30).as<int32_t>());
        if (config.MetricUnits)
        {
            speed = static_cast<int32_t>(roundf(speed * 1.609f));
        }
        
        char speedText[8] = {};
        sprintf_s(speedText, "%d", speed);

        static ImVec2 posText(-128.0f, 102.0f);
        static ImVec2 posNeedle(-128.0f, 72.0f);
        ImGui::SliderFloat2("Pos text##speed", reinterpret_cast<float*>(&posText), -300.0f, 300.0f);
        ImGui::SliderFloat2("Pos needle##speed", reinterpret_cast<float*>(&posNeedle), -300.0f, 300.0f);

        drawText(posText, speedText, m_Font29);
        drawNeedle(posNeedle, static_cast<float>(speed), 0.0f, 360.0f);
    }

    // Trip meter
    {
        ImGui::SeparatorText("Trip Meter");

        float tripMeter = config.MetricUnits ? (m_TripMeter / 1000.0f) : (m_TripMeter / 1609.0f);

        char tripMeterText[16] = {};
        sprintf_s(tripMeterText, "%.1f", tripMeter);

        static ImVec2 posText(-128.0f, 69.0f);
        ImGui::SliderFloat2("Pos text##tripmeter", reinterpret_cast<float*>(&posText), -300.0f, 300.0f);

        drawText(posText, tripMeterText, m_Font24);
    }

    // Odometer
    {
        ImGui::SeparatorText("Odometer");

        int32_t odometer = config.MetricUnits ? (config.Odometer / 1000.0f) : (config.Odometer / 1609.0f);

        char odometerText[16] = {};
        sprintf_s(odometerText, "%06d", odometer);

        static ImVec2 posText(-128.0f, 42.0f);
        ImGui::SliderFloat2("Pos text##odometer", reinterpret_cast<float*>(&posText), -300.0f, 300.0f);

        drawText(posText, odometerText, m_Font29);
    }

    // RPM
    {
        ImGui::SeparatorText("RPM");

        int32_t rpm = guiPlayerInfo.at(0x34).as<int32_t>();

        char rpmText[8] = {};
        sprintf_s(rpmText, "%d", rpm);
        
        static ImVec2 posText(128.0f, 102.0f);
        static ImVec2 posNeedle(128.0f, 72.0f);
        ImGui::SliderFloat2("Pos text##rpm", reinterpret_cast<float*>(&posText), -300.0f, 300.0f);
        ImGui::SliderFloat2("Pos needle##rpm", reinterpret_cast<float*>(&posNeedle), -300.0f, 300.0f);

        drawText(posText, rpmText, m_Font29);
        drawNeedle(posNeedle, static_cast<float>(rpm), 0.0f, 12000.0f);
    }

    // Gear
    {
        ImGui::SeparatorText("Gear");

        int32_t gear = guiPlayerInfo.at(0x38).as<int32_t>();

        static constexpr char gears[][2] = { "R", "1", "2", "3", "4", "5" };

        static ImVec2 posText(128.0f, 70.0f);
        ImGui::SliderFloat2("Pos text##gear", reinterpret_cast<float*>(&posText), -300.0f, 300.0f);

        drawText(posText, gears[gear], m_Font37);
    }

    // Local Time
    {
        ImGui::SeparatorText("Local Time");

        SYSTEMTIME localTime = {};
        GetLocalTime(&localTime);

        char localTimeText[8] = {};
        sprintf_s(localTimeText, "%02d:%02d", localTime.wHour, localTime.wMinute);

        static ImVec2 posText(128.0f, 42.0f);
        ImGui::SliderFloat2("Pos text##localtime", reinterpret_cast<float*>(&posText), -300.0f, 300.0f);

        drawText(posText, localTimeText, m_Font29);
    }

    ImGui::End();
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

    uint32_t width = ddsData.at(0x10).as<uint32_t>();
    uint32_t height = ddsData.at(0xC).as<uint32_t>();

    D3D11_TEXTURE2D_DESC textureDesc =
    {
        .Width      = width,
        .Height     = height,
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

    m_TextureWidth = width;
    m_TextureHeight = height;
}
