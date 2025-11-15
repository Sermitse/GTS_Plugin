#include "UI/Controls/StatusBar.hpp"
#include "UI/Controls/ToolTip.hpp"

#include "UI/Core/ImUtil.hpp"

#include "Managers/Attributes.hpp"

namespace {

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
		"The enchantment can be obtained from the 'Amulet of Giants', which will randomly in end of dungeon hests.";

	PSString TLifeAbsorb = "TODO Tooltip";

	PSString TVoreStacks = "TODO Tooltip";

}

namespace ImGuiEx {

	StatusBar::StatusBar(uint32_t a_iconSize, bool a_enableToolTips): m_iconSize(a_iconSize), m_enableToolTips(a_enableToolTips) {
		//Init Icons
		m_lifeAbsorbIcon            = std::make_unique<DynIconLifeabsorbStacks>(m_iconSize);
		m_damageReductionIcon       = std::make_unique<DynIconDamageReduction>(m_iconSize);
		m_enchantmentIcon           = std::make_unique<DynIconEnchantment>(m_iconSize);
		m_sizeReserveIcon           = std::make_unique<DynIconSizeReserve>(m_iconSize);
		m_onTheEdgeIcon             = std::make_unique<DynIconOnTheEdge>(m_iconSize);
		m_CataclysmicVoreStacksIcon = std::make_unique<DynIconCataclysmicVoreStacks>(m_iconSize);
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

		// Dummy values for preview mode
		if (a_preview) {
			fDamageResist = 100.0f;
			fGTSAspect = 100.f;
			fSizeReserve = 2.5f;
			fOnTheEdge = 100.0f;
			iLifeAbsorbStacks = 10;
			iVoreStacks = 10;
		}

		bool Draw_damageReductionIcon       = !(a_visFlags & StatusbarFlag_HideDamageReduction);
		bool Draw_lifeAbsorbIcon            = !(a_visFlags & StatusbarFlag_HideLifeAbsorbtion);
		bool Draw_enchantmentIcon           = !(a_visFlags & StatusbarFlag_HideEnchantment);
		bool Draw_CataclysmicVoreStacksIcon = !(a_visFlags & StatusbarFlag_HideVoreStacks);
		bool Draw_sizeReserveIcon           = !(a_visFlags & StatusbarFlag_HideSizeReserve);
		bool Draw_onTheEdgeIcon             = !(a_visFlags & StatusbarFlag_HideOnTheEdge);

		bool AS_damageReductionIcon       = (a_ASFlags & StatusbarFlag_HideDamageReduction);
		bool AS_lifeAbsorbIcon            = (a_ASFlags & StatusbarFlag_HideLifeAbsorbtion);
		bool AS_enchantmentIcon           = (a_ASFlags & StatusbarFlag_HideEnchantment);
		bool AS_CataclysmicVoreStacksIcon = (a_ASFlags & StatusbarFlag_HideVoreStacks);
		bool AS_sizeReserveIcon           = (a_ASFlags & StatusbarFlag_HideSizeReserve);
		bool AS_onTheEdgeIcon             = (a_ASFlags & StatusbarFlag_HideOnTheEdge);

		const float RowY = ImGui::GetCursorPosY();
		float cursorX = ImGui::GetCursorPosX();
		uint8_t drawnIcons = 0;
		float m_combiCurValueState = 0.0f;

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
				if (m_enableToolTips) Tooltip(TVoreStacks, true);
				ImUtil::AdvanceCursorInline(cursorX, RowY, m_iconSize, m_padding);
				m_combiCurValueState += iVoreStacks;
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
