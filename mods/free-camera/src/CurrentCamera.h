#pragma once


#include "core/Pointer.h"


class CurrentCamera
{
public:
    void OnUpdate(Core::Pointer camera, Core::Pointer sharedInfo);
    void OnRenderMenu();
};