#pragma once


#include "core/Pointer.h"


struct BehaviorData;
struct BehaviorParametersData;


class Behaviors
{
public:
    Behaviors();

public:
    void OnUpdate(Core::Pointer camera, Core::Pointer sharedInfo);
    void OnRenderMenu();

private:
    class Testbed
    {
        friend class Behaviors;

    public:
        Testbed(Behaviors& behaviors);

    private:
        struct BehaviorHandle
        {
            bool IsAllocated;
            int32_t BehaviorHelperIndex;
            void* BehaviorHelperPool;
            void* ParentController;
            void* Behavior;
        };

        enum class State
        {
            Inactive,
            Perpare,
            Update,
            Release
        };

    private:
        void OnPrepare(Core::Pointer sharedInfo);
        void OnUpdate(Core::Pointer camera);
        void OnRelease();

    private:
        Behaviors& m_Behaviors;
        
        BehaviorHandle m_BehaviorHandle = {};
        State m_State = State::Inactive;
    };
    
private:
    Testbed m_Testbed;
    
    const BehaviorData* m_SelectedBehavior = nullptr;
    const BehaviorParametersData* m_SelectedBehaviorParameters = nullptr;
    bool m_Selected = false;
    bool m_Activated = false;
};