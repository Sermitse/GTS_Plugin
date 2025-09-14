#include "UI/Categories/General.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "UI/UIManager.hpp"
#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGUI/ImFontManager.hpp"
#include "UI/ImGui/ImUtil.hpp"
#include "Utils/QuestUtil.hpp"
#include "Config/SettingsModHandler.hpp"

namespace {

	const char* T_Export = "Export the current mod configuration to a file.";
	const char* T_Import = "Import the current mod configuration from the selected export file.";
	const char* T_Delete = "Delete the currently selected settings export.";
	const char* T_Cleanup = "Delete all but the 5 most recent exports.";
	const char* T_Reset = "Reset all settings, this does not reset any modified action keybinds.\n"
					      "If you want to reset those press the reset button on its page instead.";

	void DrawIEUI(){

		static std::string statusText = "";
		static int selectedExportIndex = -1;
		constexpr int keepCount = 5;

		// Get available export files
		auto exportFiles = GTS::Config::GetExportedFiles();
		std::vector<std::string> fileNames;
		for (const auto& file : exportFiles) {
			fileNames.push_back(file.filename().string());
		}

		// Export section
		if (ImUtil::ImageButton("##Export","export_save", 32, T_Export)) {
			if (GTS::Config::ExportSettings()) {
				auto files = GTS::Config::GetExportedFiles();
				if (!files.empty()) {
					statusText = fmt::format("✓ Saved as {}", files.front().filename().string());
				}
			}
		}

		ImGui::SameLine();

		if (ImUtil::ImageButton("##Import", "export_load", 32, T_Import)) {
			if(selectedExportIndex < 0) {
				statusText = fmt::format("Select an export first", fileNames[selectedExportIndex]);
			}
			else if (selectedExportIndex >= 0 && selectedExportIndex < exportFiles.size()) {
				if (GTS::Config::LoadFromExport(exportFiles[selectedExportIndex].string())) {
					GTS::HandleSettingsRefresh();
					statusText = fmt::format("✓ Applied {}", fileNames[selectedExportIndex]);
				}
				else {
					statusText = "Import failed";
				}
			}
		}

		ImGui::SameLine();

		if (ImUtil::ImageButton("##Delete", "export_delete", 32, T_Delete)) {

			if (selectedExportIndex < 0) {
				statusText = fmt::format("Select an export first", fileNames[selectedExportIndex]);
			}
			else if (selectedExportIndex >= 0 && selectedExportIndex < exportFiles.size()) {
				GTS::Config::DeleteExport(exportFiles[selectedExportIndex].string());
				statusText = fmt::format("Deleted {}", fileNames[selectedExportIndex]);
				selectedExportIndex = -1;
			}
		}

		ImGui::SameLine();

		if (ImUtil::ImageButton("##Cleaunup", "export_cleanup", 32, T_Cleanup)) {
			GTS::Config::CleanOldExports(keepCount);
			statusText = fmt::format("✓ Removed old exports [Kept {0} most recent]", keepCount);
		}

		ImUtil::SeperatorV();

		if (ImUtil::ImageButton("##Reset", "generic_reset", 32, T_Reset)) {
			GTS::HandleSettingsReset();
			statusText = fmt::format("✓ Mod settings have been reset");
		}

		// Combo box for selecting exports
		const char* previewValue = (selectedExportIndex >= 0 && selectedExportIndex < fileNames.size()) ? fileNames[selectedExportIndex].c_str() : "Select export file...";

		if (ImGui::BeginCombo("##ExportCombo", previewValue)) {
			for (int i = 0; i < fileNames.size(); i++) {
				bool isSelected = (selectedExportIndex == i);
				if (ImGui::Selectable(fileNames[i].c_str(), isSelected)) {
					selectedExportIndex = i;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::Spacing();

		// Status message
		if (!statusText.empty()) {
			GTS::ImFontManager::PushActiveFont(GTS::ImFontManager::ActiveFontType::kLargeText);
			ImGui::TextColored(ImUtil::ColorMessage, statusText.c_str());
			GTS::ImFontManager::PopActiveFont();
		}
	}
}



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

				ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kWidgetTitle);

				ImGui::Text("Animations Installed: ");
				if (WorkingAnims) {
					ImGui::SameLine(0,1);
					ImGui::TextColored(ImUtil::ColorOK,"Yes");
				}
				else {
					ImGui::SameLine(0);
					ImGui::TextColored(ImUtil::ColorError, "No");
				}

				ImFontManager::PopActiveFont();

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

		//----------- Settings Export

		ImUtil_Unique {
			if (ImGui::CollapsingHeader("Export/Import Settings", ImUtil::HeaderFlagsDefaultOpen)) {
				DrawIEUI();
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
			const char* T3 = "Changes Size-Related Movement Speed calculation\n"
                            "- from: 1.0 / animation slowdown\n"
                            "- to:   1.0 * (size * anim slowdown)\n\n"
                            "As a result, movement speed will be faster (which isn't always good)\n"
                            "But it should drastically reduce or even fix ice-skating effect";


	        if (ImGui::CollapsingHeader("Experimental")) {
	            ImUtil::CheckBox("Allow Male Actors", &Settings.bEnableMales, T0);
				ImUtil::CheckBox("Apply Size Effects to all Actors", &Settings.bAllActorSizeEffects, T1);
				ImUtil::CheckBox("Override Item/NPC Interaction Range", &Settings.bOverrideInteractionDist, T2);
				ImUtil::CheckBox("Alternative Movement Speed",&Settings.bAlternativeSpeedFormula, T3);


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

	            ImUtil::CheckBox("Height Adjustment", &Settings.bEnableHighHeels, T0);

				ImGui::SameLine();

	        	if (ImUtil::CheckBox("Disable With Furniture", &Settings.bHighheelsFurniture, T1, !Settings.bEnableHighHeels)){

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

				ImGui::Spacing();

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


		//------------- Gravity

		ImUtil_Unique {

			const char* T0 = "Enables or Disables gravity acceleration based on size \n"
							"- If enabled, gravity will slightly increase as the player grows: 1.0 * sqrt(size)\n"
							"  (This means large player falls faster, but not too fast)\n"
							"- If disabled, gravity stays constant at 1.0\n"
							"- This option is player exclusive.";

			const char* T1 = "Some animations have bad event timings during jump lands\n"
							"- If anim timings are bad = then damage zones may spawn in air, not hitting anyone\n"
							"- Use this slider to adjust delay, 0 = no delay, 1 = 1sec delay on jump land";

			const char* T2 = "Adjusts extra jump land delay when 'Affect Player Gravity' is on\n"
							"- It may be needed because animation may not have enough time to do foot events on ground\n"
							"- Which will lead to not dealing any damage to enemies on the ground\n"
							"- Acts as additional value on top of original jump land delay\n\n"
							"- This value is further multiplied by Gravity Power.";

			if (ImGui::CollapsingHeader("Jumping", ImUtil::HeaderFlagsDefaultOpen)) {
				ImUtil::CheckBox("Affect Player Gravity", &Settings.bAlterPlayerGravity, T0);
				ImUtil::SliderF("Damage Effect Delay - Gravity", &Settings.fAdditionalJumpEffectDelay_Gravity, 0.0f, 1.0f, T2, "%.2fs", !Settings.bAlterPlayerGravity);
				ImUtil::SliderF("Damage Effect Delay", &Settings.fAdditionalJumpEffectDelay, 0.0f, 1.0f, T1, "%.2fs");

				ImGui::Spacing();
			}
		}


		//----------- Progress

		ImUtil_Unique{

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

				ImGui::Spacing();
			}
		}
	}
}
