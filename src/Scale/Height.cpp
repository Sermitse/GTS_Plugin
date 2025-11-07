#include "Scale/Height.hpp"

using namespace GTS;

namespace {

	float height_to_scale(Actor* actor, float height) {
		if (!actor) {
			return 1.0f;
		}
		auto temp_actor_data = Transient::GetActorData(actor);
		if (!temp_actor_data) {
			return 1.0f;
		}
		return height / temp_actor_data->BaseHeight;
	}

	float scale_to_height(Actor* actor, float scale) {
		if (!actor) {
			return 1.0f;
		}
		auto temp_actor_data = Transient::GetActorData(actor);
		if (!temp_actor_data) {
			return 1.0f;
		}
		return scale * temp_actor_data->BaseHeight;
	}

}

namespace GTS {

	void set_target_height(Actor* actor, float height) {
		float scale = height_to_scale(actor, height);
		set_target_scale(actor, scale);
	}

	float get_target_height(Actor* actor) {
		float scale = get_target_scale(actor);
		return scale_to_height(actor, scale);
	}

	void mod_target_height(Actor* actor, float amt) {
		float current_scale = get_target_scale(actor);
		float current_height = scale_to_height(actor, current_scale);
		float target_height = (current_height + amt);
		float target_scale = height_to_scale(actor, target_height);
		float scale_delta = target_scale - current_scale;
		mod_target_scale(actor, scale_delta);
	}

	void set_max_height(Actor* actor, float height) {
		float scale = height_to_scale(actor, height);
		set_max_scale(actor, scale);
	}

	float get_max_height(Actor* actor) {
		float scale = get_max_scale(actor);
		return scale_to_height(actor, scale);
	}
	void mod_max_height(Actor* actor, float amt) {
		float current_scale = get_max_scale(actor);
		float current_height = scale_to_height(actor, current_scale);
		float target_height = (current_height + amt);
		float target_scale = height_to_scale(actor, target_height);
		float scale_delta = target_scale - current_scale;
		mod_max_scale(actor, scale_delta);
	}

	float get_visual_height(Actor* actor) {
		float scale = get_visual_scale(actor);
		return scale_to_height(actor, scale);
	}

	float get_giantess_height(Actor* actor) {
		float scale = get_giantess_scale(actor);
		return scale_to_height(actor, scale);
	}

	float get_base_height(Actor* actor) {
		auto temp_actor_data = Transient::GetActorData(actor);
		if (!temp_actor_data) {
			return 1.0f;
		}
		return temp_actor_data->BaseHeight;
	}

	float get_bounding_box_to_mult(Actor* actor) {
		auto nif_dim = NiPoint3(22.0f, 14.0f, 64.0f);
		auto temp_actor_data = Transient::GetActorData(actor);
		if (temp_actor_data) {
			nif_dim = temp_actor_data->BoundingBoxCache;
		}
		float x = nif_dim.x;
		float y = nif_dim.y;
		float z = nif_dim.z;

		float box = pow(x*y*z/(22.0f*14.0f*64.0f), 1.0f/3.0f);
		return box;
	}

	float get_bounding_box_z(Actor* actor) {
		auto nif_bb = get_bound_values(actor);
		return nif_bb.z;
	}
}
