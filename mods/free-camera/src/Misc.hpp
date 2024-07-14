#pragma once


#include "core/Pointer.hpp"


class Misc
{
public:
    void OnSetPlayerVehicleIndex(Core::Pointer directorInputBuffer);

    void OnRenderMenu();

private:
    int32_t m_OverridenPlayerVehicleIndex = 0;
    bool m_OverridePlayerVehicleIndex = false;
};
