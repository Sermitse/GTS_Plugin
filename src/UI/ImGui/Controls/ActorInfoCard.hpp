#pragma once

#include "UI/ImGui/Controls/Button.hpp"
#include "UI/ImGui/Lib/imgui.h"

#include "Utils/UnitConverter.hpp"

namespace ImGuiEx {

    class ActorInfoCard {

        struct ActorInfo {

            std::string Name = {};

            //Scale
            float fScaleCurrent = {};
            float fScaleMax = {};
            float fScaleProgress = {};  //Normalized Value from 0 to 1
            std::string sFmtScale = {};

            //Bonuses
            float fScaleBonus = {};
            float fWeight = {};
            float fGTSAspect = {};
            float fDamageResist = {};
            float fDamageMultiplier = {};
            float fHighHeelDamageBonus = {};
            float fCarryWeightBonus = {};
            float fSpeedMult = {};
            float fJumpMult = {};

            float fStolenAtributes;
            float fStolenAtributesCap;
            float fStolenHealth;
            float fStolenMagicka;
            float fStolenStamina;

            //Player Only
            float fShrinkResistance;
            float fOnTheEdge;
            float fSizeReserve;

            //Flags
            bool bMassModeEnabled;
            bool bIsPlayerInMassMode;

            static std::optional<ActorInfo> GetData(RE::Actor* a_actor) {

                auto I = ActorInfo{};

                if (!a_actor) return std::nullopt;
                if (!a_actor->Get3D(false)) return std::nullopt;

                I.Name =                   a_actor->GetName();

                //Scale
                I.fScaleCurrent =           GTS::get_visual_scale(a_actor);
                I.fScaleMax =               GTS::get_max_scale(a_actor);
                I.fScaleProgress =          I.fScaleMax < 250.0f ? I.fScaleCurrent / I.fScaleMax : 0.0f;
                I.sFmtScale =               fmt::format("{} ({:.2f}x)", GTS::GetFormatedHeight(a_actor), I.fScaleCurrent);

                return std::move(I);

            }

        };






        public:
        enum class Section {
            kNone,
            kSectionExtra,
            kSectionKillInfo
        };

        ActorInfoCard();

        void Draw(RE::Actor* a_actor, const ImVec2& card_size = { 400, -FLT_MIN });

        // Override these to customize card content
        virtual void RenderMainContent(const ActorInfo& Data);

        virtual void RenderSection1(const ActorInfo&);

        virtual void RenderSection2(RE::Actor* a_actor);

    private:
        Section expanded_section_;

        ImU32 m_wChildFlags = ImGuiChildFlags_None;
        ImU32 m_wWindowFlags = ImGuiWindowFlags_None;
        const uint32_t m_baseIconSize = 32;
    };
}
