#pragma once


#include "core/Pointer.hpp"


struct BehaviorData;
struct BehaviorParametersData;


class Behaviors
{
public:
    void OnArbitratorUpdate(Core::Pointer camera, Core::Pointer arbStateSharedInfo);

    void OnRenderMenu();

private:
    enum class BehaviorState
    {
        Inactive,
        Prepare,
        Update,
        Release,
    };

private:
    void PrepareBehavior(Core::Pointer arbStateSharedInfo);
    void UpdateBehavior(Core::Pointer camera);
    void ReleaseBehavior();

private:
    BehaviorState m_BehaviorState = BehaviorState::Inactive;
    
    const BehaviorData* m_SelectedBehavior = nullptr;
    const BehaviorParametersData* m_SelectedBehaviorParameters = nullptr;

    Core::Pointer m_BehaviorHelper = nullptr; // BrnDirector::Camera::BehaviourManager::BehaviourHelper*
    int32_t m_BehaviorHelperIndex = -1;
};
