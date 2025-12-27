#pragma once

namespace GTS {
	void PlayAnimation(Actor* actor, std::string_view animName);
	float GetAnimationSlowdown(Actor* giant);
}