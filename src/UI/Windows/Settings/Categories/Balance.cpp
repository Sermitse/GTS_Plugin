#include "UI/Windows/Settings/Categories/Balance.hpp"

#include "UI/Core/ImUtil.hpp"

#include "UI/Controls/CheckBox.hpp"
#include "UI/Controls/Slider.hpp"
#include "UI/Controls/ToolTip.hpp"
#include "UI/Controls/ComboBox.hpp"
#include "UI/Controls/ConditionalHeader.hpp"

#include "Managers/MaxSizeManager.hpp"
#include "Config/Config.hpp"

#include "UI/Controls/Text.hpp"


namespace GTS {

	CategoryBalance::CategoryBalance() {
		m_name = "Balance";
	}

    void CategoryBalance::DrawLeft() {

        ImUtil_Unique 
		{

            PSString T0 = "Enable/disable balance mode.";
            PSString T1 = "Penalty multiplier to all size gain if balance mode is enabled.";
            PSString T2 = "Shrink over time multiplier, works both in and out of combat. ";
            PSString T3 = "When in combat, multiplies the shrink modifier above.";
            PSString T4 = "Affects amount of size you lose when being hit.";

            PSString THelp = "Balance mode makes it harder to gain size in general.\n"
                             "- Enemies resist size damage while they have stamina.\n"
                             "- You constantly shrink on hit and out of combat.\n\n"
                             "- On top of that all attribute increases are 50%% weaker and some configuration options are disabled.\n"
                             "- You cannot skip the quest while Balance Mode is enabled.";

            if (ImGui::CollapsingHeader("Balance Mode", ImUtil::HeaderFlagsDefaultOpen)) {

                ImGuiEx::HelpText("What is balance mode", THelp);
                ImGuiEx::CheckBox("Enable Balance Mode", &Config::Balance.bBalanceMode, T0);

                ImGui::BeginDisabled(!Config::Balance.bBalanceMode);

                ImGuiEx::SliderF("Size Gain Penalty", &Config::Balance.fBMSizeGainPenaltyMult, 1.0f, 10.0f, T1, "%.2fx");
                ImGuiEx::SliderF("Shrink Rate", &Config::Balance.fBMShrinkRate, 0.01f, 10.0f, T2, "%.2fx");
                ImGuiEx::SliderF("Shrink Rate Combat", &Config::Balance.fBMShrinkRateCombat, 0.01f, 1.0f, T3, "%.2fx");
                ImGuiEx::SliderF("Shrink On Hit", &Config::Balance.fBMShrinkOnHitMult, 0.01f, 2.0f, T4, "%.2fx");

                ImGui::EndDisabled();

                ImGui::Spacing();
            }
        }

    	// ---- Misc

        ImUtil_Unique 
		{

            PSString T0 = "Toggle whether the player should receive friendly size-related damage.";
            PSString T1 = "Toggle whether followers should receive friendly size-related damage.";
            PSString T2 = "Toggle whether the player and followers stagger / ragdoll due to others' size.";
            PSString T3 = "Toggle whether other NPCs stagger / ragdoll due to others' size.";

            if (ImGui::CollapsingHeader("Misc Settings", ImUtil::HeaderFlagsDefaultOpen)) {

                {
                    ImGui::Text("Friendly Size Damage Immunity");
                    ImGuiEx::CheckBox("For Player", &Config::Balance.bPlayerFriendlyImmunity, T0);
                    ImGui::SameLine();
                    ImGuiEx::CheckBox("For Followers", &Config::Balance.bFollowerFriendlyImmunity, T1);
                }

                ImGui::Spacing();

                {

                    ImGui::Text("Size Based Stagger");
                    ImGuiEx::CheckBox("Allow", &Config::Balance.bAllowOthersStagger, T3);
                    ImGui::SameLine();
                    ImGuiEx::CheckBox("Allow Friendly", &Config::Balance.bAllowFriendlyStagger, T2);

                }

                ImGui::Spacing();
            }
        }

		ImUtil_Unique
        {

            PSString T1 = "Enable sharing of some player aquired perks with followers if they don't already have them.\n"
                          "(Perks that affect maximum size are not shared)";

            if (ImGui::CollapsingHeader("Perks", ImUtil::HeaderFlagsDefaultOpen)) {
            	ImGuiEx::CheckBox("Share Perks With Followers", &Config::Balance.bSharePerks, T1);
                ImGui::Spacing();
            }
        }
    }

    void CategoryBalance::DrawRight() {

        ImUtil_Unique 
		{

            PSString T0 = "Change the formula used for all size gain.";
            PSString T1 = "Adjust the global multiplier for all size gains and losses.";

            PSString THelp = "Size gain mode defines the way you gain maximum size.\n"
        					 "In normal mode the maximum size you can grow to is defined by:\n"
							 "Current perks, GTS skill level, player level, and essense absorbed (through potions or by killing dragons while having a perk) and how far you are into the quest.\n"
        					 "In \"Mass Based\" mode your maximum size is determiend by how many things you have absorbed/eaten/etc...\n"
        					 "Mass based mode starts off at 1.0x scale and will allow you to grow up to your normal maximum skill based scale\n"
                             "(or if you have the perk to whatever you set max scale to) once you've absorbed enough things";

            if (ImGui::CollapsingHeader("Size Options", ImUtil::HeaderFlagsDefaultOpen)) {

                ImGuiEx::HelpText("What is this", THelp);

                if (ImGuiEx::ComboEx<LSizeMode_t>("Size Gain Mode", Config::Balance.sSizeMode, T0)) {
                    if (Config::Balance.sSizeMode == "kNormal") Config::Balance.fSpellEfficiency = 0.55f;
                    if (Config::Balance.sSizeMode == "kMassBased") Config::Balance.fSpellEfficiency = 0.33f;
                }

                ImGuiEx::SliderF("Efficiency Mult", &Config::Balance.fSpellEfficiency, 0.1f, 1.0f, T1, "%.2fx");

                ImGui::Spacing();
            }
        }

        ImUtil_Unique 
		{

            PSString THelp = "Maximal Size isn't always determined only by these sliders\n"
                             "It is further affected by:\n"
                             "- Game Scale (SetScale)\n"
                             "- Natural Size";

        	const bool HasPerk = Runtime::HasPerk(PlayerCharacter::GetSingleton(), Runtime::PERK.GTSPerkColossalGrowth);
            const bool Unlock = Persistent::UnlockMaxSizeSliders.value;

            std::string DisableReason = "";
            if (Config::Balance.bBalanceMode) {
                DisableReason += "Balance Mode Active";
            }
            else if (!HasPerk) {
                DisableReason += "Missing Perk: \"Colossal Growth\"";
            }
            else if (!Unlock) {
                DisableReason += "Unlimited Mode Required";
            }

            if (ImGuiEx::ConditionalHeader("Size Limits", DisableReason, HasPerk && Unlock && !Config::Balance.bBalanceMode)) {
	            constexpr float Max = 255.0f;
	            constexpr float Min = 0.0;

                const bool IsMassBased = Config::Balance.sSizeMode == "kMassBased";
                const float MassLimit = get_max_scale(PlayerCharacter::GetSingleton());

                ImGuiEx::HelpText("What is max size determined", THelp);

                {   //Player Size
                    float* Scale = &Config::Balance.fMaxPlayerSizeOverride;
                    const bool ShouldBeInf = *Scale > Max - 5.0000f;
                    const bool ShouldBeAuto = *Scale < Min + 0.1000f;
                    std::string _Frmt;

                    if (ShouldBeInf) {
                        *Scale = 1000000.0f;
                        _Frmt = "Infinite";
                    }
                    else if (ShouldBeAuto) {
                        float SkillBasedLimit = MassMode_GetValuesForMenu(PlayerCharacter::GetSingleton());
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

                        ImGuiEx::SliderF("Max Player Size", Scale, Min, Max, ToolTip.c_str(), _Frmt.c_str());

                    }

                }

                {   //Max Follower Size

                    float* Scale = &Config::Balance.fMaxFollowerSize;
                    const bool ShouldBeInf = *Scale > Max - 5.0000f;
                    const bool ShouldBeAuto = *Scale < Min + 0.1000f;

                    std::string _Frmt;
                    if (ShouldBeInf) {
                        _Frmt = "Infinite";
                        *Scale = 1000000.0f;
                    }
                    else if (ShouldBeAuto) {
                    
                        if (IsMassBased) {
                            _Frmt = fmt::format("Mass Based");
                        }
                        else {
                            _Frmt = fmt::format("Skill Based");
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
                            "At 0.0x scale the limit will be based on the follower's GTS Level and perks.",
                            Max - 5.0f
                        );

                        ImGuiEx::SliderF("Max Follower Size", Scale, Min, Max, ToolTip.c_str(), _Frmt.c_str());

                    }
                }

                {   //Other NPC Max Size

                    float* Scale = &Config::Balance.fMaxOtherSize;
                    const bool ShouldBeInf = *Scale > Max - 5.0000f;
                    const bool ShouldBeAuto = *Scale < Min + 0.1000f;

                    std::string _Frmt;
                    if (ShouldBeInf) {
                        _Frmt = "Infinite";
                        *Scale = 1000000.0f;
                    }
                    else if (ShouldBeAuto) {

                        if (IsMassBased) {
                            _Frmt = fmt::format("Mass Based");
                        }
                        else {
                            _Frmt = fmt::format("Skill Based");
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
                            "At 0.0x scale the limit will be based on the npc's GTS Level and perks.",
                            Max - 5.0f
                        );
                        ImGuiEx::SliderF("Max NPC Size", Scale, Min, Max, ToolTip.c_str(), _Frmt.c_str());
                    }
                }
                ImGui::Spacing();
            }
        }

        // ---- Multipiers

        ImUtil_Unique 
		{

            PSString T0 = "Changes the amount of damage size-related actions do.";
            PSString T1 = "Changes the amount of damage increase regular melee and magic atacks gain.";
            PSString T2 = "Adjust the speed at which you gain size-related experience.";
            PSString T3 = "Change the ammount of carry weight capacity gained based on your size.";

            if (ImGuiEx::ConditionalHeader("Multipiers", "Balance Mode Active", !Config::Balance.bBalanceMode)) {

                ImGuiEx::SliderF("Size-Related Damage Multiplier", &Config::Balance.fSizeDamageMult, 0.02f, 2.0f, T0, "%.2fx");
                ImGuiEx::SliderF("Weapon/Magic Damage Multiplier", &Config::Balance.fStatBonusDamageMult, 0.02f, 2.0f, T1, "%.2fx");
                ImGuiEx::SliderF("Carry Weight Multiplier", &Config::Balance.fStatBonusCarryWeightMult, 0.02f, 2.0f, T3, "%.2fx");
                ImGuiEx::SliderF("Experience Multiplier", &Config::Balance.fExpMult, 0.02f, 5.0f, T2, "%.2fx");
            }
        }
    }
}
