#pragma once


#include <string>
#include <vector>

#include "core/Logger.h"
#include "core/Pointer.h"


class GameplayExternalCamera
{
public:
    GameplayExternalCamera(Core::Logger& logger, const std::string& customParametersFilePath);

public:
    void OnUpdate(Core::Pointer camera, Core::Pointer sharedInfo);
    void OnRenderMenu();

    void LoadCustomParameters();
    void SaveCustomParameters();

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

    struct CustomParameters
    {
        std::string Name;
        float PitchSpring;
        float YawSpring;
        float PivotY;
        float PivotZ;
        float PivotZOffset;
        float FOV;
        float InFrontFOVMax;
        float FrontInAmount;
        float DriftYawSpring;
        float BoostFOVZoomCompensation;
        float DownAngle;
        float DropFactor;
    };

private:
    void SetCurrentParametersFromCustomParameters(const CustomParameters& customParameters);
    void AddCurrentParametersIntoCustomParameters(const char* name);

private:
    Core::Logger& m_Logger;

    uint64_t m_PreviousAttribKey = 0x0000000000000000;

    Parameters m_Parameters;
    bool m_ResetAllParameters = false;

    std::vector<CustomParameters> m_CustomParameters;
    std::string m_CustomParametersFilePath;
};