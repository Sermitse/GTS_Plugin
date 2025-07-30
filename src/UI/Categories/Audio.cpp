#include "Audio.hpp"
#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/ImUtil.hpp"


using namespace GTS;

namespace {

	//TODO Unhardcode this
	void SelectVoiceBank(RE::Actor* a_actor) {

		if (!a_actor) return;

		const char* const EntriesFemale = "Default\0"
										"SL F Voice 1\0"
										"SL F Voice 2\0"
										"SL F Voice 3\0"
										"SL F Voice 4\0"
										"SL F Voice 5\0"
										"SL F Voice 6\0"
										"SL F Voice 7\0"
										"SL F Voice 8\0";

		if (auto ActorData = Persistent::GetSingleton().GetData(a_actor)) {

			const std::string ActorName = a_actor->GetName();
			ImGui::PushID(a_actor);

			//idk how ComboBoxes internally work so casting to a larger type may be unsafe
			//best to just use a middleman value instead.
			int Out = ActorData->MoanSoundDescriptorIndex;

			ImGui::Combo(ActorName.c_str(), &Out, EntriesFemale);
			ActorData->MoanSoundDescriptorIndex = static_cast<uint8_t>(Out);

			ImGui::PopID();

		}

	}

}

namespace GTS {

	void CategoryAudio::DrawLeft(){

	    ImUtil_Unique {

	        const char* T0 = "Enable footstep sounds when player size meets or exceeds a certain threshold.";
	        const char* T1 = "Enable moaning sounds during spells like Slow Growth.";
			const char* T2 = "Enables size variance for moans/laughs similar to footsteps, but without blendings\n"
							 "If disabled, it will always use normal size sounds.\n"
							 "Else picks matching sound category based on current size range:\n"
							 "normal/x2/x4/x8/x12/x24/x48/x96\n\n"
							 "- If you have no custom Moan/Laugh sounds, it does nothing.\n"
							 "- Sounds location: Data\\Sound\\fx\\GTS\\Moans_Laughs. Folders are empty by default.\n"
							 "- Possible usage: put edited variants of sounds with reverb/echo/etc in matching size folders";
			const char* T3 = "If true, only Player Character will be able to moan/laugh";
			const char* T4 = "Replaces custom High Heel Size Sounds past x2 size with different sound sets\n"
			                 "When not wearing High Heels - plays old sounds.\n"
							 "Requires High Heels: 'Enable Height Adjustment' to be ON to work";
			const char* T5 = "If True:\n"
							 "- audio of footsteps will attempt to smoothly swap between each other"
			                 "- It can result in footsteps being quieter than without blending\n"
							 "- Because of blending, it can even play 2 sounds at once\n"
							 "If False: \n"
							 "- Audio of footsteps changes as soon as you pass size threshold";

			const char* THelp = "Note: Moan/Laugh sounds are included in the mod if you chose to install them.\n"
			"You can also add sounds by adding your own .wav files in the following folder:\n"
	    	"(Your Skyrim Folder)\\Data\\Sound\\fx\\GTS\\Moans_Laughs";
	        
	        if(ImGui::CollapsingHeader("Sounds",ImUtil::HeaderFlagsDefaultOpen)){
				ImGui::TextColored(ImUtil::ColorSubscript, "A Note On Sounds (?)");
				ImUtil::Tooltip(THelp ,true);

	            ImUtil::CheckBox("Footstep Sounds",&Settings.bFootstepSounds,T0);
	            ImUtil::CheckBox("Moans On Slow Growth",&Settings.bSlowGrowMoans, T1);
				ImUtil::CheckBox("Moans and Laughs: Size Variance", &Settings.bMoanLaughSizeVariants, T2);
				ImUtil::CheckBox("Moans and Laughs: Player Exclusive", &Settings.bMoanLaughPCExclusive, T3);
				ImUtil::CheckBox("Alternative High Heel Size Sounds", &Settings.bUseOtherHighHeelSet, T4);
				ImUtil::CheckBox("Smoothly Blend between Footstep Sounds", &Settings.bBlendBetweenFootsteps, T5);
	            ImGui::Spacing();
	        }
	    }

		if (Runtime::IsSexlabInstalled()) {

			ImUtil_Unique{

				const char* THelp = "Sexlab has been detected.\n"
									"This mod can now use it's voice files as a substitute in case "
									"you don't have any files that you can add.\n"
									"This also allows you to select a different voice per actor for some variety.\n\n"
									"Note: Only The Player/Current Followers will be listed as to not clutter this menu.\n"
									"If this menu is empty it means none of the currently loaded npc's are elidgible for this feature.";

				if (ImGui::CollapsingHeader("Moan Voice Select",ImUtil::HeaderFlagsDefaultOpen)) {
					ImGui::TextColored(ImUtil::ColorSubscript, "What is this (?)");
					ImUtil::Tooltip(THelp, true);

					const auto Player = PlayerCharacter::GetSingleton();

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
			}
		}
	}

	void CategoryAudio::DrawRight() {

		ImUtil_Unique{

			const char* T0 = "Alter the voiceline pitch of NPCs when small/large.";
			const char* T1 = "Change the maximum voice pitch, Higher values will lower the pitch when the actor is large.\n"
							 "It's recommended to leave this at 1.0x. Anything above 1.2x doesn't sound good.";
			
			const char* T2 = "Enable/Disable npc's making death sounds/screams when being shrunken to nothing.";
			const char* T3 = "Enable/Disable npc's making death sounds/screams when being absorbed by breasts.";
			const char* T4 = "Enable/Disable npc's making death sounds/screams when killed through Wrathful Calamity.";
			const char* T5 = "Enable/Disable npc's making death sounds/screams when killed through being Hug Crushed.";
			const char* T6 = "Enable/Disable npc's making death sounds/screams when being crushed";
			const char* T7 = "Enable/Disable npc's making death sounds/screams when being eaten through vore.";

			const char* T8 = "FallOff Range Multiplier for Moans and Laughs. Large values = can be heard from further dist";
			const char* T9 = "Moan and Laugh volume multiplier";
			
			if (ImGui::CollapsingHeader("Voice",ImUtil::HeaderFlagsDefaultOpen)) {
				ImUtil::CheckBox("Enable Voice Override",&Settings.bEnableVoiceOverride, T0);
				ImUtil::SliderF("Voice Pitch Max",&Settings.fMaxVoiceFrequency, 1.0f, 1.6f, T1, "%.2fx", !Settings.bEnableVoiceOverride);
				
				ImUtil::CheckBox("Shrink To Nothing: Mute Death Sound", &Settings.bMuteShrinkToNothingDeathScreams,T2);
				ImUtil::CheckBox("Breast Absorption: Mute Death Sound", &Settings.bMuteBreastAbsorptionDeathScreams,T3);
				ImUtil::CheckBox("Wrathful Calamity: Mute Death Sound", &Settings.bMuteFingerSnapDeathScreams,T4);
				ImUtil::CheckBox("Hug Crush: Mute Death Sound", &Settings.bMuteHugCrushDeathScreams,T5);
				ImUtil::CheckBox("Crush: Mute Death Sounds", &Settings.bMuteCrushDeathScreams, T6);
				ImUtil::CheckBox("Vore: Mute Death Sound", &Settings.bMuteVoreDeathScreams,T7);
				

				ImUtil::SliderF("Moan/Laugh Falloff", &Settings.fFallOffMultiplier, 0.02f, 6.0f, T8, "%.2fx");
				ImUtil::SliderF("Moan/Laugh Volume", &Settings.fVoiceVolumeMult, 0.02f, 1.0f, T9, "%.2fx");

				ImGui::Spacing();
			}
		}
	}
}