#include "VehicleManager.hpp"

#include <algorithm>

#include "vendor/imgui.hpp"

#include "bpr-sdk/CgsID.hpp"
#include "bpr-sdk/CgsLanguage.hpp"
#include "bpr-sdk/CgsResource.hpp"
#include "bpr-sdk/GameEvents.hpp"
#include "bpr-sdk/GameActions.hpp"
#include "core/Pointer.hpp"


namespace BPR
{
    // BrnResource::VehicleListEntry* __thiscall BrnResource::VehicleList::GetVehicleData(CgsID)
    static void* VehicleList_GetVehicleData(uint64_t vehicleID)
    {
        void* vehicleData = nullptr;
        
        __asm
        {
            push dword ptr [vehicleID + 0x4]
            push dword ptr [vehicleID + 0x0]
            mov ecx, dword ptr ds:[0x013FC8E0]
            add ecx, 0x68C350
            
            mov eax, 0x004B7080
            call eax

            mov dword ptr [vehicleData], eax
        }

        return vehicleData;
    }

    static int32_t ManufacturersIcon_GetVehicleManufacturer(uint64_t vehicleID)
    {
        int32_t vehicleManufacturer = 9;
        
        __asm
        {
            push dword ptr [vehicleID + 0x4]
            push dword ptr [vehicleID + 0x0]

            mov eax, 0x00A69A20
            call eax
            add esp, 0x8

            mov dword ptr [vehicleManufacturer], eax
        }

        return vehicleManufacturer;
    }
}


static std::string CreateVehicleName(
    Core::Pointer vehicleData // BrnResource::VehicleListEntry*
)
{
    uint64_t vehicleID = vehicleData.at(0x0).as<uint64_t>();
    uint64_t parentVehicleID = vehicleData.at(0x8).as<uint64_t>();
    uint8_t liveryType = vehicleData.at(0xFD).as<uint8_t>();

    auto getVehicleManufacturer = [=]() -> const char*
    {
        int32_t vehicleManufacturerType = BPR::ManufacturersIcon_GetVehicleManufacturer((parentVehicleID != 0) ? parentVehicleID : vehicleID);
        switch (vehicleManufacturerType)
        {
        case 0: return "Carson";
        case 1: return "Hunter";
        case 2: return "Jansen";
        case 3: return "Krieger";
        case 4: return "Kitano";
        case 5: return "Montgomery";
        case 6: return "Nakamura";
        case 7: return "Rossolini";
        case 8: return "Watson";
        }

        char vehicleManufacturerStringID[32] = "CAR_MAN_CAPS_";
        BPR::CgsID_ConvertToString((parentVehicleID != 0) ? parentVehicleID : vehicleID, vehicleManufacturerStringID + 12);
        const char* vehicleManufacturer = BPR::LanguageManager_FindString(vehicleManufacturerStringID);
        if (vehicleManufacturer != nullptr)
        {
            return vehicleManufacturer;
        }

        return vehicleData.at(0x70).as<char[32]>();
    };
    
    auto getVehicleName = [=]() -> const char*
    {
        char vehicleNameStringID[32] = "CAR_CAPS_";
        BPR::CgsID_ConvertToString((liveryType == 2 || parentVehicleID == 0) ? vehicleID : parentVehicleID, vehicleNameStringID + 9);
        const char* vehicleName = BPR::LanguageManager_FindString(vehicleNameStringID);
        if (vehicleName != nullptr)
        {
            return vehicleName;
        }

        return vehicleData.at(0x30).as<char[64]>();
    };

    auto getVehicleLivery = [=]() -> const char*
    {
        switch (liveryType)
        {
        case 0: return "Finish 1";
        case 1: return "Color"; // TODO: number
        case 2: return "Finish 1";
        case 3: return "Platinum";
        case 4: return "Gold";
        case 5: return "Community"; // TODO: number
        }

        return "";
    };

    char vehicleNameBuffer[1024] = {};
    sprintf_s(vehicleNameBuffer, "%s %s %s", getVehicleManufacturer(), getVehicleName(), getVehicleLivery());
    return vehicleNameBuffer;
}

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
    Core::Pointer vehicleData = BPR::VehicleList_GetVehicleData(playerRaceVehicle.at(0x68).as<uint64_t>()); // BrnResource::VehicleListEntry*
    
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

    if (m_ChangeDeformation)
    {
        BPR::GameAction_ResetPlayerVehicle gameAction =
        {
            .Position                     = { 0.0f, 0.0f, 0.0f, 0.0f },
            .Direction                    = { 0.0f, 0.0f, 0.0f, 0.0f },
            .VehicleID                    = playerRaceVehicle.at(0x68).as<uint64_t>(),
            .WheelID                      = 0,
            .PlayerScoringIndex           = 8,
            .DeformationAmount            = playerActiveRaceVehicle.at(0x8AC).as<float>(),
            .DeformationType              = BPR::DeformationType::VehicleSelect,
            .VehicleSelectType            = BPR::VehicleSelectType::DontDrop,
            .VehicleType                  = playerRaceVehicle.at(0xA4).as<BPR::VehicleType>(),
            .InVehicleSelect              = false,
            .VehicleSelectDontStreamAudio = false,
            .ResetPlayerCamera            = true,
            .KeepResetSection             = true,
            .Unknown1                     = false,
            .Unknown2                     = 0,
        };
        BPR::GameActionQueue_AddGameAction(gameActionQueue, &gameAction, gameAction.ID, sizeof(gameAction));

        m_ChangeDeformation = false;
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
            .Speed          = vehicleData.at(0x99).as<uint8_t>(),
            .Strength       = vehicleData.at(0x9B).as<uint8_t>(),
            .BoostLossLevel = gameModule.at(0x40758).as<int32_t>(),
            .BoostLevel     = gameModule.at(0x40754).as<int32_t>(),
            .DamageLimit    = vehicleData.at(0x90).as<float>(),
            .BoostType      = static_cast<BPR::BoostType>(gameModule.at(0x3FFD4).as<int32_t>() - 1),
        };
        BPR::GameActionQueue_AddGameAction(gameActionQueue, &gameAction, gameAction.ID, sizeof(gameAction));

        m_ChangeBoost = false;
    }
}

void VehicleManager::OnUpdateActiveRaceVehicleColors()
{
    Core::Pointer playerActiveRaceVehicle = GetPlayerActiveRaceVehicle(); // BrnWorld::ActiveRaceCar*

    if (m_OverrideColor)
    {
        DirectX::XMStoreFloat3A(
            &playerActiveRaceVehicle.at(0x1480).as<DirectX::XMFLOAT3A>(),
            DirectX::XMColorModulate(DirectX::XMLoadFloat3A(&m_OverridenPaintColor), DirectX::XMLoadFloat3A(&m_OverridenPaintColorIntensity))
        );
        DirectX::XMStoreFloat3A(
            &playerActiveRaceVehicle.at(0x1490).as<DirectX::XMFLOAT3A>(),
            DirectX::XMColorModulate(DirectX::XMLoadFloat3A(&m_OverridenPearlColor), DirectX::XMLoadFloat3A(&m_OverridenPearlColorIntensity))
        );
    }
    else
    {
        m_OverridenPaintColor = playerActiveRaceVehicle.at(0x1480).as<DirectX::XMFLOAT3A>();
        m_OverridenPaintColorIntensity = { 1.0f, 1.0f, 1.0f };
        m_OverridenPearlColor = playerActiveRaceVehicle.at(0x1490).as<DirectX::XMFLOAT3A>();
        m_OverridenPearlColorIntensity = { 1.0f, 1.0f, 1.0f };
    }
}

void VehicleManager::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Vehicle Manager"))
    {
        Core::Pointer gameModule = Core::Pointer(0x013FC8E0).deref(); // BrnGame::BrnGameModule*
        Core::Pointer playerActiveRaceVehicle = GetPlayerActiveRaceVehicle(); // BrnWorld::ActiveRaceCar*
        Core::Pointer playerRaceVehicle = playerActiveRaceVehicle.at(0x7C0).as<void*>(); // BrnWorld::RaceCar*

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
                            if (vehicleFilter.PassFilter(vehicle.Name.c_str()))
                            {
                                ImGui::PushID(&vehicle);

                                bool selected = vehicle.ID == vehicleID;
                                if (ImGui::Selectable(vehicle.Name.c_str(), selected))
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
                            if (wheelFilter.PassFilter(wheel.Name.c_str()))
                            {
                                ImGui::PushID(&wheel);

                                bool selected = wheel.ID == wheelID;
                                if (ImGui::Selectable(wheel.Name.c_str(), selected))
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
            ImGui::SeparatorText("Misc");

            if (ImGui::Button("Change##deformation"))
            {
                m_ChangeDeformation = true;
            }
            
            ImGui::SliderFloat("Deformation", &playerActiveRaceVehicle.at(0x8AC).as<float>(), 0.0f, 2.0f);

            ImGui::Separator();

            if (ImGui::Button("Reset on Track"))
            {
                m_ResetOnTrack = true;
            }

            ImGui::Separator();

            ImGui::Checkbox("Boost Trails", &m_OverrideBoostTrails);
            ImGui::Checkbox("Switchable Boost", &m_OverrideSwitchableBoost);
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
                "Party",
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

            int32_t& boostType = gameModule.at(0x3FFD4).as<int32_t>();
            
            static constexpr const char* boostTypes[] =
            {
                "Speed",
                "Aggression",
                "Stunt",
                "None",
                "Locked",
            };
            if (ImGui::BeginCombo("Boost Type", boostTypes[boostType - 1]))
            {
                for (int i = 0; i < IM_ARRAYSIZE(boostTypes); ++i)
                {
                    bool selected = i == (boostType - 1);
                    if (ImGui::Selectable(boostTypes[i], selected))
                    {
                        boostType = i + 1;
                        m_ChangeBoost = true;
                    }
                }
                
                ImGui::EndCombo();
            }

            if (ImGui::SliderInt("Boost Level", &gameModule.at(0x40754).as<int32_t>(), 1, 20))
            {
                m_ChangeBoost = true;
            }
            if (ImGui::SliderInt("Boost Loss Level", &gameModule.at(0x40758).as<int32_t>(), 0, 20))
            {
                m_ChangeBoost = true;
            }
        }
    }
}

void VehicleManager::LoadVehicles()
{
    Core::Pointer vehicleList = BPR::PoolModule_FindResource("B5VehicleList")->Memory[0]; // BrnResource::VehicleListResource*

    uint32_t vehiclesCount = vehicleList.at(0x0).as<uint32_t>();
    for (uint32_t i = 0; i < vehiclesCount; ++i)
    {
        Core::Pointer vehicleData = vehicleList.at(0x4).deref().at(i * 0x108); // BrnResource::VehicleListEntry*

        m_Vehicles.push_back(
            Vehicle
            {
                .ID   = vehicleData.at(0x0).as<uint64_t>(),
                .Name = CreateVehicleName(vehicleData),
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
        Core::Pointer wheelData = wheelList.at(0x4).deref().at(i * 0x48); // BrnResource::WheelListEntry*

        m_Wheels.push_back(
            Wheel
            {
                .ID   = wheelData.at(0x0).as<uint64_t>(),
                .Name = wheelData.at(0x8).as<char[64]>(),
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
            return vehicle.Name.c_str();
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
            return wheel.Name.c_str();
        }
    }

    return "???";
}
