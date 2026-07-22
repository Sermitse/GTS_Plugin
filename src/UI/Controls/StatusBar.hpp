#pragma once

#include "UI/Controls/Icons/DynIcon_CataclysmicStompStacks.hpp"
#include "UI/Controls/Icons/DynIcon_LifeAbsorbStacks.hpp"
#include "UI/Controls/Icons/DynIcon_DamageReduction.hpp"
#include "UI/Controls/Icons/DynIcon_Enchantment.hpp"
#include "UI/Controls/Icons/DynIcon_OnTheEdge.hpp"
#include "UI/Controls/Icons/DynIcon_SizeReserve.hpp"
#include "UI/Controls/Icons/DynIcon_VoreBeingAbsorbed.hpp"
#include "UI/Controls/Icons/DynIcon_Cooldown_CalamityShrink.hpp"
#include "UI/Controls/Icons/DynIcon_Cooldown_WrathfulCalamity.hpp"
#include "UI/Controls/Icons/DynIcon_Cooldown_Hugs.hpp"
#include "UI/Controls/Icons/DynIcon_Cooldown_HugAbsorb.hpp"
#include "UI/Controls/Icons/DynIcon_Cooldown_HealthGate.hpp"
#include "UI/Controls/Icons/DynIcon_Cooldown_BreastAbsorb.hpp"
#include "UI/Controls/Icons/DynIcon_Cooldown_BreastSuffocate.hpp"
#include "UI/Controls/Icons/DynIcon_Cooldown_BreastVore.hpp"
#include "UI/Controls/Icons/DynIcon_Cooldown_ButtCrush.hpp"
#include "UI/Controls/Icons/DynIcon_Cooldown_ShrinkOutburst.hpp"
#include "UI/Controls/Icons/DynIcon_Duration_TinyCalamity.hpp"

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
		StatusbarFlag_HideCalamityShrink	= 1 << 7,
		StatusbarFlag_HideWrathfulCalamity  = 1 << 8,
		StatusbarFlag_HideHugs				= 1 << 9,
		StatusbarFlag_HideHugCrush			= 1 << 10,
		StatusbarFlag_HideHealthGate		= 1 << 11,
		StatusbarFlag_HideBreastAbsorb		= 1 << 12,
		StatusbarFlag_HideBreastSuffocate	= 1 << 13,
		StatusbarFlag_HideBreastVore		= 1 << 14,
		StatusbarFlag_HideButtCrush			= 1 << 15,
		StatusbarFlag_HideShrinkOutburst	= 1 << 16,
		StatusbarFlag_HideCalamityDuration  = 1 << 17,
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
		StatusbarASFlag_ASCalamityShrink	= 1 << 7,
		StatusbarASFlag_ASWrathfulCalamity	= 1 << 8,
		StatusbarASFlag_ASHugs				= 1 << 9,
		StatusbarASFlag_ASHugCrush			= 1 << 10,
		StatusbarASFlag_ASHealthGate		= 1 << 11,
		StatusbarASFlag_ASBreastAbsorb		= 1 << 12,
		StatusbarASFlag_ASBreastSuffocate	= 1 << 13,
		StatusbarASFlag_ASBreastVore		= 1 << 14,
		StatusbarASFlag_ASButtCrush			= 1 << 15,
		StatusbarASFlag_ASShrinkOutburst	= 1 << 16,
		StatusbarASFlag_ASCalamityDuration	= 1 << 17,
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
		std::unique_ptr<DynIconCooldownCalamityShrink> m_TinyCalamityShrink;
		std::unique_ptr<DynIconCooldownWrathfulCalamity> m_WrathfulCalamity;
		std::unique_ptr<DynIconCooldownHugs> m_Hugs;
		std::unique_ptr<DynIconCooldownHugAbsorb> m_HugCrush;
		std::unique_ptr<DynIconCooldownHealthGate> m_HealthGate;
		std::unique_ptr<DynIconCooldownBreastAbsorb> m_BreastAbsorb;
		std::unique_ptr<DynIconCooldownBreastSuffocate> m_BreastSuffocate;
		std::unique_ptr<DynIconCooldownBreastVore> m_BreastVore;
		std::unique_ptr<DynIconCooldownButtCrush> m_ButtCrush;
		std::unique_ptr<DynIconCooldownShrinkOutburst> m_ShrinkOutburst;
		std::unique_ptr<DynIconDurationTinyCalamity> m_CalamityDuration;
		
	};
}
