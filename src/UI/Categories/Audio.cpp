#include "Audio.hpp"
#include "UI/DearImGui/imgui.h"
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
			const char* T3 = "Replaces custom High Heel Size Sounds past x2 size with different sound sets\n"
			                 "When not wearing High Heels - plays old sounds.\n"
							 "Requires High Heels: 'Enable Height Adjustment' to be ON to work";

			const char* THelp = "Note: Moan/Laugh sounds are not included in the mod.\n"
			"You can add sounds by adding your own .wav files in the following folder:\n"
	    	"(Your Skyrim Folder)\\Data\\Sound\\fx\\GTS\\Moans_Laughs";
	        
	        if(ImGui::CollapsingHeader("Sounds",ImUtil::HeaderFlagsDefaultOpen)){
				ImGui::TextColored(ImUtil::ColorSubscript, "A Note On Sounds (?)");
				ImUtil::Tooltip(THelp ,true);

	            ImUtil::CheckBox("Footstep Sounds",&Settings.bFootstepSounds,T0);
	            ImUtil::CheckBox("Moans On Slow Growth",&Settings.bSlowGrowMoans, T1);
				ImUtil::CheckBox("Moan/Laugh Size Variance", &Settings.bMoanLaughSizeVariants, T2);
				ImUtil::CheckBox("Alternative High Heel Size Sounds", &Settings.bUseOtherHighHeelSet, T3);
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
			
			const char* T2 = "Enable/Disable actors making death sounds/screams when being absorbed by breasts.";
			const char* T3 = "Enable/Disable actors making death sounds/screams when killed through Wrathful Calamity.";
			const char* T4 = "Enable/Disable actors making death sounds/screams when killed through being Hug Crushed.";
			const char* T5 = "Enable/Disable actors making death sounds/screams when killed through vore.";
			
			if (ImGui::CollapsingHeader("Voice",ImUtil::HeaderFlagsDefaultOpen)) {
				ImUtil::CheckBox("Enable Voice Override",&Settings.bEnableVoiceOverride, T0);
				ImUtil::SliderF("Voice Pitch Max",&Settings.fMaxVoiceFrequency, 1.0f, 1.6f, T1, "%.2fx", !Settings.bEnableVoiceOverride);

				ImUtil::CheckBox("Breast Absorption: Mute Death Sound", &Settings.bMuteBreastAbsorptionDeathScreams,T2);
				ImUtil::CheckBox("Wrathful Calamity: Mute Death Sound", &Settings.bMuteFingerSnapDeathScreams,T3);
				ImUtil::CheckBox("Hug Crush: Mute Death Sound", &Settings.bMuteHugCrushDeathScreams,T4);
				ImUtil::CheckBox("Vore: Mute Death Sound", &Settings.bMuteVoreDeathScreams,T5);
				
				ImGui::Spacing();
			}
		}
	}
}