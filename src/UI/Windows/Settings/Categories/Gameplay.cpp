#include "UI/Windows/Settings/Categories/Gameplay.hpp"

#include "UI/Lib/imgui.h"
#include "UI/Core/ImUtil.hpp"

#include "UI/Controls/CheckBox.hpp"
#include "UI/Controls/ComboBox.hpp"
#include "UI/Controls/ConditionalHeader.hpp"
#include "UI/Controls/Slider.hpp"

#include "Config/Config.hpp"

#include "UI/GTSMenu.hpp"
#include "UI/Controls/CollapsingTabHeader.hpp"

namespace GTS {

    void CategoryGameplay::GameModeOptions(GameplayActorSettings_t* a_Settings, bool a_isPlayer) {

        PSString T0 = "Select the game mode\n\n"
                      "Basic:\n"
                      "- Grow: Slowly grow to your size limit.\n"
                      "- Shrink: Slowly shrink back to your natural scale.\n"
                      "- Combat Growth: Grow during combat and shrink outside of combat back to your natural scale.\n"
                      "- Slow Combat Growth: Slowly grow during combat and retain any size gained.\n\n"
                      "Curses:\n"
                      "- Curse of Growth: You will continiously grow in size like \"Grow\" but in spurts of varying strength up until the specified scale limit which you can change below.\n"
                      "- Curse of the Giantess: You will rapidly grow to the specified size if you are smaller. Spells like \"Restore Size\" will not shrink you below this size.\n"
                      "- Curse of Diminishing: When not in combat or when not performing any giantess actions. You will slowly shrink to the target scale if too large.\n"
                      "- Size Locked: Combines the effects of both curses. You will grow to the specified size and slowly shrink back to it if larger.";

        PSString T1 = "Modify the amount grown or shrunk each tick.\n"
    	              "Grow Rate | Shrink Rate";

        PSString T3 = "Set the maximum size for the \"Curse of Growth\" game mode";

        PSString T4 = "Multiply the size-gain rate by 25%% of your current size.\n"
                      "The amount gained caps out at 10x scale.";

        PSString T5 = "Set the target scale to grow/shrink to when using the following game modes:\n\n"
                      "- Curse of the Giantess\n"
                      "- Curse of the Diminishing\n"
                      "- Size Locked";

        PSString T6 = "Change how often the curse effects should be applied.\n"
                      "The value you select is offset by +/- 10%% each time.";


        ImGuiEx::ComboEx<LActiveGamemode_t>("Game Mode", a_Settings->sGameMode, T0);

		auto currentMode = magic_enum::enum_cast<LActiveGamemode_t>(a_Settings->sGameMode);

        ImGui::BeginDisabled(a_Settings->sGameMode == "kNone");

        ImGui::Spacing();
        static std::array const pTemp = { &a_Settings->fGrowthRate, &a_Settings->fShrinkRate };
        

        if (currentMode.has_value()) {

            const bool CurseModes = currentMode.value() == LActiveGamemode_t::kCurseOfGrowth      ||
                                    currentMode.value() == LActiveGamemode_t::kCurseOfTheGiantess ||
                                    currentMode.value() == LActiveGamemode_t::kCurseOfDiminishing ||
                                    currentMode.value() == LActiveGamemode_t::kSizeLocked         ||
                                    currentMode.value() == LActiveGamemode_t::kLevelLocked;

            const bool UsesMultiplier = !CurseModes;
			const bool UsesRate = currentMode.value() != LActiveGamemode_t::kLevelLocked;

            ImGui::Spacing();

            if (UsesRate) {
                ImGuiEx::SliderF2("Grow/Shrink Rate", pTemp.at(0), 0.001f, 0.2f, T1, "%.3fx");
            }

            if (UsesMultiplier) {
                ImGuiEx::CheckBox("Multiply Rates", &a_Settings->bMultiplyGrowthrate, T4);
            }

            if (CurseModes) {
                ImGuiEx::SliderF("Update Interval", &a_Settings->fGameModeUpdateInterval, 1.0f, 60.0f, T6, "Every %.2f Seconds");
            }

            if (currentMode.value() == LActiveGamemode_t::kCurseOfTheGiantess ||
                currentMode.value() == LActiveGamemode_t::kCurseOfDiminishing ||
                currentMode.value() == LActiveGamemode_t::kSizeLocked){
                ImGuiEx::SliderF("Curse of Growth Limit", &a_Settings->fCurseGrowthSizeLimit, 1.1f, 50.0f, T3, "%.2fx");
                ImGuiEx::SliderF("Target Scale", &a_Settings->fCurseTargetScale, 0.5f, 5.0f, T5, "%.2fx");
            }
            else if (currentMode.value() == LActiveGamemode_t::kLevelLocked) {

                if (a_isPlayer) {
                    Actor* Target = PlayerCharacter::GetSingleton();
                    float TargetScale = (a_Settings->bUseGTSSkill ? GetGtsSkillLevel(Target) : Target->GetLevel()) * a_Settings->fScalePerLevel;
                    ImGui::Text("%s's Minimum Scale Will Be: %.2fx", Target->GetName(), TargetScale);
                }
                else {
                    for (const auto& teammate : GTSMenu::WindowManager->GetCachedTeamMateList()) {
                        float TargetScale = (a_Settings->bUseGTSSkill ? GetGtsSkillLevel(teammate) : teammate->GetLevel()) * a_Settings->fScalePerLevel;
                        ImGui::PushID(teammate);
                        ImGui::Text("%s's Minimum Scale Will Be: %.2fx", teammate->GetName(), TargetScale);
                        ImGui::PopID();
                    }
                }

                ImGuiEx::SliderF("Scale Per Level", &a_Settings->fScalePerLevel, 0.0001f, 0.2f, "Increase Size per Level", "Adds %.4fx per Level");
                ImGuiEx::CheckBox("Use GTS Skill Level", &a_Settings->bUseGTSSkill, "Use the GTS Skill Level instead of the regular Level to calculate target scale.");

            }
        }

        ImGui::EndDisabled();
        ImGui::Spacing();
    }

    CategoryGameplay::CategoryGameplay() {
	    m_name = "Gameplay";
    }

    void CategoryGameplay::DrawLeft() {


        //----- Perk Settings

        ImUtil_Unique {

            if (ImGui::CollapsingHeader("Perk Settings", ImUtil::HeaderFlagsDefaultOpen)) {

                const bool PerkCondCrush = Runtime::HasPerk(PlayerCharacter::GetSingleton(), Runtime::PERK.GTSPerkGrowthDesire);
                const bool PerkCondHit = Runtime::HasPerk(PlayerCharacter::GetSingleton(), Runtime::PERK.GTSPerkHitGrowth);
                const bool PerkCondAtribCap = Runtime::HasPerk(PlayerCharacter::GetSingleton(), Runtime::PERK.GTSPerkGrowthDesire);

                PSString T1 = "Upon crushing someone: Grow in size.\n"
                              "Applies to both the player and followers.";

                PSString T2 = "On being hit: Grow in size.\n"
                              "Applies to both the player and followers.";

                PSString T3 = "By default, the maximum amount of bonus attributes per player level is 2.\n"
                              "You can adjust the multiplier for this here.\n";

                const std::string tGrowOnCrush = fmt::format("Grow On Crush {}", (!PerkCondCrush ? "[Missing Perk]" : ""));
                ImGuiEx::CheckBox(tGrowOnCrush.c_str(), &Config::Gameplay.bEnableCrushGrowth, T1, !PerkCondCrush);

                ImGui::SameLine();

                const std::string tGrowOnHit = fmt::format("Grow On Hit {}", (!PerkCondHit ? "[Missing Perk]" : ""));
                ImGuiEx::CheckBox(tGrowOnHit.c_str(), &Config::Gameplay.bEnableGrowthOnHit, T2, !PerkCondHit);

                const char* tSizeConfFMT = (!PerkCondAtribCap ? "[Missing Perk]" : "%.2fx");
                ImGuiEx::SliderF("Full Assimilation Attrib. Cap", &Config::Gameplay.fFullAssimilationLevelCap, 0.1f, 5.0f, T3, tSizeConfFMT, !PerkCondAtribCap);

                ImGui::Spacing();
            }
        }

    	//----- Armor Stripping

        ImUtil_Unique {

            if (ImGui::CollapsingHeader("Armor Stripping", ImUtil::HeaderFlagsDefaultOpen)) {

                PSString T1 = "Enable/disable the automatic unequipping of clothing/armor when large enough.\n"
                              "Applies to both the player and followers.\n"
                              "This system works based on size thresholds.\n"
                              "When an armor piece is stripped, the size required for the next piece to be removed increases.";

                PSString T2 = "Set the scale threshold at which to start unequipping pieces of clothing/armor.";
                PSString T3 = "Set the scale threshold at which all clothing/armor is unequipped.";

                ImGuiEx::CheckBox("Enable Armor Strip", &Config::Gameplay.bClothTearing, T1);

                ImGui::BeginDisabled(!Config::Gameplay.bClothTearing);
                ImGuiEx::SliderF("Starting Threshold", &Config::Gameplay.fClothRipStart, 1.1f, 2.5f, T2, "%.2fx");
                ImGuiEx::SliderF("Strip All Threshold", &Config::Gameplay.fClothRipThreshold, Config::Gameplay.fClothRipStart + 0.1f, 3.5f, T3, "%.2fx",false, true);
                ImGui::EndDisabled();

                ImGui::Spacing();
            }
        }

    	//----- Size Effects

        ImUtil_Unique {

            if (ImGui::CollapsingHeader("Size Effects", ImUtil::HeaderFlagsDefaultOpen)) {

                PSString T1 = "When large enough, footsteps or size-related actions will launch physics-enabled items.";

                PSString T2 = "Apply the physics hit impulse to objects outside of the current cell.\n"
                              "Beware: This is computationally very heavy. Hence the existence of this toggle.\n"
                              "If you experience FPS drops, try disabling this.";

                PSString T3 = "Enable effects like dust and camera shake/rumble.";

                ImGuiEx::CheckBox("Launch Objects", &Config::Gameplay.bLaunchObjects, T1);
                ImGui::SameLine();
                ImGuiEx::CheckBox("Enable In All Cells", &Config::Gameplay.bLaunchAllCells, T2, !Config::Gameplay.bLaunchObjects);
                ImGuiEx::CheckBox("Player Dust/Rumble Effects", &Config::Gameplay.bPlayerAnimEffects, T3);
                ImGuiEx::CheckBox("Follower Dust/Rumble Effects", &Config::Gameplay.bNPCAnimEffects, T3);

                ImGui::Spacing();
            }
        }

        //----- Random Growth

        ImUtil_Unique {

            const bool HasPerk = Runtime::HasPerk(PlayerCharacter::GetSingleton(), Runtime::PERK.GTSPerkRandomGrowth);
            const bool BalancedMode = Config::Balance.bBalanceMode;
        	const char* Reason = "Requires \"Pleasurable Growth\" Perk";

            if (ImGuiEx::ConditionalHeader("Random Growth", Reason, HasPerk)) {

                PSString T1 = "Change how often the random growth should trigger.\n"
                              "Lower values = More often.\n"
                              "Higher values = Less often.\n"
                              "Set to 0.0 to disable it entirely.\n"
							  "Note: If Balance Mode is enabled the multiplier is locked to 1.0x. It can still be disabled however by setting it to 0.0";

                PSString THelp = "Random Growth chance is decreased if your size is > than x1.5, up to 4 times decreased chance\n"
                                 "If you have 'Breaching Growth' perk, random growth has a chance to start 'Large Growth' animation";

                const char* FmtBalance = BalancedMode ? "Balance Mode (1.0x)" : "%.2fx";

                const char* Fmt1 = Config::Gameplay.GamemodePlayer.fRandomGrowthDelay != 0.0f ? FmtBalance : "Disabled";
                const char* Fmt2 = Config::Gameplay.GamemodeFollower.fRandomGrowthDelay != 0.0f ? FmtBalance : "Disabled";

                

                ImGui::TextColored(ImUtil::Colors::Subscript,"About Random Growth (?)");
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip(THelp);
                }

                ImGuiEx::SliderF("Growth Delay Player", &Config::Gameplay.GamemodePlayer.fRandomGrowthDelay, 0.00f, 4.0f, T1, Fmt1);
                ImGuiEx::SliderF("Growth Delay Followers", &Config::Gameplay.GamemodeFollower.fRandomGrowthDelay, 0.00f, 4.0f, T1, Fmt2);

                ImGui::Spacing();
            }
        }
    }

    void CategoryGameplay::DrawRight() {

        static ImGuiEx::CollapsingTabHeader GameModeSettingsHeader(
            "Alternative Gamemodes",
            {
                "Player",
                "Followers"
            }
        );

        bool shouldDisable = false;
        std::string Reason = "";
        if (Config::Balance.bBalanceMode) {
            shouldDisable = true;
            Reason = "Balance Mode Active";
        }
        else if (!Runtime::HasPerk(PlayerCharacter::GetSingleton(), Runtime::PERK.GTSPerkColossalGrowth)) {
            shouldDisable = true;
            Reason = "Missing Perk: \"Colossal Growth\"";
        }

        {
            GameModeSettingsHeader.SetExtraInfo(Reason);
            GameModeSettingsHeader.SetDisabledState(shouldDisable);

            if (ImGuiEx::BeginCollapsingTabHeader(GameModeSettingsHeader)) {
                // Content based on active tab
                switch (GameModeSettingsHeader.GetActiveTab()) {
                    case 0: GameModeOptions(&Config::Gameplay.GamemodePlayer, true);    break;
                    case 1: GameModeOptions(&Config::Gameplay.GamemodeFollower, false); break;
                    default:                                                            break;
                }
            }

            ImGuiEx::EndCollapsingTabHeader(GameModeSettingsHeader);
        }

    }
}