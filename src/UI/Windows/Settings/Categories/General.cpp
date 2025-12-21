#include "UI/Windows/Settings/Categories/General.hpp"

#include "UI/Core/ImUtil.hpp"
#include "UI/Lib/imgui.h"

#include "UI/Controls/Button.hpp"
#include "UI/Controls/CheckBox.hpp"
#include "UI/Controls/Misc.hpp"
#include "UI/Controls/Slider.hpp"
#include "UI/Controls/ToolTip.hpp"
#include "UI/Core/ImFontManager.hpp"
#include "UI/GTSMenu.hpp"

#include "Config/Config.hpp"
#include "Config/ConfigModHandler.hpp"

#include "Utils/QuestUtil.hpp"
#include "Managers/Animation/AnimationManager.hpp"

namespace {

	PSString T_Export = "Export the current mod configuration to a file.";
	PSString T_Import = "Import the current mod configuration from the selected export file.";
	PSString T_Delete = "Delete the currently selected settings export.";
	PSString T_Cleanup = "Delete all but the 5 most recent exports.";

	PSString T_Reset = "Reset all settings, this does not reset any modified action keybinds.\n"
	                   "If you want to reset those press the reset button on its page instead.";

	PSString T_PandoraWarn = "WARNING: Pandora is not fully compatible with nemesis based behavior mods\n"
		                     "You WILL experience odd/delayed/bugged animations If you continue to use pandora with this mod.\n";

	void DrawImportExport(){

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
		if (ImGuiEx::ImageButton("##Export", ImageList::Export_Save, 32, T_Export)) {
			if (GTS::Config::ExportSettings()) {
				auto files = GTS::Config::GetExportedFiles();
				if (!files.empty()) {
					statusText = fmt::format("✓ Saved as {}", files.front().filename().string());
				}
			}
		}

		ImGui::SameLine();

		if (ImGuiEx::ImageButton("##Import", ImageList::Export_Load, 32, T_Import)) {
			if(selectedExportIndex < 0) {
				statusText = fmt::format("Select an export first", fileNames[selectedExportIndex]);
			}
			else if (selectedExportIndex >= 0 && selectedExportIndex < exportFiles.size()) {
				if (GTS::Config::LoadFromExport(exportFiles[selectedExportIndex].string())) {
					GTS::EventDispatcher::DoConfigRefreshEvent();
					statusText = fmt::format("✓ Applied {}", fileNames[selectedExportIndex]);
				}
				else {
					statusText = "Import failed";
				}
			}
		}

		ImGui::SameLine();

		if (ImGuiEx::ImageButton("##Delete", ImageList::Export_Delete, 32, T_Delete)) {

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

		if (ImGuiEx::ImageButton("##Cleaunup", ImageList::Export_Cleanup, 32, T_Cleanup)) {
			GTS::Config::CleanOldExports(keepCount);
			statusText = fmt::format("✓ Removed old exports [Kept {0} most recent]", keepCount);
		}

		ImGuiEx::SeperatorV();

		if (ImGuiEx::ImageButton("##Reset", ImageList::Generic_Reset, 32, T_Reset)) {
			GTS::EventDispatcher::DoConfigResetEvent();
			statusText = fmt::format("✓ Mod settings have been reset");
		}

		// Combo box for selecting exports
		const char* previewValue = (
			selectedExportIndex >= 0 && selectedExportIndex < fileNames.size()) ?
			fileNames[selectedExportIndex].c_str() :
			"Select export file...";

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
			GTS::ImFontManager::Push(GTS::ImFontManager::ActiveFontType::kLargeText);
			ImGui::TextColored(ImUtil::Colors::Message, statusText.c_str());
			GTS::ImFontManager::Pop();
		}
	}
}

namespace GTS {

	CategoryGeneral::CategoryGeneral() {
		m_name = "General";
	}

	void CategoryGeneral::DrawLeft() {

		// ----- Animation Check

		ImUtil_Unique {

			PSString T0 = "The automatic check can sometimes be unreliable.\n"
						  "By pressing this you can forcefully try to play an animation.\n"
						  "It is highly recommended to stand still on the ground when using this.\n\n"
						  "A messagebox should appear stating wether the animation was successfully played or not.";

			if (ImGui::CollapsingHeader("Animations Check", ImUtil::HeaderFlagsDefaultOpen)) {
				const auto Player = PlayerCharacter::GetSingleton();
				const bool WorkingAnims = AnimationsInstalled(Player);

				bool IsPandoraGenerated = false;
				bool IsNemesisGenerated = false;
				Player->GetGraphVariableBool("bIsPandoraGenerated", IsPandoraGenerated);
				Player->GetGraphVariableBool("bIsNemesisGenerated", IsNemesisGenerated);

				ImFontManager::Push(ImFontManager::ActiveFontType::kWidgetTitle);

				ImGui::Text("Animations Installed: ");
				if (WorkingAnims && IsPandoraGenerated) {
					ImGui::SameLine(0);
					ImGui::TextColored(ImUtil::Colors::Message, "Yes (Warning)");
				}
				else if (WorkingAnims) {
					ImGui::SameLine(0,1);
					ImGui::TextColored(ImUtil::Colors::OK,"Yes");
				}
				else {
					ImGui::SameLine(0);
					ImGui::TextColored(ImUtil::Colors::Error, "No");
				}

				ImFontManager::Pop();

				if (IsPandoraGenerated) {
					ImGuiEx::Tooltip(T_PandoraWarn, true);
				}

				ImGui::Text("Behavior Generator: ");
				if (IsPandoraGenerated) {
					ImGui::SameLine(0);
					ImGui::TextColored(ImUtil::Colors::Error, "Pandora");
					ImGuiEx::Tooltip(T_PandoraWarn, true);
				}
				else if (IsNemesisGenerated) {
					ImGui::SameLine(0);
					ImGui::TextColored(ImUtil::Colors::OK, "Nemesis");
				}

				else {
					ImGui::SameLine(0);
					ImGui::TextColored(ImUtil::Colors::Error, "None/Other (WARNING)");
				}

				if (ImGuiEx::Button("Manualy Test Animations", T0)) {

					//Simulate Closing the menu
					GTSMenu::CloseInputConsumers();

					TaskManager::Run("AnimTestTask", [=](auto& progressData) {

						if (progressData.runtime > 0.2) {

							AnimationManager::StartAnim("StrongStompRight", Player);

							if (progressData.runtime > 0.5) {

								if (IsGtsBusy(Player)) {
									PrintMessageBox("Animations should be working.");
								}
								else {
									PrintMessageBox("Animation did not start.");
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
				DrawImportExport();
			}
		}

		//----------- Skill Tree

		ImUtil_Unique {

			PSString T0 = "Open this mod's custom skill tree";

			if (ImGui::CollapsingHeader("Skill Tree", ImUtil::HeaderFlagsDefaultOpen)) {
				if (ImGuiEx::Button("Open Skill Tree",T0)) {
					GTSMenu::CloseInputConsumers();
					Runtime::SetFloat(Runtime::GLOB.GTSSkillMenu, 1.0);
				}

				ImGui::Spacing();
			}
		}

		//------ Protect Actors

		ImUtil_Unique {

			PSString T0 = "Protect essential NPCs from being crushed, eaten, or affected by size-related spells/actions.";
			PSString T1 = "Protect followers from being crushed, eaten, or affected by size-related spells/actions.";

			if (ImGui::CollapsingHeader("Protect Actors", ImUtil::HeaderFlagsDefaultOpen)) {
				ImGuiEx::CheckBox("Protect Essential NPCs",&Config::General.bProtectEssentials, T0);
				ImGui::SameLine();
				ImGuiEx::CheckBox("Protect Followers",&Config::General.bProtectFollowers, T1);
				ImGui::Spacing();
			}
	}
		//------ Compatibility

		ImUtil_Unique {

			PSString T0 = "Enable or disable experimental compatibility with the Devourment mod.\n"
							 "This compatibility toggle may lead to issues such as actors being swallowed with delay (because Papyrus lags) or other bugs\n\n"
							 "If enabled, when this mod's vore actions are completed, the NPC will be delegated to the Devourment mod.";

			PSString T1 = "Enable or disable compatibility with the Alternate Conversation Camera mod.\n"
							 "If enabled, this mod's camera offsets during dialogue will be disabled.";

			if (ImGui::CollapsingHeader("Compatibility", ImUtil::HeaderFlagsDefaultOpen)) {
				ImGuiEx::CheckBox("Devourment Compatibility",&Config::General.bDevourmentCompat, T0, !Runtime::IsDevourmentInstalled());
				ImGuiEx::CheckBox("Alt Conversation Cam. Compatibility", &Config::General.bConversationCamCompat, T1, !Runtime::IsAltConversationCamInstalled());
				ImGui::Spacing();

			}
		}

		//------ Experimental

	    ImUtil_Unique {

	        PSString T0 = "Male Actor Support:\n"
	                         "No support is provided for this feature.\n"
	                         "This mod was primarily designed with female NPCs in mind\n"
	                         "and always assumes the player/followers are female.\n"
	                         "Animations may not look good and could cause issues even.\n"
	                         "Use at your own risk.";

			PSString T1 = "Apply computationally expensive damage calculations to all NPC's in the scene.\n"
	    	              "This toggle can be EXTREMELY fps hungry, it is highly recommended to keep it off.";
			
			PSString T2 = "Enable or disable dynamic alteration of fActivatePickLength and fActivatePickRadius ini\n"
	    	              "It will be altered from 180 and 18 (default) to 180 and 18 * Player Scale";



	        if (ImGui::CollapsingHeader("Experimental")) {
				ImGuiEx::CheckBox("Allow Male Actors", &Config::General.bEnableMales, T0);
				ImGuiEx::CheckBox("Apply Size Effects to all Actors", &Config::General.bAllActorSizeEffects, T1);
				ImGuiEx::CheckBox("Override Item/NPC Interaction Range", &Config::General.bOverrideInteractionDist, T2);

	        	ImGui::Spacing();
	        }
	    }
	}

	void CategoryGeneral::DrawRight() {


		//----- Misc

	    ImUtil_Unique {

	        PSString T0 = "This toggle enables automatic size adjustment:\n"
	                         "If the player or their followers are too large to fit within a room, they will be temporarily scaled down to roughly 90%% of the room's current height.\n"
	                         "Once outside the small room, they will regrow to their previous size.";

	        PSString T1 = "Adjust the speed of all animations based on an actor's scale.";
			PSString T2 = "Reduce the amount of gore in some sound and visual effects.";

			PSString T3 = "Temporarily shrink grow the target actor to fit the target furniture";

			PSString T4 = "Changes Size-Related Movement Speed calculation\n"
						  "- from: 1.0 / animation slowdown\n"
						  "- to:   1.0 * (size * anim slowdown)\n\n"
						  "As a result, movement speed will be faster (which isn't always good)\n"
						  "But it should drastically reduce or even fix ice-skating effect";

			PSString T5 = "Prevent NPCs from sprinting when they are above this size.";

	        if (ImGui::CollapsingHeader("Miscellaneous", ImUtil::HeaderFlagsDefaultOpen)) {

				ImGuiEx::CheckBox("Dynamic Size (Player)", &Config::General.bDynamicSizePlayer, T0);
				ImGui::SameLine();
				ImGuiEx::CheckBox("Dynamic Size (Followers)", &Config::General.bDynamicSizeFollowers, T0);

				ImGuiEx::CheckBox("Scale to furniture (Player)", &Config::General.bDynamicFurnSizePlayer, T3);
				ImGui::SameLine();
				ImGuiEx::CheckBox("Scale to furniture (Followers)", &Config::General.bDynamicFurnSizeFollowers, T3);

				ImGuiEx::CheckBox("Dynamic Animation Speed", &Config::General.bDynamicAnimspeed, T1);
				ImGuiEx::CheckBox("Less Gore", &Config::General.bLessGore, T2);
				ImGuiEx::CheckBox("Alternative Movement Speed", &Config::General.bAlternativeSpeedFormula, T4);
				ImGuiEx::SliderF("Block NPC Sprinting", &Config::General.fPreventSprintAtScale, 0.5f, 20.0f, T5, "When larger than %.1fx");
	            ImGui::Spacing();
	        }
	    }

		//----- HH

	    ImUtil_Unique {

	        PSString T0 = "Enable height adjustment/correction for actors wearing high heels.";
	        PSString T1 = "Disable HH height adjustments when using furniture to allow other mods to handle it.";

	        if (ImGui::CollapsingHeader("High-Heels", ImUtil::HeaderFlagsDefaultOpen)) {

				ImGuiEx::CheckBox("Height Adjustment", &Config::General.bEnableHighHeels, T0);

				ImGui::SameLine();

	        	if (ImGuiEx::CheckBox("Disable With Furniture", &Config::General.bHighheelsFurniture, T1, !Config::General.bEnableHighHeels)){
	        		ConfigModHandler::DoHighHeelStateReset();
	            }

				ImGui::Spacing();

	        }
	    }


		//------------- Looting

	    ImUtil_Unique {

	        PSString T0 = "Toggle whether actions like vore, shrink to death, or crushing\n"
	                         "should spawn loot piles containing the dead actors' inventory.\n"
	                         "If disabled, the inventory will be automatically transferred to the killer upon death.";

	        if (ImGui::CollapsingHeader("Looting", ImUtil::HeaderFlagsDefaultOpen)) {
				ImGuiEx::CheckBox("Player: Spawn Loot Piles",&Config::General.bPlayerLootpiles, T0);
				ImGui::SameLine();
				ImGuiEx::CheckBox("Followers: Spawn Loot Piles",&Config::General.bFollowerLootpiles, T0);
	            ImGui::Spacing();

	        }
	    }


		//------------- Gravity

		ImUtil_Unique {

			PSString T0 = "Enables or Disables gravity acceleration based on size \n"
							"- If enabled, gravity will slightly increase as the player grows: 1.0 * sqrt(size)\n"
							"  (This means large player falls faster, but not too fast)\n"
							"- If disabled, gravity stays constant at 1.0\n"
							"- This option is player exclusive.";

			PSString T1 = "Some animations have bad event timings during jump lands\n"
							"- If anim timings are bad = then damage zones may spawn in air, not hitting anyone\n"
							"- Use this slider to adjust delay, 0 = no delay, 1 = 1sec delay on jump land";

			PSString T2 = "Adjusts extra jump land delay when 'Affect Player Gravity' is on\n"
							"- It may be needed because animation may not have enough time to do foot events on ground\n"
							"- Which will lead to not dealing any damage to enemies on the ground\n"
							"- Acts as additional value on top of original jump land delay\n\n"
							"- This value is further multiplied by Gravity Power.";

			if (ImGui::CollapsingHeader("Jumping", ImUtil::HeaderFlagsDefaultOpen)) {
				ImGuiEx::CheckBox("Affect Player Gravity", &Config::General.bAlterPlayerGravity, T0);
				ImGuiEx::SliderF("Damage Effect Delay - Gravity", &Config::General.fAdditionalJumpEffectDelay_Gravity, 0.0f, 1.0f, T2, "%.2fs", !Config::General.bAlterPlayerGravity);
				ImGuiEx::SliderF("Damage Effect Delay", &Config::General.fAdditionalJumpEffectDelay, 0.0f, 1.0f, T1, "%.2fs");

				ImGui::Spacing();
			}
		}


		//----------- Progress

		ImUtil_Unique {

			PSString T0 = "Automatically complete this mod's quest.";
			PSString T1 = "Get all of the mod's spells";
			PSString T2 = "Instantly complete the perk tree.";
			PSString T3 = "Get all of the mod's shouts";

			if (ImGui::CollapsingHeader("Skip Progression")) {

				const auto Complete = ProgressionQuestCompleted();

				if (ImGuiEx::Button("Skip Quest",T0, Complete)) {
					SkipProgressionQuest();
				}

				ImGui::SameLine();

				if (ImGuiEx::Button("Get All Spells", T1, !Complete)) {
					GiveAllSpellsToPlayer();
				}

				ImGui::SameLine();

				if (ImGuiEx::Button("Get All Perks",T2, !Complete)) {
					GiveAllPerksToPlayer();
				}

				ImGui::SameLine();

				if (ImGuiEx::Button("Get All Shouts", T3, !Complete)) {
					GiveAllShoutsToPlayer();
				}

				ImGui::Spacing();
			}
		}
	}
}
