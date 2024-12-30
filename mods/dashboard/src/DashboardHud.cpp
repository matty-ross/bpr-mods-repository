#include "DashboardHud.hpp"

#include <DirectXMath.h>

#include "vendor/imgui.hpp"

#include "core/File.hpp"


DashboardHud::DashboardHud(const Core::Logger& logger)
    :
    m_Logger(logger)
{
}

void DashboardHud::LoadTexture(const std::string& filePath)
{
    static constexpr char name[] = "dashboard texture";
    
    m_Logger.Info("Loading %s from file '%s' ...", name, filePath.c_str());
    
    Core::File file(filePath, Core::File::Mode::Read);
    std::vector<BYTE> textureData = file.ReadBinary();
    CreateTexture(textureData.data());

    m_Logger.Info("Loaded %s.", name);
}

void DashboardHud::LoadFonts(const std::string& filePath)
{
    m_Logger.Info("Loading fonts from file '%s' ...", filePath.c_str());
    
    ImGuiIO& io = ImGui::GetIO();

    // TODO: load the font at different sizes

    m_Font = io.Fonts->AddFontFromFileTTF(filePath.c_str(), 20.0f);

    ImGui_ImplDX11_InvalidateDeviceObjects();

    m_Logger.Info("Loaded fonts.");
}

void DashboardHud::OnProgressionAddDistanceDriven(float distance, int32_t vehicleType)
{
    // Distance is in meters.

    if (vehicleType != -1)
    {
        m_DistanceDriven[vehicleType] += distance;
    }
}

void DashboardHud::OnRenderOverlay()
{
    // TODO: ImGui window is just here for debugging purposes
    ImGui::Begin("Dashboard debug");

    Core::Pointer guiPlayerInfo = Core::Pointer(0x013FC8E0).deref().at(0x8EFEC0); // BrnGui::GuiPlayerInfo*

    ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    ImDrawList* foregroundDrawList = ImGui::GetForegroundDrawList();

    auto mainViewportPositionX = [=](float position) { return mainViewport->Pos.x + mainViewport->Size.x * (position / 100.0f); };
    auto mainViewportPositionY = [=](float position) { return mainViewport->Pos.y + mainViewport->Size.y * (position / 100.0f); };

    auto drawText = [=](const ImFont* font, const ImVec2& position, const char* text)
    {
        constexpr ImU32 color = IM_COL32(0x24, 0xFF, 0xFC, 0xC8);

        ImVec2 textSize = font->CalcTextSizeA(font->FontSize, FLT_MAX, 0.0f, text);

        float x = mainViewportPositionX(position.x) - textSize.x / 2.0f;
        float y = mainViewportPositionY(position.y) - textSize.y / 2.0f;
        
        foregroundDrawList->AddText(font, font->FontSize, ImVec2(x, y), color, text);
    };

    auto drawNeedle = [=](const ImVec2& position, float radiusOuter, float radiusInner, float min, float max, float value)
    {
        // TODO: radius should be constexpr

        constexpr ImU32 color = IM_COL32(0xFF, 0x26, 0x34, 0xC8);

        constexpr float angleMin = DirectX::XMConvertToRadians(-225);
        constexpr float angleMax = DirectX::XMConvertToRadians(45);

        float angle = angleMin + (angleMax - angleMin) * ((value - min) / (max - min));
        float angleSin = 0.0f, angleCos = 0.0f;
        DirectX::XMScalarSinCos(&angleSin, &angleCos, angle);

        float x1 = mainViewportPositionX(position.x) + angleCos * radiusInner;
        float y1 = mainViewportPositionY(position.y) + angleSin * radiusInner;
        float x2 = mainViewportPositionX(position.x) + angleCos * radiusOuter;
        float y2 = mainViewportPositionY(position.y) + angleSin * radiusOuter;
        
        foregroundDrawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color, 2.0f);
    };

    // Texture
    {
        float left = mainViewportPositionX(50.0f) - m_TextureWidth / 2.0f;
        float right = mainViewportPositionX(50.0f) + m_TextureWidth / 2.0f;
        float top = mainViewportPositionY(100.0f) - m_TextureHeight;
        float bottom = mainViewportPositionY(100.0f);

        foregroundDrawList->AddImage(static_cast<ImTextureID>(m_TextureView.Get()), ImVec2(left, top), ImVec2(right, bottom));
    }

    // Speed
    {
        ImGui::SeparatorText("Speed");
        
        int32_t speed = abs(guiPlayerInfo.at(0x30).as<int32_t>());
        
        char speedText[8] = {};
        sprintf_s(speedText, "%d", speed);

        static ImVec2 pos;
        ImGui::SliderFloat2("Pos##speed", reinterpret_cast<float*>(&pos), 0.0f, 100.0f);
        static float radiusOuter = 0.0f;
        ImGui::SliderFloat("Radius outer##speed", &radiusOuter, 0.0f, 300.0f);
        static float radiusInner = 0.0f;
        ImGui::SliderFloat("Radius inner##speed", &radiusInner, 0.0f, 300.0f);

        drawText(m_Font, pos, speedText);
        drawNeedle(pos, radiusOuter, radiusInner, 0.0f, 360.0f, static_cast<float>(speed));
    }

    // RPM text
    {
        ImGui::SeparatorText("RPM");

        int32_t rpm = guiPlayerInfo.at(0x34).as<int32_t>();

        char rpmText[8] = {};
        sprintf_s(rpmText, "%d", rpm);
        
        static ImVec2 pos;
        ImGui::SliderFloat2("Pos##rpm", reinterpret_cast<float*>(&pos), 0.0f, 100.0f);

        drawText(m_Font, pos, rpmText);
    }

    // Gear text
    {
        ImGui::SeparatorText("Gear");

        int32_t gear = guiPlayerInfo.at(0x38).as<int32_t>();

        char gearText[8] = {};
        sprintf_s(gearText, "%d", gear);

        static ImVec2 pos;
        ImGui::SliderFloat2("Pos##gear", reinterpret_cast<float*>(&pos), 0.0f, 100.0f);

        drawText(m_Font, pos, gearText);
    }

    // Tripmeter text
    {
        ImGui::SeparatorText("Tripmeter");

        int32_t vehicleType = guiPlayerInfo.at(0x20).as<int32_t>();
        float tripmeter = m_DistanceDriven[vehicleType] / 1000.0f;

        char tripmeterText[16] = {};
        sprintf_s(tripmeterText, "%.1f", tripmeter);

        static ImVec2 pos;
        ImGui::SliderFloat2("Pos##tripmeter", reinterpret_cast<float*>(&pos), 0.0f, 100.0f);

        drawText(m_Font, pos, tripmeterText);
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
