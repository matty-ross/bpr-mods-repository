#include "DashboardHud.hpp"


void DashboardHud::LoadTexture()
{
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
