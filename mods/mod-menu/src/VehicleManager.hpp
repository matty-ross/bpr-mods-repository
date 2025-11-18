#pragma once


#include <string>
#include <vector>
#include <DirectXMath.h>

#include "CustomColorsFile.hpp"


class VehicleManager
{
public:
    VehicleManager(CustomColorsFile& customColorsFile);

public:
    void OnDoUpdate();
    void OnPreWorldUpdate(void* gameEventQueue, void* gameActionQueue);
    void OnUpdateActiveRaceVehicleColors();

    void OnRenderMenu();

    void LoadVehicles();
    void LoadWheels();

private:
    struct Vehicle
    {
        uint64_t ID;
        std::string Name;
    };

    struct Wheel
    {
        uint64_t ID;
        std::string Name;
    };

private:
    void OverrideCurrentColorFromCustomColor(const CustomColor& customColor);
    void AddCurrentColorToCustomColorsFile(const char* name);

private:
    CustomColorsFile& m_CustomColorsFile;

    std::vector<Vehicle> m_Vehicles;
    bool m_ChangeVehicle = false;
    uint64_t m_NewVehicleID = 0;
    
    std::vector<Wheel> m_Wheels;
    bool m_ChangeWheel = false;
    uint64_t m_NewWheelID = 0;

    bool m_ReloadVehicle = false;

    bool m_OverrideSwitchableBoost = false;
    bool m_OverrideHoverMode = false;

    bool m_OverrideColor = false;
    DirectX::XMFLOAT3A m_OverridenPaintColor = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3A m_OverridenPaintColorIntensity = { 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT3A m_OverridenPearlColor = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3A m_OverridenPearlColorIntensity = { 1.0f, 1.0f, 1.0f };

    bool m_ChangeBoost = false;
};
