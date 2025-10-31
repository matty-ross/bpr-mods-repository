#pragma once


#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"
#include "core/Path.hpp"
#include "core/Logger.hpp"

#include "DashboardConfigFile.hpp"
#include "DashboardTexture.hpp"


class DashboardHud
{
public:
    DashboardHud(DashboardConfigFile& dashboardConfigFile, const Core::Path& assetsDirectory, const Core::Logger& logger);

public:
    void LoadTexture();
    void LoadFont();

    void OnProgressionAddDistanceDriven(Core::Pointer progressionManager, float distance, int32_t vehicleType);
    
    void OnRenderMenu();
    void OnRenderOverlay();

private:
    void RenderTextureSegment(const ImVec2& position, DashboardTexture::TextureSegment textureSegment, bool useColor = true) const;
    void RenderText(const ImVec2& position, const char* text, float size) const;
    void RenderNeedle(const ImVec2& position, float value, float minValue, float maxValue) const;

private:
    DashboardConfigFile& m_DashboardConfigFile;
    const Core::Path& m_AssetsDirectory;
    const Core::Logger& m_Logger;
    
    DashboardTexture m_DashboardTexture;
    
    ImFont* m_Font = nullptr;
    
    float m_TripMeter = 0.0f;
    float m_Odometer = 0.0f;
};
