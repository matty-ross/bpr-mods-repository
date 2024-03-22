#include "Behaviors.h"

#include <array>
#include <vector>

#include "imgui.h"


struct BehaviorData
{
    using PostPrepareFn = void(*)(Core::Pointer);

    const char* Name;
    Core::Pointer NewBehaviorFunction;
    ptrdiff_t ParametersOffset;
    PostPrepareFn PostPrepare;
    std::vector<BehaviorParametersData> ParametersData;
};

struct BehaviorParametersData
{
    const char* Name;
    ptrdiff_t Offset;
};


static const std::array<BehaviorData, 8> k_BehaviorData =
{
    // AftertouchCrash
    BehaviorData
    {
        .Name                = "Aftertouch Crash",
        .NewBehaviorFunction = 0x004D8D50,
        .ParametersOffset    = 0x3C8,
        .PostPrepare         =
        [](Core::Pointer behavior) -> void
        {
        },
        .ParametersData
        {
            BehaviorParametersData{ .Name = "Aftertouch Crash", .Offset = 0x6C },
            BehaviorParametersData{ .Name = "Crash Debug",      .Offset = 0xDC },
        },
    },
    
    // GyroCam
    BehaviorData
    {
        .Name                = "Gyro Cam",
        .NewBehaviorFunction = 0x004D8B30,
        .ParametersOffset    = 0x10,
        .PostPrepare         =
        [](Core::Pointer behavior) -> void
        {
            behavior.at(0x28).as<bool>() = false;
        },
        .ParametersData
        {
            BehaviorParametersData{ .Name = "Default",                     .Offset = 0x1E0 },
            BehaviorParametersData{ .Name = "Truck Front",                 .Offset = 0x2AC },
            BehaviorParametersData{ .Name = "Left",                        .Offset = 0x378 },
            BehaviorParametersData{ .Name = "Right",                       .Offset = 0x444 },
            BehaviorParametersData{ .Name = "Default Side Trucking Left",  .Offset = 0x510 },
            BehaviorParametersData{ .Name = "Default Side Trucking Right", .Offset = 0x5DC },
            BehaviorParametersData{ .Name = "Follow",                      .Offset = 0x6A8 },
            BehaviorParametersData{ .Name = "Always Low",                  .Offset = 0x774 },
            BehaviorParametersData{ .Name = "Takedown",                    .Offset = 0x840 },
            BehaviorParametersData{ .Name = "Takedown Zoomed Out",         .Offset = 0x90C },
            BehaviorParametersData{ .Name = "High",                        .Offset = 0x9D8 },
            BehaviorParametersData{ .Name = "Helicam",                     .Offset = 0xAA4 },
            BehaviorParametersData{ .Name = "Drive By L",                  .Offset = 0xB70 },
            BehaviorParametersData{ .Name = "Drive By R",                  .Offset = 0xC3C },
        },
    },
    
    // BystanderCam
    BehaviorData
    {
        .Name                = "Bystander Cam",
        .NewBehaviorFunction = 0x004D9990,
        .ParametersOffset    = 0x340,
        .PostPrepare         =
        [](Core::Pointer behavior) -> void
        {
            behavior.at(0xC8).as<bool>() = false;
        },
        .ParametersData
        {
            BehaviorParametersData{ .Name = "Jump Left",        .Offset = 0xD08 },
            BehaviorParametersData{ .Name = "Jump",             .Offset = 0xDA4 },
            BehaviorParametersData{ .Name = "Jump From Behind", .Offset = 0xE40 },
            BehaviorParametersData{ .Name = "Close",            .Offset = 0xEDC },
            BehaviorParametersData{ .Name = "Medium",           .Offset = 0xF78 },
            BehaviorParametersData{ .Name = "Far",              .Offset = 0x1014 },
            BehaviorParametersData{ .Name = "Far Tall",         .Offset = 0x10B0 },
        },
    },
    
    // Rig
    BehaviorData
    {
        .Name                = "Rig",
        .NewBehaviorFunction = 0x004D9760,
        .ParametersOffset    = 0x450,
        .PostPrepare         =
        [](Core::Pointer behavior) -> void
        {
            behavior.at(0x18).as<bool>() = false;
        },
        .ParametersData
        {
            BehaviorParametersData{ .Name = "Bonnet Low Right",    .Offset = 0x1150 },
            BehaviorParametersData{ .Name = "Rear Q Fwd",          .Offset = 0x1270 },
            BehaviorParametersData{ .Name = "Front Q Cu Fwd",      .Offset = 0x1390 },
            BehaviorParametersData{ .Name = "Front Q Bwd",         .Offset = 0x14B0 },
            BehaviorParametersData{ .Name = "Front Rearview",      .Offset = 0x15D0 },
            BehaviorParametersData{ .Name = "Boot View Fwd",       .Offset = 0x16F0 },
            BehaviorParametersData{ .Name = "Front Q Low Bwd",     .Offset = 0x1810 },
            BehaviorParametersData{ .Name = "Roof Fwd",            .Offset = 0x1930 },
            BehaviorParametersData{ .Name = "Boot Fwd",            .Offset = 0x1A50 },
            BehaviorParametersData{ .Name = "Front Q Cu Fwd2",     .Offset = 0x1B70 },
            BehaviorParametersData{ .Name = "Underbelly",          .Offset = 0x1C90 },
            BehaviorParametersData{ .Name = "Drop Underbelly",     .Offset = 0x1DB0 },
            BehaviorParametersData{ .Name = "Drop Front Q Cu Fwd", .Offset = 0x1ED0 },
            BehaviorParametersData{ .Name = "Drop Boot View Fwd",  .Offset = 0x1FF0 },
        },
    },
    
    // LooseAttachment
    BehaviorData
    {
        .Name                = "Loose Attachment",
        .NewBehaviorFunction = 0x004D8C40,
        .ParametersOffset    = 0x314,
        .PostPrepare         =
        [](Core::Pointer behavior) -> void
        {
            behavior.at(0x258).as<bool>() = false;
            behavior.at(0x310).as<bool>() = true;
        },
        .ParametersData
        {
            BehaviorParametersData{ .Name = "Takedown 1", .Offset = 0x21F8 },
            BehaviorParametersData{ .Name = "Takedown 2", .Offset = 0x225C },
            BehaviorParametersData{ .Name = "Takedown 3", .Offset = 0x22C0 },
        },
    },
    
    // FixedCam
    BehaviorData
    {
        .Name                = "Fixed Cam",
        .NewBehaviorFunction = 0x004D9A90,
        .ParametersOffset    = 0x290,
        .PostPrepare         =
        [](Core::Pointer behavior) -> void
        {
            behavior.at(0x18).as<bool>() = false;
        },
        .ParametersData
        {
            BehaviorParametersData{ .Name = "Default", .Offset = 0x2324 },
        },
    },
    
    // RotateAboutVehicle
    BehaviorData
    {
        .Name                = "Rotate About Vehicle",
        .NewBehaviorFunction = 0x004D8FF0,
        .ParametersOffset    = 0x374,
        .PostPrepare         =
        [](Core::Pointer behavior) -> void
        {
        },
        .ParametersData
        {
            BehaviorParametersData{ .Name = "Default", .Offset = 0x2334 },
        },
    },
    
    // SpirallingDeathCam
    BehaviorData
    {
        .Name                = "Spiralling Death Cam",
        .NewBehaviorFunction = 0x004D8A20,
        .ParametersOffset    = 0x2C0,
        .PostPrepare         =
        [](Core::Pointer behavior) -> void
        {
        },
        .ParametersData
        {
            BehaviorParametersData{ .Name = "Default", .Offset = 0x23B4 },
        },
    },
};


namespace BPR
{
    // Originally templated, hence the function address parameter.
    static void BehaviorManager_NewBehavior(const void* functionAddress, void* behaviorHandle)
    {
        __asm
        {
            push 0
            push 0
            push 0
            push dword ptr [behaviorHandle]
            mov ecx, dword ptr ds:[0x013FC8E0]
            add ecx, 0x701890
        
            call dword ptr [functionAddress]
        }
    }

    static void BehaviorManager_UnsetBehaviorUsedByHandle(int32_t behaviorHelperIndex)
    {
        __asm
        {
            push dword ptr [behaviorHelperIndex]
            mov ecx, dword ptr ds:[0x013FC8E0]
            add ecx, 0x701890
        
            mov eax, 0x004D56D0
            call eax
        }
    }

    static void Camera_OperatorEquals(void* destination, void* source)
    {
        __asm
        {
            push dword ptr [source]
            mov ecx, dword ptr [destination]

            mov eax, 0x004D4F20
            call eax
        }
    }
}


Behaviors::Behaviors()
    :
    m_Testbed(*this)
{
}

void Behaviors::OnUpdate(Core::Pointer camera, Core::Pointer sharedInfo)
{
    switch (m_Testbed.m_State)
    {
    case Testbed::State::Inactive:
        {
        }
        break;

    case Testbed::State::Prepare:
        {
            m_Testbed.OnPrepare(sharedInfo);
            m_Testbed.m_State = Testbed::State::Update;
        }
        break;

    case Testbed::State::Update:
        {
            m_Testbed.OnUpdate(camera);
        }
        break;

    case Testbed::State::Release:
        {
            m_Testbed.OnRelease();
            m_Testbed.m_State = Testbed::State::Inactive;
        }
        break;
    }
}

void Behaviors::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Behaviors"))
    {
        if (ImGui::Button("Deactivate"))
        {
            if (m_Testbed.m_State == Testbed::State::Update)
            {
                m_Testbed.m_State = Testbed::State::Release;
                m_SelectedBehavior = nullptr;
                m_SelectedBehaviorParameters = nullptr;
            }
        }
        for (const BehaviorData& behaviorData : k_BehaviorData)
        {
            ImGui::SeparatorText(behaviorData.Name);
            ImGui::PushID(&behaviorData);
            if (
                ImGui::BeginListBox(
                    "##behavior-parameters-list",
                    ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * behaviorData.ParametersData.size() + ImGui::GetStyle().FramePadding.y * 2.0f)
                )
            )
            {
                for (const BehaviorParametersData& behaviorParametersData : behaviorData.ParametersData)
                {
                    ImGui::PushID(&behaviorParametersData);
                    if (ImGui::Selectable(behaviorParametersData.Name, m_SelectedBehaviorParameters == &behaviorParametersData))
                    {
                        if (m_Testbed.m_State == Testbed::State::Inactive)
                        {
                            m_Testbed.m_State = Testbed::State::Prepare;
                            m_SelectedBehavior = &behaviorData;
                            m_SelectedBehaviorParameters = &behaviorParametersData;
                        }
                    }
                    ImGui::PopID();
                }
                ImGui::EndListBox();
            }
            ImGui::PopID();
        }
    }
}


Behaviors::Testbed::Testbed(Behaviors& behaviors)
    :
    m_Behaviors(behaviors)
{
}

void Behaviors::Testbed::OnPrepare(Core::Pointer sharedInfo)
{
    memset(&m_BehaviorHandle, 0, sizeof(m_BehaviorHandle));
    BPR::BehaviorManager_NewBehavior(m_Behaviors.m_SelectedBehavior->NewBehaviorFunction.GetAddress(), &m_BehaviorHandle);

    Core::Pointer behavior = Core::Pointer(m_BehaviorHandle.BehaviorHelperPool).at(m_BehaviorHandle.BehaviorHelperIndex * 0x1A0).at(0x0).as<void*>();
    Core::Pointer namedParameters = sharedInfo.at(0x1C).as<void*>();
    behavior.at(m_Behaviors.m_SelectedBehavior->ParametersOffset).as<void*>() = namedParameters.at(m_Behaviors.m_SelectedBehaviorParameters->Offset).GetAddress();

    m_Behaviors.m_SelectedBehavior->PostPrepare(behavior);
}

void Behaviors::Testbed::OnUpdate(Core::Pointer camera)
{
    Core::Pointer behaviorHelperCamera = Core::Pointer(m_BehaviorHandle.BehaviorHelperPool).at(m_BehaviorHandle.BehaviorHelperIndex * 0x1A0).at(0x10);
    BPR::Camera_OperatorEquals(camera.GetAddress(), behaviorHelperCamera.GetAddress());
}

void Behaviors::Testbed::OnRelease()
{
    BPR::BehaviorManager_UnsetBehaviorUsedByHandle(m_BehaviorHandle.BehaviorHelperIndex);
    memset(&m_BehaviorHandle, 0, sizeof(m_BehaviorHandle));
}