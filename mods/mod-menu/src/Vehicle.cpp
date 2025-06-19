#include "Vehicle.hpp"

#include "vendor/imgui.hpp"

#include "bpr-sdk/CgsResource.hpp"
#include "bpr-sdk/GameEvents.hpp"
#include "core/Pointer.hpp"


void Vehicle::OnPreWorldUpdate(
    void* gameEventQueue, // BrnGameState::GameStateModuleIO::GameEventQueue*
    void* gameActionQueue // BrnGameState::GameStateModuleIO::BaseGameActionQueue<13312>*
)
{
    if (m_ChangeVehicle)
    {
        BPR::GameEvent_ChangePlayerVehicle gameEvent =
        {
            .VehicleID = m_NewVehicleID,
        };
        BPR::GameEventQueue_AddGameEvent(gameEventQueue, &gameEvent, gameEvent.ID, sizeof(gameEvent));

        m_ChangeVehicle = false;
        m_NewVehicleID = 0;
    }
}

void Vehicle::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Vehicle"))
    {
        static ImGuiTextFilter vehicleFilter;
        vehicleFilter.Draw("Filter##vehicle");
        
        if (ImGui::BeginListBox("##vehicles", ImVec2(-FLT_MIN, 0.0f)))
        {
            Core::Pointer vehicleList = BPR::PoolModule_FindResource("B5VehicleList")->Memory[0]; // BrnResource::VehicleListResource*

            uint32_t vehiclesCount = vehicleList.at(0x0).as<uint32_t>();
            for (uint32_t i = 0; i < vehiclesCount; ++i)
            {
                Core::Pointer entry = vehicleList.at(0x4).deref().at(i * 0x108); // BrnResource::VehicleListEntry*

                uint64_t vehicleID = entry.at(0x0).as<uint64_t>();
                const char* vehicleName = entry.at(0x30).as<char[64]>();
                
                if (vehicleFilter.PassFilter(vehicleName))
                {
                    ImGui::PushID(entry.GetAddress());

                    if (ImGui::Selectable(vehicleName))
                    {
                        m_ChangeVehicle = true;
                        m_NewVehicleID = vehicleID;
                    }

                    ImGui::PopID();
                }
            }
            
            ImGui::EndListBox();
        }
    }
}
