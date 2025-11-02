#include "UI/Windows/Settings/Categories/Actions.hpp"

#include "Config/Config.hpp"
#include "Config/ConfigModHandler.hpp"

#include "UI/Controls/CheckBox.hpp"
#include "UI/Controls/Slider.hpp"

#include "UI/Core/ImUtil.hpp"
#include "UI/Lib/imgui.h"

namespace GTS {

	CategoryActions::CategoryActions() {
		m_name = "Actions";
	}

	void CategoryActions::DrawLeft() {

        // ------- Misc Settings
        
		ImUtil_Unique
		{

			PSString T0 = "Allow adjustments to the field of view during certain actions (e.g., Second Wind).";
			PSString T1 = "Track biped skeleton bone positions during certain animated actions.";

			if (ImGui::CollapsingHeader("Misc", ImUtil::HeaderFlagsDefaultOpen)) {

				ImGuiEx::CheckBox("Enable FOV Edits", &Config::General.bEnableFOVEdits, T0);
				if (ImGuiEx::CheckBox("Track Bones During Actions", &Config::General.bTrackBonesDuringAnim, T1)) {
                    ConfigModHandler::DoCameraStateReset();
				}

                ImGui::Spacing();

			}
		}

		//------ Visuals

        ImUtil_Unique
		{

            PSString T0 = "Show heart particle effects during certain actions.";
            PSString T1 = "Show or hide icons above NPCs indicating which GTS actions can be performed on them.";

            if (ImGui::CollapsingHeader("Visuals", ImUtil::HeaderFlagsDefaultOpen)) {
                ImGuiEx::CheckBox("Heart Effects",&Config::General.bShowHearts, T0);
                ImGui::SameLine();
                ImGuiEx::CheckBox("Show Action Icons",&Config::General.bShowIcons, T1);
                ImGui::Spacing();
            }
        }

		//----- Sneak Animations

        ImUtil_Unique
		{

            PSString T1 = "Replace sneaking with crawling for the player only.\n(Save specific setting)\n"
						  "Note: If sneak/crawl transitions are off you wont automatically switch between crawl/sneak states.";

        	PSString T2 = "Replace sneaking with crawling for followers.\n(Save specific setting)\n"
						  "Note: If sneak / crawl transitions are off you wont automatically switch between crawl / sneak states.";

            PSString T3 = "This mod introduces new subtle transition animations when entering/exiting sneak or crawl states.\n"
						  "This toggle disables/enables them.\n";


            if (ImGui::CollapsingHeader("Sneaking/Crawling", ImUtil::HeaderFlagsDefaultOpen)) {
                auto& Persi = Persistent::GetSingleton();

                bool PlayerBusy = IsTransitioning(PlayerCharacter::GetSingleton());
                bool FollowersBusy = false;

                for (const auto& Fol : FindTeammates()) {
                    if (Fol) {
                        if (IsTransitioning(Fol)) {
                            FollowersBusy = true;
                            break;
                        }
                    }
                }

                ImGui::Text("Replace Sneaking With Crawling");
                ImGuiEx::CheckBox("Player##CrawlToggle", &Persi.EnableCrawlPlayer.value, T1, PlayerBusy);
                ImGui::SameLine();
                ImGuiEx::CheckBox("Follower##CrawlToggle", &Persi.EnableCrawlFollower.value, T2, FollowersBusy);

                ImGui::Spacing();

                ImGui::Text("Sneak Transition Animations");
                ImGuiEx::CheckBox("Player##STransToggle", &Config::Gameplay.ActionSettings.bSneakTransitions, T3);
                ImGui::SameLine();
                ImGuiEx::CheckBox("Follower##STransToggle", &Config::Gameplay.ActionSettings.bSneakTransitionsOther, T3);

                ImGui::Spacing();
            }
        }

        //----- Stomps/Kicks

        ImUtil_Unique
		{

        	PSString T0 = "Increase/lower the chance to start a foot grinding animation when doing understomps.";

        	PSString T1 = "When enabled:\n"
        				  "Replaces the light, non understomp stomp animations made by SonderBain with different\n"
        				  "versions made by NickNack.";

            PSString T3 = "Toggle whether actions like kicks ragdoll the player, if done by followers";
           

			if (ImGui::CollapsingHeader("Stomps/Kicks", ImUtil::HeaderFlagsDefaultOpen)) {

				ImGuiEx::SliderF("Foot Grind On Understomp Chance", &Config::Gameplay.ActionSettings.fPlayerUnderstompGrindChance, 0.0f, 100.0f, T0, "%.0f%%");
                ImGuiEx::CheckBox("Alternative Stomp Player", &Config::Gameplay.ActionSettings.bStompAlternative, T1);
			    ImGui::SameLine();
                ImGuiEx::CheckBox("Alternative Stomp NPCs", &Config::Gameplay.ActionSettings.bStomAlternativeOther, T1);
                ImGuiEx::CheckBox("Follower Kicks Affect Player", &Config::Gameplay.ActionSettings.bEnablePlayerPushBack, T3);
			    ImGui::Spacing();
			}
        }
	}

	void CategoryActions::DrawRight() {

        //----- Vore Settings

        ImUtil_Unique
		{

			PSString T1 = "Modify the amount of growth gained after vore.";
            PSString T2 = "Enable Skyrim's free camera when doing any vore actions.";
            PSString T3 = "Increase vanilla character weight after vore.";
            PSString T4 = "Allow voring insects.";
            PSString T5 = "Allow voring undead actors (like draugr).";

            PSString T6 = "Toggle whether the GTS should do DV's Endo on the player and teammates instead of doing lethal vore\n"
			              "if devourment compatibility is enabled.\n";
						  

            if (ImGui::CollapsingHeader("Vore Settings", ImUtil::HeaderFlagsDefaultOpen)) {

                ImGuiEx::SliderF("Vore Gain Mult", &Config::Gameplay.ActionSettings.fVoreGainMult, 0.1f, 3.0f, T1, "%.1fx");
                ImGuiEx::CheckBox("Allow Insects", &Config::Gameplay.ActionSettings.bAllowInsects, T4);
                ImGui::SameLine();
                ImGuiEx::CheckBox("Allow Undead", &Config::Gameplay.ActionSettings.bAllowUndead, T5);
                ImGuiEx::CheckBox("Enable FreeCam During Vore", &Config::Gameplay.ActionSettings.bVoreFreecam, T2);
                ImGuiEx::CheckBox("Increase Character Weight After Vore", &Config::Gameplay.ActionSettings.bVoreWeightGain, T3);
                ImGuiEx::CheckBox("Endo Vore Followers/Player", &Config::Gameplay.ActionSettings.bDVDoEndoOnTeam, T6);

                ImGui::Spacing();
            }
        }

		//----- Grab Settings

        ImUtil_Unique
		{

            PSString T1 = "Toggle whether initial grab is hostile\n"
                          "- true = it will start combat on grab\n"
                          "- false = npc won't start combat";
            

            PSString T2 = "Modify the placement of actors during Kiss Vore.\n"
                          "Offset is affected by size difference.";

            if (ImGui::CollapsingHeader("Grab Settings", ImUtil::HeaderFlagsDefaultOpen)) {

                ImGuiEx::CheckBox("Initial Grab is hostile", &Config::Gameplay.ActionSettings.bGrabStartIsHostile, T1);

                ImGui::Spacing();

                ImGui::Text("Grab-Play: Kiss Vore Offsets");
                ImGuiEx::SliderF("Up/Down", &Config::Gameplay.ActionSettings.fGrabPlayVoreOffset_Z, -15.0f, 15.0f, T2, "%.2f");

                ImGui::Spacing();
            }

        }

        //----- Hug Settings

        ImUtil_Unique
		{

            PSString T0 = "Toggle whether non lethal hug actions\n"
			              "like Hug-Heal or Hug-Shrink should start combat.";

            PSString T1 = "Toggle whether after hug healing to full HP\n"
                          "The held actor should be let go.\n"
			              "Applies only to the player/followers.";

            if (ImGui::CollapsingHeader("Hug Settings", ImUtil::HeaderFlagsDefaultOpen)) {

                ImGuiEx::CheckBox("Non Lethal Hugs Are Hostile", &Config::Gameplay.ActionSettings.bNonLethalHugsHostile, T0);
                ImGui::SameLine();
                ImGuiEx::CheckBox("Hug Heal Stops At Full HP", &Config::Gameplay.ActionSettings.bHugsStopAtFullHP, T1);

            	ImGui::Spacing();
            }
        }

        ImUtil_Unique
		{

            if (ImGui::CollapsingHeader("Cleavage Settings", ImUtil::HeaderFlagsDefaultOpen)) {

                PSString T0 = "Modify the placement of actors during cleavage actions.\n"
            	              "Up/Down | Forward/Back";

                ImGuiEx::SliderF2("Placement Offset", &Config::Gameplay.ActionSettings.f2CleavageOffset.at(0), -15.0f, 15.0f, T0, "%.2f");

                ImGui::Spacing();
            }
        }
	}
}