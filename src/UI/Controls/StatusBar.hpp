#pragma once

#include "UI/Controls/Icons/DynIcon_CataclysmicStompStacks.hpp"
#include "UI/Controls/Icons/DynIcon_LifeAbsorbStacks.hpp"
#include "UI/Controls/Icons/DynIcon_DamageReduction.hpp"
#include "UI/Controls/Icons/DynIcon_Enchantment.hpp"
#include "UI/Controls/Icons/DynIcon_OnTheEdge.hpp"
#include "UI/Controls/Icons/DynIcon_SizeReserve.hpp"
#include "UI/Controls/Icons/DynIcon_VoreBeingAbsorbed.hpp"

namespace ImGuiEx {

	enum StatusbarIconFlags : uint32_t {
		StatusbarFlag_None                  = 0,
		StatusbarFlag_HideDamageReduction   = 1 << 0,
		StatusbarFlag_HideLifeAbsorbtion    = 1 << 1,
		StatusbarFlag_HideEnchantment       = 1 << 2,
		StatusbarFlag_HideVoreStacks        = 1 << 3,
		StatusbarFlag_HideSizeReserve       = 1 << 4,
		StatusbarFlag_HideOnTheEdge         = 1 << 5,
		StatusbarFlag_HideVoreBeingAbsorbed = 1 << 6,
	};

	enum StatusbarAlwaysShowFlags : uint32_t {
		StatusbarASFlag_None                = 0,
		StatusbarASFlag_ASDamageReduction   = 1 << 0,
		StatusbarASFlag_ASLifeAbsorbtion    = 1 << 1,
		StatusbarASFlag_ASEnchantment       = 1 << 2,
		StatusbarASFlag_ASVoreStacks        = 1 << 3,
		StatusbarASFlag_ASSizeReserve       = 1 << 4,
		StatusbarASFlag_ASOnTheEdge         = 1 << 5,
		StatusbarASFlag_ASVoreBeingAbsorbed = 1 << 6,
	};

	class StatusBar {

		public:
        StatusBar(uint32_t a_iconSize, bool a_enableToolTips);
		uint8_t Draw(RE::Actor* a_actor, uint32_t a_visFlags = StatusbarFlag_None, uint32_t a_ASFlags = StatusbarASFlag_None, bool* a_stateChanged = nullptr, bool a_preview = false);

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
        std::unique_ptr<DynIconCataclysmicStompStacks> m_CataclysmicVoreStacksIcon;
		std::unique_ptr<DynIconVoreBeingAbsorbed> m_VoreBeingAbsorbedIcon;

	};
}
