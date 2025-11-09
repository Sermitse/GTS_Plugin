#include "Utils/ButtCrushUtils.hpp"

namespace GTS {

	float GetButtCrushSize(Actor* giant) {
		auto saved_data = Transient::GetActorData(giant);
		if (saved_data) {
			float butt_crush_size = std::clamp(saved_data->ButtCrushMaxSize, 0.0f, 1000000.0f);
			return butt_crush_size;
		}
		return 0.0f;
	}

	float GetGrowthCount(Actor* giant) {
		auto transient = Transient::GetActorData(giant);
		if (transient) {
			return transient->ButtCrushGrowthAmount;
		}
		return 1.0f;
	}

	float GetGrowthLimit(Actor* actor) {
		float limit = 0;
		if (Runtime::HasPerkTeam(actor, "GTSPerkButtCrushAug2")) {
			limit += 3.0f;
		}
		if (Runtime::HasPerkTeam(actor, "GTSPerkButtCrushAug3")) {
			limit += 4.0f;
		}
		if (Runtime::HasPerkTeam(actor, "GTSPerkButtCrushAug4")) {
			limit += 5.0f;
		}

		return limit;
	}

	float GetButtCrushDamage(Actor* actor) {
		float damage = 1.0f;
		if (Runtime::HasPerkTeam(actor, "GTSPerkButtCrush")) {
			damage += 0.30f;
		}
		if (Runtime::HasPerkTeam(actor, "GTSPerkButtCrushAug3")) {
			damage += 0.70f;
		}
		return damage;
	}

	void ModGrowthCount(Actor* giant, float value, bool reset) {
		auto transient = Transient::GetActorData(giant);
		if (transient) {
			transient->ButtCrushGrowthAmount += value;
			if (reset) {
				transient->ButtCrushGrowthAmount = 0.0f;
			}
		}
	}

	void RecordStartButtCrushSize(Actor* giant) {
		auto saved_data = Transient::GetActorData(giant);
		if (saved_data) {
			saved_data->ButtCrushStartScale = get_target_scale(giant);
		}
	}

	void SetButtCrushSize(Actor* giant, float value, bool reset) {
		auto saved_data = Transient::GetActorData(giant);
		if (saved_data) {
			float scale = game_getactorscale(giant);
			saved_data->ButtCrushMaxSize += value * scale;

			if (!reset && GetGrowthCount(giant) <= 1) {
				RecordStartButtCrushSize(giant);
			}

			if (reset) {
				float start_scale = saved_data->ButtCrushStartScale;

				start_scale = std::max(start_scale, get_natural_scale(giant));

				set_target_scale(giant, start_scale);

				saved_data->ButtCrushMaxSize = 0;
				saved_data->ButtCrushStartScale = 0.0f;
			}
		}
	}
}
