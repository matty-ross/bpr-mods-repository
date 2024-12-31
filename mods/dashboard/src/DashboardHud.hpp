#pragma once


#include <string>
#include <wrl.h>
#include <d3d11.h>

#include "core/Logger.hpp"
#include "core/Pointer.hpp"


class DashboardHud
{
public:
    DashboardHud(const Core::Logger& logger);

public:
    void LoadTexture(const std::string& filePath);
    void LoadFonts(const std::string& filePath);

    void OnProgressionAddDistanceDriven(float distance, int32_t vehicleType);
    
    void OnRenderOverlay();

private:
    void CreateTexture(Core::Pointer ddsData);

private:
    const Core::Logger& m_Logger;
    
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TextureView = nullptr;
    uint32_t m_TextureWidth = 0;
    uint32_t m_TextureHeight = 0;
    
    struct ImFont* m_Font21 = nullptr;
    struct ImFont* m_Font25 = nullptr;
    struct ImFont* m_Font33 = nullptr;
    
    float m_DistanceDriven[3] = { 0.0f, 0.0f, 0.0f };
};
