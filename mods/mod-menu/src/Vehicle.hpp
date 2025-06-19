#pragma once


#include <cstdint>


class Vehicle
{
public:
    void OnPreWorldUpdate(void* gameEventQueue, void* gameActionQueue);

    void OnRenderMenu();

private:
    bool m_ChangeVehicle = false;
    uint64_t m_NewVehicleID = 0;
};
