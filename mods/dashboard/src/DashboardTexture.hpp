#pragma once


#include <d3d11.h>
#include <wrl.h>

#include "core/Pointer.hpp"


class DashboardTexture
{
public:
    enum TextureSegment
    {
        Background,
        RPM,
        KMH,
        MPH,
    };

    struct TextureSegmentUVs
    {
        float Left;
        float Top;
        float Right;
        float Bottom;
    };

public:
    static constexpr const TextureSegmentUVs& GetTextureSegmentUVs(TextureSegment textureSegment);

public:
    ID3D11ShaderResourceView* GetTextureView() const;
    
    void CreateTexture(Core::Pointer dds);

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TextureView;
};
