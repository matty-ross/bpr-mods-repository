#pragma once


#include "core/Pointer.hpp"

#include "CustomParametersFile.hpp"


class GameplayExternalCamera
{
public:
    GameplayExternalCamera(CustomParametersFile& customParametersFile);

public:
    void OnArbitratorUpdate(Core::Pointer camera, Core::Pointer arbStateSharedInfo);
    
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
    void SetCurrentParametersFromCustomParameters(const CustomParameters& customParameters);
    void AddCurrentParametersIntoCustomParameters(const char* name);

private:
    CustomParametersFile& m_CustomParametersFile;

    Parameters m_Parameters;
    bool m_ResetAllParameters = false;

    uint64_t m_PreviousAttribKey = 0x0000000000000000;
};
