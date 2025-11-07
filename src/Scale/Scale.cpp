#include "Scale/Scale.hpp"

namespace {
	//constexpr float EPS = std::numeric_limits<float>::epsilon();
	constexpr float EPS = 1e-5;
}

namespace GTS {

	void set_target_scale(Actor& actor, float scale) {
		auto actor_data = Persistent::GetActorData(&actor);
		if (actor_data) {
			float natural_scale = get_natural_scale(&actor, true);
			float target_scale = actor_data->fTargetScale * natural_scale;
			float max_scale = actor_data->fMaxScale;

			scale /= natural_scale;

			if (scale < (max_scale + EPS)) { // If new value is below max: allow it
				actor_data->fTargetScale = scale;
			} else if (target_scale < (max_scale - EPS) || target_scale > (max_scale + EPS)) { // If we are below max currently and we are trying to scale over max: make it max
				actor_data->fTargetScale = max_scale / natural_scale;
			} else {
				// If we are over max: forbid it
			}
		}
	}

	void set_target_scale(Actor* actor, float scale) {
		if (actor) {
			Actor& a = *actor;
			set_target_scale(a, scale);
		}
	}

	float get_target_scale(Actor& actor) {
		auto actor_data = Persistent::GetActorData(&actor);
		if (actor_data) {
			return actor_data->fTargetScale * get_natural_scale(&actor, true);
		} else {
			return 1.0f;
		}
	}

	float get_target_scale(Actor* actor) {
		if (actor) {
			Actor& a = *actor;
			return get_target_scale(a);
		} else {
			return 1.0f;
		}
	}

	void mod_target_scale(Actor& actor, float amt) {
       GTS_PROFILE_SCOPE("Scale: ModTargetScale");
        auto actor_data = Persistent::GetActorData(&actor);
        if (actor_data) {
            float natural_scale = get_natural_scale(&actor, true);
            float target_scale = actor_data->fTargetScale * natural_scale;
			float max_scale = actor_data->fMaxScale;

            amt /= natural_scale;

            if (amt < -EPS) { // If negative change always: allow
                actor_data->fTargetScale += amt;
            } else if (target_scale + amt < (max_scale + EPS)) { // If change results is below max: allow it
                actor_data->fTargetScale += amt;
            } else if (target_scale < (max_scale - EPS) || target_scale > (max_scale + EPS)) { // If we are currently below max and we are scaling above max: make it max
                set_target_scale(actor, max_scale);
            } else { // if we are over max then forbid it
            }
        }
    }

	void mod_target_scale(Actor* actor, float amt) {
		if (actor) {
			mod_target_scale(*actor, amt);
		}
	}

	void set_max_scale(Actor& actor, float scale) {
		auto actor_data = Persistent::GetActorData(&actor);
		if (actor_data) {
			actor_data->fMaxScale = scale;
		}
	}

	void set_max_scale(Actor* actor, float scale) {
		if (actor) {
			set_max_scale(*actor, scale);
		}
	}

	float get_max_scale(Actor& actor) {
		auto actor_data = Persistent::GetActorData(&actor);
		if (actor_data) {
			return actor_data->fMaxScale;
		}
		return 1.0f;
	}

	float get_max_scale(Actor* actor) {
		if (actor) {
			return get_max_scale(*actor);
		}
		return 1.0f;
	}

	void mod_max_scale(Actor& actor, float amt) {
		auto actor_data = Persistent::GetActorData(&actor);
		if (actor_data) {
			actor_data->fMaxScale += amt;
		}
	}

	void mod_max_scale(Actor* actor, float amt) {
		if (actor) {
			mod_max_scale(*actor, amt);
		}
	}

	float get_visual_scale(Actor& actor) {
		auto actor_data = Persistent::GetActorData(&actor);
		if (actor_data) {
			return actor_data->fVisualScale * get_natural_scale(&actor, true);
		}
		return 1.0f;
	}

	float get_visual_scale(Actor* actor) {
		if (actor) {
			return get_visual_scale(*actor);
		}
		return 1.0f;
	}

	float get_natural_scale(Actor& actor, bool game_scale) {
		auto actor_data = Transient::GetActorData(&actor);
		if (actor_data) {
		    float initialScale = GetInitialScale(&actor);
			float result = actor_data->OtherScales * initialScale;
			if (game_scale) {
				result *= game_getactorscale(&actor);
			}
			return result;
			// otherScales reads RaceMenu scale
		}
		return 1.0f;
	}

	float get_natural_scale(Actor* actor, bool game_scale) {
		if (actor) {
			return get_natural_scale(*actor, game_scale);
		}
		return 1.0f;
	}

	float get_natural_scale(Actor* actor) {
		if (actor) {
			return get_natural_scale(*actor, false);
		}
		return 1.0f;
	}

	float get_neutral_scale(Actor* actor) {
		return 1.0f;
	}

	float get_giantess_scale(Actor& actor) {
		auto actor_data = Persistent::GetActorData(&actor);
		if (actor_data) {
			float result = actor_data->fVisualScale * get_natural_scale(&actor, true);
			// Sadly had to add natural scale to it so it will respect GetScale * RaceMenu alterations
			return result;
		}
		return 1.0f;
	}

	float get_giantess_scale(Actor* actor) {
		if (actor) {
			return get_giantess_scale(*actor);
		}
		return 1.0f;
	}

	float get_raw_scale(Actor* actor) {
		auto actor_data = Persistent::GetActorData(actor);
		if (actor_data) {
			return actor_data->fVisualScale;
		}
		return 1.0f;
	}
}
