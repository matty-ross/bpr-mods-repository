#include "VehicleProtection.h"

#include "imgui/imgui.h"


VehicleProtection::VehicleProtection(VehiclesFile& vehiclesFile)
    :
    m_VehiclesFile(vehiclesFile)
{
}

void VehicleProtection::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Vehicle Protection"))
    {
    }
}