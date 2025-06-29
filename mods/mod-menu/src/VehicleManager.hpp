#pragma once


#include <vector>
#include <DirectXMath.h>


class VehicleManager
{
public:
    void OnPreWorldUpdate(void* gameEventQueue, void* gameActionQueue);
    void OnUpdateActiveRaceVehicleColors();

    void OnRenderMenu();

    void LoadVehicles();
    void LoadWheels();
    void LoadColorPalettes();

    bool OverrideBoostFlames() const;

private:
    struct Vehicle
    {
        uint64_t ID;
        const char* Name;
    };

    struct Wheel
    {
        uint64_t ID;
        const char* Name;
    };

    struct ColorPalette
    {
        int32_t ColorsCount;
    };

private:
    const char* GetVehicleName(uint64_t vehicleID) const;
    const char* GetWheelName(uint64_t wheelID) const;

private:
    std::vector<Vehicle> m_Vehicles;
    bool m_ChangeVehicle = false;
    uint64_t m_NewVehicleID = 0;
    
    std::vector<Wheel> m_Wheels;
    bool m_ChangeWheel = false;
    uint64_t m_NewWheelID = 0;

    bool m_ChangeDeformation = false;
    
    bool m_ResetOnTrack = false;

    bool m_OverrideBoostFlames = false;

    ColorPalette m_ColorPalettes[5] = {};
    bool m_OverrideColor = false;
    DirectX::XMFLOAT3A m_OverridenPaintColor = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3A m_OverridenPaintColorIntensity = { 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT3A m_OverridenPearlColor = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3A m_OverridenPearlColorIntensity = { 1.0f, 1.0f, 1.0f };

    bool m_ChangeBoost = false;
};
