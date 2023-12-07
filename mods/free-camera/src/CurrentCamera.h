#pragma once


#include "core/Pointer.h"

#include "MouseController.h"


class CurrentCamera
{
public:
    CurrentCamera(MouseController& mouseController);

public:
    void OnUpdate(Core::Pointer camera, Core::Pointer sharedInfo);
    void OnRenderMenu();

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
        float Rotation[3] = { 0.0f, 0.0f, 0.0f };
        float RotationDelta[3] = { 0.0f, 0.0f, 0.0f };
        float Translation[3] = { 0.0f, 0.0f, 0.0f };
        float TranslationDelta[3] = { 0.0f, 0.0f, 0.0f };
        bool Init = false;
        bool UseMouseController = false;
        bool Override = false;
    };

    struct Misc
    {
        Property<float> Fov;
        Property<float> Blurriness;
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

    struct BackgroundEffect
    {
        const char* HookName = nullptr;
        float BlendAmount = 0.0f;
        bool Active = false;
        bool Stop = false;
    };

private:
    MouseController& m_MouseController;

    Transformation m_Transformation;
    Misc m_Misc;
    Effects m_Effects;
    MotionBlur m_MotionBlur;
    BackgroundEffect m_BackgroundEffect;
};