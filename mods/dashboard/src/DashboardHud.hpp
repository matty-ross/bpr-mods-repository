#pragma once


#include <string>

#include "core/Logger.hpp"

#include "DashboardConfigFile.hpp"
#include "DashboardTexture.hpp"


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
    DashboardConfigFile& m_DashboardConfigFile;
    const Core::Logger& m_Logger;
    
    DashboardTexture m_DashboardTexture;
    
    struct ImFont* m_Font24 = nullptr;
    struct ImFont* m_Font29 = nullptr;
    struct ImFont* m_Font37 = nullptr;
    
    float m_TripMeter = 0.0f;
};
