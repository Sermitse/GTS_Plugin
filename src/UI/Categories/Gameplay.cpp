#include "UI/Categories/Gameplay.hpp"
#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/ImUtil.hpp"

namespace {

    const std::string CollossalGrowthPerk = "GTSPerkColossalGrowth"; //AKA GtsTotalSizeControl
    const std::string PleasurableGrowthPerk = "GTSPerkRandomGrowth"; //AKA GtsCrushGrowthAug
    const std::string CrushGrowthAugmetationPerk = "GTSPerkGrowthDesire";
    const std::string GrowOnHitPerk = "GTSPerkHitGrowth";
}


namespace GTS {

    void CategoryGameplay::GameModeOptions(const char* a_title, GameplayActorSettings* a_Settings, bool a_DefaultOpen) {

        const char* T0 = "Select the game mode\n\n"
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

        const char* T1 = "Modify the amount grown each tick.";
        const char* T2 = "Modify the amount shrunk each tick.";

        const char* T3 = "Set the maximum size for the \"Curse of Growth\" game mode";

        const char* T4 = "Multiply the size-gain rate by 25%% of your current size.\n"
            "The amount gained caps out at 10x scale.";

        const char* T5 = "Set the target scale to grow/shrink to when using the following game modes:\n\n"
            "- Curse of the Giantess\n"
            "- Curse of the Diminishing\n"
            "- Size Locked";

        const char* T6 = "Change how often the curse effects should be applied.\n"
            "The value you select is offset by +/- 10%% each time.";


        bool HasPerk = Runtime::HasPerk(PlayerCharacter::GetSingleton(), CollossalGrowthPerk);

        const char* Reason;
        if (Config::GetBalance().bBalanceMode) {
            Reason = "Balance Mode Active";
            HasPerk = false;
        }
        else {
            Reason = "Requires \"Colossal Growth\" Perk";
        }

        if (ImUtil::ConditionalHeader(a_title, Reason, HasPerk, a_DefaultOpen)) {

            ImUtil::ComboEx<SelectedGameMode>("Game Mode", a_Settings->sGameMode, T0);

            ImGui::BeginDisabled(a_Settings->sGameMode == "kNone");

            ImGui::Spacing();
            ImGui::Text("Basic Game Modes");
            ImUtil::SliderF("Growth Rate", &a_Settings->fGrowthRate, 0.001f, 0.2f, T1, "%.3fx");
            ImUtil::SliderF("Shrink Rate", &a_Settings->fShrinkRate, 0.001f, 0.2f, T2, "%.3fx");

            ImUtil::CheckBox("Multiply Rates", &a_Settings->bMultiplyGrowthrate, T4);

            ImGui::Spacing();
            ImGui::Text("Curse Game Modes");
            ImUtil::SliderF("Curse Update Interval", &a_Settings->fGameModeUpdateInterval, 1.0f, 60.0f, T6, "Every %.2f Seconds");
            ImUtil::SliderF("Curse of Growth Limit", &a_Settings->fCurseGrowthSizeLimit, 1.1f, 50.0f, T3, "%.2fx");
        	ImUtil::SliderF("Target Scale", &a_Settings->fCurseTargetScale, 1.0f, 5.0f, T5, "%.2fx");

            ImGui::EndDisabled();
            ImGui::Spacing();
        }
    }

    void CategoryGameplay::DrawLeft() {


        //----- Perk Settings

        ImUtil_Unique {

            if (ImGui::CollapsingHeader("Perk Settings", ImUtil::HeaderFlagsDefaultOpen)) {

                const bool PerkCondCrush = Runtime::HasPerk(PlayerCharacter::GetSingleton(), CrushGrowthAugmetationPerk);
                const bool PerkCondHit = Runtime::HasPerk(PlayerCharacter::GetSingleton(), GrowOnHitPerk);
                const bool PerkCondAtribCap = Runtime::HasPerk(PlayerCharacter::GetSingleton(), CrushGrowthAugmetationPerk);;

                const char* T1 = "Upon crushing someone: Grow in size.\n"
                                 "Applies to both the player and followers.";

                const char* T2 = "On being hit: Grow in size.\n"
                                 "Applies to both the player and followers.";

                const char* T3 = "By default, the maximum amount of bonus attributes per player level is 2.\n"
                                 "You can adjust the multiplier for this here.\n";

                const std::string tGrowOnCrush = fmt::format("Grow On Crush {}", (!PerkCondCrush ? "[Missing Perk]" : ""));
                ImUtil::CheckBox(tGrowOnCrush.c_str(), &Settings.bEnableCrushGrowth, T1, !PerkCondCrush);

                ImGui::SameLine();

                const std::string tGrowOnHit = fmt::format("Grow On Hit {}", (!PerkCondHit ? "[Missing Perk]" : ""));
                ImUtil::CheckBox(tGrowOnHit.c_str(), &Settings.bEnableGrowthOnHit, T2, !PerkCondHit);

                const char* tSizeConfFMT = (!PerkCondAtribCap ? "[Missing Perk]" : "%.2fx");
                ImUtil::SliderF("Size Conversion Attrib. Cap", &Settings.fSizeConvLevelCap, 0.1f, 5.0f, T3, tSizeConfFMT, !PerkCondAtribCap);

                ImGui::Spacing();
            }
        }

    	//----- Armor Stripping

        ImUtil_Unique {
            if (ImGui::CollapsingHeader("Armor Stripping", ImUtil::HeaderFlagsDefaultOpen)) {

                const char* T1 = "Enable/disable the automatic unequipping of clothing/armor when large enough.\n"
                                 "Applies to both the player and followers.\n"
                                 "This system works based on size thresholds.\n"
                                 "When an armor piece is stripped, the size required for the next piece to be removed increases.";

                const char* T2 = "Set the scale threshold at which to start unequipping pieces of clothing/armor.";

                const char* T3 = "Set the scale threshold at which all clothing/armor is unequipped.";

                ImUtil::CheckBox("Enable Armor Strip", &Settings.bClothTearing, T1);
                ImUtil::SliderF("Starting Threshold", &Settings.fClothRipStart, 1.1f, 2.5f, T2, "%.2fx");
                ImUtil::SliderF("Strip All Threshold", &Settings.fClothRipThreshold, Settings.fClothRipStart + 0.1f, 3.5f, T3, "%.2fx",false, true);

                ImGui::Spacing();
            }
        }

    	//----- Size Effects

        ImUtil_Unique{

            if (ImGui::CollapsingHeader("Size Effects")) {

                const char* T1 = "When large enough, footsteps or size-related actions will launch physics-enabled items.";

                const char* T2 = "Apply the physics hit impulse to objects outside of the current cell.\n"
                                 "Beware: This is computationally very heavy. Hence the existence of this toggle.\n"
                                 "If you experience FPS drops, try disabling this.";

                const char* T3 = "Enable effects like dust and camera shake/rumble.";

                ImUtil::CheckBox("Launch Objects", &Settings.bLaunchObjects, T1);
                ImGui::SameLine();
                ImUtil::CheckBox("Enable In All Cells", &Settings.bLaunchAllCells, T2, !Settings.bLaunchObjects);
                ImUtil::CheckBox("Player Dust/Rumble Effects", &Settings.bPlayerAnimEffects, T3);
                ImUtil::CheckBox("Follower Dust/Rumble Effects", &Settings.bNPCAnimEffects, T3);

                ImGui::Spacing();
            }
        }

        //----- Random Growth

        ImUtil_Unique{

            const bool HasPerk = Runtime::HasPerk(PlayerCharacter::GetSingleton(), PleasurableGrowthPerk);
            const bool BalancedMode = Config::GetBalance().bBalanceMode;
        	const char* Reason = "Requires \"Pleasurable Growth\" Perk";

            if (ImUtil::ConditionalHeader("Random Growth", Reason, HasPerk)) {

                const char* T1 = "Change how often the random growth should trigger.\n"
                                 "Lower values = More often.\n"
                                 "Higher values = Less often.\n"
                                 "Set to 0.0 to disable it entirely.\n"
								 "Note: If Balance Mode is enabled the multiplier is locked to 1.0x. It can still be disabled however by setting it to 0.0";

                const char* THelp = "Random Growth chance is decreased if your size is > than x1.5, up to 4 times decreased chance\n"
                                    "If you have 'Breaching Growth' perk, random growth has a chance to start 'Large Growth' animation";

                const char* FmtBalance = BalancedMode ? "Balance Mode (1.0x)" : "%.2fx";

                const char* Fmt1 = Settings.GamemodePlayer.fRandomGrowthDelay != 0.0f ? FmtBalance : "Disabled";
                const char* Fmt2 = Settings.GamemodeFollower.fRandomGrowthDelay != 0.0f ? FmtBalance : "Disabled";

                

                ImGui::TextColored(ImUtil::ColorSubscript,"About Random Growth (?)");
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip(THelp);
                }

                ImUtil::SliderF("Growth Delay Player", &Settings.GamemodePlayer.fRandomGrowthDelay, 0.00f, 4.0f, T1, Fmt1);
                ImUtil::SliderF("Growth Delay Followers", &Settings.GamemodeFollower.fRandomGrowthDelay, 0.00f, 4.0f, T1, Fmt2);

                ImGui::Spacing();
            }
        }
    }

    void CategoryGameplay::DrawRight() {

    	//----------- Game Modes
        ImUtil_Unique{
            GameModeOptions("Gamemode Player", &Settings.GamemodePlayer, true);
        }

        ImUtil_Unique{
            GameModeOptions("Gamemode Followers", &Settings.GamemodeFollower, false);
        }
    }
}