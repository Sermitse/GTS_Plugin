#pragma once 

namespace GTS {
	
	void DoFootstepSound(Actor* giant, float modifier, FootEvent kind, std::string_view node);
	float Sound_GetFallOff(NiAVObject* source, float mult);
	std::tuple<float, float> CalculateVoicePitch(Actor* a_actor);
	float CalculateGorePitch(float TinyScale);

}