#pragma once


#include <cstdint>

#include "core/Pointer.h"


class GameplayExternalCamera
{
public:
    void OnUpdate(Core::Pointer camera, Core::Pointer sharedInfo);
    void OnRenderMenu();

private:
    struct Parameter
    {
        float DefaultValue;
        bool Reset;
    };

private:
    uint64_t m_PreviousAttribKey = 0x0000000000000000;

    Parameter m_PitchSpring = {};
    Parameter m_YawSpring = {};
    Parameter m_PivotY = {};
    Parameter m_PivotZ = {};
    Parameter m_PivotZOffset = {};
    Parameter m_FOV = {};
    Parameter m_InFrontFOVMax = {};
    Parameter m_FrontInAmount = {};
    Parameter m_DriftYawSpring = {};
    Parameter m_BoostFOVZoomCompensation = {};
    Parameter m_DownAngle = {};
    Parameter m_DropFactor = {};

    bool m_UpdateDefaultValues = false;
    bool m_ResetAllParameters = false;
};