#pragma once

// Module that handles footsteps

namespace GTS {

	class FootStepManager : public EventListener, public CInitSingleton <FootStepManager> {
		public:
		virtual std::string DebugName() override;
		virtual void OnImpact(const Impact& impact) override;
		static void PlayHighHeelSounds_Walk(float modifier, NiAVObject* foot, FootEvent foot_kind, float scale, bool UseOtherHeelSet);
		static void PlayNormalSounds(float modifier, NiAVObject* foot, FootEvent foot_kind, float scale, bool UseOtherHeelSet);
		static void PlayHighHeelSounds_Jump(float modifier, NiAVObject* foot, FootEvent foot_kind, float scale, bool UseOtherHeelSet);
		static float Volume_Multiply_Function(Actor* actor, FootEvent Kind);
		static void PlayVanillaFootstepSounds(Actor* giant, bool right);
		static void DoStompSounds(Actor* giant, float modifier, NiAVObject* foot, FootEvent foot_kind, float scale, bool Strong);
		static void DoStrongSounds(Actor* giant, float animspeed, std::string_view feet);
	};
}
