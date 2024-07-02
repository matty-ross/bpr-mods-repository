#pragma once


#include "core/Pointer.hpp"


struct BehaviorData;
struct BehaviorParametersData;


class Behaviors
{
public:
    Behaviors();

public:
    void OnArbitratorUpdate(Core::Pointer camera, Core::Pointer arbStateSharedInfo);

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
            Prepare,
            Update,
            Release
        };

    private:
        void OnPrepare(Core::Pointer arbStateSharedInfo);
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
};
