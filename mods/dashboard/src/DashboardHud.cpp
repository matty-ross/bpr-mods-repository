#include "DashboardHud.hpp"

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
}

void DashboardHud::CreateTexture(Core::Pointer ddsData)
{
    // https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dx-graphics-dds-pguide

    /*
    * DDS texture
    * DXT5 compression
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
        .Format     = DXGI_FORMAT_BC3_UNORM,
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
        .SysMemPitch = ((width + 3) / 4) * 16,
    };

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc =
    {
        .Format        = DXGI_FORMAT_BC3_UNORM,
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
