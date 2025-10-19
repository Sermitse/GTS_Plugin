#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/Lib/imgui_internal.h"

#include "UI/ImGui/Controls/ActorInfoCard.hpp"
#include "UI/ImGui/Controls/Button.hpp"
#include "UI/ImGui/Controls/Misc.hpp"
#include "UI/ImGui/Controls/ToolTip.hpp"
#include "UI/ImGui/Controls/Text.hpp"
#include "UI/ImGui/Controls/ProgressBar.hpp"

#include "UI/ImGui/Core/ImColorUtils.hpp"
#include "UI/ImGui/Core/ImFontManager.hpp"

#include "Utils/KillDataUtils.hpp"
#include "Managers/SpectatorManager.hpp"

namespace ImGuiEx {

	using namespace GTS;

	ActorInfoCard::ActorInfoCard(): expanded_section_(Section::kNone) {

		m_wChildFlags = ImGuiChildFlags_Borders           |
						ImGuiChildFlags_FrameStyle        |
						ImGuiChildFlags_NavFlattened      |
						ImGuiChildFlags_AutoResizeY       |
						ImGuiChildFlags_AlwaysAutoResize;

		m_wWindowFlags = ImGuiWindowFlags_NoSavedSettings;


	}

	void ActorInfoCard::Draw(Actor* a_actor, const ImVec2& card_size) {

		if (!a_actor) return;
		if (!a_actor->Get3D1(false)) return;
		const auto& Style = ImGui::GetStyle();

		auto Data = ActorInfo::GetData(a_actor);
		if (!Data.has_value()) return;

		ImGui::PushID(a_actor);

		ImGui::BeginChild("##Card", card_size, m_wChildFlags, m_wWindowFlags);

		// Calculate layout
		const float content_width = ImGui::GetContentRegionAvail().x;
		const float button_width = m_baseIconSize + (Style.FramePadding.x * 2.f) + (Style.CellPadding.x * 2.f);

		// Main content area (left side)
		ImGui::BeginChild("##MainContent", { content_width - button_width, 0 }, 
			ImGuiChildFlags_NavFlattened      |
			ImGuiChildFlags_AutoResizeY       |
			ImGuiChildFlags_AlwaysAutoResize,
			m_wWindowFlags
		);
		RenderMainContent(Data.value());

		// Expanded sections
		if (expanded_section_ == Section::kSectionExtra) {
			ImGui::Separator();
			RenderSection1(Data.value());
		}
		else if (expanded_section_ == Section::kSectionKillInfo) {
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			DrawKillData(a_actor);
		}

		ImGui::EndChild();

		// Button column (top right)
		ImGui::SameLine();
		ImGui::BeginGroup();

		bool section1_active = expanded_section_ == Section::kSectionExtra;
		if (section1_active) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
		if (ImageButton("##ExtraInfo", "extra_info", m_baseIconSize, "Show Extended Info")) {
			expanded_section_ = section1_active ? Section::kNone : Section::kSectionExtra;
		}
		if (section1_active) ImGui::PopStyleColor();

		bool section2_active = expanded_section_ == Section::kSectionKillInfo;
		if (section2_active) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
		if (ImageButton("##KillInfo", "extra_kill_info", m_baseIconSize, "Show Kill Count Info")) {
			expanded_section_ = section2_active ? Section::kNone : Section::kSectionKillInfo;
		}
		if (section2_active) ImGui::PopStyleColor();

		DrawSpectateButton(a_actor);

		ImGui::EndGroup();
		ImGui::EndChild();
		ImGui::PopID();
	}

	void ActorInfoCard::RenderMainContent(const ActorInfo& Data) {

		ImFontManager::Push(ImFontManager::ActiveFontType::kWidgetTitle);
		{
			ImGui::Text(str_toupper(Data.Name).c_str());
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 1.0f);
			{
				ImFontManager::Push(ImFontManager::ActiveFontType::kWidgetBody);

				ProgressBar(
					Data.fScaleProgress,
					{ ImGui::GetContentRegionAvail().x, 0.0f },
					Data.sFmtScale.c_str(),
					ImGuiExProgresbarFlag_Gradient | ImGuiExProgresbarFlag_Rounding,
					1.0f, 1.0f, 0.7f, 1.3f,
					ImUtil::Colors::fRGBToU32(Config::UI.f3AccentColor)
				);

				//TODO: Buff Icon Draw Code goes here
				//
				//-----------------

				//Max size
				//Weight
				//



				ImGui::Spacing();
			}
		}
		ImFontManager::Pop(2);
	}

	void ActorInfoCard::RenderSection1(const ActorInfo&) {
		ImGui::Text("Section 1 Content");
		ImGui::Text("Override RenderSection1()");
	}

	void ActorInfoCard::DrawSpectateButton(Actor* a_actor) const {

		if (!a_actor) return;

		const bool isPlayer = a_actor->formID == 0x14;
		const bool isTargeted = SpectatorManager::GetCameraTarget() == a_actor;
		const bool isCameraOnPlayer = SpectatorManager::IsCameraTargetPlayer();

		// 'X' button appears if this actor is targeted OR if this is the player and camera is NOT on player
		if ((isTargeted && !isPlayer) || (isPlayer && !isCameraOnPlayer)) {
			if (ImageButton("##Spectate", "generic_x", m_baseIconSize, "Revert Camera To Player")) {
				SpectatorManager::ResetTarget(true);
			}
		}
		// "Spectate" button appears for non-player actors that are not targeted
		else if (!isPlayer) {
			if (ImageButton("##Spectate", "spectate", m_baseIconSize, "Spectate This NPC")) {
				SpectatorManager::SetCameraTarget(a_actor, false);
			}
		}
	}

	void ActorInfoCard::DrawKillStat(RE::Actor* a_actor, const char* a_name, SizeKillType a_type, const char* a_toolTip) {

		if (!a_actor) return;

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::Text(a_name);
		if (a_toolTip) {
			ImGuiEx::Tooltip(a_toolTip, true);
		}

		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%d", GetKillCount(a_actor, a_type));
		
		if (a_toolTip) {
			ImGuiEx::Tooltip(a_toolTip, true);
		}

	}

	void ActorInfoCard::DrawKillData(RE::Actor* a_actor) {

		if (!a_actor) return;

		ImGui::Spacing();

		ImFontManager::Push(ImFontManager::kWidgetTitle);
		ImGui::Text("Kills");
		ImFontManager::Pop();

		if (ImGui::BeginTable("##KillCountTable", 2, 
			ImGuiTableFlags_NoSavedSettings | 
			ImGuiTableFlags_NoBordersInBody | 
			ImGuiTableFlags_Hideable)) {

			DrawKillStat(a_actor, "Erased From Existence:", SizeKillType::kErasedFromExistence);
			DrawKillStat(a_actor, "Shrunk To Nothing:",     SizeKillType::kShrunkToNothing);
			DrawKillStat(a_actor, "Breast Suffocated:",     SizeKillType::kBreastSuffocated);
			DrawKillStat(a_actor, "Breast Absorbed:",       SizeKillType::kBreastAbsorbed);
			DrawKillStat(a_actor, "Breast Crushed:",        SizeKillType::kBreastCrushed);

			//ImGui::TableNextRow(); //used as new line

			DrawKillStat(a_actor, "Thigh Suffocated:",      SizeKillType::kThighSuffocated);
			DrawKillStat(a_actor, "Thigh Sandwiched:",      SizeKillType::kThighSandwiched);
			DrawKillStat(a_actor, "Thigh Crushed:",         SizeKillType::kThighCrushed);

			//ImGui::TableNextRow();

			DrawKillStat(a_actor, "Grind Crushed:",         SizeKillType::kGrinded);
			DrawKillStat(a_actor, "Kick Crushed:",          SizeKillType::kKicked);

			//ImGui::TableNextRow();

			DrawKillStat(a_actor, "Finger Crushed:",        SizeKillType::kFingerCrushed);
			DrawKillStat(a_actor, "Grab Crushed:",          SizeKillType::kGrabCrushed);
			DrawKillStat(a_actor, "Butt Crushed:",          SizeKillType::kButtCrushed);
			DrawKillStat(a_actor, "Hug Crushed:",           SizeKillType::kHugCrushed);
			DrawKillStat(a_actor, "Crushed:",               SizeKillType::kCrushed);

			//ImGui::TableNextRow();

			DrawKillStat(a_actor, "Eaten:",                 SizeKillType::kEaten);
			DrawKillStat(a_actor, "Other Sources:",         SizeKillType::kOtherSources, 
				"Other Sources are:\n"
				"- Shockwave deaths\n"
				"- Tiny calamity collision deaths\n"
				"- Tiny death while grabbed with no action happening\n"
				"- Overkill weapon damage when large\n"
			);

			ImGui::EndTable();
		}
	}
}
