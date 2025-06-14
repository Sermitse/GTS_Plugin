#include "Actions.hpp"
#include "UI/Categories/General.hpp"
#include "UI/DearImGui/imgui.h"
#include "UI/ImGui/ImUtil.hpp"

namespace GTS {

	void CategoryActions::DrawLeft() {

        // ------- Misc Settings
        
		ImUtil_Unique{

			const char* T0 = "Allow adjustments to the field of view during certain actions (e.g., Second Wind).";
			const char* T1 = "Track biped skeleton bone positions during certain animated actions.";

			if (ImGui::CollapsingHeader("Misc"), ImUtil::HeaderFlagsDefaultOpen) {
				ImUtil::CheckBox("Enable FOV Edits", &SGeneral.bEnableFOVEdits, T0);
				if (ImUtil::CheckBox("Track Bones During Actions", &SGeneral.bTrackBonesDuringAnim, T1)) {
					if (!SGeneral.bTrackBonesDuringAnim) {
						auto actors = find_actors();
						for (auto actor : actors) {
							if (actor) {
								ResetCameraTracking(actor);
							}
						}
					}
				}
			}
		}

		//------ Visuals

        ImUtil_Unique{

            const char* T0 = "Show heart particle effects during certain actions.";
            const char* T1 = "Show or hide icons above NPCs indicating which GTS actions can be performed on them.";

            if (ImGui::CollapsingHeader("Visuals", ImUtil::HeaderFlagsDefaultOpen)) {
                ImUtil::CheckBox("Heart Effects",&SGeneral.bShowHearts, T0);
                ImGui::SameLine();
                ImUtil::CheckBox("Show Action Icons",&SGeneral.bShowIcons, T1);
                ImGui::Spacing();
            }
        }

		//----- Sneak Animations

        ImUtil_Unique{

            const char* T1 = "Replace sneaking with crawling for the player only.\n(Save specific setting)\n"
							 "Note: If sneak/crawl transitions are off you wont automatically switch between crawl/sneak states.";

        	const char* T2 = "Replace sneaking with crawling for followers.\n(Save specific setting)\n"
							 "Note: If sneak / crawl transitions are off you wont automatically switch between crawl / sneak states.";

            const char* T3 = "This mod introduces new subtle transition animations when entering/exiting sneak or crawl states.\n"
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

                ImUtil::CheckBox("Player Crawling", &Persi.EnableCrawlPlayer.value, T1, PlayerBusy);
                ImGui::SameLine();
                ImUtil::CheckBox("Follower Crawling", &Persi.EnableCrawlFollower.value, T2, FollowersBusy);

                ImUtil::CheckBox("Player Sneak Transitions", &SGameplay.ActionSettings.bSneakTransitions, T3);
                ImGui::SameLine();
                ImUtil::CheckBox("Follower Sneak Transitions", &SGameplay.ActionSettings.bSneakTransitionsOther, T3);

                ImGui::Spacing();
            }
        }

        ImUtil_Unique {

        	const char* T0 = "Increase/lower the chance to start a foot grinding animation when doing understomps.";

        	const char* T1 = "When enabled:\n"
        					 "Replaces the light, non understomp stomp animations made by SonderBain with different\n"
        					 "versions made by NickNack.";

            const char* T3 = "Toggle whether actions like kicks ragdoll the player, if done by followers";
           

           if (ImGui::CollapsingHeader("Stomps/Kicks", ImUtil::HeaderFlagsDefaultOpen)) {

               ImUtil::SliderF("Foot Grind On Understomp Chance", &SGameplay.ActionSettings.fPlayerUnderstompGrindChance, 0.0f, 100.0f, T0, "%.0f%%");
               ImUtil::CheckBox("Alternative Stomp Player", &SGameplay.ActionSettings.bStompAlternative, T1);
               ImGui::SameLine();
               ImUtil::CheckBox("Alternative Stomp NPCs", &SGameplay.ActionSettings.bStomAlternativeOther, T1);
               ImUtil::CheckBox("Follower Kicks Affect Player", &SGameplay.ActionSettings.bEnablePlayerPushBack, T3);
               ImGui::Spacing();
           }
        }


	}

	void CategoryActions::DrawRight() {



        ImUtil_Unique{

			const char* T1 = "Modify the amount of growth gained after vore.";
            const char* T2 = "Enable Skyrim's free camera when doing any vore actions.";
            const char* T3 = "Increase vanilla character weight after vore.";
            const char* T4 = "Allow voring insects.";
            const char* T5 = "Allow voring undead actors (like draugr).";
            const char* T6 = "If devourment compatibility is enabled.\nToggle whether the GTS should do DV's Endo on the player and teammates instead of doing lethal vore.";

            if (ImGui::CollapsingHeader("Vore Settings", ImUtil::HeaderFlagsDefaultOpen)) {

                ImUtil::SliderF("Vore Gain Mult", &SGameplay.ActionSettings.fVoreGainMult, 0.1f, 3.0f, T1, "%.1fx");
                ImUtil::CheckBox("Allow Insects", &SGameplay.ActionSettings.bAllowInsects, T4);
                ImGui::SameLine();
                ImUtil::CheckBox("Allow Undead", &SGameplay.ActionSettings.bAllowUndead, T5);

            	ImUtil::CheckBox("Enable FreeCam During Vore", &SGameplay.ActionSettings.bVoreFreecam, T2);
                ImUtil::CheckBox("Increase Character Weight After Vore", &SGameplay.ActionSettings.bVoreWeightGain, T3);

                ImUtil::CheckBox("Endo Vore Followers/Player", &SGameplay.ActionSettings.bDVDoEndoOnTeam, T6);

                ImGui::Spacing();
            }
        }

        ImUtil_Unique{

			const char* T1 = "Toggle whether non lethal hug actions\n"
        					 "like Hug-Heal or Hug-Shrink should start combat.";

			const char* T2 = "Toggle whether after hug healing to full HP\n"
        					 "The held actor should be let go.";

            if (ImGui::CollapsingHeader("Hug Settings", ImUtil::HeaderFlagsDefaultOpen)) {

                ImUtil::CheckBox("Non Lethal Hugs Are Hostile", &SGameplay.ActionSettings.bNonLethalHugsHostile, T1);
                ImUtil::CheckBox("Hug Heal Stops At Full HP (Player & Followers)", &SGameplay.ActionSettings.bHugsStopAtFullHP, T2);

            	ImGui::Spacing();
            }
        }

        ImUtil_Unique{

            if (ImGui::CollapsingHeader("Cleavage Offsets", ImUtil::HeaderFlagsDefaultOpen)) {

                const char* T1 = "Modify the placement of actors during cleavage actions.";

                ImUtil::SliderF("Forward/Back", &SGameplay.ActionSettings.f2CleavageOffset.at(1), -15.0f, 15.0f, T1, "%.2f");
                ImUtil::SliderF("Up/Down", &SGameplay.ActionSettings.f2CleavageOffset.at(0), -15.0f, 15.0f, T1, "%.2f");

                ImGui::Spacing();
            }
            
        }

        ImUtil_Unique{

            if (ImGui::CollapsingHeader("Grab-Play: Kiss Vore Offsets", ImUtil::HeaderFlagsDefaultOpen)) {

                const char* T1 = "Modify the placement of actors during Kiss Vore.\n"
                "Offset is affected by size difference.";

                ImUtil::SliderF("Up/Down", &SGameplay.ActionSettings.fGrabPlayVoreOffset_Z, -15.0f, 15.0f, T1, "%.2f");

                ImGui::Spacing();
            }
            
        }
	}
}