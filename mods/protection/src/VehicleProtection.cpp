#include "VehicleProtection.h"

#include "imgui/imgui.h"

#include "OriginalVehicles.h"


VehicleProtection::VehicleProtection(VehiclesFile& vehiclesFile)
    :
    m_VehiclesFile(vehiclesFile)
{
}

void VehicleProtection::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Vehicle Protection"))
    {
        ImGui::Checkbox("Vehicle Protection Enabled", &m_VehicleProtectionEnabled);

        if (ImGui::BeginCombo("Fallback Vehicle", nullptr))
        {
            for (const VehicleID& originalVehicleID : k_OriginalVehicleIDs)
            {
                bool selected = originalVehicleID.Compressed == m_VehiclesFile.GetFallbackVehicleID();
                if (ImGui::Selectable(originalVehicleID.Uncompressed, selected))
                {
                    m_VehiclesFile.SetFallbackVehicleID(originalVehicleID.Compressed);
                }
                if (selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::Button("Save"))
        {
            m_VehiclesFile.Save();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load"))
        {
            m_VehiclesFile.Load();
        }
        if (ImGui::BeginTable("##replacement-vehicles-table", 2))
        {
            ImGui::TableSetupColumn("New Vehicle");
            ImGui::TableSetupColumn("Replacement Vehicle");
            ImGui::TableHeadersRow();

            ImGui::EndTable();
        }
    }
}