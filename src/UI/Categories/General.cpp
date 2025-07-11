#include "UI/Categories/General.hpp"
#include "Config/Keybinds.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "UI/UIManager.hpp"
#include "UI/DearImGui/imgui.h"
#include "UI/ImGUI/ImFontManager.hpp"
#include "UI/ImGUI/ImStyleManager.hpp"
#include "UI/ImGui/ImUtil.hpp"
#include "Utils/QuestUtil.hpp"


namespace GTS {

	void CategoryGeneral::DrawLeft() {

		// ----- Animation Check

		ImUtil_Unique{

			const char* T0 = "The automatic check can sometimes be unreliable.\n"
							 "By pressing this you can forcefully try to play an animation.\n"
							 "It is highly recommended to stand still on the ground when using this.\n\n"
							 "A messagebox should appear stating wether the animation was successfully played or not.";

			if (ImGui::CollapsingHeader("Animations Check", ImUtil::HeaderFlagsDefaultOpen)) {
				const auto Player = PlayerCharacter::GetSingleton();
				const bool WorkingAnims = AnimationsInstalled(Player);

				ImGui::PushFont(ImFontManager::GetFont("widgettitle"));
				ImGui::Text("Animations Installed: ");
				if (WorkingAnims) {
					ImGui::SameLine(0,1);
					ImGui::TextColored(ImUtil::ColorOK,"Yes");
				}
				else {
					ImGui::SameLine(0);
					ImGui::TextColored(ImUtil::ColorError, "No");
				}

				ImGui::PopFont();

				if (ImUtil::Button("Manualy Test Animations", T0)) {

					UIManager::CloseSettings();

					TaskManager::Run("AnimTestTask", [=](auto& progressData) {

						if (progressData.runtime > 0.2) {

							GTS::AnimationManager::StartAnim("StrongStompRight", Player);

							if (progressData.runtime > 0.5) {

								if (GTS::IsGtsBusy(Player)) {
									RE::DebugMessageBox("Animations are working.");
								}
								else {
									RE::DebugMessageBox("Animations are NOT working.");
								}
								return false;
							}
						}
						return true;
					});
				}
				ImGui::Spacing();
			}
		}

		//------ Protect Actors

		ImUtil_Unique{

			const char* T0 = "Protect essential NPCs from being crushed, eaten, or affected by size-related spells/actions.";
			const char* T1 = "Protect followers from being crushed, eaten, or affected by size-related spells/actions.";

			if (ImGui::CollapsingHeader("Protect Actors", ImUtil::HeaderFlagsDefaultOpen)) {
				ImUtil::CheckBox("Protect Essential NPCs",&Settings.bProtectEssentials, T0);
				ImGui::SameLine();
				ImUtil::CheckBox("Protect Followers",&Settings.bProtectFollowers, T1);
				ImGui::Spacing();
			}
	}
		//------ Compatibility

		ImUtil_Unique{

			const char* T0 = "Enable or disable experimental compatibility with the Devourment mod.\n"
							 "This compatibility toggle may lead to issues such as actors being swallowed with delay (because Papyrus lags) or other bugs\n\n"
							 "If enabled, when this mod's vore actions are completed, the NPC will be delegated to the Devourment mod.";

			const char* T1 = "Enable or disable compatibility with the Alternate Conversation Camera mod.\n"
							 "If enabled, this mod's camera offsets during dialogue will be disabled.";

			if (ImGui::CollapsingHeader("Compatibility", ImUtil::HeaderFlagsDefaultOpen)) {
				ImUtil::CheckBox("Devourment Compatibility",&Settings.bDevourmentCompat, T0);
				ImUtil::CheckBox("Alt Conversation Cam. Compatibility",&Settings.bConversationCamCompat, T1);
				ImGui::Spacing();

			}
	}


		//------ Settings Storage

		ImUtil_Unique{

			const char* T0 = "Should the mod settings be saved globaly or on a per save basis like older versions of the mod?\n"
			"Effectively this mimicks the behavior you'd experience when using a MCM menu.\n";

			if (ImGui::CollapsingHeader("Settings Storage", ImUtil::HeaderFlagsDefaultOpen)) {
				ImUtil::CheckBox("Save Specific Settings", &Persistent::GetSingleton().LocalSettingsEnable.value, T0);
				ImGui::Spacing();
			}

		}

		//------ Experimental

	    ImUtil_Unique {

	        const char* T0 = "Male Actor Support:\n"
	                         "No support is provided for this feature.\n"
	                         "This mod was primarily designed with female NPCs in mind\n"
	                         "and always assumes the player/followers are female.\n"
	                         "Animations may not look good and could cause issues even.\n"
	                         "Use at your own risk.";

			const char* T1 = "Apply computationally expensive damage calculations to all NPC's in the scene.\n"
							 "This toggle can be EXTREMELY fps hungry, it is highly recommended to keep it off.";
			
			const char* T2 = "Enable or disable dynamic alteration of fActivatePickLength and fActivatePickRadius ini\n"
			                 "It will be altered from 180 and 18 (default) to 180 and 18 * Player Scale";
			const char* T3 = "Enables or Disables gravity acceleration based on size \n"
							"- If enabled, gravity will slightly increase as the player grows: 1.0 * sqrt(size)\n"
							"  (This means large player falls faster, but not too fast)\n"
							"- If disabled, gravity stays constant at 1.0\n"
							"- This option is player exclusive.";
			const char* T4 = "Enables or Disables extra effect delay when jump landing\n"
							"It may be needed because with increased gravity player falls down faster\n"
							"Which can lead to effects playing a lot earlier than intended";

	        if (ImGui::CollapsingHeader("Experimental", ImUtil::HeaderFlagsDefaultOpen)) {
	            ImUtil::CheckBox("Allow Male Actors", &Settings.bEnableMales, T0);
				ImUtil::CheckBox("Apply Size Effects to all Actors", &Settings.bAllActorSizeEffects, T1);
				ImUtil::CheckBox("Override Item/NPC Interaction Range", &Settings.bOverrideInteractionDist, T2);
				ImUtil::CheckBox("Affect Player Gravity", &Settings.bAlterPlayerGravity, T3);
				ImUtil::CheckBox("Delay Jump Land Effects", &Settings.bAdditionalJumpDelay, T4, !Settings.bAlterPlayerGravity);

	        	ImGui::Spacing();
	        }
	    }
	}

	void CategoryGeneral::DrawRight() {


		//----- Misc

	    ImUtil_Unique {

	        const char* T0 = "This toggle enables automatic size adjustment:\n"
	                         "If the player or their followers are too large to fit within a room, they will be temporarily scaled down to roughly 90%% of the room's current height.\n"
	                         "Once outside the small room, they will regrow to their previous size.";

	        const char* T1 = "Adjust the speed of all animations based on an actor's scale.";
			const char* T2 = "Reduce the amount of gore in some sound and visual effects.";


	        if (ImGui::CollapsingHeader("Miscellaneous", ImUtil::HeaderFlagsDefaultOpen)) {
	            ImUtil::CheckBox("Dynamic Size Player", &Settings.bDynamicSizePlayer, T0);
				ImGui::SameLine();
	            ImUtil::CheckBox("Dynamic Size Followers", &Settings.bDynamicSizeFollowers, T0);
	            ImUtil::CheckBox("Dynamic Animation Speed", &Settings.bDynamicAnimspeed, T1);
				ImUtil::CheckBox("Less Gore", &Settings.bLessGore, T2);

	            ImGui::Spacing();
	        }
	    }

		//----- HH

	    ImUtil_Unique {

	        const char* T0 = "Enable height adjustment/correction for actors wearing high heels.";
	        const char* T1 = "Disable HH height adjustments when using furniture to allow other mods to handle it.";

	        if (ImGui::CollapsingHeader("High-Heels", ImUtil::HeaderFlagsDefaultOpen)) {

	            ImUtil::CheckBox("High Heels: Enable Height Adjustment", &Settings.bEnableHighHeels, T0);

				ImGui::SameLine();

	        	if (ImUtil::CheckBox("High Heels: Disable When Using Furniture", &Settings.bHighheelsFurniture, T1, !Settings.bEnableHighHeels)){
	            	if (!Settings.bHighheelsFurniture) {

						auto actors = find_actors();

						for (auto actor : actors) {
							if (!actor) {
								return;
							}

							for (bool person : {false, true}) {
								auto npc_root_node = find_node(actor, "NPC", person);
								if (npc_root_node && actor->GetOccupiedFurniture()) {
									npc_root_node->local.translate.z = 0.0f;
									update_node(npc_root_node);
								}
							}
						}
					}
	            }
	        }
	    }


		//------------- Looting

	    ImUtil_Unique {

	        const char* T0 = "Toggle whether actions like vore, shrink to death, or crushing\n"
	                         "should spawn loot piles containing the dead actors' inventory.\n"
	                         "If disabled, the inventory will be automatically transferred to the killer upon death.";

	        if (ImGui::CollapsingHeader("Looting", ImUtil::HeaderFlagsDefaultOpen)) {
	            ImUtil::CheckBox("Player: Spawn Loot Piles",&Settings.bPlayerLootpiles, T0);
				ImGui::SameLine();
	            ImUtil::CheckBox("Followers: Spawn Loot Piles",&Settings.bFollowerLootpiles, T0);
	            ImGui::Spacing();

	        }
	    }

		//----------- Skill Tree

		ImUtil_Unique{

			const char* T0 = "Open this mod's custom skill tree";

			if (ImGui::CollapsingHeader("Skill Tree", ImUtil::HeaderFlagsDefaultOpen)) {
				if (ImUtil::Button("Open Skill Tree",T0)) {
					UIManager::CloseSettings();
					Runtime::SetFloat("GTSSkillMenu", 1.0);
				}

				ImGui::Spacing();
			}
		}

		//----------- Progress

	    ImUtil_Unique {

	        const char* T0 = "Automatically complete this mod's quest.";
			const char* T1 = "Get all of the mod's spells";
	        const char* T2 = "Instantly complete the perk tree.";
			const char* T3 = "Get all of the mod's shouts";

	        if (ImGui::CollapsingHeader("Skip Progression")) {

				const auto Complete = ProgressionQuestCompleted();

	            if (ImUtil::Button("Skip Quest",T0, Complete)) {
					SkipProgressionQuest();
	            }

	            ImGui::SameLine();

				if (ImUtil::Button("Get All Spells", T1, !Complete)) {
					GiveAllSpellsToPlayer();
				} 

	            ImGui::SameLine();

	            if (ImUtil::Button("Get All Perks",T2, !Complete)) {
					GiveAllPerksToPlayer();
	            }

				ImGui::SameLine();

				if (ImUtil::Button("Get All Shouts", T3, !Complete)) {
					GiveAllShoutsToPlayer();
				}
	        }
	    }

		//-------- Settings Reset

		ImUtil_Unique{

			const char* T0 = "Reset this mod's setting do their default values";

			//Reset
			if (ImGui::CollapsingHeader("Reset Settings")) {
				if (ImUtil::Button("Reset Mod Settings", T0)) {

					Config::GetSingleton().ResetToDefaults();
					Keybinds::GetSingleton().ResetKeybinds();
					ImStyleManager::GetSingleton().LoadStyle();
					ImFontManager::GetSingleton().RebuildFonts();

					spdlog::set_level(spdlog::level::from_str(Config::GetAdvanced().sLogLevel));
					spdlog::flush_on(spdlog::level::from_str(Config::GetAdvanced().sFlushLevel));

					// ----- If You need to do something when settings reset add it here.

					if (!Settings.bTrackBonesDuringAnim) {
						auto actors = find_actors();
						for (auto actor : actors) {
							if (actor) {
								ResetCameraTracking(actor);
							}
						}
					}

					if (!Settings.bHighheelsFurniture) {

						auto actors = find_actors();

						for (auto actor : actors) {
							if (!actor) {
								return;
							}

							for (bool person : {false, true}) {
								auto npc_root_node = find_node(actor, "NPC", person);
								if (npc_root_node && actor->GetOccupiedFurniture()) {
									npc_root_node->local.translate.z = 0.0f;
									update_node(npc_root_node);
								}
							}
						}
					}

					Notify("Mod settins have been reset");
					logger::info("All Mod Settings Reset");
				}
			}
		}
	}
}
