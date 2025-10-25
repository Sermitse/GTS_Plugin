#pragma once

#include "UI/ImGui/Controls/BuffIcons/DynIcon_CataclysmicVoreStacks.hpp"
#include "UI/ImGui/Controls/BuffIcons/DynIcon_LifeAbsorbStacks.hpp"
#include "UI/ImGui/Controls/BuffIcons/DynIcon_DamageReduction.hpp"
#include "UI/ImGui/Controls/BuffIcons/DynIcon_Enchantment.hpp"
#include "UI/ImGui/Controls/BuffIcons/DynIcon_OnTheEdge.hpp"
#include "UI/ImGui/Controls/BuffIcons/DynIcon_SizeReserve.hpp"

#include "UI/ImGui/Lib/imgui.h"
#include "Utils/KillDataUtils.hpp"

namespace ImGuiEx {

    class ActorInfoCard {

        struct ActorInfo {

            std::string Name = {};

            //Scale
            float fScaleCurrent = {};
            float fScaleMax = {};
            float fScaleNatural = {};
			float fMassModeScaleMax = {};
            float fScaleProgress = {};  //Normalized Value from 0 to 1

			int iLifeAbsorbStacks = {};
            int iTotalKills = {};
            int iVoreStacks = {};

            float fOverkills = {};
            float fOverkillMult = {};

            //Bonuses
            float fScaleBonus = {};
            float fGTSAspect = {};
            float fDamageResist = {};
            float fDamageBonus = {};
            float fHighHeelDamageBonus = {};
            float fCarryWeightBonus = {};
            float fSpeedMult = {};
            float fJumpMult = {};

            float fStolenAtributes = {};
            float fStolenAtributesCap = {};
            float fStolenHealth = {};
            float fStolenMagicka = {};
            float fStolenStamina = {};
            float fStolenCap = {};

            //Player Only
            bool bIsPlayer = {};
            float fShrinkResistance = {};
            float fOnTheEdge = {};
            float fSizeReserve = {};
            float fSizeEssence = {};

            //Formated
            std::string sFmtScale = {};
			std::string sFmtWeight = {};

            //Perk Check
            bool bHasPerk_GTSFullAssimilation = {};


            static std::optional<ActorInfo> GetData(RE::Actor* a_actor);
        };

        public:
        enum class Section {
            kNone,
            kSectionExtra,
            kSectionKillInfo
        };

        ActorInfoCard();
        void Draw(RE::Actor* a_actor, const ImVec2& card_size = { 500, 0 });
        void DrawBuffIcons(const ActorInfo& Data) const;
        void DrawBaseInfoTable(const ActorInfo& Data) const;

		private:
		void DrawSpectateButton(RE::Actor* a_actor) const;
        static void DrawKillStat(RE::Actor* a_actor, const char* a_name, GTS::SizeKillType a_type, uint8_t a_colOffset = 0, const char* a_toolTip = nullptr);
        static void DrawKillData(RE::Actor* a_actor);
        void DrawMainContent(const ActorInfo& Data) const;
        void DrawExtraStats(const ActorInfo& Data) const;


        //Other UI Data
        bool bMassModeEnabled = false;
        bool bIsPlayerMassMode = false;

        Section m_expandedSec;
        ImU32 m_wChildFlags = ImGuiChildFlags_None;
        ImU32 m_wWindowFlags = ImGuiWindowFlags_None;
        const uint32_t m_baseIconSize = 32;
        const uint32_t m_buffIconSize = 64;

        //Icon Instances
		std::unique_ptr<DynIconLifeabsorbStacks> m_lifeAbsorbIcon;
        std::unique_ptr<DynIconDamageReduction> m_damageReductionIcon;
        std::unique_ptr<DynIconEnchantment> m_enchantmentIcon;
        std::unique_ptr<DynIconSizeReserve> m_sizeReserveIcon;
        std::unique_ptr<DynIconOnTheEdge> m_onTheEdgeIcon;
        std::unique_ptr<DynIconCataclysmicVoreStacks> m_CataclysmicVoreStacksIcon;
   
    };
}
