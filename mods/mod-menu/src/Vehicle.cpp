#include "Vehicle.hpp"

#include <algorithm>

#include "vendor/imgui.hpp"

#include "bpr-sdk/CgsResource.hpp"
#include "bpr-sdk/GameEvents.hpp"
#include "bpr-sdk/GameActions.hpp"
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
            .VehicleID         = m_NewVehicleID,
            .WheelID           = 0,
            .ResetPlayerCamera = false,
            .KeepResetSection  = true,
        };
        BPR::GameEventQueue_AddGameEvent(gameEventQueue, &gameEvent, gameEvent.ID, sizeof(gameEvent));

        m_ChangeVehicle = false;
        m_NewVehicleID = 0;
    }

    if (m_ChangeWheel)
    {
        BPR::GameEvent_ChangePlayerVehicle gameEvent =
        {
            .VehicleID         = playerRaceVehicle.at(0x68).as<uint64_t>(),
            .WheelID           = m_NewWheelID,
            .ResetPlayerCamera = false,
            .KeepResetSection  = true,
        };
        BPR::GameEventQueue_AddGameEvent(gameEventQueue, &gameEvent, gameEvent.ID, sizeof(gameEvent));

        m_ChangeWheel = false;
        m_NewWheelID = 0;
    }

    if (m_ResetOnTrack)
    {
        BPR::GameAction_ResetPlayerVehicleOnTrack gameAction =
        {
            .Speed = 0.0f,
        };
        BPR::GameActionQueue_AddGameAction(gameActionQueue, &gameAction, gameAction.ID, sizeof(gameAction));

        m_ResetOnTrack = false;
    }
}

void Vehicle::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Vehicle"))
    {
        Core::Pointer playerRaceVehicle = GetPlayerRaceVehicle(); // BrnWorld::RaceCar*

        {
            ImGui::SeparatorText("General");

            {
                uint64_t vehicleID = playerRaceVehicle.at(0x68).as<uint64_t>();
            
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Vehicle        %s", GetVehicleName(vehicleID));

                ImGui::SameLine(0.0f, 20.0f);
            
                if (ImGui::Button("Change...##vehicle"))
                {
                    ImGui::OpenPopup("vehicle-list");
                }

                {
                    ImGui::SetNextWindowSize(ImVec2(400.0f, 500.0f));

                    if (ImGui::BeginPopup("vehicle-list"))
                    {
                        ImGui::SeparatorText("Vehicle List");

                        static ImGuiTextFilter vehicleFilter;
                        vehicleFilter.Draw("Filter##vehicle-list");

                        if (ImGui::BeginListBox("##vehicle-list", ImVec2(-FLT_MIN, -FLT_MIN)))
                        {
                            for (const VehicleData& vehicle : m_VehicleList)
                            {
                                if (vehicleFilter.PassFilter(vehicle.Name))
                                {
                                    ImGui::PushID(&vehicle);

                                    bool selected = vehicle.ID == vehicleID;
                                    if (ImGui::Selectable(vehicle.Name, selected))
                                    {
                                        m_ChangeVehicle = true;
                                        m_NewVehicleID = vehicle.ID;
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

                        ImGui::EndPopup();
                    }
                }
            }

            {
                uint64_t wheelID = playerRaceVehicle.at(0x70).as<uint64_t>();

                ImGui::AlignTextToFramePadding();
                ImGui::Text("Wheel          %s", GetWheelName(wheelID));

                ImGui::SameLine(0.0f, 20.0f);

                if (ImGui::Button("Change...##wheel"))
                {
                    ImGui::OpenPopup("wheel-list");
                }

                {
                    ImGui::SetNextWindowSize(ImVec2(400.0f, 500.0f));

                    if (ImGui::BeginPopup("wheel-list"))
                    {
                        ImGui::SeparatorText("Wheel List");

                        static ImGuiTextFilter wheelFilter;
                        wheelFilter.Draw("Filter##wheel-list");

                        if (ImGui::BeginListBox("##wheel-list", ImVec2(-FLT_MIN, -FLT_MIN)))
                        {
                            for (const WheelData& wheel : m_WheelList)
                            {
                                if (wheelFilter.PassFilter(wheel.Name))
                                {
                                    ImGui::PushID(&wheel);

                                    bool selected = wheel.ID == wheelID;
                                    if (ImGui::Selectable(wheel.Name, selected))
                                    {
                                        m_ChangeWheel = true;
                                        m_NewWheelID = wheel.ID;
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

                        ImGui::EndPopup();
                    }
                }
            }

            {
                int32_t vehicleType = playerRaceVehicle.at(0xA4).as<int32_t>();

                static constexpr const char* vehicleTypes[] =
                {
                    "Car",
                    "Motorbike",
                    "Plane",
                };

                ImGui::AlignTextToFramePadding();
                ImGui::Text("Vehicle Type   %s", vehicleTypes[vehicleType]);
            }

            if (ImGui::Button("Reset on Track"))
            {
                m_ResetOnTrack = true;
            }
        }

        {
            ImGui::SeparatorText("Color");

            int32_t& colorPaletteType = playerRaceVehicle.at(0x98).as<int32_t>();
            int32_t& colorIndex = playerRaceVehicle.at(0x94).as<int32_t>();

            static constexpr const char* colorPaletteTypes[] =
            {
                "Gloss",
                "Metallic",
                "Pearlescent",
                "Special",
                "Team",
            };    
            if (ImGui::Combo("Color Palette Type", &colorPaletteType, colorPaletteTypes, IM_ARRAYSIZE(colorPaletteTypes)))
            {
                colorIndex = std::clamp(colorIndex, 0, m_ColorPalettes[colorPaletteType].ColorsCount - 1);
            }

            if (ImGui::InputInt("Color Index", &colorIndex))
            {
                colorIndex = std::clamp(colorIndex, 0, m_ColorPalettes[colorPaletteType].ColorsCount - 1);
            }

            ImGui::Separator();

            ImGui::Checkbox("Override Color", &m_OverrideColor);
            ImGui::ColorEdit3("Paint Color", m_OverridenPaintColor);
            ImGui::ColorEdit3("Pearlescent Color", m_OverridenPearlescentColor);
        }
    }
}

void Vehicle::LoadVehicleList()
{
    Core::Pointer vehicleList = BPR::PoolModule_FindResource("B5VehicleList")->Memory[0]; // BrnResource::VehicleListResource*

    uint32_t vehiclesCount = vehicleList.at(0x0).as<uint32_t>();
    for (uint32_t i = 0; i < vehiclesCount; ++i)
    {
        Core::Pointer entry = vehicleList.at(0x4).deref().at(i * 0x108); // BrnResource::VehicleListEntry*

        m_VehicleList.push_back(
            VehicleData
            {
                .ID   = entry.at(0x0).as<uint64_t>(),
                .Name = entry.at(0x30).as<char[64]>(),
            }
        );
    }
}

void Vehicle::LoadWheelList()
{
    Core::Pointer wheelList = BPR::PoolModule_FindResource("B5WheelList")->Memory[0]; // BrnResource::WheelListResource*

    uint32_t wheelsCount = wheelList.at(0x0).as<uint32_t>();
    for (uint32_t i = 0; i < wheelsCount; ++i)
    {
        Core::Pointer entry = wheelList.at(0x4).deref().at(i * 0x48); // BrnResource::WheelListEntry*

        m_WheelList.push_back(
            WheelData
            {
                .ID   = entry.at(0x0).as<uint64_t>(),
                .Name = entry.at(0x8).as<char[64]>(),
            }
        );
    }
}

void Vehicle::LoadColorPalettes()
{
    Core::Pointer colorPalettes = BPR::PoolModule_FindResource("CarColours")->Memory[0]; // BrnWorld::GlobalColourPalette*

    for (int i = 0; i < 5; ++i)
    {
        m_ColorPalettes[i].ColorsCount = colorPalettes.at(i * 0xC + 0x8).as<int32_t>();
    }
}

const char* Vehicle::GetVehicleName(uint64_t vehicleID) const
{
    for (const VehicleData& vehicle : m_VehicleList)
    {
        if (vehicle.ID == vehicleID)
        {
            return vehicle.Name;
        }
    }

    return "???";
}

const char* Vehicle::GetWheelName(uint64_t wheelID) const
{
    for (const WheelData& wheel : m_WheelList)
    {
        if (wheel.ID == wheelID)
        {
            return wheel.Name;
        }
    }

    return "???";
}
