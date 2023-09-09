#pragma once


#include "core/Pointer.h"


class GameplayExternalCamera
{
public:
    GameplayExternalCamera();

public:
    void OnUpdate(Core::Pointer camera, Core::Pointer sharedInfo);
    void OnRenderMenu();

private:
    struct Parameter
    {
        ptrdiff_t Offset;
        float DefaultValue = 0.0f;
        bool Reset = false;

        Parameter(ptrdiff_t offset)
        {
            Offset = offset;
        }
    };

    struct Parameters
    {
        Parameter PitchSpring;
        Parameter YawSpring;
        Parameter PivotY;
        Parameter PivotZ;
        Parameter PivotZOffset;
        Parameter FOV;
        Parameter InFrontFOVMax;
        Parameter FrontInAmount;
        Parameter DriftYawSpring;
        Parameter BoostFOVZoomCompensation;
        Parameter DownAngle;
        Parameter DropFactor;
    };

private:
    uint64_t m_PreviousAttribKey = 0x0000000000000000;

    Parameters m_Parameters;
    bool m_ResetAllParameters = false;
};