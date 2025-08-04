#include "Managers/Audio/PitchShifter.hpp"
#include "Config/Config.hpp"

using namespace GTS;

namespace GTS {
	 void ShiftAudioFrequency() {
	 	auto enable = Config::GetAudio().bEnableVoiceOverride;
		if (!enable) {
			return;
		}
		for (auto tiny: find_actors()) {
			if (tiny) {
				if (tiny->formID != 0x14) {
					auto ai = tiny->GetActorRuntimeData().currentProcess;
					if (ai) {
						auto high = ai->high;
						if (high) {
							auto Audio_1 = high->soundHandles[0];
							auto Audio_2 = high->soundHandles[1];

							const auto [volume, freq] = CalculateVoicePitch(tiny);

							// < 1  = deep voice, below 0.5 = audio bugs out, not recommended
							// > 1 = mouse-like voice, not recommended to go above 1.5	

							if (Audio_1.soundID != BSSoundHandle::kInvalidID) {
								Audio_1.SetFrequency(freq);
								Audio_1.SetVolume(volume);
							}
							if (Audio_2.soundID != BSSoundHandle::kInvalidID) {
								Audio_2.SetFrequency(freq);
								Audio_2.SetVolume(volume);
							}
						}
					}
				}
			}
		}
	}
}
