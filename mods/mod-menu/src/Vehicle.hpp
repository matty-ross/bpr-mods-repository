#pragma once


#include <vector>


class Vehicle
{
public:
    void OnPreWorldUpdate(void* gameEventQueue, void* gameActionQueue);

    void OnRenderMenu();

    void LoadVehicleList();
    void LoadWheelList();
    void LoadColorPalettes();

private:
    struct VehicleData
    {
        uint64_t ID;
        const char* Name;
    };

    struct WheelData
    {
        uint64_t ID;
        const char* Name;
    };

    struct ColorPaletteData
    {
        int32_t ColorsCount;
    };

private:
    const char* GetVehicleName(uint64_t vehicleID) const;
    const char* GetWheelName(uint64_t wheelID) const;

private:
    std::vector<VehicleData> m_VehicleList;
    bool m_ChangeVehicle = false;
    uint64_t m_NewVehicleID = 0;
    
    std::vector<WheelData> m_WheelList;
    bool m_ChangeWheel = false;
    uint64_t m_NewWheelID = 0;

    bool m_ResetOnTrack = false;

    ColorPaletteData m_ColorPalettes[5];
    bool m_OverrideColor = false;
    alignas(16) float m_OverridenPaintColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    alignas(16) float m_OverridenPearlescentColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};
