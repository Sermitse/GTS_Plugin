#pragma once

namespace GTS {

	void override_actor_scale(Actor* giant, float amt, SizeEffectType type);

	void update_target_scale(Actor* giant, float amt, SizeEffectType type);

	float get_update_target_scale(Actor* giant, float amt, SizeEffectType type);

	float get_scale_difference(Actor* giant, Actor* tiny, SizeType Type, bool Check_SMT, bool HH);

	float GetSizeFromBoundingBox(Actor* tiny);

	float GetRoomStateScale(Actor* giant);
}
