#pragma once


#include <cstdint>


class DashboardHud
{
public:
    void LoadTexture();

    void OnProgressionAddDistanceDriven(float distance, int32_t vehicleType);
    
    void OnRenderOverlay();

private:
    float m_DistanceDriven[3] = { 0.0f, 0.0f, 0.0f, };
};
