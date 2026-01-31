#include "Magic/Effects/Potions/ExperiencePotion.hpp"
#include "Magic/Effects/Common.hpp"

// A Potion that gives size experience

namespace GTS {

	std::string ExperiencePotion::GetName() {
		return "ExperiencePotion";
	}

	void ExperiencePotion::OnStart() {
		auto caster = GetCaster();

		if (caster) {
            int RNG = RandomInt(0, 30);
            ModSizeExperience(caster, 1.0f * (1.0f + (RNG * 0.01f)));
			Potion_Penalty(caster);

			if (caster->IsPlayerRef()) {
				shake_camera(caster, 0.50f, 0.50f);
			}

			float scale = get_visual_scale(caster);

			SpawnCustomParticle(caster, ParticleType::Red, NiPoint3(), "NPC COM [COM ]", scale * 2.0f); // Just some nice visuals
        }
	}
}


