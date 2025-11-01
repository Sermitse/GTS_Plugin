
#include "UI/ImGui/Windows/Settings/Categories/Stats.hpp"

#include "config/Config.hpp"

#include "UI/ImGui/Controls/ActorInfoCard.hpp"
#include "UI/ImGui/Controls/Misc.hpp"
#include "UI/ImGui/Core/ImColorUtils.hpp"
#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/Lib/imgui_internal.h"

namespace GTS {

	CategoryStats::CategoryStats() {
		m_name = "Stats";
	}

    
    void CategoryStats::Draw() {

        const float Width = (ImGui::GetContentRegionAvail().x / 3.0f - ImGui::GetStyle().ItemSpacing.x - ImGui::GetStyle().CellPadding.x);

		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize,  1.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg,
            ImUtil::Colors::AdjustLightness(
                ImVec4(Config::UI.f3AccentColor[0], Config::UI.f3AccentColor[1], Config::UI.f3AccentColor[2], ImGui::GetStyle().Colors[ImGuiCol_FrameBg].w),
                0.7
            )
        );

        // Draw player card
        if (const auto& Data = Transient::GetSingleton().GetActorData(PlayerCharacter::GetSingleton())) {
            Data->InfoCard.Draw(PlayerCharacter::GetSingleton(), { Width, 0 });
        }

        ImGuiEx::SeperatorVFullLength();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);

        // Draw teammate cards with paging
        auto teammates = FindTeammates();
        if (!teammates.empty()) {
            static int currentPage = 0;
            constexpr int cardsPerPage = 2;
            const int totalPages = (static_cast<int>(teammates.size()) + cardsPerPage - 1) / cardsPerPage;
            // Clamp current page if teammates vector shrunk
            if (currentPage >= totalPages) {
                currentPage = std::max(0, totalPages - 1);
            }

            // Scrollable area for teammate cards
            if (ImGui::BeginChild("##TeammateCards", { ImGui::GetContentRegionAvail().x, 0 }, ImGuiChildFlags_None, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings)) {
                const int startIdx = currentPage * cardsPerPage;
                const int endIdx = std::min(startIdx + cardsPerPage, static_cast<int>(teammates.size()));
                for (int idx = startIdx; idx < endIdx; ++idx) {
                    if (const auto& Data = Transient::GetSingleton().GetActorData(teammates[idx])) {
                        if (idx != startIdx) ImGui::SameLine();
                        Data->InfoCard.Draw(teammates[idx], { Width, 0 });
                    }
                }

                // Paging controls
                std::string text = fmt::format("Page {:d} / {:d}", currentPage + 1, totalPages);
                ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetFrameHeightWithSpacing());
                float buttonWidth = ImGui::CalcTextSize("<").x + ImGui::GetStyle().FramePadding.x * 2;
                float textWidth = ImGui::CalcTextSize(text.c_str()).x;
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (buttonWidth * 2 + textWidth + ImGui::GetStyle().ItemSpacing.x * 2));

                if (ImGui::Button("<") && currentPage > 0) {
                    currentPage--;
                }

                ImGui::SameLine();
                ImGui::Text(text.c_str());
                ImGui::SameLine();

                if (ImGui::Button(">") && currentPage < totalPages - 1) {
                    currentPage++;
                }
            }
            ImGui::EndChild();
        }
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();
    }
}
