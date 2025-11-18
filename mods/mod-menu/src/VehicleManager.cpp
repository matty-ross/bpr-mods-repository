#include "VehicleManager.hpp"

#include <map>
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
    Core::Pointer vehicleData, // BrnResource::VehicleListEntry*
    std::map<uint64_t, int>& colorLiveries,
    std::map<uint64_t, int>& communityLiveries
)
{
    uint64_t vehicleID = vehicleData.at(0x0).as<uint64_t>();
    uint64_t parentVehicleID = vehicleData.at(0x8).as<uint64_t>();
    uint8_t liveryType = vehicleData.at(0xFD).as<uint8_t>();

    auto getVehicleID = [&]() -> std::string
    {
        char stringVehicleID[13] = {};
        BPR::CgsID_ConvertToString(vehicleID, stringVehicleID);

        char id[16] = {};
        sprintf_s(id, "%-8s - ", stringVehicleID);

        return id;
    };
    
    auto getVehicleManufacturer = [&]() -> std::string
    {
        int32_t vehicleManufacturer = BPR::ManufacturersIcon_GetVehicleManufacturer((parentVehicleID != 0) ? parentVehicleID : vehicleID);
        switch (vehicleManufacturer)
        {
        case 0: return "Carson ";
        case 1: return "Hunter ";
        case 2: return "Jansen ";
        case 3: return "Krieger ";
        case 4: return "Kitano ";
        case 5: return "Montgomery ";
        case 6: return "Nakamura ";
        case 7: return "Rossolini ";
        case 8: return "Watson ";
        }

        return "";
    };
    
    auto getVehicleName = [&]() -> std::string
    {
        char vehicleNameStringID[32] = "CAR_CAPS_";
        BPR::CgsID_ConvertToString((liveryType == 2 || parentVehicleID == 0) ? vehicleID : parentVehicleID, vehicleNameStringID + 9);
        const char* vehicleName = BPR::LanguageManager_FindString(vehicleNameStringID);
        
        return (vehicleName != nullptr) ? vehicleName : vehicleData.at(0x30).as<char[64]>();
    };

    auto getVehicleLivery = [&]() -> std::string
    {
        char livery[16] = {};
        
        switch (liveryType)
        {
        case 0:
        case 2:
            sprintf_s(livery, " Finish %d", ++colorLiveries[vehicleID]);
            return livery;
            
        case 1:
            sprintf_s(livery, " Finish %d", ++colorLiveries[parentVehicleID]);
            return livery;

        case 5:
            sprintf_s(livery, " Community %d", ++communityLiveries[parentVehicleID]);
            return livery;
        
        case 3:
            return " Platinum";
        
        case 4:
            return " Gold";
        }

        return "";
    };

    return getVehicleID() + getVehicleManufacturer() + getVehicleName() + getVehicleLivery();
}

static std::string CreateWheelName(
    Core::Pointer wheelData // BrnResource::WheelListEntry*
)
{
    uint64_t wheelID = wheelData.at(0x0).as<uint64_t>();
    
    auto getWheelID = [&]() -> std::string
    {
        char stringWheelID[13] = {};
        BPR::CgsID_ConvertToString(wheelID, stringWheelID);

        char id[16] = {};
        sprintf_s(id, "%-8s - ", stringWheelID);

        return id;
    };

    auto getWheelName = [&]() -> std::string
    {
        return wheelData.at(0x8).as<char[64]>();
    };
    
    return getWheelID() + getWheelName();
}

static Core::Pointer GetPlayerActiveRaceVehicle()
{
    int32_t playerVehicleIndex = Core::Pointer(0x013FC8E0).deref().at(0x40C28).as<int32_t>();
    Core::Pointer playerActiveRaceVehicle = Core::Pointer(0x013FC8E0).deref().at(0x12980 + playerVehicleIndex * 0x4180); // BrnWorld::ActiveRaceCar*

    return playerActiveRaceVehicle;
}

static bool CanResetPlayerVehicle()
{
    static constexpr int32_t disallowedGameModeTypes[] =
    {
        10, // Online Race
        12, // Online Stunt Run
        14, // Online Stunt Run Free-For-All
        17, // Online Stunt Run Co-Op
    };

    int32_t currentGameModeType = Core::Pointer(0x013FC8E0).deref().at(0x69D58C).as<int32_t>();
    for (int32_t disallowedGameModeType : disallowedGameModeTypes)
    {
        if (currentGameModeType == disallowedGameModeType)
        {
            return false;
        }
    }

    bool isChallengeTimerRunning = Core::Pointer(0x013FC8E0).deref().at(0x6A4104).as<bool>();
    if (isChallengeTimerRunning)
    {
        return false;
    }

    void* currentFreeburnGame = Core::Pointer(0x013FC8E0).deref().at(0x6EBE50).as<void*>();
    if (currentFreeburnGame != nullptr)
    {
        return false;
    }

    return true;
}


VehicleManager::VehicleManager(CustomColorsFile& customColorsFile)
    :
    m_CustomColorsFile(customColorsFile)
{
}

void VehicleManager::OnDoUpdate()
{
    Core::Pointer playerActiveRaceVehicle = GetPlayerActiveRaceVehicle(); // BrnWorld::ActiveRaceCar*
    Core::Pointer playerRaceVehicle = playerActiveRaceVehicle.at(0x7C0).as<void*>(); // BrnWorld::RaceCar*

    constexpr uint64_t originalVehicleID = 0xA56601CB30510000; // Jansen P12 88 Special
    uint64_t vehicleID = playerRaceVehicle.at(0x68).as<uint64_t>();

    Core::Pointer(0x01490588).as<uint64_t>() = m_OverrideHoverMode ? vehicleID : originalVehicleID;
    Core::Pointer(0x0148EC20).as<uint64_t>() = m_OverrideHoverMode ? vehicleID : originalVehicleID;
    Core::Pointer(0x014A08D0).as<uint64_t>() = m_OverrideHoverMode ? vehicleID : originalVehicleID;
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

    if (m_ReloadVehicle)
    {
        BPR::GameAction_ResetPlayerVehicle gameAction =
        {
            .Position                     = { 0.0f, 0.0f, 0.0f, 0.0f },
            .Direction                    = { 0.0f, 0.0f, 0.0f, 0.0f },
            .VehicleID                    = playerRaceVehicle.at(0x68).as<uint64_t>(),
            .WheelID                      = playerRaceVehicle.at(0x70).as<uint64_t>(),
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

        m_ReloadVehicle = false;
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

        bool canResetPlayerVehicle = CanResetPlayerVehicle();

        {
            ImGui::SeparatorText("Vehicle List");

            static ImGuiTextFilter vehicleFilter;
            vehicleFilter.Draw("Filter##vehicle-list");

            if (!canResetPlayerVehicle)
            {
                ImGui::BeginDisabled();
            }

            if (ImGui::BeginListBox("##vehicle-list", ImVec2(-FLT_MIN, 0.0f)))
            {
                uint64_t vehicleID = playerRaceVehicle.at(0x68).as<uint64_t>();
                
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

            if (!canResetPlayerVehicle)
            {
                ImGui::EndDisabled();
            }
        }
        
        {
            ImGui::SeparatorText("Wheel List");

            static ImGuiTextFilter wheelFilter;
            wheelFilter.Draw("Filter##wheel-list");

            if (!canResetPlayerVehicle)
            {
                ImGui::BeginDisabled();
            }
            
            if (ImGui::BeginListBox("##wheel-list", ImVec2(-FLT_MIN, 0.0f)))
            {
                uint64_t wheelID = playerRaceVehicle.at(0x70).as<uint64_t>();
                
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

            if (!canResetPlayerVehicle)
            {
                ImGui::EndDisabled();
            }
        }
        
        {
            ImGui::SeparatorText("Misc");

            if (!canResetPlayerVehicle)
            {
                ImGui::BeginDisabled();
            }
            
            if (ImGui::Button("Reload Vehicle"))
            {
                m_ReloadVehicle = true;
            }

            if (!canResetPlayerVehicle)
            {
                ImGui::EndDisabled();
            }

            ImGui::SliderFloat("Deformation", &playerActiveRaceVehicle.at(0x8AC).as<float>(), 0.0f, 2.0f);

            ImGui::Separator();

            ImGui::Checkbox("Switchable Boost", &m_OverrideSwitchableBoost);
            ImGui::Checkbox("Hover Mode", &m_OverrideHoverMode);
        }

        {
            ImGui::SeparatorText("Color");

            static constexpr const char* colorPaletteTypes[] =
            {
                "Gloss",
                "Metallic",
                "Pearlescent",
                "Special",
                "Party",
            };    
            ImGui::Combo("Color Palette Type", &playerRaceVehicle.at(0x98).as<int32_t>(), colorPaletteTypes, IM_ARRAYSIZE(colorPaletteTypes));
            ImGui::InputInt("Color Index", &playerRaceVehicle.at(0x94).as<int32_t>());

            ImGui::Separator();

            ImGui::Checkbox("Override Color", &m_OverrideColor);
            ImGui::ColorEdit3("Paint Color", reinterpret_cast<float*>(&m_OverridenPaintColor));
            ImGui::DragFloat3("Paint Color Intensity", reinterpret_cast<float*>(&m_OverridenPaintColorIntensity));
            ImGui::ColorEdit3("Pearl Color", reinterpret_cast<float*>(&m_OverridenPearlColor));
            ImGui::DragFloat3("Pearl Color Intensity", reinterpret_cast<float*>(&m_OverridenPearlColorIntensity));
        }

        {
            ImGui::SeparatorText("Custom Colors");
            
            {
                if (ImGui::Button("Save##custom-colors-file"))
                {
                    m_CustomColorsFile.Save();
                }

                ImGui::SameLine();

                if (ImGui::Button("Load##custom-colors-file"))
                {
                    m_CustomColorsFile.Load();
                }
            }

            ImGui::Separator();

            {
                static char name[64] = {};
                if (ImGui::Button("Add Current Color"))
                {
                    if (name[0] != '\0')
                    {
                        AddCurrentColorToCustomColorsFile(name);
                        name[0] = '\0';
                    }
                }

                ImGui::SameLine();

                ImGui::InputText("Name", name, IM_ARRAYSIZE(name));
            }

            ImGui::Separator();

            {
                static ImGuiTextFilter customColorsFilter;
                customColorsFilter.Draw("Filter##custom-colors");

                if (ImGui::BeginListBox("##custom-colors", ImVec2(-FLT_MIN, 0.0f)))
                {
                    for (const CustomColor& customColor : m_CustomColorsFile.GetCustomColors())
                    {
                        if (customColorsFilter.PassFilter(customColor.Name.c_str()))
                        {
                            ImGui::PushID(&customColor);

                            if (ImGui::Selectable(customColor.Name.c_str()))
                            {
                                OverrideCurrentColorFromCustomColor(customColor);
                                m_OverrideColor = true;
                            }

                            ImGui::PopID();
                        }
                    }

                    ImGui::EndListBox();
                }
            }
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

            if (ImGui::SliderInt("Boost Level", &gameModule.at(0x40754).as<int32_t>(), 1, 20, "%d", ImGuiSliderFlags_AlwaysClamp))
            {
                m_ChangeBoost = true;
            }
            if (ImGui::SliderInt("Boost Loss Level", &gameModule.at(0x40758).as<int32_t>(), 0, 20, "%d", ImGuiSliderFlags_AlwaysClamp))
            {
                m_ChangeBoost = true;
            }
        }
    }
}

void VehicleManager::LoadVehicles()
{
    static constexpr uint64_t disallowedVehicleIDs[] =
    {
        0xA5939AE858360000, // P_EU_AstonMartin_01
    };
    
    Core::Pointer vehicleList = BPR::PoolModule_FindResource("B5VehicleList")->Memory[0]; // BrnResource::VehicleListResource*

    std::map<uint64_t, int> colorLiveries;
    std::map<uint64_t, int> communityLiveries;

    uint32_t vehiclesCount = vehicleList.at(0x0).as<uint32_t>();
    m_Vehicles.reserve(vehiclesCount);
    
    for (uint32_t i = 0; i < vehiclesCount; ++i)
    {
        Core::Pointer vehicleData = vehicleList.at(0x4).deref().at(i * 0x108); // BrnResource::VehicleListEntry*

        auto it = std::find(std::begin(disallowedVehicleIDs), std::end(disallowedVehicleIDs), vehicleData.at(0x0).as<uint64_t>());
        if (it == std::end(disallowedVehicleIDs))
        {
            m_Vehicles.push_back(
                Vehicle
                {
                    .ID   = vehicleData.at(0x0).as<uint64_t>(),
                    .Name = CreateVehicleName(vehicleData, colorLiveries, communityLiveries),
                }
            );
        }
    }
}

void VehicleManager::LoadWheels()
{
    Core::Pointer wheelList = BPR::PoolModule_FindResource("B5WheelList")->Memory[0]; // BrnResource::WheelListResource*

    uint32_t wheelsCount = wheelList.at(0x0).as<uint32_t>();
    m_Wheels.reserve(wheelsCount);
    
    for (uint32_t i = 0; i < wheelsCount; ++i)
    {
        Core::Pointer wheelData = wheelList.at(0x4).deref().at(i * 0x48); // BrnResource::WheelListEntry*

        m_Wheels.push_back(
            Wheel
            {
                .ID   = wheelData.at(0x0).as<uint64_t>(),
                .Name = CreateWheelName(wheelData),
            }
        );
    }
}

void VehicleManager::OverrideCurrentColorFromCustomColor(const CustomColor& customColor)
{
    auto convert = [](const std::array<float, 3> input) -> DirectX::XMFLOAT3A
    {
        DirectX::XMFLOAT3A output = {};
        output.x = input[0];
        output.y = input[1];
        output.z = input[2];
        return output;
    };

    m_OverridenPaintColor = convert(customColor.PaintColor);
    m_OverridenPaintColorIntensity = convert(customColor.PaintColorIntensity);
    m_OverridenPearlColor = convert(customColor.PearlColor);
    m_OverridenPearlColorIntensity = convert(customColor.PearlColorIntensity);
}

void VehicleManager::AddCurrentColorToCustomColorsFile(const char* name)
{
    auto convert = [](const DirectX::XMFLOAT3A& input) -> std::array<float, 3>
    {
        std::array<float, 3> output = {};
        output[0] = input.x;
        output[1] = input.y;
        output[2] = input.z;
        return output;
    };
    
    m_CustomColorsFile.GetCustomColors().push_back(
        CustomColor
        {
            .Name                = name,
            .PaintColor          = convert(m_OverridenPaintColor),
            .PaintColorIntensity = convert(m_OverridenPaintColorIntensity),
            .PearlColor          = convert(m_OverridenPearlColor),
            .PearlColorIntensity = convert(m_OverridenPearlColorIntensity),
        }
    );
}
