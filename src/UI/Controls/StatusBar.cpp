#include "UI/Controls/StatusBar.hpp"
#include "UI/Controls/ToolTip.hpp"
#include "UI/Core/ImUtil.hpp"
#include "Managers/AttributeManager.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Constants.hpp"

using namespace GTS;
namespace {
	PSString TCalamityShrink 	= "Remaining cooldown of Tiny Calamity shrink";
	PSString TWrathfulCalamity 	= "Remaining cooldown of Wrathful Calamity";
	PSString THugs			   	= "Remaining cooldown of Hugs";
	PSString THugCrush			= "Remaining cooldown of Hug Crush";
	PSString THealthGate		= "Remaining cooldown of Health Gate";
	PSString TBreastAbsorb		= "Remaining cooldown of Breast Absorb";
	PSString TBreastSuffocate	= "Remaining cooldown of Breast Suffocate";
	PSString TBreastVore		= "Remaining cooldown of Breast Vore";
	PSString TButtCrush			= "Remaining cooldown of Butt Crush";
	PSString TShrinkOutburst 	= "Remaining cooldown of ShrinkOutburst";

	PSString TDamageResist = "Total damage resistance in percent.\n"
		"Effectively increases total HP.";

	PSString TOnTheEdge = "When health drops below 60%%:\n"
		"- All growth gained becomes stronger the less health there is.\n"
		"- Hostile shrinking is less effective the less health there is.\n\n"
		"The effect is strongest when 10%% (or less) HP remains.";

	PSString TSizeReserve = "Total size size stored from the size reserve perk.\n"
		"Gained by eating/absorbing/crushing others";

	PSString TAspectOfGTS = "This is the strength of Aspect of the Giantess enchantment\n"
		"Aspect Of Giantess affects:\n"
		"- Maximum Size, genral shrink power and the strength of size steal spells\n"
		"- Size-Related damage, mimimum shrink threshold from quest/balance mode\n"
		"- Growth rate and growth chance from random growth\n"
		"- The stregth of the shrink outburst power and the amount of growth gained on hit\n"
		"- Shrink resistance towards all hostile shrink sources\n\n"
		"The enchantment can be obtained from the 'Amulet of Giants', which has a chance to appear in end of dungeon chests.";

	PSString TLifeAbsorb = "Current amount of Life Absorption stacks.\n"
				           "Gained by having the relevant perk and performing Size-Related Overkills.";


	PSString TCataclismicStompStacks = "Currrent amount of Cataclismic Stomp stacks.\n"
		                               "Gained by having the relevant perk and after Vore fully consumes someone.";

	PSString TVoreBeingAbsorbed = "Current count of tinies being digested/absorbed";

}

namespace ImGuiEx {

	StatusBar::StatusBar(uint32_t a_iconSize, bool a_enableToolTips): m_iconSize(a_iconSize), m_enableToolTips(a_enableToolTips) {
		//Init Icons
		m_lifeAbsorbIcon            = std::make_unique<DynIconLifeabsorbStacks>(m_iconSize);
		m_damageReductionIcon       = std::make_unique<DynIconDamageReduction>(m_iconSize);
		m_enchantmentIcon           = std::make_unique<DynIconEnchantment>(m_iconSize);
		m_sizeReserveIcon           = std::make_unique<DynIconSizeReserve>(m_iconSize);
		m_onTheEdgeIcon             = std::make_unique<DynIconOnTheEdge>(m_iconSize);
		m_CataclysmicVoreStacksIcon = std::make_unique<DynIconCataclysmicStompStacks>(m_iconSize);
		m_VoreBeingAbsorbedIcon     = std::make_unique<DynIconVoreBeingAbsorbed>(m_iconSize);

		m_TinyCalamityShrink 		= std::make_unique<DynIconCooldownCalamityShrink>(m_iconSize);
		m_WrathfulCalamity			= std::make_unique<DynIconCooldownWrathfulCalamity>(m_iconSize);
		m_Hugs						= std::make_unique<DynIconCooldownHugs>(m_iconSize);
		m_HugCrush					= std::make_unique<DynIconCooldownHugAbsorb>(m_iconSize);
		m_HealthGate				= std::make_unique<DynIconCooldownHealthGate>(m_iconSize);
		m_BreastAbsorb				= std::make_unique<DynIconCooldownBreastAbsorb>(m_iconSize);
		m_BreastSuffocate			= std::make_unique<DynIconCooldownBreastSuffocate>(m_iconSize);
		m_BreastVore				= std::make_unique<DynIconCooldownBreastVore>(m_iconSize);
		m_ButtCrush					= std::make_unique<DynIconCooldownButtCrush>(m_iconSize);
		m_ShrinkOutburst			= std::make_unique<DynIconCooldownShrinkOutburst>(m_iconSize);
	}

	uint8_t StatusBar::Draw(RE::Actor* a_actor, uint32_t a_visFlags, uint32_t a_ASFlags, bool* a_stateChanged, bool a_preview) {

		if (!a_actor) return 0;
		if (!a_actor->Get3D(false)) return 0;

		const auto& P = GTS::Persistent::GetActorData(a_actor);
		const auto& T = GTS::Transient::GetActorData(a_actor);
		if (!T || !P)  return 0;

		// ----- Vars
		float fDamageResist = (1.0f - GTS::AttributeManager::GetAttributeBonus(a_actor, RE::ActorValue::kHealth)) * 100.f;
		float fGTSAspect =  GTS::Ench_Aspect_GetPower(a_actor) * 100.0f;
		float fSizeReserve = P->fSizeReserve;
		float fOnTheEdge = (GTS::GetPerkBonus_OnTheEdge(a_actor, 0.01f) - 1.0f) * 100.f;
		int iLifeAbsorbStacks = T->Stacks_Perk_LifeForce;
		int iVoreStacks = T->Stacks_Perk_CataclysmicStomp;
		int iVoreAbsorbing = T->VoreCurrentlyAbsorbingCount;

		// ----- Cooldown Vars
		const float CalamityShrink_Remaining = GetRemainingCooldown(a_actor, CooldownSource::Misc_TinyCalamity_Shrink);
		float CalamityShrink_ToSeconds = std::clamp(CalamityShrink_Remaining, 0.0f, TINYCALAMITY_SHRINK_COOLDOWN);
		// 
		const float WrathfulCalamity_Remaining = GetRemainingCooldown(a_actor, CooldownSource::Misc_TinyCalamity_WrathfulCalamity);
		float WrathfulCalamity_ToSeconds = std::clamp(WrathfulCalamity_Remaining, 0.0f, TINYCALAMITY_ONESHOT_COOLDOWN);
		// 
		const float Hugs_Remaining = GetRemainingCooldown(a_actor, CooldownSource::Action_Hugs);
		float Hugs_ToSeconds = std::clamp(Hugs_Remaining, 0.0f, HUGS_COOLDOWN);
		//
		const float HugCrush_Remaining = GetRemainingCooldown(a_actor, CooldownSource::Action_HugAbsorbOther);
		float HugCrush_ToSeconds = std::clamp(HugCrush_Remaining, 0.0f, ABSORB_OTHER_COOLDOWN);
		// 
		const float HealthGate_Remaining = GetRemainingCooldown(a_actor, CooldownSource::Action_HealthGate);
		float HealthGate_ToSeconds = std::clamp(HealthGate_Remaining, 0.0f, HEALTHGATE_COOLDOWN);
		// 
		const float BreastAbsorb_Remaining = GetRemainingCooldown(a_actor, CooldownSource::Action_Breasts_Absorb);
		float BreastAbsorb_ToSeconds = std::clamp(BreastAbsorb_Remaining, 0.0f, BREAST_ABSORB_OTHER_COOLDOWN);
		//
		const float BreastSuffocate_Remaining = GetRemainingCooldown(a_actor, CooldownSource::Action_Breasts_Suffocate);
		float BreastSuffocate_ToSeconds = std::clamp(BreastSuffocate_Remaining, 0.0f, BREAST_SUFFOCATE_OTHER_COOLDOWN);
		//
		const float BreastVore_Remaining = GetRemainingCooldown(a_actor, CooldownSource::Action_Breasts_Vore);
		float BreastVore_ToSeconds = std::clamp(BreastVore_Remaining, 0.0f, BREAST_VORE_OTHER_COOLDOWN);
		//
		const float ButtCrush_Remaining = GetRemainingCooldown(a_actor, CooldownSource::Action_ButtCrush);
		float ButtCrush_ToSeconds = std::clamp(ButtCrush_Remaining, 0.0f, BUTTCRUSH_COOLDOWN);
		//
		const float ShrinkOutburst_Remaining = GetRemainingCooldown(a_actor, CooldownSource::Misc_ShrinkOutburst);
		float ShrinkOutburst_ToSeconds = std::clamp(ShrinkOutburst_Remaining, 0.0f, SHRINK_OUTBURST_COOLDOWN);

		// Dummy values for preview mode
		if (a_preview) {
			fDamageResist = 100.0f;
			fGTSAspect = 100.f;
			fSizeReserve = 2.5f;
			fOnTheEdge = 100.0f;
			iLifeAbsorbStacks = 10;
			iVoreStacks = 10;
			iVoreAbsorbing = 4;
			CalamityShrink_ToSeconds = 10;
			WrathfulCalamity_ToSeconds = 10;
			Hugs_ToSeconds = 10;
			HugCrush_ToSeconds = 10;
			HealthGate_ToSeconds = 10;
			BreastAbsorb_ToSeconds = 10;
			BreastSuffocate_ToSeconds = 10;
			BreastVore_ToSeconds = 10;
			ButtCrush_ToSeconds = 10;
			ShrinkOutburst_ToSeconds = 10;
		}

		bool Draw_damageReductionIcon       = !(a_visFlags & StatusbarFlag_HideDamageReduction);
		bool Draw_lifeAbsorbIcon            = !(a_visFlags & StatusbarFlag_HideLifeAbsorbtion);
		bool Draw_enchantmentIcon           = !(a_visFlags & StatusbarFlag_HideEnchantment);
		bool Draw_CataclysmicVoreStacksIcon = !(a_visFlags & StatusbarFlag_HideVoreStacks);
		bool Draw_sizeReserveIcon           = !(a_visFlags & StatusbarFlag_HideSizeReserve);
		bool Draw_onTheEdgeIcon             = !(a_visFlags & StatusbarFlag_HideOnTheEdge);
		bool Draw_voreBeingAbsorbed         = !(a_visFlags & StatusbarFlag_HideVoreBeingAbsorbed);
		bool Draw_CalamityShrinkCooldown 	= !(a_visFlags & StatusBarFlag_HideCalamityShrink);
		bool Draw_WrathFulCalamityCooldown  = !(a_visFlags & StatusBarFlag_HideWrathfulCalamity);
		bool Draw_Hugs						= !(a_visFlags & StatusBarFlag_HideHugs);
		bool Draw_HugCrush					= !(a_visFlags & StatusBarFlag_HideHugCrush);
		bool Draw_HealthGate				= !(a_visFlags & StatusBarFlag_HideHealthGate);
		bool Draw_BreastAbsorb				= !(a_visFlags & StatusBarFlag_HideBreastAbsorb);
		bool Draw_BreastSuffocate			= !(a_visFlags & StatusBarFlag_HideBreastSuffocate);
		bool Draw_BreastVore				= !(a_visFlags & StatusBarFlag_HideBreastVore);
		bool Draw_ButtCrush					= !(a_visFlags & StatusBarFlag_HideButtCrush);
		bool Draw_ShrinkOutburst 			= !(a_visFlags & StatusBarFlag_HideShrinkOutburst);

		bool AS_damageReductionIcon       	= (a_ASFlags & StatusbarASFlag_ASDamageReduction);
		bool AS_lifeAbsorbIcon            	= (a_ASFlags & StatusbarASFlag_ASLifeAbsorbtion);
		bool AS_enchantmentIcon           	= (a_ASFlags & StatusbarASFlag_ASEnchantment);
		bool AS_CataclysmicVoreStacksIcon 	= (a_ASFlags & StatusbarASFlag_ASVoreStacks);
		bool AS_sizeReserveIcon           	= (a_ASFlags & StatusbarASFlag_ASSizeReserve);
		bool AS_onTheEdgeIcon             	= (a_ASFlags & StatusbarASFlag_ASOnTheEdge);
		bool AS_voreBeingAbsorbed         	= (a_ASFlags & StatusbarASFlag_ASVoreBeingAbsorbed);
		bool AS_CalamityShrinkCooldown    	= (a_ASFlags & StatusBarASFlag_ASCalamityShrink);
		bool AS_WrathfulCalamityCooldown  	= (a_ASFlags & StatusBarASFlag_ASWrathfulCalamity);
		bool AS_Hugs					  	= (a_ASFlags & StatusBarASFlag_ASHugs);
		bool AS_HugCrush					= (a_ASFlags & StatusBarASFlag_ASHugCrush);
		bool AS_HealthGate					= (a_ASFlags & StatusBarASFlag_ASHealthGate);
		bool AS_BreastAbsorb				= (a_ASFlags & StatusBarASFlag_ASBreastAbsorb);
		bool AS_BreastSuffocate				= (a_ASFlags & StatusBarASFlag_ASBreastSuffocate);
		bool AS_BreastVore					= (a_ASFlags & StatusBarASFlag_ASBreastVore);
		bool AS_ButtCrush					= (a_ASFlags & StatusBarASFlag_ASButtCrush);
		bool AS_ShrinkOutburst				= (a_ASFlags & StatusBarASFlag_ASShrinkOutburst);

		const float RowY = ImGui::GetCursorPosY();
		float cursorX = ImGui::GetCursorPosX();
		uint8_t drawnIcons = 0;
		float m_combiCurValueState = 0.0f;
		// -------------   Cooldowns
		// Calamity Shrink
		if (Draw_CalamityShrinkCooldown){
			m_TinyCalamityShrink->Resize(m_iconSize);
			if (m_TinyCalamityShrink->Draw(CalamityShrink_ToSeconds, TINYCALAMITY_SHRINK_COOLDOWN, AS_CalamityShrinkCooldown)) {
				if (m_enableToolTips) Tooltip(TCalamityShrink, true);
				ImUtil::AdvanceCursorInline(cursorX, RowY, m_iconSize, m_padding);
				m_combiCurValueState += CalamityShrink_ToSeconds;
				drawnIcons++;
			}
		}
		// Wrathful Calamity
		if (Draw_WrathFulCalamityCooldown){
			m_WrathfulCalamity->Resize(m_iconSize);
			if (m_WrathfulCalamity->Draw(WrathfulCalamity_ToSeconds, TINYCALAMITY_ONESHOT_COOLDOWN, AS_WrathfulCalamityCooldown)) {
				if (m_enableToolTips) Tooltip(TWrathfulCalamity, true);
				ImUtil::AdvanceCursorInline(cursorX, RowY, m_iconSize, m_padding);
				m_combiCurValueState += WrathfulCalamity_ToSeconds;
				drawnIcons++;
			}
		}
		// Hugs
		if (Draw_Hugs) {
			m_Hugs->Resize(m_iconSize);
			if (m_Hugs->Draw(Hugs_ToSeconds, HUGS_COOLDOWN, AS_Hugs)) {
				if (m_enableToolTips) Tooltip(THugs, true);
				ImUtil::AdvanceCursorInline(cursorX, RowY, m_iconSize, m_padding);
				m_combiCurValueState += Hugs_ToSeconds;
				drawnIcons++;
			}
		}
		// Hug Crush
		if (Draw_HugCrush) {
			m_HugCrush->Resize(m_iconSize);
			if (m_HugCrush->Draw(HugCrush_ToSeconds, Calculate_HugCrushCooldown(a_actor), AS_HugCrush)) {
				if (m_enableToolTips) Tooltip(THugCrush, true);
				ImUtil::AdvanceCursorInline(cursorX, RowY, m_iconSize, m_padding);
				m_combiCurValueState += HugCrush_ToSeconds;
				drawnIcons++;
			}
		}
		// Health Gate
		if (Draw_HealthGate) {
			m_HealthGate->Resize(m_iconSize);
			if (m_HealthGate->Draw(HealthGate_ToSeconds, HEALTHGATE_COOLDOWN, AS_HealthGate)) {
				if (m_enableToolTips) Tooltip(THealthGate, true);
				ImUtil::AdvanceCursorInline(cursorX, RowY, m_iconSize, m_padding);
				m_combiCurValueState += HealthGate_ToSeconds;
				drawnIcons++;
			}
		}
		// Breast Absorb
		if (Draw_BreastAbsorb) {
			m_BreastAbsorb->Resize(m_iconSize);
			if (m_BreastAbsorb->Draw(BreastAbsorb_ToSeconds, Calculate_BreastActionCooldown(a_actor, 2), AS_BreastAbsorb)) {
				if (m_enableToolTips) Tooltip(TBreastAbsorb, true);
				ImUtil::AdvanceCursorInline(cursorX, RowY, m_iconSize, m_padding);
				m_combiCurValueState += BreastAbsorb_ToSeconds;
				drawnIcons++;
			}
		}
		// Breast Suffocate
		if (Draw_BreastSuffocate) {
			m_BreastSuffocate->Resize(m_iconSize);
			if (m_BreastSuffocate->Draw(BreastSuffocate_ToSeconds, Calculate_BreastActionCooldown(a_actor, 0), AS_BreastSuffocate)) {
				if (m_enableToolTips) Tooltip(TBreastSuffocate, true);
				ImUtil::AdvanceCursorInline(cursorX, RowY, m_iconSize, m_padding);
				m_combiCurValueState += BreastSuffocate_ToSeconds;
				drawnIcons++;
			}
		}
		// Breast Vore
		if (Draw_BreastVore) {
			m_BreastVore->Resize(m_iconSize);
			if (m_BreastVore->Draw(BreastVore_ToSeconds, Calculate_BreastActionCooldown(a_actor, 1), AS_BreastVore)) {
				if (m_enableToolTips) Tooltip(TBreastVore, true);
				ImUtil::AdvanceCursorInline(cursorX, RowY, m_iconSize, m_padding);
				m_combiCurValueState += BreastVore_ToSeconds;
				drawnIcons++;
			}
		}
		// Butt Crush
		if (Draw_ButtCrush) {
			m_ButtCrush->Resize(m_iconSize);
			if (m_ButtCrush->Draw(ButtCrush_ToSeconds, Calculate_ButtCrushTimer(a_actor), AS_ButtCrush)) {
				if (m_enableToolTips) Tooltip(TButtCrush, true);
				ImUtil::AdvanceCursorInline(cursorX, RowY, m_iconSize, m_padding);
				m_combiCurValueState += ButtCrush_ToSeconds;
				drawnIcons++;
			}
		}
		// Shrink Outburst
		if (Draw_ShrinkOutburst) {
			m_ShrinkOutburst->Resize(m_iconSize);
			if (m_ShrinkOutburst->Draw(ShrinkOutburst_ToSeconds, Calculate_ShrinkOutburstTimer(a_actor), AS_ShrinkOutburst)) {
				if (m_enableToolTips) Tooltip(TShrinkOutburst, true);
				ImUtil::AdvanceCursorInline(cursorX, RowY, m_iconSize, m_padding);
				m_combiCurValueState += ShrinkOutburst_ToSeconds;
				drawnIcons++;
			}
		}
		// -------------------------
		
		//m_damageReductionIcon
		if (Draw_damageReductionIcon) {
			m_damageReductionIcon->Resize(m_iconSize);
			if (m_damageReductionIcon->Draw(fDamageResist, AS_damageReductionIcon)) {
				if (m_enableToolTips) Tooltip(TDamageResist, true);
				ImUtil::AdvanceCursorInline(cursorX, RowY, m_iconSize, m_padding);
				m_combiCurValueState += fDamageResist;
				drawnIcons++;
			}
		}

		//m_lifeAbsorbIcon
		if (Draw_lifeAbsorbIcon) {
			m_lifeAbsorbIcon->Resize(m_iconSize);
			if (m_lifeAbsorbIcon->Draw(iLifeAbsorbStacks, AS_lifeAbsorbIcon)) {
				if (m_enableToolTips) Tooltip(TLifeAbsorb, true);
				ImUtil::AdvanceCursorInline(cursorX, RowY, m_iconSize, m_padding);
				m_combiCurValueState += iLifeAbsorbStacks;
				drawnIcons++;
			}
		}

		//m_enchantmentIcon
		if (Draw_enchantmentIcon) {
			m_enchantmentIcon->Resize(m_iconSize);
			if (m_enchantmentIcon->Draw(fGTSAspect, AS_enchantmentIcon)) {
				if (m_enableToolTips) Tooltip(TAspectOfGTS, true);
				ImUtil::AdvanceCursorInline(cursorX, RowY, m_iconSize, m_padding);
				m_combiCurValueState += fGTSAspect;
				drawnIcons++;
			}
		}

		//m_CataclysmicVoreStacksIcon
		if (Draw_CataclysmicVoreStacksIcon) {
			m_CataclysmicVoreStacksIcon->Resize(m_iconSize);
			if (m_CataclysmicVoreStacksIcon->Draw(iVoreStacks, AS_CataclysmicVoreStacksIcon)) {
				if (m_enableToolTips) Tooltip(TCataclismicStompStacks, true);
				ImUtil::AdvanceCursorInline(cursorX, RowY, m_iconSize, m_padding);
				m_combiCurValueState += iVoreStacks;
				drawnIcons++;
			}
		}

		//m_VoreBeingAbsorbedIcon
		if (Draw_voreBeingAbsorbed) {
			m_VoreBeingAbsorbedIcon->Resize(m_iconSize);
			if (m_VoreBeingAbsorbedIcon->Draw(iVoreAbsorbing, AS_voreBeingAbsorbed)) {
				if (m_enableToolTips) Tooltip(TVoreBeingAbsorbed, true);
				ImUtil::AdvanceCursorInline(cursorX, RowY, m_iconSize, m_padding);
				m_combiCurValueState += iVoreAbsorbing;
				drawnIcons++;
			}
		}

		// Player Only
		if (a_actor->IsPlayerRef()) {

			//m_sizeReserveIcon
			if (Draw_sizeReserveIcon) {
				m_sizeReserveIcon->Resize(m_iconSize);
				if (m_sizeReserveIcon->Draw(fSizeReserve, AS_sizeReserveIcon)) {
					if (m_enableToolTips) Tooltip(TSizeReserve, true);
					ImUtil::AdvanceCursorInline(cursorX, RowY, m_iconSize, m_padding);
					m_combiCurValueState += fSizeReserve;
					drawnIcons++;
				}
			}

			//m_onTheEdgeIcon
			if (Draw_onTheEdgeIcon) {
				m_onTheEdgeIcon->Resize(m_iconSize);
				if (m_onTheEdgeIcon->Draw(fOnTheEdge, AS_onTheEdgeIcon)) {
					if (m_enableToolTips) Tooltip(TOnTheEdge, true);
					m_combiCurValueState += fOnTheEdge;
					drawnIcons++;
				}
				ImUtil::AdvanceCursorInline(cursorX, RowY, m_iconSize, m_padding);
			}

		}

		if (std::abs(m_combiCurValueState - m_combiLastValueState) >= 0.01f) {
			m_combiLastValueState = m_combiCurValueState;
			if (a_stateChanged) *a_stateChanged = true;
		}
		else {
			if (a_stateChanged) *a_stateChanged = false;
		}

		return drawnIcons;

	}
}
