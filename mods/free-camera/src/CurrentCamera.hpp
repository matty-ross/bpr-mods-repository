#pragma once


#include <Windows.h>

#include "core/Pointer.hpp"


class CurrentCamera
{
public:
    CurrentCamera();

public:
    void OnUpdate(Core::Pointer camera, Core::Pointer sharedInfo);
    void OnRenderMenu();
    void OnMouseInput(const RAWMOUSE& mouse);
    void OnWindowMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

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
        bool UseMouse = false;
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

    struct BackgroundEffect
    {
        const char* HookName = nullptr;
        float BlendAmount = 0.0f;
        bool Active = false;
        bool Stop = false;
    };

    struct MouseData
    {
        float PreviousPositionX = 0.0f;
        float PreviousPositionY = 0.0f;
    };

private:
    Transformation m_Transformation;
    Misc m_Misc;
    Effects m_Effects;
    MotionBlur m_MotionBlur;
    DepthOfField m_DepthOfField;
    BackgroundEffect m_BackgroundEffect;
    MouseData m_MouseData;
};
