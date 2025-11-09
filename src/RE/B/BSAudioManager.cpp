#include "RE/B/BSAudioManager.hpp"

namespace RE {

	void SetSoundHandleFrequency(RE::BSAudioManager* a_manager, std::uint32_t a_soundID, float a_freq) {
		if (a_soundID != 0xffffffff){
			using func_t = decltype(&SetSoundHandleFrequency);
			static const REL::Relocation<func_t> func{ REL::RelocationID(66422, 67685, NULL) };
			func(a_manager, a_soundID, a_freq);
		}
	}

}