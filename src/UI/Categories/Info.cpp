#include "UI/Categories/Info.hpp"
#include "UI/ImGui/Lib/imgui.h"
#include "UI/imGui/ImUtil.hpp"
#include "UI/Windows/GTSInfo.hpp"
#include "UI/ImGui/ImFontManager.hpp"
#include "Utils/Text.hpp"

namespace GTS {

    void CategoryInfo::Draw() {

        //Get Curr Follower Count;

        ImGui::BeginChild("InfoWrapper",ImGui::GetContentRegionAvail());

        auto FollowerList = FindTeammates();

        const int TempFollowerCount = static_cast<int>(FollowerList.size());

        const int FollowerDiv = TempFollowerCount < 5 ? TempFollowerCount : 4;

        const float DivWidth = (ImGui::GetContentRegionAvail().x / (FollowerDiv + 1u)) - (ImGui::GetStyle().CellPadding.x + ImGui::GetStyle().FramePadding.x + ImGui::GetStyle().WindowPadding.x);

        ImGui::BeginChild("PlayerInfo", { DivWidth,0 }, ImGuiChildFlags_AutoResizeY);
        {
            auto Player = RE::PlayerCharacter::GetSingleton();

            if (Player) {

                std::string Name = Player->GetName();

                ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kWidgetTitle);

                //Player Singleton -> Get Name;
                ImGui::Text(str_toupper(Name).c_str());

                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 1.0f);

                ImFontManager::PopActiveFont();

                ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kWidgetBody);
                DrawGTSInfo(0, RE::PlayerCharacter::GetSingleton(), false);

                ImGui::Spacing();
                ImFontManager::PopActiveFont();
            }

        }
        ImGui::EndChild();

        if (TempFollowerCount == 0) {
            ImGui::EndChild();
            return;
        }

        //Sort by scale
        ranges::sort(FollowerList, [](Actor* a, Actor* b) {
            return get_visual_scale(a) > get_visual_scale(b);
        });


        for (auto Follower : FollowerList) {

            if (!Follower)
                continue;
            if (!Follower->Get3D())
                continue;
            if (!Follower->Is3DLoaded())
                continue;

            const std::string Name = Follower->GetName();

            //Derive a unique Id From the actors pointer
            const ImGuiID ActPtrUID = static_cast<uint32_t>(reinterpret_cast<std::uintptr_t>(Follower));

            ImUtil::SeperatorV();

            // Check if the next child will fit in the available space
            if (ImGui::GetContentRegionAvail().x < DivWidth) {
                ImGui::NewLine();
            }

            ImGui::BeginChild(ActPtrUID, { DivWidth, 0 }, ImGuiChildFlags_AutoResizeY);
            {
                ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kWidgetTitle);
                ImGui::Text(str_toupper(Name).c_str());
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 1.0f);
                ImFontManager::PopActiveFont();
            }
            {
                ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kWidgetBody);
                DrawGTSInfo(0, Follower, false);
                ImGui::Spacing();
                ImFontManager::PopActiveFont();
            }
            ImGui::EndChild();
        }

        ImGui::EndChild();
    }


}


