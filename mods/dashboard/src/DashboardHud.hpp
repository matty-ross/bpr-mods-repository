#pragma once


#include <string>

#include "core/Logger.hpp"


class DashboardHud
{
public:
    DashboardHud(const Core::Logger& logger);

public:
    void LoadTexture(const std::string& filePath);

    void OnProgressionAddDistanceDriven(float distance, int32_t vehicleType);
    
    void OnRenderOverlay();

private:
    const Core::Logger& m_Logger;
    
    float m_DistanceDriven[3] = { 0.0f, 0.0f, 0.0f };
};
