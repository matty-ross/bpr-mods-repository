#pragma once


#include <string>
#include <wrl.h>
#include <d3d11.h>

#include "core/Logger.hpp"
#include "core/Pointer.hpp"

#include "DashboardConfigFile.hpp"


class DashboardHud
{
public:
    DashboardHud(DashboardConfigFile& dashboardConfigFile, const Core::Logger& logger);

public:
    void LoadTexture(const std::string& filePath);
    void LoadFonts(const std::string& filePath);

    void OnProgressionAddDistanceDriven(float distance, int32_t vehicleType);
    
    void OnRenderMenu();
    void OnRenderOverlay();

private:
    void CreateTexture(Core::Pointer ddsData);

private:
    DashboardConfigFile& m_DashboardConfigFile;
    const Core::Logger& m_Logger;
    
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TextureView = nullptr;
    
    struct ImFont* m_Font11 = nullptr;
    struct ImFont* m_Font24 = nullptr;
    struct ImFont* m_Font29 = nullptr;
    struct ImFont* m_Font37 = nullptr;
    
    float m_TripMeter = 0.0f;
};
