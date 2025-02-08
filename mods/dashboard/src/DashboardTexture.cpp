#include "DashboardTexture.hpp"

#include <exception>


static constexpr DashboardTexture::TextureSegmentUVs k_TextureSegmentUVs[] =
{
    // Background
    DashboardTexture::TextureSegmentUVs
    {
        .Left   = 0.0f,
        .Top    = 0.0f,
        .Right  = 0.5f,
        .Bottom = 0.5f,
    },
    
    // RMP
    DashboardTexture::TextureSegmentUVs
    {
        .Left   = 0.5f,
        .Top    = 0.0f,
        .Right  = 1.0f,
        .Bottom = 0.5f,
    },

    // KMH
    DashboardTexture::TextureSegmentUVs
    {
        .Left   = 0.0f,
        .Top    = 0.5f,
        .Right  = 0.5f,
        .Bottom = 1.0f,
    },

    // MPH
    DashboardTexture::TextureSegmentUVs
    {
        .Left   = 0.5f,
        .Top    = 0.5f,
        .Right  = 1.0f,
        .Bottom = 1.0f,
    },
};


constexpr const DashboardTexture::TextureSegmentUVs& DashboardTexture::GetTextureSegmentUVs(DashboardTexture::TextureSegment textureSegment)
{
    return k_TextureSegmentUVs[static_cast<int>(textureSegment)];
}

ID3D11ShaderResourceView* DashboardTexture::GetTextureView() const
{
    return m_TextureView.Get();
}

void DashboardTexture::CreateTexture(Core::Pointer dds)
{
    // https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dx-graphics-dds-pguide
    // https://learn.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-textures-how-to

    /*
    * DDS texture
    * no compression
    * no mipmaps
    */

    uint32_t magic = dds.at(0x0).as<uint32_t>();
    if (magic != MAKEFOURCC('D', 'D', 'S', ' '))
    {
        throw std::exception("DDS magic number mismatch.");
    }

    D3D11_TEXTURE2D_DESC textureDesc =
    {
        .Width = dds.at(0x10).as<uint32_t>(),
        .Height = dds.at(0xC).as<uint32_t>(),
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = DXGI_FORMAT_B8G8R8A8_UNORM,
        .SampleDesc =
        {
            .Count = 1,
            .Quality = 0,
        },
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_SHADER_RESOURCE,
    };

    D3D11_SUBRESOURCE_DATA initialData =
    {
        .pSysMem = dds.at(0x80).GetAddress(),
        .SysMemPitch = dds.at(0x14).as<uint32_t>(),
    };

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc =
    {
        .Format = DXGI_FORMAT_B8G8R8A8_UNORM,
        .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
        .Texture2D =
        {
            .MostDetailedMip = 0,
            .MipLevels = 1,
        },
    };

    ID3D11Device* device = Core::Pointer(0x01485BF8).as<ID3D11Device*>();

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    if (FAILED(device->CreateTexture2D(&textureDesc, &initialData, &texture)))
    {
        throw std::exception("Failed to create D3D11 texture.");
    }

    if (FAILED(device->CreateShaderResourceView(texture.Get(), &shaderResourceViewDesc, &m_TextureView)))
    {
        throw std::exception("Failed to create D3D11 shader resource view.");
    }
}
