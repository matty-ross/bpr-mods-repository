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
        ptrdiff_t Offset = 0x0;
        float DefaultValue = 0.0f;
        bool Reset = false;
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
        bool ResetAll = false;
    };

private:
    void SetCurrentParametersFromCustomParameters(const CustomParameters& customParameters);
    void AddCurrentParametersToCustomParametersFile(const char* name);

private:
    CustomParametersFile& m_CustomParametersFile;

    Parameters m_Parameters;
    uint64_t m_PreviousAttribKey = 0;
};
