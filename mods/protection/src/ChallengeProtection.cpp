#include <cstdint>

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"
#include "core/Logger.hpp"
#include "core/Patch.hpp"

#include "Protection.hpp"
#include "Challenges.hpp"
#include "ChallengesFile.hpp"
#include "ChallengeProtection.hpp"


ChallengeProtection::ChallengeProtection(ChallengesFile& challengesFile, const Core::Logger& logger)
    :
    m_ChallengesFile(challengesFile),
    m_Logger(logger)
{
}

void ChallengeProtection::Load()
{
    Core::Patch(0x0790A490, 5, m_Logger).WriteJMP(Hook_CheckFreeburnChallengeMessageBeforePacking);
    Core::Patch(0x0790A49A, 7, m_Logger).WriteJMP(Hook_CheckFreeburnChallengeMessageAfterUnpacking);
}

void ChallengeProtection::RenderMenu()
{
    if (ImGui::CollapsingHeader("Challenge Protection"))
    {
        static constexpr char vanillaChallengesPopupID[] = "vanilla-challenges-popup";

        auto renderVanillaChallengesPopup = []<typename Fn>(const char* title, uint64_t selectedChallengeID, Fn onSelected) -> void
        {
            ImGui::SetNextWindowSize(ImVec2(0.0f, 500.0f));

            if (ImGui::BeginPopup(vanillaChallengesPopupID))
            {
                ImGui::SeparatorText(title);

                static ImGuiTextFilter vanillaChallengeFilter;
                vanillaChallengeFilter.Draw("Filter##vanilla-challenge");

                if (ImGui::BeginListBox("##vanilla-challenges", ImVec2(-FLT_MIN, -FLT_MIN)))
                {
                    for (const VanillaChallenge& vanillaChallenge : k_VanillaChallenges)
                    {
                        if (vanillaChallengeFilter.PassFilter(vanillaChallenge.Title))
                        {
                            ImGui::PushID(&vanillaChallenge);

                            bool selected = vanillaChallenge.ID == selectedChallengeID;
                            if (ImGui::Selectable(vanillaChallenge.Title, selected))
                            {
                                onSelected(vanillaChallenge);
                                vanillaChallengeFilter.Clear();
                                ImGui::CloseCurrentPopup();
                            }
                            if (selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }

                            ImGui::PopID();
                        }
                    }

                    ImGui::EndListBox();
                }

                ImGui::EndPopup();
            }
        };

        {
            ImGui::Checkbox("Challenge Protection Enabled", &m_ChallengeProtectionEnabled);

            if (ImGui::Button("Save##challenges-file"))
            {
                m_ChallengesFile.Save();
            }

            ImGui::SameLine();

            if (ImGui::Button("Load##challenges-file"))
            {
                m_ChallengesFile.Load();
            }
        }

        ImGui::Separator();

        {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Fallback Challenge   %s", m_ChallengesFile.GetFallbackChallenge()->Title);

            ImGui::SameLine(0.0f, 20.0f);

            if (ImGui::Button("Change##fallback-challenge"))
            {
                ImGui::OpenPopup(vanillaChallengesPopupID);
            }

            renderVanillaChallengesPopup(
                "Fallback Challenge",
                m_ChallengesFile.GetFallbackChallenge()->ID,
                [&](const VanillaChallenge& vanillaChallenge) -> void
                {
                    m_ChallengesFile.SetFallbackChallenge(&vanillaChallenge);
                }
            );
        }

        ImGui::Separator();

        {
            static ImGuiTextFilter challengeFilter;
            challengeFilter.Draw("Filter##challenge");

            if (ImGui::BeginTable("##challenges", 3, ImGuiTableFlags_ScrollY, ImVec2(0.0f, 400.0f)))
            {
                ImGui::TableSetupScrollFreeze(0, 1);
                ImGui::TableSetupColumn("Challenge", ImGuiTableColumnFlags_WidthStretch, 0.3f);
                ImGui::TableSetupColumn("Replacement Challenge", ImGuiTableColumnFlags_WidthStretch, 0.5f);
                ImGui::TableSetupColumn("##change-challenge", ImGuiTableColumnFlags_WidthStretch, 0.2f);
                ImGui::TableHeadersRow();

                for (ChallengesFile::Challenge& challenge : m_ChallengesFile.GetChallenges())
                {
                    if (challengeFilter.PassFilter(challenge.Title.c_str()))
                    {
                        ImGui::PushID(&challenge);

                        ImGui::TableNextRow();
                        {
                            ImGui::TableNextColumn();

                            ImGui::AlignTextToFramePadding();
                            ImGui::TextUnformatted(challenge.Title.c_str());
                        }
                        {
                            ImGui::TableNextColumn();

                            ImGui::AlignTextToFramePadding();
                            ImGui::TextUnformatted(challenge.Replacement->Title);
                        }
                        {
                            ImGui::TableNextColumn();

                            if (ImGui::Button("Change##replacement-challenge"))
                            {
                                ImGui::OpenPopup(vanillaChallengesPopupID);
                            }

                            renderVanillaChallengesPopup(
                                "Replacement Challenge",
                                challenge.Replacement->ID,
                                [&](const VanillaChallenge& vanillaChallenge) -> void
                                {
                                    challenge.Replacement = &vanillaChallenge;
                                }
                            );
                        }

                        ImGui::PopID();
                    }
                }

                ImGui::EndTable();
            }
        }
    }
}

void ChallengeProtection::AddNonVanillaChallengesToChallengesFile()
{
    Core::Pointer challengeListResource = Core::Pointer(0x013FC8E0).deref().at(0x690B70).as<void*>(); // BrnResource::ChallengeListResource*

    uint32_t challengesCount = challengeListResource.at(0x0).as<uint32_t>();
    for (uint32_t i = 0; i < challengesCount; ++i)
    {
        Core::Pointer challengeListEntry = challengeListResource.at(0x4).deref().at(i * 0xD8); // BrnResource::ChallengeListEntry*

        uint64_t challengeID = challengeListEntry.at(0xC0).as<uint64_t>();
        bool isVanilla = FindVanillaChallengeByID(challengeID) != nullptr;
        bool isInFile = m_ChallengesFile.FindChallengeByID(challengeID) != nullptr;

        if (!isVanilla && !isInFile)
        {
            m_ChallengesFile.AddChallenge(
                ChallengesFile::Challenge
                {
                    .ID = challengeID,
                    .Title = challengeListEntry.at(0xB0).as<char[16]>(),
                    .Replacement = m_ChallengesFile.GetFallbackChallenge(),
                }
            );
        }
    }
}

uint64_t ChallengeProtection::HandleChallengeID(uint64_t challengeID) const
{
    bool isVanilla = FindVanillaChallengeByID(challengeID) != nullptr;
    if (isVanilla)
    {
        return challengeID;
    }

    ChallengesFile::Challenge* challenge = m_ChallengesFile.FindChallengeByID(challengeID);
    if (challenge != nullptr)
    {
        return challenge->Replacement->ID;
    }

    return m_ChallengesFile.GetFallbackChallenge()->ID;
}

void ChallengeProtection::CheckFreeburnChallengeMessageBeforePacking(
    Core::Pointer freeburnChallengeMessage // BrnNetwork::FreeburnChallengeMessage*
)
{
    if (!m_ChallengeProtectionEnabled)
    {
        return;
    }

    uint64_t challengeID = freeburnChallengeMessage.at(0x38).as<uint64_t>();
    challengeID = HandleChallengeID(challengeID);
    freeburnChallengeMessage.at(0x38).as<uint64_t>() = challengeID;
}

void ChallengeProtection::CheckFreeburnChallengeMessageAfterUnpacking(
    Core::Pointer freeburnChallengeMessage // BrnNetwork::FreeburnChallengeMessage*
)
{
    if (!m_ChallengeProtectionEnabled)
    {
        return;
    }

    uint64_t challengeID = freeburnChallengeMessage.at(0x38).as<uint64_t>();
    challengeID = HandleChallengeID(challengeID);
    freeburnChallengeMessage.at(0x38).as<uint64_t>() = challengeID;
}

__declspec(naked) void ChallengeProtection::Hook_CheckFreeburnChallengeMessageBeforePacking()
{
    /*
        BrnNetwork::BrnNetworkManager::PackOrUnpackResult __thiscall BrnNetwork::FreeburnChallengeMessage::PackOrUnpack()
    */

    __asm
    {
        // esi: BrnNetwork::FreeburnChallengeMessage* this

        pushfd
        pushad

        cmp dword ptr [esi + 0x4], 0 // CgsNetwork::Message::EPackOrUnpack::E_PACK_INTO_BITSTREAM
        jne _end

        push esi
        mov ecx, offset Protection::s_Instance.m_ChallengeProtection
        call ChallengeProtection::CheckFreeburnChallengeMessageBeforePacking

    _end:
        popad
        popfd

        // Original code.
        mov ecx, esi
        push edx
        mov bl, al

        // Jump back.
        push 0x0790A495
        ret
    }
}

__declspec(naked) void ChallengeProtection::Hook_CheckFreeburnChallengeMessageAfterUnpacking()
{
    /*
        BrnNetwork::BrnNetworkManager::PackOrUnpackResult __thiscall BrnNetwork::FreeburnChallengeMessage::PackOrUnpack()
    */

    __asm
    {
        // BrnNetwork::FreeburnChallengeMessage* this

        pushfd
        pushad

        cmp dword ptr [esi + 0x4], 1 // CgsNetwork::Message::EPackOrUnpack::E_UNPACK_FROM_BITSTREAM
        jne _end

        push esi
        mov ecx, offset Protection::s_Instance.m_ChallengeProtection
        call ChallengeProtection::CheckFreeburnChallengeMessageAfterUnpacking

    _end:
        popad
        popfd

        // Original code.
        push 2
        push 0
        lea ecx, [esi + 48]

        // Jump back.
        push 0x0790A4A1
        ret
    }
}
