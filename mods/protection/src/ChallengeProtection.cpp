#include "ChallengeProtection.hpp"

#include "vendor/imgui.hpp"


ChallengeProtection::ChallengeProtection(ChallengesFile& challengesFile)
    :
    m_ChallengesFile(challengesFile)
{
}

void ChallengeProtection::OnFreeburnChallengeMessagePack(Core::Pointer freeburnChallengeMessage)
{
    // BrnNetwork::FreeburnChallengeMessage* freeburnChallengeMessage
    
    if (!m_ChallengeProtectionEnabled)
    {
        return;
    }

    uint64_t challengeID = freeburnChallengeMessage.at(0x38).as<uint64_t>();
    challengeID = HandleChallengeID(challengeID);
    freeburnChallengeMessage.at(0x38).as<uint64_t>() = challengeID;
}

void ChallengeProtection::OnFreeburnChallengeMessageUnpack(Core::Pointer freeburnChallengeMessage)
{
    // BrnNetwork::FreeburnChallengeMessage* freeburnChallengeMessage

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
        using OnVanillaChallengeSelectedFn = void(*)(const VanillaChallenge&, void*);
        auto renderVanillaChallengesPopup = [](const char* title, uint64_t selectedChallengeID, void* parameter, OnVanillaChallengeSelectedFn onSelected) -> void
        {
            ImGui::SetNextWindowSize(ImVec2(0.0f, 500.0f));
            if (ImGui::BeginPopup("vanilla-challenges-popup"))
            {
                ImGui::SeparatorText(title);

                static ImGuiTextFilter vanillaChallengeFilter;
                vanillaChallengeFilter.Draw("Filter##vanilla-challenge");

                if (ImGui::BeginListBox("##vanilla-challenges-list", ImVec2(-FLT_MIN, -FLT_MIN)))
                {
                    for (const VanillaChallenge& vanillaChallenge : k_VanillaChallenges)
                    {
                        if (vanillaChallengeFilter.PassFilter(vanillaChallenge.Title))
                        {
                            bool selected = vanillaChallenge.ID == selectedChallengeID;
                            if (ImGui::Selectable(vanillaChallenge.Title, selected))
                            {
                                onSelected(vanillaChallenge, parameter);
                                ImGui::CloseCurrentPopup();
                            }
                            if (selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                    }

                    ImGui::EndListBox();
                }

                ImGui::EndPopup();
            }
        };
        
        {
            ImGui::Checkbox("Challenge Protection Enabled", &m_ChallengeProtectionEnabled);

            if (ImGui::Button("Save"))
            {
                m_ChallengesFile.Save();
            }
            ImGui::SameLine();
            if (ImGui::Button("Load"))
            {
                m_ChallengesFile.Load();
            }
        }

        {
            ImGui::SeparatorText("Fallback Challenge");
            
            ImGui::TextUnformatted(m_ChallengesFile.GetFallbackChallenge()->Title);
            ImGui::SameLine(300.0f);
            if (ImGui::Button("Change...##fallback-challenge-button", ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing())))
            {
                ImGui::OpenPopup("vanilla-challenges-popup");
            }
            renderVanillaChallengesPopup(
                "Fallback Challenge",
                m_ChallengesFile.GetFallbackChallenge()->ID,
                &m_ChallengesFile,
                [](const VanillaChallenge& vanillaChallenge, void* challengesFile) -> void
                {
                    static_cast<ChallengesFile*>(challengesFile)->SetFallbackChallenge(&vanillaChallenge);
                }
            );
        }

        {
            ImGui::SeparatorText("Challenges");
        
            static ImGuiTextFilter challengeFilter;
            challengeFilter.Draw("Filter##challenge");

            if (ImGui::BeginTable("##challenges-table", 3, ImGuiTableFlags_ScrollY, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 20)))
            {
                ImGui::TableSetupColumn("Challenge", ImGuiTableColumnFlags_WidthStretch, 0.3f);
                ImGui::TableSetupColumn("Replacement Challenge", ImGuiTableColumnFlags_WidthStretch, 0.5f);
                ImGui::TableSetupColumn("##change-challenge-column", ImGuiTableColumnFlags_WidthStretch, 0.2f);
                ImGui::TableHeadersRow();
            
                for (Challenge& challenge : m_ChallengesFile.GetChallenges())
                {
                    if (challengeFilter.PassFilter(challenge.Title.c_str()))
                    {
                        ImGui::PushID(&challenge);
                        ImGui::TableNextRow();
                        {
                            ImGui::TableNextColumn();
                            ImGui::TextUnformatted(challenge.Title.c_str());
                
                            ImGui::TableNextColumn();
                            ImGui::TextUnformatted(challenge.Replacement->Title);
                    
                            ImGui::TableNextColumn();
                            if (ImGui::Button("Change...##replacement-challenge-button", ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing())))
                            {
                                ImGui::OpenPopup("vanilla-challenges-popup");
                            }
                            renderVanillaChallengesPopup(
                                "Replacement Challenge",
                                challenge.Replacement->ID,
                                &challenge,
                                [](const VanillaChallenge& vanillaChallenge, void* challenge) -> void
                                {
                                    static_cast<Challenge*>(challenge)->Replacement = &vanillaChallenge;
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
    Core::Pointer challengeList = Core::Pointer(0x013FC8E0).deref().at(0x690B70);

    int32_t challengesCount = challengeList.at(0x32E0).as<int32_t>();
    for (int32_t i = 0; i < challengesCount; ++i)
    {
        Core::Pointer challengeSlot = challengeList.at(0x400 + i * 0xC);
        int32_t listIndex = challengeSlot.at(0x4).as<int32_t>();
        int32_t entryIndex = challengeSlot.at(0x8).as<int32_t>();

        Core::Pointer list = challengeList.at(0x0 + listIndex * 0x20).as<void*>();
        Core::Pointer entry = list.at(0x4).deref().at(entryIndex * 0xD8);

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
