#pragma once

#include "UI/ImGui/Core/ImGraphics.hpp"
#include "UI/ImGui/Controls/BuffIcons/DynIcon_CataclysmicVoreStacks.hpp"
#include "UI/ImGui/Controls/BuffIcons/DynIcon_LifeAbsorbStacks.hpp"
#include "UI/ImGui/Controls/BuffIcons/DynIcon_DamageReduction.hpp"
#include "UI/ImGui/Controls/BuffIcons/DynIcon_Enchantment.hpp"
#include "UI/ImGui/Controls/BuffIcons/DynIcon_OnTheEdge.hpp"
#include "UI/ImGui/Controls/BuffIcons/DynIcon_SizeReserve.hpp"

namespace ImGuiEx {

	enum StatusbarIconFlags : uint32_t {
		StatusbarFlag_None = 0,
		StatusbarFlag_HideDamageReduction = 1 << 0,
		StatusbarFlag_HideLifeAbsorbtion = 1 << 1,
		StatusbarFlag_HideEnchantment = 1 << 2,
		StatusbarFlag_HideVoreStacks = 1 << 3,
		StatusbarFlag_HideSizeReserve = 1 << 4,
		StatusbarFlag_HideOnTheEdge = 1 << 5,
	};

	class StatusBar {

		public:
        StatusBar(uint32_t a_iconSize, bool a_enableToolTips);
		uint8_t Draw(RE::Actor* a_actor, uint32_t a_flags = StatusbarFlag_None, bool* a_stateChanged = nullptr, bool a_preview = false);

		uint32_t m_iconSize;
		float m_padding = 8.0f;
		bool m_enableToolTips;

		float m_combiLastValueState = 0.0f;

		private:
        //Icon Instances
        std::unique_ptr<DynIconLifeabsorbStacks> m_lifeAbsorbIcon;
        std::unique_ptr<DynIconDamageReduction> m_damageReductionIcon;
        std::unique_ptr<DynIconEnchantment> m_enchantmentIcon;
        std::unique_ptr<DynIconSizeReserve> m_sizeReserveIcon;
        std::unique_ptr<DynIconOnTheEdge> m_onTheEdgeIcon;
        std::unique_ptr<DynIconCataclysmicVoreStacks> m_CataclysmicVoreStacksIcon;

	};
}
