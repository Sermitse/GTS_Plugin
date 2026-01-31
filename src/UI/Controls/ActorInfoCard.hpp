#pragma once

#include "UI/Controls/StatusBar.hpp"
#include "Utils/KillDataUtils.hpp"

namespace ImGuiEx {

    class ActorInfoCard {

        struct ActorInfo {

            std::string Name = {};

            //Scale
            float fScaleCurrent     = {};
            float fScaleMax         = {};
            float fScaleNatural     = {};
			float fMassModeScaleMax = {};
            float fScaleProgress    = {};  //Normalized Value from 0 to 1

            int iTotalKills     = {};
            float fOverkills    = {};
            float fOverkillMult = {};

            //Bonuses
            float fScaleBonus          = {};
            float fDamageBonus         = {};
            float fHighHeelDamageBonus = {};
            float fCarryWeightBonus    = {};
            float fSpeedMult           = {};
            float fJumpMult            = {};
            float fGTSAspect           = {};

            float fStolenAtributes    = {};
            float fStolenAtributesCap = {};
            float fStolenHealth       = {};
            float fStolenMagicka      = {};
            float fStolenStamina      = {};
            float fStolenCap          = {};

            //Player Only
            bool bIsPlayer          = {};
            float fShrinkResistance = {};
            float fSizeEssence      = {};

            //Formated
            std::string sFmtScale  = {};
			std::string sFmtWeight = {};

            //Skill
            float fSkillLevel    = {};
            float fSkillProgress = {};
            float fSkillRatio    = {};

            //Perk Check
            bool bHasPerk_GTSFullAssimilation = {};

            RE::ActorHandle pTargetActor = {};

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
        static void DrawKillStat(RE::Actor* a_actor, const char* a_name, GTS::DeathType a_type, uint8_t a_colOffset = 0, const char* a_toolTip = nullptr);
        static void DrawKillData(RE::Actor* a_actor);
        void DrawMainContent(const ActorInfo& Data) const;
        void DrawExtraStats(const ActorInfo& Data) const;

        //Other UI Data
        bool bMassModeEnabled = false;

        Section m_expandedSec;
        ImU32 m_wChildFlags = ImGuiChildFlags_None;
        ImU32 m_wWindowFlags = ImGuiWindowFlags_None;
        const uint32_t m_baseIconSize = 32;
        const uint32_t m_buffIconSize = 48;
        std::unique_ptr<StatusBar> m_buffs;
   
    };
}
