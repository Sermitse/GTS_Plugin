
#include "UI/ImGui/Windows/Settings/Categories/Stats.hpp"

#include "Config/Config.hpp"

#include "UI/ImGui/Controls/CheckBox.hpp"
#include "UI/ImGui/Controls/ActorInfoCard.hpp"
#include "UI/ImGui/Controls/Slider.hpp"
#include "UI/ImGui/Controls/ToolTip.hpp"
#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/Core/ImUtil.hpp"

using namespace GTS;

namespace {

}

namespace GTS {

	CategoryStats::CategoryStats() {
		m_name = "Stats";
	}

	void CategoryStats::Draw() {
		static ImGuiEx::ActorInfoCard card;
		card.Draw(PlayerCharacter::GetSingleton());
	}

}