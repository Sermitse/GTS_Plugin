#include "UI/Controls/QuestTracker.hpp"

#include "ProgressBar.hpp"

#include "Config/Config.hpp"

#include "UI/Core/ImColorUtils.hpp"
#include "UI/Core/ImFontManager.hpp"
#include "UI/Lib/imgui.h"

namespace ImGuiEx {
	
	void DrawQuestProgress(const ImVec2& a_size) {
		auto Quest = GTS::Runtime::GetQuest(GTS::Runtime::QUST.GTSQuestProgression);
		if (!Quest) {
			return;
		}

		uint16_t currentstage = Quest->GetCurrentStageID();
		if (currentstage >= 100 || currentstage < 10) {
			return;
		}

		std::string questname = fmt::format("Quest: {}", Quest->GetName());

		std::string questobjectiveText;
		for (auto objective : Quest->objectives) {
			if (objective->index == currentstage) {
				questobjectiveText = objective->displayText;
				break;
			}
		}

		float Current, Total;
		switch (currentstage)  {
			case 5:
			case 10:
			{
				Current = GTS::Persistent::HugStealCount.value;
				Total = 2.0f;
			} break;
			case 20:
			case 25:
			case 26:
			{
				Current = GTS::Persistent::StolenSize.value;
				Total = 5.0f;
			} break;

			case 30: 
			{
				Current = GTS::Persistent::CrushCount.value;
				Total = 3.0f;
			} break;
			case 40: 
			{
				Current = GTS::Persistent::CrushCount.value + GTS::Persistent::STNCount.value;
				Total = 9.0f;
			} break;
			case 50:
			case 55:
			case 56:
			{
				Current = GTS::Persistent::HandCrushed.value;
				Total = 3.0f;
			} break;
			case 60:
			case 65:
			{
				Current = GTS::Persistent::VoreCount.value;
				Total = 6.0f;
			} break;
			case 70: 
			{
				Current = GTS::Persistent::GiantCount.value;
				Total = 1.0f;
			} break;
			case 80: 
			{
				Current = 0.0f;
				Total = 1.0f;
			} break;

			default: break;
		}

		ImGui::BeginChild(
			"##QuestProgression",
			a_size,

			ImGuiChildFlags_Borders |
			ImGuiChildFlags_FrameStyle |
			ImGuiChildFlags_NavFlattened |
			ImGuiChildFlags_AutoResizeY,

			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoNav

		);

		{
			GTS::ImFontManager::Push(GTS::ImFontManager::kLargeText);
			ImGui::Text(questname.c_str());
			GTS::ImFontManager::Pop();
		}

		ImGui::TextWrapped(questobjectiveText.c_str());

		std::string Text = fmt::format("{:.2f} Out of {:.0f}", Current, Total);
		ProgressBar(
			Current / Total,
			{ ImGui::GetContentRegionAvail().x, 0.0f },
			Text.c_str(),
			ImGuiExProgresbarFlag_Gradient | ImGuiExProgresbarFlag_Rounding,
			//Height, Border Thickness, Rounding, DarkFactor, LightFactor
			1.25f, 0.5f, 5.0f, 0.7f, 1.3f,
			ImUtil::Colors::fRGBToU32(GTS::Config::UI.f3AccentColor)
		);

		ImGui::EndChild();
	}
}
