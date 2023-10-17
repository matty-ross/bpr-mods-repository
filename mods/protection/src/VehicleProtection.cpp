#include "VehicleProtection.h"

#include "imgui/imgui.h"

#include "Vehicles.h"


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

        if (ImGui::Button("Save"))
        {
            m_VehiclesFile.Save();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load"))
        {
            m_VehiclesFile.Load();
        }
        if (ImGui::BeginTable("##vehicles-table", 2))
        {
            ImGui::TableSetupColumn("New Vehicle");
            ImGui::TableSetupColumn("Replacement Vehicle");
            ImGui::TableHeadersRow();

            for (uint64_t vehicleID : m_VehiclesFile.GetVehicleIDs())
            {
                Vehicle& vehicle = m_VehiclesFile.GetVehicles().at(vehicleID);

                ImGui::PushID(&vehicle);

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(vehicle.NewID.Uncompressed);

                ImGui::TableSetColumnIndex(1);
                if (ImGui::BeginCombo("##replacement-vehicle-combo", vehicle.ReplacementID->Uncompressed))
                {
                    for (const VehicleID& vanillaVehicleID : k_VanillaVehicleIDs)
                    {
                        bool selected = vehicle.ReplacementID->Compressed == vanillaVehicleID.Compressed;
                        if (ImGui::Selectable(vanillaVehicleID.Uncompressed, selected))
                        {
                            vehicle.ReplacementID = &vanillaVehicleID;
                        }
                        if (selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                ImGui::PopID();
            }

            ImGui::EndTable();
        }
    }
}