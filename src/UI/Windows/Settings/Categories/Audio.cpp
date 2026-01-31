#include "UI/Windows/Settings/Categories/Audio.hpp"

#include "Config/Config.hpp"

#include "UI/Controls/CheckBox.hpp"
#include "UI/Controls/Slider.hpp"
#include "UI/Controls/Text.hpp"
#include "UI/Controls/ToolTip.hpp"
#include "UI/Core/ImUtil.hpp"

using namespace GTS;

namespace {

	void SelectVoiceBank(RE::Actor* a_actor) {

		if (!a_actor) return;

		static constexpr std::array<const char*, 9> EntriesFemale = {
			"GTS Voice",
			"SL F Voice 1",
			"SL F Voice 2",
			"SL F Voice 3",
			"SL F Voice 4",
			"SL F Voice 5",
			"SL F Voice 6",
			"SL F Voice 7",
			"SL F Voice 8"
		};

		if (auto ActorData = Persistent::GetActorData(a_actor)) {
			ImGui::PushID(a_actor);
			int CurrentIndex = ActorData->iVoiceBankIndex;
			if (ImGui::BeginCombo(a_actor->GetName(), EntriesFemale[CurrentIndex])) {
				for (int i = 0; i < EntriesFemale.size(); ++i) {
					const bool IsSelected = (CurrentIndex == i);
					if (ImGui::Selectable(EntriesFemale[i], IsSelected)) {
						ActorData->iVoiceBankIndex = static_cast<uint8_t>(i);
					}
					if (IsSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::PopID();
		}
	}
}

namespace GTS {

	CategoryAudio::CategoryAudio() {
		m_name = "Audio";
	}

	void CategoryAudio::DrawLeft(){

	    ImUtil_Unique 
		{

	        PSString T0 = "Enable footstep sounds when player size meets or exceeds a certain threshold.";
	        PSString T1 = "Enable moaning sounds during spells like Slow Growth.";

			PSString T2 = "Enables size variance for moans/laughs similar to footsteps, but without blendings\n"
	    	              "If disabled, it will always use normal size sounds.\n"
						  "Else picks matching sound category based on current size range:\n"
						  "normal/x2/x4/x8/x12/x24/x48/x96\n\n"
						  "- If you have no custom Moan/Laugh sounds, it does nothing.\n"
						  "- Sounds location: Data\\Sound\\fx\\GTS\\Moans_Laughs. Folders are empty by default.\n"
						  "- Possible usage: put edited variants of sounds with reverb/echo/etc in matching size folders";

			PSString T3 = "If true, only Player Character will be able to moan/laugh";

			PSString T4 = "Replaces the custom High Heel Size sounds past x2 size with different sound sets\n"
	    	              "When not wearing High Heels - plays old sounds.\n"
	    	              "Requires High Heels: 'Enable Height Adjustment' to be ON to work";

			PSString T5 = "If True:\n"
						  "- audio of footsteps will attempt to smoothly swap between each other"
			              "- It can result in footsteps being quieter than without blending\n"
						  "- Because of blending, it can even play 2 sounds at once\n"
						  "If False: \n"
						  "- Footstep audio changes as soon as you pass a size threshold.";

			PSString T6 = "Enable/Disable Laugh Sounds";
			PSString T7 = "Enable/Disable Moan Sounds";

			PSString THelp = "Note: Moan/Laugh sounds are included in the mod if you chose to install them.\n"
							 "You can also add sounds by adding your own .wav files in the following folder:\n"
	    					 "(Your Skyrim Folder)\\Data\\Sound\\fx\\GTS\\Moans_Laughs";
	        
	        if(ImGui::CollapsingHeader("Sounds",ImUtil::HeaderFlagsDefaultOpen)){
				ImGuiEx::HelpText("A Note On Sounds", THelp);

				ImGuiEx::CheckBox("Enable Moans", &Config::Audio.bMoanEnable, T6);
				ImGui::SameLine();
				ImGuiEx::CheckBox("Enable Laughs", &Config::Audio.bLaughEnable, T7);
				ImGuiEx::CheckBox("Footstep Sounds",&Config::Audio.bFootstepSounds,T0);
				ImGuiEx::CheckBox("Moans On Slow Growth",&Config::Audio.bSlowGrowMoans, T1, !Config::Audio.bMoanEnable);

				ImGui::Spacing();

				ImGui::Text("Moans and Laughs");

				ImGui::BeginDisabled(!Config::Audio.bMoanEnable && !Config::Audio.bLaughEnable);
		        {
			        ImGuiEx::CheckBox("Size Variance", &Config::Audio.bMoanLaughSizeVariants, T2);
					ImGui::SameLine();
					ImGuiEx::CheckBox("Player Exclusive", &Config::Audio.bMoanLaughPCExclusive, T3);
		        }
				ImGui::EndDisabled();

				ImGui::Spacing();

				ImGuiEx::CheckBox("Alternative High Heel Size Sounds", &Config::Audio.bUseOtherHighHeelSet, T4);
				ImGuiEx::CheckBox("Smoothly Blend Between Footstep Sounds", &Config::Audio.bBlendBetweenFootsteps, T5);

	            ImGui::Spacing();
	        }
	    }

		if (Runtime::IsSexlabInstalled()) {

			ImUtil_Unique 
			{

				PSString THelp = "Sexlab is installed.\n"
								 "This mod can now use it's voice files as an alternative\n\n"
								 "Note: Only The Player\\Current Followers will be listed as to not clutter this menu.\n"
								 "If this menu is empty it means none of the currently loaded npc's are elidgible for this feature.";
				ImGui::BeginDisabled(!Config::Audio.bMoanEnable);
				if (ImGui::CollapsingHeader("Alternative Voice Options", ImUtil::HeaderFlagsDefaultOpen)) {
					ImGuiEx::HelpText("What is this", THelp);

					static const auto Player = PlayerCharacter::GetSingleton();

					if (IsFemale(Player)){
						SelectVoiceBank(Player);
					}

					const auto ActiveTeammates = FindFemaleTeammates();

					if (!ActiveTeammates.empty()) {
						for (const auto Teammate : ActiveTeammates) {
							SelectVoiceBank(Teammate);
						}
					}

					ImGui::Spacing();
				}
				ImGui::EndDisabled();
			}
		}
	}

	void CategoryAudio::DrawRight() {

		ImUtil_Unique 
		{

			PSString TpLow = "The lowest voice pitch multipler when over 1.0x scale, lower values will lower the pitch when the actor is large(r).";
			PSString TsMax = "Change the target scale at which the pitch will be the lowest.";
			PSString TpHi = "The highest voice pitch multipler when under 1.0x scale, lower values will lower the pitch when the actor is smaller(r).";
			PSString TsMin = "Change the target scale at which the pitch will be highest.";

			PSString T0 = "Toggle wether the NPC voicelines should have their pitch modified based on size.";
			PSString T2 = "Enable/Disable npc's making death sounds/screams when being shrunken to nothing.";
			PSString T3 = "Enable/Disable npc's making death sounds/screams when being absorbed by breasts.";
			PSString T4 = "Enable/Disable npc's making death sounds/screams when killed through Wrathful Calamity.";
			PSString T5 = "Enable/Disable npc's making death sounds/screams when killed through being Hug Crushed.";
			PSString T6 = "Enable/Disable npc's making death sounds/screams when being crushed";
			PSString T7 = "Enable/Disable npc's making death sounds/screams when being eaten through vore.";
			PSString T8 = "FallOff Range Multiplier for Moans and Laughs. Large values = can be heard from further dist";
			PSString T9 = "Moan and Laugh volume multiplier.";
			PSString T10 = "Toggle wether the GTS moan/laugh voice should have its pitch modified based on size.";
			PSString T11 = "Toggle whether gore sounds should have its pitch modified based on tiny size below 1.0";
			
			if (ImGui::CollapsingHeader("Voice",ImUtil::HeaderFlagsDefaultOpen)) {
				ImGuiEx::CheckBox("Normal Voice Pitch Override",&Config::Audio.bEnableVoicePitchOverrideN, T0);
				ImGuiEx::CheckBox("GTS Voice Pitch Override", &Config::Audio.bEnableVoicePitchOverrideG, T10);
				ImGuiEx::CheckBox("Tiny Gore Pitch Override", &Config::Audio.bEnableGorePitchOverride, T11);

				const bool Enable = !Config::Audio.bEnableVoicePitchOverrideN && !Config::Audio.bEnableVoicePitchOverrideG;

				ImGuiEx::SliderF("Lowest Pitch", &Config::Audio.fMinVoiceFreq, 0.65f, 1.0f, TpLow, "%.3fx", Enable);
				ImGuiEx::SliderF("Lowest Pitch At Scale", &Config::Audio.fTargetPitchAtScaleMax, 1.5f, 50.0f, TsMax, "At %.1fx", Enable);
				ImGui::Spacing();
				ImGuiEx::SliderF("Highest Pitch", &Config::Audio.fMaxVoiceFreq, 1.0f, 1.75f, TpHi, "%.3fx", Enable);
				ImGuiEx::SliderF("Highest Pitch At Scale", &Config::Audio.fTargetPitchAtScaleMin, 0.10f, 0.6f, TsMin, "At %.2fx", Enable);

				ImGui::Spacing();
				
				ImGui::Text("Mute Death Sounds");
				ImGuiEx::CheckBox("Shrink To Nothing", &Config::Audio.bMuteShrinkToNothingDeathScreams,T2);
				ImGui::SameLine();
				//Store x-Pos of 2nd element
				auto FirstPos = ImGui::GetCursorPosX();

				ImGuiEx::CheckBox("Breast Absorption", &Config::Audio.bMuteBreastAbsorptionDeathScreams,T3);

				ImGuiEx::CheckBox("Wrathful Calamity", &Config::Audio.bMuteFingerSnapDeathScreams,T4);
				ImGui::SameLine(FirstPos);
				ImGuiEx::CheckBox("Hug Crush", &Config::Audio.bMuteHugCrushDeathScreams,T5);

				ImGuiEx::CheckBox("Crush", &Config::Audio.bMuteCrushDeathScreams, T6);
				ImGui::SameLine(FirstPos);
				ImGuiEx::CheckBox("Vore", &Config::Audio.bMuteVoreDeathScreams,T7);
				
				ImGui::Spacing();
				ImGuiEx::SliderF("Moan/Laugh Falloff", &Config::Audio.fFallOffMultiplier, 0.02f, 6.0f, T8, "%.2fx");
				ImGuiEx::SliderF("Moan/Laugh Volume", &Config::Audio.fVoiceVolumeMult, 0.02f, 1.0f, T9, "%.2fx");

				ImGui::Spacing();
			}
		}
	}
}