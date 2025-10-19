
#include "UI/ImGui/Windows/Settings/Categories/Stats.hpp"
#include "UI/ImGui/Controls/ActorInfoCard.hpp"
#include "UI/ImGui/Lib/imgui.h"

namespace GTS {

	CategoryStats::CategoryStats() {
		m_name = "Stats";
	}

	void CategoryStats::Draw() {

        static ImGuiEx::ActorInfoCard playerCard;
        playerCard.Draw(PlayerCharacter::GetSingleton());

        // Draw cards for current teammates
        for (const auto& actor : FindTeammates()) {
            if (const auto& Data = Transient::GetSingleton().GetActorData(actor)) {
                ImGui::SameLine();
                Data->InfoCard.Draw(actor);
            }
        }
	}
}