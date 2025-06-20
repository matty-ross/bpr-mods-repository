#include "Vehicle.hpp"

#include "vendor/imgui.hpp"

#include "bpr-sdk/CgsResource.hpp"
#include "bpr-sdk/GameEvents.hpp"
#include "core/Pointer.hpp"


static Core::Pointer GetPlayerRaceVehicle()
{
    int32_t playerVehicleIndex = Core::Pointer(0x013FC8E0).deref().at(0x40C28).as<int32_t>();
    Core::Pointer playerActiveRaceVehicle = Core::Pointer(0x013FC8E0).deref().at(0x12980 + playerVehicleIndex * 0x4180); // BrnWorld::ActiveRaceCar*
    Core::Pointer playerRaceVehicle = playerActiveRaceVehicle.at(0x7C0).as<void*>(); // BrnWorld::RaceCar*

    return playerRaceVehicle;
}


void Vehicle::OnPreWorldUpdate(
    void* gameEventQueue, // BrnGameState::GameStateModuleIO::GameEventQueue*
    void* gameActionQueue // BrnGameState::GameStateModuleIO::BaseGameActionQueue<13312>*
)
{
    Core::Pointer playerRaceVehicle = GetPlayerRaceVehicle(); // BrnWorld::RaceCar*
    
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

    if (m_ChangeWheel)
    {
        BPR::GameEvent_ChangePlayerVehicle gameEvent =
        {
            .VehicleID = playerRaceVehicle.at(0x68).as<uint64_t>(),
            .WheelID = m_NewWheelID,
        };
        BPR::GameEventQueue_AddGameEvent(gameEventQueue, &gameEvent, gameEvent.ID, sizeof(gameEvent));

        m_ChangeWheel = false;
        m_NewWheelID = 0;
    }
}

void Vehicle::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Vehicle"))
    {
        Core::Pointer playerRaceVehicle = GetPlayerRaceVehicle(); // BrnWorld::RaceCar*
        
        {
            ImGui::SeparatorText("Vehicle List");

            uint64_t currentVehicleID = playerRaceVehicle.at(0x68).as<uint64_t>();
            
            static ImGuiTextFilter vehicleFilter;
            vehicleFilter.Draw("Filter##vehicle-list");
        
            if (ImGui::BeginListBox("##vehicle-list", ImVec2(-FLT_MIN, 0.0f)))
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

                        bool selected = vehicleID == currentVehicleID;
                        if (ImGui::Selectable(vehicleName, selected))
                        {
                            m_ChangeVehicle = true;
                            m_NewVehicleID = vehicleID;
                        }
                        if (selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }

                        ImGui::PopID();
                    }
                }
            
                ImGui::EndListBox();
            }
        }

        {
            ImGui::SeparatorText("Wheel List");

            uint64_t currentWheelID = playerRaceVehicle.at(0x70).as<uint64_t>();

            static ImGuiTextFilter wheelFilter;
            wheelFilter.Draw("Filter##wheel-list");

            if (ImGui::BeginListBox("##wheel-list", ImVec2(-FLT_MIN, 0.0f)))
            {
                Core::Pointer wheelList = BPR::PoolModule_FindResource("B5WheelList")->Memory[0]; // BrnResource::WheelListResource*

                uint32_t wheelsCount = wheelList.at(0x0).as<uint32_t>();
                for (uint32_t i = 0; i < wheelsCount; ++i)
                {
                    Core::Pointer entry = wheelList.at(0x4).deref().at(i * 0x48); // BrnResource::WheelListEntry*

                    uint64_t wheelID = entry.at(0x0).as<uint64_t>();
                    const char* wheelName = entry.at(0x8).as<char[64]>();

                    if (wheelFilter.PassFilter(wheelName))
                    {
                        ImGui::PushID(entry.GetAddress());

                        bool selected = wheelID == currentWheelID;
                        if (ImGui::Selectable(wheelName, selected))
                        {
                            m_ChangeWheel = true;
                            m_NewWheelID = wheelID;
                        }
                        if (selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }

                        ImGui::PopID();
                    }
                }

                ImGui::EndListBox();
            }
        }

        {
            ImGui::SeparatorText("Color Palettes");

            int32_t currentColorIndex = playerRaceVehicle.at(0x94).as<int32_t>();
            int32_t currentColorPaletteIndex = playerRaceVehicle.at(0x98).as<int32_t>();

            // TODO: PoC, completely refactor, perhaps just add a combo and edit int...

            Core::Pointer colorPalettes = BPR::PoolModule_FindResource("CarColours")->Memory[0]; // BrnWorld::GlobalColourPalette*

            for (int i = 0; i < 5; ++i)
            {
                ImGui::Separator();

                ImColor* paintColors = colorPalettes.at(i * 0xC + 0x0).as<ImColor*>();
                ImColor* pearlColors = colorPalettes.at(i * 0xC + 0x4).as<ImColor*>();
                int32_t colorsCount = colorPalettes.at(i * 0xC + 0x8).as<int32_t>();

                for (int j = 0; j < colorsCount; ++j)
                {
                    ImGui::ColorButton("##color", paintColors[j]);
                }
            }
        }
    }
}
