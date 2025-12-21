#pragma once

// Handles the various methods of scaling an actor

namespace GTS {

	void set_target_scale(Actor& actor, float scale);
	void set_target_scale(Actor* actor, float scale);
	float get_target_scale(Actor& actor);
	float get_target_scale(Actor* actor);
	void mod_target_scale(Actor& actor, float amt);
	void mod_target_scale(Actor* actor, float amt);

	void set_max_scale(Actor& actor, float scale);
	void set_max_scale(Actor* actor, float scale);
	float get_max_scale(Actor& actor);
	float get_max_scale(Actor* actor);
	void mod_max_scale(Actor& actor, float amt);
	void mod_max_scale(Actor* actor, float amt);

	float get_visual_scale(Actor& actor);
	float get_visual_scale(Actor* actor);

	float get_natural_scale(Actor& actor, bool game_scale);
	float get_natural_scale(Actor* actor, bool game_scale);
	float get_natural_scale(Actor* actor);

	float get_neutral_scale(Actor* actor);

	float get_giantess_scale(Actor& actor);
	float get_giantess_scale(Actor* actor);

	float get_raw_scale(Actor* actor);
	float get_corrected_scale(Actor* a_actor);
}
