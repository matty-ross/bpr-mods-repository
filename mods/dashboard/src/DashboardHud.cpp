#include "DashboardHud.hpp"

#include <d3d11.h>

#include "core\Pointer.hpp"
#include "core\File.hpp"


static ID3D11ShaderResourceView* CreateD3D11Texture(Core::Pointer ddsData)
{
    // https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dx-graphics-dds-pguide
    
    // TODO: finish this function + error handling
    
    constexpr uint32_t ddsMagic = 0x20534444;   // 'DDS '
    constexpr uint32_t dxt5Format = 0x35545844; // 'DXT5'

    uint32_t magic = ddsData.at(0x0).as<uint32_t>();
    if (magic != ddsMagic)
    {
        throw std::exception("DDS magic number mismatch");
    }

    uint32_t format = ddsData.at(0x54).as<uint32_t>();
    if (format != dxt5Format)
    {
        throw std::exception("DXT5 format mismatch.");
    }

    ID3D11Device* device = Core::Pointer(0x01485BF8).as<ID3D11Device*>();

    D3D11_TEXTURE2D_DESC textureDesc =
    {
        .Width = ddsData.at(0x10).as<uint32_t>(),
        .Height = ddsData.at(0xC).as<uint32_t>(),
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = DXGI_FORMAT_BC3_UNORM,
        //.SampleDesc
        .Usage = D3D11_USAGE_DEFAULT,
    };
    D3D11_SUBRESOURCE_DATA subresourceData =
    {
        .pSysMem = ddsData.at(0x80).GetAddress(),
        //.SysMemPitch
    };
    ID3D11Texture2D* texture = nullptr;
    device->CreateTexture2D(&textureDesc, &subresourceData, &texture);
    
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc =
    {
        .Format = DXGI_FORMAT_BC3_UNORM,
        .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
        //.Texture2D
    };
    ID3D11ShaderResourceView* shaderResourceView = nullptr;
    device->CreateShaderResourceView(texture, &shaderResourceViewDesc, &shaderResourceView);

    texture->Release();

    return shaderResourceView;
}


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
    
    CreateD3D11Texture(textureData.data());
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
