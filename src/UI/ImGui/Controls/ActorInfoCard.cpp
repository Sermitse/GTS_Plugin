#include "UI/ImGui/Lib/imgui.h"

#include "UI/ImGui/Controls/ActorInfoCard.hpp"
#include "UI/ImGui/Controls/ProgressBar.hpp"
#include "UI/ImGui/Core/ImColorUtils.hpp"

#include "UI/ImGui/Core/ImFontManager.hpp"
#include "UI/ImGui/Lib/imgui_internal.h"


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
		ImGui::BeginChild("##MainContent", { content_width - button_width, 0 }, false);
		RenderMainContent(Data.value());

		// Expanded sections
		if (expanded_section_ == Section::kSectionExtra) {
			ImGui::Separator();
			RenderSection1(Data.value());
		}
		else if (expanded_section_ == Section::kSectionKillInfo) {
			ImGui::Separator();
			RenderSection2(a_actor);
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

		if (ImageButton("##Spectate", "spectate_camera", m_baseIconSize, "Spectate NPC")) {
			// Placeholder
		}

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

				ImGuiEx::ProgressBar(
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

	void ActorInfoCard::RenderSection2(RE::Actor* a_actor) {
		ImGui::Text("Section 2 Content");
		ImGui::Text("Override RenderSection2()");
	}
}
