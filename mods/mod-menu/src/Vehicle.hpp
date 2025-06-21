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
        char Name[100];
    };

    struct WheelData
    {
        uint64_t ID;
        char Name[100];
    };

    struct ColorPaletteData
    {
        int32_t colorsCount;
    };

private:
    std::vector<VehicleData> m_VehicleList;
    bool m_ChangeVehicle = false;
    uint64_t m_NewVehicleID = 0;
    
    std::vector<WheelData> m_WheelList;
    bool m_ChangeWheel = false;
    uint64_t m_NewWheelID = 0;

    ColorPaletteData m_ColorPalettes[5];
};
