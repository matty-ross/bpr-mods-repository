#include "VehicleManager.hpp"

#include <algorithm>

#include "vendor/imgui.hpp"

#include "bpr-sdk/CgsResource.hpp"
#include "bpr-sdk/GameEvents.hpp"
#include "bpr-sdk/GameActions.hpp"
#include "core/Pointer.hpp"


static Core::Pointer GetPlayerActiveRaceVehicle()
{
    int32_t playerVehicleIndex = Core::Pointer(0x013FC8E0).deref().at(0x40C28).as<int32_t>();
    Core::Pointer playerActiveRaceVehicle = Core::Pointer(0x013FC8E0).deref().at(0x12980 + playerVehicleIndex * 0x4180); // BrnWorld::ActiveRaceCar*

    return playerActiveRaceVehicle;
}


void VehicleManager::OnPreWorldUpdate(
    void* gameEventQueue, // BrnGameState::GameStateModuleIO::GameEventQueue*
    void* gameActionQueue // BrnGameState::GameStateModuleIO::BaseGameActionQueue<13312>*
)
{
    Core::Pointer gameModule = Core::Pointer(0x013FC8E0).deref(); // BrnGame::BrnGameModule*
    Core::Pointer playerActiveRaceVehicle = GetPlayerActiveRaceVehicle(); // BrnWorld::ActiveRaceCar*
    Core::Pointer playerRaceVehicle = playerActiveRaceVehicle.at(0x7C0).as<void*>(); // BrnWorld::RaceCar*
    
    if (m_ChangeVehicle)
    {
        BPR::GameEvent_ChangePlayerVehicle gameEvent =
        {
            .VehicleID         = m_NewVehicleID,
            .WheelID           = 0,
            .ResetPlayerCamera = true,
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
            .ResetPlayerCamera = true,
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

    if (m_ChangeBoost)
    {
        BPR::GameAction_UpdateVehicleStats gameAction =
        {
            .Speed = 0, // TODO: read from vehicle list
            .Strength = 0, // TODO: read from vehicle list
            .BoostLossLevel = gameModule.at(0x40758).as<int32_t>(),
            .BoostLevel = gameModule.at(0x40754).as<int32_t>(),
            .DamageLimit = 1.0f, // TOOD: read from vehicle list
            .BoostType = static_cast<decltype(BPR::GameAction_UpdateVehicleStats::BoostType::Speed)>(gameModule.at(0x3FFD4).as<int32_t>() - 1), // TODO: fix this bullshit
        };
        BPR::GameActionQueue_AddGameAction(gameActionQueue, &gameAction, gameAction.ID, sizeof(gameAction));

        m_ChangeBoost = false;
    }
}

void VehicleManager::OnUpdateActiveRaceVehicleColors()
{
    if (!m_OverrideColor)
    {
        return;
    }

    Core::Pointer playerActiveRaceVehicle = GetPlayerActiveRaceVehicle(); // BrnWorld::ActiveRaceCar*

    DirectX::XMStoreFloat3A(
        &playerActiveRaceVehicle.at(0x1480).as<DirectX::XMFLOAT3A>(),
        DirectX::XMColorModulate(DirectX::XMLoadFloat3A(&m_OverridenPaintColor), DirectX::XMLoadFloat3A(&m_OverridenPaintColorIntensity))
    );
    DirectX::XMStoreFloat3A(
        &playerActiveRaceVehicle.at(0x1490).as<DirectX::XMFLOAT3A>(),
        DirectX::XMColorModulate(DirectX::XMLoadFloat3A(&m_OverridenPearlColor), DirectX::XMLoadFloat3A(&m_OverridenPearlColorIntensity))
    );
}

void VehicleManager::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Vehicle Manager"))
    {
        Core::Pointer gameModule = Core::Pointer(0x013FC8E0).deref(); // BrnGame::BrnGameModule*
        Core::Pointer playerActiveRaceVehicle = GetPlayerActiveRaceVehicle(); // BrnWorld::ActiveRaceCar*
        Core::Pointer playerRaceVehicle = playerActiveRaceVehicle.at(0x7C0).as<void*>(); // BrnWorld::RaceCar*

        {
            ImGui::SeparatorText("General");

            {
                uint64_t vehicleID = playerRaceVehicle.at(0x68).as<uint64_t>();
            
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Vehicle   %s", GetVehicleName(vehicleID));

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
                            for (const Vehicle& vehicle : m_Vehicles)
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
                ImGui::Text("Wheel     %s", GetWheelName(wheelID));

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
                            for (const Wheel& wheel : m_Wheels)
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
            ImGui::ColorEdit3("Paint Color", reinterpret_cast<float*>(&m_OverridenPaintColor));
            ImGui::DragFloat3("Paint Color Intensity", reinterpret_cast<float*>(&m_OverridenPaintColorIntensity));
            ImGui::ColorEdit3("Pearl Color", reinterpret_cast<float*>(&m_OverridenPearlColor));
            ImGui::DragFloat3("Pearl Color Intensity", reinterpret_cast<float*>(&m_OverridenPearlColorIntensity));
        }

        {
            ImGui::SeparatorText("Boost");

            // TODO: remove button and chaneg it whenever any parameter changes
            if (ImGui::Button("Change##boost"))
            {
                m_ChangeBoost = true;
            }
            
            static constexpr const char* boostTypes[] =
            {
                "???", // TODO: remove
                "Speed",
                "Aggression",
                "Stunt",
                "None",
                "Locked",
            };
            ImGui::Combo("Boost Type", &gameModule.at(0x3FFD4).as<int32_t>(), boostTypes, IM_ARRAYSIZE(boostTypes));

            ImGui::SliderInt("Boost Level", &gameModule.at(0x40754).as<int32_t>(), 0, 10);
            ImGui::SliderInt("Boost Loss Level", &gameModule.at(0x40758).as<int32_t>(), 0, 10);
        }
    }
}

void VehicleManager::LoadVehicles()
{
    Core::Pointer vehicleList = BPR::PoolModule_FindResource("B5VehicleList")->Memory[0]; // BrnResource::VehicleListResource*

    uint32_t vehiclesCount = vehicleList.at(0x0).as<uint32_t>();
    for (uint32_t i = 0; i < vehiclesCount; ++i)
    {
        Core::Pointer entry = vehicleList.at(0x4).deref().at(i * 0x108); // BrnResource::VehicleListEntry*

        m_Vehicles.push_back(
            Vehicle
            {
                .ID   = entry.at(0x0).as<uint64_t>(),
                .Name = entry.at(0x30).as<char[64]>(),
            }
        );
    }
}

void VehicleManager::LoadWheels()
{
    Core::Pointer wheelList = BPR::PoolModule_FindResource("B5WheelList")->Memory[0]; // BrnResource::WheelListResource*

    uint32_t wheelsCount = wheelList.at(0x0).as<uint32_t>();
    for (uint32_t i = 0; i < wheelsCount; ++i)
    {
        Core::Pointer entry = wheelList.at(0x4).deref().at(i * 0x48); // BrnResource::WheelListEntry*

        m_Wheels.push_back(
            Wheel
            {
                .ID   = entry.at(0x0).as<uint64_t>(),
                .Name = entry.at(0x8).as<char[64]>(),
            }
        );
    }
}

void VehicleManager::LoadColorPalettes()
{
    Core::Pointer colorPalettes = BPR::PoolModule_FindResource("CarColours")->Memory[0]; // BrnWorld::GlobalColourPalette*

    for (int i = 0; i < 5; ++i)
    {
        m_ColorPalettes[i].ColorsCount = colorPalettes.at(i * 0xC + 0x8).as<int32_t>();
    }
}

const char* VehicleManager::GetVehicleName(uint64_t vehicleID) const
{
    for (const Vehicle& vehicle : m_Vehicles)
    {
        if (vehicle.ID == vehicleID)
        {
            return vehicle.Name;
        }
    }

    return "???";
}

const char* VehicleManager::GetWheelName(uint64_t wheelID) const
{
    for (const Wheel& wheel : m_Wheels)
    {
        if (wheel.ID == wheelID)
        {
            return wheel.Name;
        }
    }

    return "???";
}
