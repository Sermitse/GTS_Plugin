#pragma once

namespace GTS::AnimationVars {}

namespace GTS {

	bool IsHandStomping_L(Actor* actor);

	bool IsHandStomping_H(Actor* actor);

	bool IsBeingKilledWithMagic(Actor* tiny);

	bool IsGrabAttacking(Actor* actor);

	bool IsStrangling(Actor* giant);

	bool IsExitingStrangle(Actor* giant);

	bool IsUnderGrinding(Actor* giant);

	bool IsUnderTrampling(Actor* giant);

	bool IsInGrabPlayState(Actor* giant);

	//Not sure yet when its true, i assume its the same as GTSbusy but only when grab playing
	bool IsGrabPlaySandwiching(Actor* giant);

	bool IsHandCrushing(Actor* giant);

	bool IsGrabPlayKissing(Actor* giant);

	bool AnimationsInstalled(Actor* giant);
}