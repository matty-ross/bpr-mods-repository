#pragma once


#include <Windows.h>

#include "core/Pointer.hpp"


class CurrentCamera
{
public:
    CurrentCamera();

public:
    void OnArbitratorUpdate(Core::Pointer camera, Core::Pointer arbStateSharedInfo);
    
    void OnRenderMenu();
    
    void OnMouseInput(const RAWMOUSE& mouse);

private:
    template <typename T>
    struct Property
    {
        ptrdiff_t Offset;
        T Value = {};
        bool Override = false;

        Property(ptrdiff_t offset)
        {
            Offset = offset;
        }
    };

    struct Transformation
    {
        alignas(16) float EyePosition[3] = { 0.0f, 0.0f, 0.0f };
        alignas(16) float TargetPosition[3] = { 0.0f, 0.0f, 0.0f };
        
        float Rotation[3] = { 0.0f, 0.0f, 0.0f };
        float RotationDelta[3] = { 0.0f, 0.0f, 0.0f };
        float Translation[3] = { 0.0f, 0.0f, 0.0f };
        float TranslationDelta[3] = { 0.0f, 0.0f, 0.0f };
        bool Init = false;
        bool Override = false;
    };

    struct Misc
    {
        Property<float> Fov;
    };
    
    struct Effects
    {
        Property<float> SimulationTimeScale;
        Property<float> CameraLag;
        Property<float> BlackBarAmount;
    };
    
    struct MotionBlur
    {
        Property<float> Vehicle;
        Property<float> World;
    };

    struct DepthOfField
    {
        Property<float> FocusStartDistance;
        Property<float> PerfectFocusStartDistance;
        Property<float> PerfectFocusEndDistance;
        Property<float> FocusEndDistance;
        Property<float> Blurriness;
    };

    struct Effect
    {
        const char* Name = nullptr;
        float BlendAmount = 0.0f;
        bool Play = false;
        bool Stop = false;
    };

private:
    Transformation m_Transformation;
    Misc m_Misc;
    Effects m_Effects;
    MotionBlur m_MotionBlur;
    DepthOfField m_DepthOfField;
    Effect m_Effect;
};
