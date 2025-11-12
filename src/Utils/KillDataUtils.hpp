#pragma once

namespace GTS {

    enum class DeathType : int16_t {

		kVanilla = -1,

		kTotalKills,

		kShrunkToNothing,
		kOtherSources,

		kBreastAbsorbed,
		kBreastCrushed ,
		kBreastSuffocated,

		kHugCrushed,
		kGrabCrushed ,
		kButtCrushed,

		kThighCrushed,
		kThighSuffocated,
		kThighSandwiched,
		kThighGrinded,

		kFingerCrushed,

		kErasedFromExistence,

		kVoreAbsorbed,
		kAbsorbed,
		kCrushed,
		kKicked,
		kEaten,
		kGrinded,

		kProximity,
    	
	};

    void RecordKill(Actor* a_giant, Actor* a_victim, DeathType a_type);
    uint32_t GetKillCount(Actor* giant, DeathType Type);
}