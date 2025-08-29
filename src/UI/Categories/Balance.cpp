#include "UI/Categories/Balance.hpp"
#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/ImUtil.hpp"



namespace {


    const std::string CollossalGrowthPerk = "GTSPerkColossalGrowth"; //AKA GtsTotalSizeControl
    const std::string PleasurableGrowthPerk = "GTSPerkRandomGrowth"; //AKA GtsCrushGrowthAug
    const std::string CrushGrowthAugmetationPerk = "GTSPerkGrowthDesire";
    const std::string GrowOnHitPerk = "GTSPerkHitGrowth";

}

namespace GTS {



    void CategoryBalance::DrawLeft() {

        ImUtil_Unique{

            const char* T0 = "Enable/disable balance mode.";
            const char* T1 = "Penalty multiplier to all size gain if balance mode is enabled.";
            const char* T2 = "Shrink over time multiplier, works both in and out of combat. ";
            const char* T3 = "When in combat, multiplies the shrink modifier above.";
            const char* T4 = "Affects amount of size you lose when being hit.";

            const char* THelp = "Balance mode makes it harder to gain size in general.\n"
                                "- Enemies resist size damage while they have stamina.\n"
                                "- You constantly shrink on hit and out of combat.\n\n"
                                "- On top of that all attribute increases are 50%% weaker and some configuration options are disabled.\n"
                                "- You cannot skip the quest while Balance Mode is enabled.";

            if (ImGui::CollapsingHeader("Balance Mode", ImUtil::HeaderFlagsDefaultOpen)) {

                ImGui::TextColored(ImUtil::ColorSubscript,"What is balance mode (?)");
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip(THelp);
                }

                ImUtil::CheckBox("Enable Balance Mode", &Settings.bBalanceMode, T0);

                ImGui::BeginDisabled(!Settings.bBalanceMode);

                ImUtil::SliderF("Size Gain Penalty", &Settings.fBMSizeGainPenaltyMult, 1.0f, 10.0f, T1, "%.2fx");
                ImUtil::SliderF("Shrink Rate", &Settings.fBMShrinkRate, 0.01f, 10.0f, T2, "%.2fx");
                ImUtil::SliderF("Shrink Rate Combat", &Settings.fBMShrinkRateCombat, 0.01f, 1.0f, T3, "%.2fx");
                ImUtil::SliderF("Shrink On Hit", &Settings.fBMShrinkOnHitMult, 0.01f, 2.0f, T4, "%.2fx");

                ImGui::EndDisabled();

                ImGui::Spacing();
            }
        }

    	// ---- Misc

        ImUtil_Unique{

            const char* T0 = "Toggle whether the player should receive friendly size-related damage.";
            const char* T1 = "Toggle whether followers should receive friendly size-related damage.";
            const char* T2 = "Toggle whether the player and followers stagger / ragdoll due to others' size.";
            const char* T3 = "Toggle whether other NPCs stagger / ragdoll due to others' size.";

            if (ImGui::CollapsingHeader("Misc Settings", ImUtil::HeaderFlagsDefaultOpen)) {

                {
                    ImGui::Text("Friendly Size Damage Immunity");
                    ImUtil::CheckBox("Player", &Settings.bPlayerFriendlyImmunity, T0);
                    ImGui::SameLine();
                    ImUtil::CheckBox("Followers", &Settings.bFollowerFriendlyImmunity, T1);
                }

                ImGui::Spacing();

                {

                    ImGui::Text("Size Based Stagger");
                    ImUtil::CheckBox("Allow", &Settings.bAllowOthersStagger, T3);
                    ImGui::SameLine();
                    ImUtil::CheckBox("Allow Friendly", &Settings.bAllowFriendlyStagger, T2);

                }

                ImGui::Spacing();
            }
        }
    }

    void CategoryBalance::DrawRight() {

        ImUtil_Unique{

            const char* T0 = "Change the formula used for all size gain.";
            const char* T1 = "Adjust the global multiplier for all size gains and losses.";

            const char* Thelp = "Size gain mode defines the way you gain maximum size.\n"
        						"In normal mode the maximum size you can grow to is defined by:\n"
								"Current perks, GTS skill level, player level, and essense absorbed (through potions or by killing dragons while having a perk) and how far you are into the quest.\n"
        						"In \"Mass Based\" mode your maximum size is determiend by how many things you have absorbed/eaten/etc...\n"
        						"Mass based mode starts off at 1.0x scale and will allow you to grow up to your normal maximum skill based scale (or if you have the perk to whatever you set max scale to) once you've absorbed enough things";

            if (ImGui::CollapsingHeader("Size Options", ImUtil::HeaderFlagsDefaultOpen)) {

                ImGui::TextColored(ImUtil::ColorSubscript, "What is this (?)");
                ImUtil::Tooltip(Thelp, true);

                if (ImUtil::ComboEx<SizeMode>("Size Gain Mode", Settings.sSizeMode, T0)) {
                    if (Settings.sSizeMode == "kNormal") Settings.fSpellEfficiency = 0.55f;
                    if (Settings.sSizeMode == "kMassBased") Settings.fSpellEfficiency = 0.33f;
                }

                ImUtil::SliderF("Efficiency Mult", &Settings.fSpellEfficiency, 0.1f, 1.0f, T1, "%.2fx");

                ImGui::Spacing();
            }
        }

        ImUtil_Unique{

            const char* Thelp = "Maximal Size isn't always determined only by these sliders\n"
                                "It is further affected by:\n"
                                "- Game Scale (SetScale)\n"
                                "- Natural Size";

        	bool HasPerk = Runtime::HasPerk(PlayerCharacter::GetSingleton(), CollossalGrowthPerk);
            bool Unlock = Persistent::GetSingleton().UnlockMaxSizeSliders.value;

			const std::string HeaderHelpText = Settings.bBalanceMode ? "Balance Mode Active" : "Requires \"Colossal Growth\" Perk and \"gts unlimited\" console command";
            if (ImUtil::ConditionalHeader("Size Limits", HeaderHelpText, HasPerk && Unlock && !Settings.bBalanceMode)) {
	            constexpr float Max = 255.0f;
	            constexpr float Min = 0.0;

                const bool IsMassBased = Settings.sSizeMode == "kMassBased";
                const float MassLimit = get_max_scale(PlayerCharacter::GetSingleton());

                ImGui::TextColored(ImUtil::ColorSubscript,"How is Max Size Determined (?)");
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip(Thelp);
                }

                {   //Player Size
                    float* Scale = &Settings.fMaxPlayerSizeOverride;
                    const bool ShouldBeInf = *Scale > Max - 5.0000f;
                    const bool ShouldBeAuto = *Scale < Min + 0.1000f;
                    std::string _Frmt;

                    if (ShouldBeInf) {

                        *Scale = 1000000.0f;

                        if (IsMassBased) {
                            _Frmt = fmt::format("Mass Based [{:.2f}x] Max [Inf]", MassLimit);
                        }
                        else {
                            _Frmt = "Infinite";
                        }
                    }
                    else if (ShouldBeAuto) {
                      
                        const float SkillBasedLimit = Persistent::GetSingleton().GlobalSizeLimit.value;

                        if (IsMassBased) {
                            _Frmt = fmt::format("Mass Based [{:.2f}x] Max [{:.2f}x]", MassLimit, SkillBasedLimit);
                        }
                        else {
                            _Frmt = fmt::format("Skill Based [{:.2f}x]", SkillBasedLimit);
                        }

                        *Scale = 0.0f;
                    }
                    else {

                        if (IsMassBased) {
                            _Frmt = fmt::format("Mass Based [{:.2f}x] Max [{:.2f}]", MassLimit, *Scale);
                        }

                        _Frmt = "%.1fx";
                    }

                    {

                        std::string ToolTip = fmt::format(
                            "Change the maximum player size\n"
                            "Higher than {:.0f}x scale disables the limit entirely\n"
                            "At 0.0x the limit is based on your skill level and perks.",
                            Max - 5.0f
                        );

                        ImUtil::SliderF("Max Player Size", Scale, Min, Max, ToolTip.c_str(), _Frmt.c_str());

                    }

                }

                {   //Max Follower Size

                    float* Scale = &Settings.fMaxFollowerSize;
                    const bool ShouldBeInf = *Scale > Max - 5.0000f;
                    const bool ShouldBeAuto = *Scale < Min + 0.1000f;

                    std::string _Frmt;
                    if (ShouldBeInf) {
                        _Frmt = "Infinite";
                        *Scale = 1000000.0f;
                    }
                    else if (ShouldBeAuto) {

                        const float PlayersLimit = Persistent::GetSingleton().GlobalSizeLimit.value;

                        if (PlayersLimit >= 250.01f) {
                            _Frmt = fmt::format("Based on Player [Infinite]", PlayersLimit);
                        }
                        else {
                            _Frmt = fmt::format("Based on Player [{:.2f}x]", PlayersLimit);
                        }

                        *Scale = 0.0f;
                    }
                    else {
                        _Frmt = "%.1fx";
                    }

                    {
                        std::string ToolTip = fmt::format(
                            "Change the maximum follower size\n"
                            "Higher than {:.0f}x scale disables the limit entirely\n"
                            "At 0.0x scale the limit will be based on the player's.",
                            Max - 5.0f
                        );

                        ImUtil::SliderF("Max Follower Size", Scale, Min, Max, ToolTip.c_str(), _Frmt.c_str());

                    }
                }

                {   //Other NPC Max Size

                    float* Scale = &Settings.fMaxOtherSize;
                    const bool ShouldBeInf = *Scale > Max - 5.0000f;
                    const bool ShouldBeAuto = *Scale < Min + 0.1000f;

                    std::string _Frmt;
                    if (ShouldBeInf) {
                        _Frmt = "Infinite";
                        *Scale = 1000000.0f;
                    }
                    else if (ShouldBeAuto) {

                        const float NPCsLimit = Persistent::GetSingleton().GlobalSizeLimit.value;

                        if (NPCsLimit >= 250.01f) {
                            _Frmt = fmt::format("Based on Player [Infinite]", NPCsLimit);
                        }
                        else {
                            _Frmt = fmt::format("Based on Player [{:.2f}x]", NPCsLimit);
                        }
                        *Scale = 0.0f;
                    }
                    else {
                        _Frmt = "%.1fx";
                    }

                    {
                        std::string ToolTip = fmt::format(
                            "Change the maximum size for non-follower NPCs\n"
                            "Higher than {:.0f}x scale disables the limit entirely\n"
                            "At 0.0x scale the limit will be based on the player's.",
                            Max - 5.0f
                        );
                        ImUtil::SliderF("Max NPC Size", Scale, Min, Max, ToolTip.c_str(), _Frmt.c_str());
                    }
                }
                ImGui::Spacing();
            }
        }

        // ---- Multipiers

        ImUtil_Unique{

            const char* T0 = "Changes the amount of damage size-related actions do.";
            const char* T1 = "Changes the amount of damage increase regular melee and magic atacks gain.";
            const char* T2 = "Adjust the speed at which you gain size-related experience.";
            const char* T3 = "Change the ammount of carry weight capacity gained based on your size.";

            if (ImUtil::ConditionalHeader("Multipiers", "Balance Mode Active", !Settings.bBalanceMode)) {

                ImUtil::SliderF("Size Damage Multiplier", &Settings.fSizeDamageMult, 0.02f, 2.0f, T0, "%.2fx");
                ImUtil::SliderF("Damage Multiplier", &Settings.fStatBonusDamageMult, 0.02f, 2.0f, T1, "%.2fx");
                ImUtil::SliderF("Carry Weight Multiplier", &Settings.fStatBonusCarryWeightMult, 0.02f, 2.0f, T3, "%.2fx");
                ImUtil::SliderF("Experience Multiplier", &Settings.fExpMult, 0.02f, 5.0f, T2, "%.2fx");
            }
        }
    }
}