#include "UI/ImGui/Controls/ActorInfoCard.hpp"

#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/Lib/imgui_internal.h"

#include "UI/ImGui/Core/ImColorUtils.hpp"
#include "UI/ImGui/Core/ImFontManager.hpp"

#include "UI/ImGui/Controls/Button.hpp"
#include "UI/ImGui/Controls/ToolTip.hpp"
#include "UI/ImGui/Controls/ProgressBar.hpp"

#include "Managers/SpectatorManager.hpp"
#include "Managers/MaxSizeManager.hpp"
#include "Managers/Attributes.hpp"

#include "Utils/KillDataUtils.hpp"
#include "Utils/UnitConverter.hpp"

namespace {

	// ------- Tooltips -------

	PSString TDamageBonus = "Non-size related damage multiplier. Affects both Physical and Magic damage.";
	PSString THHDamage = "Extra foot damage multiplier when wearing high heels.";
	PSString TShrinkResist = "Shrink Resistance reduces the effectiveness of any shrinking spell and/or effect.";
	PSString TAbsorbedAttributesCap = "Absorbed Attributes cannot exceed this number";

	PSString TStoredAttributes = "Stored Attributes are permanent increases to either Health, Magicka or Stamina, that have not been absorbed yet.\n"
						         "They'll be randomly distributed between the three main attributes\n"
						         "Complete perk the requirements to fully utilize them and convert them to absorbed attributes";

	PSString TAbsorbedAttributes = "Absorbed attributes are permanent a permantent increase to Health, Magicka or Stamina";

}

namespace ImGuiEx {

	using namespace GTS;

	std::optional<ActorInfoCard::ActorInfo> ActorInfoCard::ActorInfo::GetData(Actor* a_actor) {

		auto I = ActorInfo{};

		if (!a_actor) return std::nullopt;
		if (!a_actor->Get3D(false)) return std::nullopt;

		const auto& P = Persistent::GetSingleton().GetActorData(a_actor);
		const auto& T = Transient::GetSingleton().GetActorData(a_actor);
		if (!T || !P)  return std::nullopt;

		I.pTargetActor =            a_actor->CreateRefHandle();
		I.Name =                    a_actor->GetName();

		//Scale
		I.fScaleCurrent =           get_visual_scale(a_actor);
		I.fScaleMax =               get_max_scale(a_actor);
		I.fScaleNatural =           get_natural_scale(a_actor);
		I.fMassModeScaleMax =       MassMode_GetValuesForMenu(a_actor);
		I.fScaleProgress =          I.fScaleMax < 250.0f ? I.fScaleCurrent / I.fScaleMax : -1.0f;

		//Bonuses
		I.fScaleBonus =				T->PotionMaxSize;
		I.fShrinkResistance =       (1.0f - 1.0f * Potion_GetShrinkResistance(a_actor) * Perk_GetSprintShrinkReduction(a_actor)) * 100.f;
		I.fHighHeelDamageBonus =    (GetHighHeelsBonusDamage(a_actor, true) - 1.0f) * 100.0f;
		I.fGTSAspect =              Ench_Aspect_GetPower(a_actor) * 100.0f;

		//Stats
		I.fSpeedMult =              (AttributeManager::GetAttributeBonus(a_actor, ActorValue::kSpeedMult) - 1.0f) * 100.f;
		I.fJumpMult =               (AttributeManager::GetAttributeBonus(a_actor, ActorValue::kJumpingBonus) - 1.0f) * 100.0f;
		I.fDamageBonus =            (AttributeManager::GetAttributeBonus(a_actor, ActorValue::kAttackDamageMult) - 1.0f) * 100.0f;

		//Stolen Attributes
		I.fStolenAtributes =        P->stolen_attributes;
		I.fStolenHealth =           GetStolenAttributes_Values(a_actor, ActorValue::kHealth);
		I.fStolenMagicka =          GetStolenAttributes_Values(a_actor, ActorValue::kMagicka);
		I.fStolenStamina =          GetStolenAttributes_Values(a_actor, ActorValue::kStamina);
		I.fStolenCap =              GetStolenAttributeCap(a_actor);

		//Player Only
		if (a_actor->IsPlayerRef()) {
			I.bIsPlayer = true;
			I.fSizeEssence =        Persistent::GetSingleton().PlayerExtraPotionSize.value;
		}

		//Other
		I.iTotalKills =             GetKillCount(a_actor, SizeKillType::kTotalKills);

		//Formated
		I.sFmtScale =               fmt::format("{} ({:.2f}x)", GetFormatedHeight(a_actor), I.fScaleCurrent);
		I.sFmtWeight =              GetFormatedWeight(a_actor).c_str();

		//Perk Check
		I.bHasPerk_GTSFullAssimilation = Runtime::HasPerk(a_actor, "GTSPerkFullAssimilation");

		return std::move(I);

	}

	ActorInfoCard::ActorInfoCard(): m_expandedSec(Section::kSectionExtra) {

		m_wChildFlags = ImGuiChildFlags_Borders           |
						ImGuiChildFlags_NavFlattened      |
						ImGuiChildFlags_AutoResizeY       |
						ImGuiChildFlags_AlwaysAutoResize;

		m_wWindowFlags = ImGuiWindowFlags_NoSavedSettings;


		m_buffs = std::make_unique<StatusBar>(m_buffIconSize, true);


	}

	void ActorInfoCard::Draw(Actor* a_actor, const ImVec2& card_size) {

		if (!a_actor) return;
		if (!a_actor->Get3D1(false)) return;
		const auto& Style = ImGui::GetStyle();

		auto Data = ActorInfo::GetData(a_actor);
		if (!Data.has_value()) return;

		//Update Vars
		bMassModeEnabled = Config::Balance.sSizeMode == "kMassBased";
		bIsPlayerMassMode = a_actor->IsPlayerRef() && bMassModeEnabled;

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
			m_wWindowFlags | ImGuiWindowFlags_NoBackground
		);
		DrawMainContent(Data.value());

		// Expanded sections
		if (m_expandedSec == Section::kSectionExtra) {
			ImGui::Separator();
			DrawExtraStats(Data.value());
		}
		else if (m_expandedSec == Section::kSectionKillInfo) {
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			DrawKillData(a_actor);
		}

		ImGui::EndChild();

		// Button column (top right)
		ImGui::SameLine();
		ImGui::BeginGroup();

		const bool section1_active = m_expandedSec == Section::kSectionExtra;
		const bool section2_active = m_expandedSec == Section::kSectionKillInfo;

		if (section1_active) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
		if (ImageButton("##ExtraInfo", "infocard_extra", m_baseIconSize, "Show Extended Info")) {
			m_expandedSec = section1_active ? Section::kNone : Section::kSectionExtra;
		}
		if (section1_active) ImGui::PopStyleColor();
		
		if (section2_active)
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
		if (ImageButton("##KillInfo", "infocard_kills", m_baseIconSize, "Show Kill Count Info")) {
			m_expandedSec = section2_active ? Section::kNone : Section::kSectionKillInfo;
		}
		if (section2_active) ImGui::PopStyleColor();

		DrawSpectateButton(a_actor);

		ImGui::EndGroup();
		ImGui::EndChild();
		ImGui::PopID();
	}

	void ActorInfoCard::DrawBuffIcons(const ActorInfo& Data) const {

		ImVec2 Size = { ImGui::GetContentRegionAvail().x, m_buffIconSize + (ImGui::GetStyle().FramePadding.y * 2.0f) };

		ImGui::BeginChild("##BuffIcons", Size,
			ImGuiChildFlags_Borders |
			ImGuiChildFlags_FrameStyle |
			ImGuiChildFlags_NavFlattened,
			m_wWindowFlags | ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoNav
		);

		uint32_t drawnIcons = m_buffs->Draw(Data.pTargetActor.get().get());

		if (drawnIcons == 0) {
			const char* txt = "No Buffs Active";
			const ImVec2 textSize = ImGui::CalcTextSize(txt);
			ImGui::SetCursorPos({
				Size.x *.5f - (textSize.x * .5f),
				Size.y *.5f - (textSize.y * .5f)
			});
			ImGui::Text(txt);
		}

		ImGui::Dummy({});
		ImGui::EndChild();
	}

	void ActorInfoCard::DrawBaseInfoTable(const ActorInfo& Data) const {

		ImFontManager::Push(ImFontManager::ActiveFontType::kLargeText);

		if (ImGui::BeginTable("##MainStatsTable", 2,
			ImGuiTableFlags_NoSavedSettings |
			ImGuiTableFlags_NoBordersInBody |
			ImGuiTableFlags_Hideable,
			{ ImGui::GetContentRegionAvail().x, 0.0f })) {

			// --------------------------------- Max Size
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Max Size:");
			ImGui::TableSetColumnIndex(1);

			if (Data.fScaleMax > 250.f) {
				ImGui::Text("∞");
			}
			else {
				if (bMassModeEnabled) {
					ImGui::Text("%.2fx out of %.2fx", Data.fScaleMax, Data.fMassModeScaleMax);
				}
				else {
					ImGui::Text("%.2fx", Data.fScaleMax);
				}
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Kills:");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u", Data.iTotalKills);

			ImGui::EndTable();

		}

		ImFontManager::Pop();

	}

	void ActorInfoCard::DrawMainContent(const ActorInfo& Data) const {

		ImFontManager::Push(ImFontManager::ActiveFontType::kWidgetTitle);
		{
			ImGui::Text(str_toupper(Data.Name).c_str());
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 1.0f);
			{
				ImFontManager::Push(ImFontManager::ActiveFontType::kWidgetBody);

				ProgressBar(
					Data.fScaleProgress == -1.0f ? 1.0f : Data.fScaleProgress,
					{ ImGui::GetContentRegionAvail().x, 0.0f },
					Data.sFmtScale.c_str(),
					ImGuiExProgresbarFlag_Gradient | ImGuiExProgresbarFlag_Rounding,
					//Height, Border Thickness, Rounding, DarkFactor, LightFactor
					1.45f, 0.5f, 5.0f, 0.7f, 1.3f,
					ImUtil::Colors::fRGBToU32(Config::UI.f3AccentColor)
				);

				DrawBuffIcons(Data);
				DrawBaseInfoTable(Data);

			}
		}
		ImFontManager::Pop(2);
	}

	void ActorInfoCard::DrawExtraStats(const ActorInfo& Data) const {


		if (ImGui::BeginTable("##MainStatsTable", 2,
			ImGuiTableFlags_NoSavedSettings |
			ImGuiTableFlags_NoBordersInBody |
			ImGuiTableFlags_Hideable, 
			{ ImGui::GetContentRegionAvail().x, 0.0f })) {

			//---------Total Max Size Calculation and Text Formating
			const float BonusSize_EssenceAndDragons =    Data.fSizeEssence;
			const float BonusSize_TempPotionBoost =      Data.fScaleBonus * 100.0f;
			const float BonusSize_AspectOfGiantess =     Data.fGTSAspect;
			const float BonusSize_Overkills =            Data.fOverkills;
			const float BonusSize_Overkills_Multiplier = Data.fOverkillMult;

			const std::string TotalSizeBonusCalculation = fmt::format(
				fmt::runtime(
					"Size Essence & Absorbed Dragons: +{:.2f}x\n"
					"Colossal Growth: +{:.2f}x\n"
					"Potion Of Heights: +{:.0f}%%\n"
					"Aspect Of Giantess: +{:.0f}%%\n"
					"Overkills & C.Growth: +{:.2f}x\n\n"
					"- Size Essence Increases the maximum achievable size when the size limit cap is set to \"Skill Based\"\n"
					"- If Size Gain mode is set to \"Mass Mode\", then Essence Bonus is reduced by {:.0f}%% \n"
					"- Essence is gained by killing and absorbing dragons while having the correct perk.\n"
					"- Or by consuming specific potions found all around the world."
				),
				bMassModeEnabled ? BonusSize_EssenceAndDragons * MassMode_ElixirPowerMultiplier : BonusSize_EssenceAndDragons * 1.0f,
				BonusSize_Overkills,
				BonusSize_TempPotionBoost,
				BonusSize_AspectOfGiantess,
				BonusSize_Overkills_Multiplier,
				(1.0f - MassMode_ElixirPowerMultiplier) * 100.0f
			);

			// --------------------------------- Player Only Stuff 
			if (Data.bIsPlayer) {

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Shrink Resistance:");
				ImGuiEx::Tooltip(TShrinkResist, true);
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%.1f%%", Data.fShrinkResistance);

			}

			// --------------------------------- Weight
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Weight:");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text(Data.sFmtWeight.c_str());


			// --------------------------------- Bonus Size
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Bonus Size:");
			ImGuiEx::Tooltip(TotalSizeBonusCalculation.c_str(), true);
			ImGui::TableSetColumnIndex(1);
			ImGui::Text(
				bIsPlayerMassMode ? "%.0f%% + [%.2F + %.2Fx possible]" : "%.0f%% + %.2F + %.2Fx",
				(Data.fScaleBonus * 100.0f) + Data.fGTSAspect,
				bMassModeEnabled ? (Data.fSizeEssence * MassMode_ElixirPowerMultiplier) + BonusSize_Overkills : (Data.fSizeEssence * 1.0f) + BonusSize_Overkills,
				std::clamp(BonusSize_Overkills_Multiplier, 1.0f, 999999.0f)
			);

			// ---------------------------------  High Heel Damage
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("High Heel Damage:");
			ImGuiEx::Tooltip(THHDamage, true);
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("+%.0f%%", Data.fHighHeelDamageBonus);


			// ---------------------------------  Damage Multiplier
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Bonus Damage:");
			ImGuiEx::Tooltip(TDamageBonus, true);
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%.1f%%", Data.fDamageBonus);


			// --------------------------------- Carry Weight
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Bonus Carry Weight:");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%.1f", Data.fCarryWeightBonus);


			// --------------------------------- Speed Multiplier
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Bonus Speed:");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%.1f%%", Data.fSpeedMult);


			// ---------------------------------  Jump Multiplier
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Bonus Jump Height:");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%.1f%%", Data.fJumpMult);

			// --------------------------------- Perk Related

			if (Data.bHasPerk_GTSFullAssimilation) {

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Stored Attributes:");
				ImGuiEx::Tooltip(TStoredAttributes, true);
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("+%.2f", Data.fStolenAtributes);

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Absorbed Attributes:");
				ImGuiEx::Tooltip(TAbsorbedAttributes, true);
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("HP: +%.2f, MP: +%.2f, SP: +%.2f", Data.fStolenHealth, Data.fStolenMagicka, Data.fStolenStamina);

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Max Attributes:");
				ImGuiEx::Tooltip(TAbsorbedAttributesCap, true);
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%.2f", Data.fStolenCap);

			}
			ImGui::EndTable();
		}
	}

	void ActorInfoCard::DrawSpectateButton(Actor* a_actor) const {

		if (!a_actor) return;

		const bool isPlayer = a_actor->formID == 0x14;
		const bool isTargeted = SpectatorManager::GetCameraTarget() == a_actor;
		const bool isCameraOnPlayer = SpectatorManager::IsCameraTargetPlayer();

		// 'X' button appears if this actor is targeted OR if this is the player and camera is NOT on player
		if ((isTargeted && !isPlayer) || (isPlayer && !isCameraOnPlayer)) {
			if (ImageButton("##Spectate", "generic_x", m_baseIconSize, "Revert the camera back to the player character.")) {
				SpectatorManager::ResetTarget(true);
			}
		}
		// "Spectate" button appears for non-player actors that are not targeted
		else if (!isPlayer) {
			if (ImageButton("##Spectate", "infocard_spectate", m_baseIconSize, "Spectate this NPC.")) {
				SpectatorManager::SetCameraTarget(a_actor, false);
			}
		}
	}

	void ActorInfoCard::DrawKillStat(Actor* a_actor, const char* a_name, SizeKillType a_type, uint8_t a_colOffset, const char* a_toolTip) {

		if (!a_actor) return;

		if (a_colOffset == 0) {
			ImGui::TableNextRow();
		}

		ImGui::TableSetColumnIndex(0 + a_colOffset);
		ImGui::Text(a_name);
		if (a_toolTip) {
			ImGuiEx::Tooltip(a_toolTip, true);
		}

		ImGui::TableSetColumnIndex(1 + a_colOffset);
		ImGui::Text("%u", GetKillCount(a_actor, a_type));

	}

	void ActorInfoCard::DrawKillData(Actor* a_actor) {

		if (!a_actor) return;

		ImGui::Spacing();

		if (ImGui::BeginTable("##KillCountTable", 4, 
			ImGuiTableFlags_NoSavedSettings | 
			ImGuiTableFlags_NoBordersInBody | 
			ImGuiTableFlags_Hideable, 
			{ ImGui::GetContentRegionAvail().x, 0.0f })) {

			//The Row Draws Must be interleaved, as we have 4 columns, a_colOffset > 0 Prevents moving to then next row.

			DrawKillStat(a_actor, "Erased From Existence:", SizeKillType::kErasedFromExistence, 0 );
			DrawKillStat(a_actor, "Finger Crushed:",        SizeKillType::kFingerCrushed,       2 );
			DrawKillStat(a_actor, "Shrunk To Nothing:",     SizeKillType::kShrunkToNothing ,    0 );
			DrawKillStat(a_actor, "Grab Crushed:",          SizeKillType::kGrabCrushed,         2 );
			DrawKillStat(a_actor, "Breast Suffocated:",     SizeKillType::kBreastSuffocated,    0 );
			DrawKillStat(a_actor, "Butt Crushed:",          SizeKillType::kButtCrushed,         2 );
			DrawKillStat(a_actor, "Breast Absorbed:",       SizeKillType::kBreastAbsorbed,      0 );
			DrawKillStat(a_actor, "Hug Crushed:",           SizeKillType::kHugCrushed,          2 );
			DrawKillStat(a_actor, "Breast Crushed:",        SizeKillType::kBreastCrushed,       0 );
			DrawKillStat(a_actor, "Crushed:",               SizeKillType::kCrushed,             2 );
			DrawKillStat(a_actor, "Thigh Suffocated:",      SizeKillType::kThighSuffocated,     0 );
			DrawKillStat(a_actor, "Eaten:",                 SizeKillType::kEaten,               2 );
			DrawKillStat(a_actor, "Thigh Sandwiched:",      SizeKillType::kThighSandwiched,     0 );
			DrawKillStat(a_actor, "Other Sources:",         SizeKillType::kOtherSources,        2,
				"Other Sources are:\n"
				"- Shockwave deaths\n"
				"- Tiny calamity collision deaths\n"
				"- Tiny death while grabbed with no action happening\n"
				"- Overkill weapon damage when large\n"
			);
			DrawKillStat(a_actor, "Thigh Crushed:",         SizeKillType::kThighCrushed);
			DrawKillStat(a_actor, "Grind Crushed:",         SizeKillType::kGrinded);
			DrawKillStat(a_actor, "Kick Crushed:",          SizeKillType::kKicked);

			ImGui::EndTable();
		}
	}
}
