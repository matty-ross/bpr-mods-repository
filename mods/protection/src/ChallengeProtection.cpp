#include "ChallengeProtection.hpp"

#include "vendor/imgui.hpp"


ChallengeProtection::ChallengeProtection(ChallengesFile& challengesFile)
    :
    m_ChallengesFile(challengesFile)
{
}

void ChallengeProtection::OnFreeburnChallengeMessagePack(
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

void ChallengeProtection::OnFreeburnChallengeMessageUnpack(
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

void ChallengeProtection::OnRenderMenu()
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
            
            if (ImGui::Button("Change...##fallback-challenge"))
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
            
                for (Challenge& challenge : m_ChallengesFile.GetChallenges())
                {
                    if (challengeFilter.PassFilter(challenge.Title.c_str()))
                    {
                        ImGui::PushID(&challenge);
                        
                        ImGui::TableNextRow();
                        {
                            ImGui::TableNextColumn();
                            {
                                ImGui::AlignTextToFramePadding();
                                ImGui::TextUnformatted(challenge.Title.c_str());
                            }
                
                            ImGui::TableNextColumn();
                            {
                                ImGui::AlignTextToFramePadding();
                                ImGui::TextUnformatted(challenge.Replacement->Title);
                            }
                    
                            ImGui::TableNextColumn();
                            {
                                if (ImGui::Button("Change..."))
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
    Core::Pointer challengeList = Core::Pointer(0x013FC8E0).deref().at(0x690B70); // BrnResource::ChallengeList*

    int32_t challengesCount = challengeList.at(0x32E0).as<int32_t>();
    for (int32_t i = 0; i < challengesCount; ++i)
    {
        Core::Pointer challengeSlot = challengeList.at(0x400 + i * 0xC); // BrnResource::ChallengeSlot*
        int32_t listIndex = challengeSlot.at(0x4).as<int32_t>();
        int32_t entryIndex = challengeSlot.at(0x8).as<int32_t>();

        Core::Pointer list = challengeList.at(0x0 + listIndex * 0x20).as<void*>(); // BrnResource::ChallengeListResource*
        Core::Pointer entry = list.at(0x4).deref().at(entryIndex * 0xD8); // BrnResource::ChallengeListEntry*

        uint64_t challengeID = entry.at(0xC0).as<uint64_t>();
        bool isVanilla = GetVanillaChallenge(challengeID) != nullptr;
        bool isInFile = m_ChallengesFile.GetChallenge(challengeID) != nullptr;
        if (!isVanilla && !isInFile)
        {
            m_ChallengesFile.GetChallenges().push_back(
                Challenge
                {
                    .ID          = challengeID,
                    .Title       = entry.at(0xB0).as<char[16]>(),
                    .Replacement = m_ChallengesFile.GetFallbackChallenge(),
                }
            );
        }
    }
}

uint64_t ChallengeProtection::HandleChallengeID(uint64_t challengeID) const
{
    bool isVanilla = GetVanillaChallenge(challengeID) != nullptr;
    if (isVanilla)
    {
        return challengeID;
    }

    Challenge* challenge = m_ChallengesFile.GetChallenge(challengeID);
    if (challenge != nullptr)
    {
        return challenge->Replacement->ID;
    }

    return m_ChallengesFile.GetFallbackChallenge()->ID;
}
