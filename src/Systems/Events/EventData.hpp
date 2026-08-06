#pragma once

namespace GTS {
	enum class AimZone { 
		None, 
		Close, 
		Far 
	};
    enum class StompAimType {
		T1, 
		T2, 
		T3, 
		T4 
	};

	enum class GrabBranch {
		None,
		Grab,
		GrabPlay,
	};

	enum class BusyFoot {
		None,
		RightFoot,
		LeftFoot,
	};

	enum class FootEvent {
		Left,
		Right,
		Front,
		Back,
		JumpLand,
		Unknown,
		Butt,
		Breasts,
	};

	enum class SizeType {
		GiantessScale,
		VisualScale,
		TargetScale,
	};

	enum class ParticleType {
		Red,
		DarkRed,
		Green,
		Blue,
		Hearts,
		None,
	};

	enum class QuestStage {
		HugSteal,
		HugSpellSteal,
		Crushing,
		ShrinkToNothing,
		HandCrush,
		Vore,
		Giant,
		Dragon,
	};

	enum class AttachToNode {
		None,
		Cleavage,
		ObjectA,
		ObjectB,
		ObjectL,
		ObjectR,
	};

	enum class ShrinkSource {
		Other,
		Magic,
		Hugs,
		Enchantment,
	};

	enum class SizeEffectType {
		kShrink,
		kGrow,
		kSteal,
		kGift,
		kNeutral,
	};

	enum class FootActionType {
		Grind_Normal,
		Grind_UnderStomp,
		Trample_NormalOrUnder,
	};

	enum class SizeAttribute {
		Normal,
		Sprint,
		JumpFall,
		HighHeel,
	};

	enum class CameraTracking {
		None,
		Butt,
		Knees,
		Breasts_02,
		Spine_02,
		Thigh_Crush,
		Thigh_Sandwich,
		Hand_Right,
		Hand_Left,
		ForearmTwist_1_Right,
		ForearmTwist_1_Left,
		ForearmTwist_2_Right,
		ForearmTwist_2_Left,
		Grab_Left,
		L_Foot,
		R_Foot,
		Mid_Butt_Legs,
		VoreHand_Right,
		Finger_Right,
		Finger_Left,
		ObjectA,
		ObjectB,
		ObjectL,
	};

	enum class NodeMovementType {
		Movement_LeftLeg,
		Movement_RightLeg,
		Movement_RightHand,
		Movement_LeftHand,
		Movement_None,
	};

	enum class DamageSource {
		HandCrawlRight,
		HandCrawlLeft,
		HandIdleL,
		HandIdleR,
		KneeIdleL,
		KneeIdleR,
		HandDropRight,
		HandDropLeft,
		HandSwipeRight,
		HandSwipeLeft,
		HandSlamRight,
		HandSlamLeft,
		RightFinger,
		RightFinger_Impact,
		LeftFinger,
		LeftFinger_Impact,
		KickedRight,
		KickedLeft,
		KneeDropRight,
		KneeDropLeft,
		KneeRight,
		KneeLeft,
		Breast,
		BreastImpact,
		BreastAbsorb,
		Hugs,
		Booty,
		BodyCrush,
		Vored,
		Spell,
		Melted,
		Explode,
		Crushed,
		CrushedRight,
		FootIdleR,
		WalkRight,
		WalkLeft,
		CrushedLeft,
		FootIdleL,
		Overkill,
		ShrinkToNothing,
		Collision,
		FootGrindedRight,
		FootGrindedLeft,
		FootGrindedRight_Impact,
		FootGrindedLeft_Impact,
		HandCrushed,
		ThighSandwiched,
		ThighSuffocated,
		ThighCrushed,
		BlockDamage,
		HitSteal,
		EraseFromExistence,
		Shockwave,
	};

	enum class GrowthAnimation {
		None,
		Growth_1,
		Growth_2,
		Growth_3,
		Growth_4,
		Growth_5,
		Growth_6,
	};

	enum class CrawlEvent {
		LeftKnee,
		RightKnee,
		LeftHand,
		RightHand,
	};

	enum class PerkUpdate {
		Perk_Acceleration,
		Perk_LifeForceAbsorption,
		Perk_None,
	};

	enum class CharEmotionType {
		Phenome,
		Modifier,
		Expression,
	};

	struct BoneTarget {
		std::vector<std::string> boneNames = {};
		float zoomScale = 1.0f;
	};

	struct Impact {
		RE::Actor* actor;
		FootEvent kind;
		float scale;
		float modifier;
		std::vector<RE::NiAVObject*> nodes;
	};

	struct EmotionInfo {
		RE::Actor* giantess;
		int ph_id;
		float speed_1;
		float speed_2;
		std::string_view task_name;
		std::string_view task_type;
	};

	struct VoreInformation {
		RE::Actor* giantess;
		bool WasLiving;
		float Scale;
		float Vore_Power;
		float Health_Regeneration;
		float Box_Scale;
		float DurationModifierMult;
		std::string_view Tiny_Name;
	};

	struct UnderFoot {
		RE::Actor* giant;
		RE::Actor* tiny;
		float force;
		/// Giant foot that is doing the squishing
		RE::NiAVObject* foot;
		/// Tiny's body parts that are underfoot
		std::vector<RE::NiAVObject*> bodyParts;
		FootEvent footEvent;
	};

	struct HighheelEquip {
		RE::Actor* actor;
		bool equipping;
		float hhLength;
		RE::NiPoint3 hhOffset;
		RE::TESObjectARMO* shoe;
	};

	struct AddPerkEvent {
		RE::Actor* actor;
		RE::BGSPerk* perk;
		std::uint32_t rank;
	};

	struct RemovePerkEvent {
		RE::Actor* actor;
		RE::BGSPerk* perk;
	};

    struct KillMoveParameters {
        bool isStrongAttack             = false;
        bool isFootAttack               = false; // if True = we take HH offset into account for camera
        float baseDamage                = 1.0f;
        float crushThresholdMult        = 1.0f;  // Multiplies size difference threshold for crushing. x1.0 = base threshold
        float lookAtNodeDistance        = 30.0f;
        float deathHoldDistance         = 40.0f;
		float orbitAngle				= 360.0f;
		float orbitTime					= 9.0f;
        std::vector<std::string_view>   nodeLookups;
        DamageSource damageSource       = DamageSource::CrushedLeft;
    };
	struct AimAssistResult {
		bool hit 				= false;     
		bool alive 				= false;  
		bool canKillMove 		= false; 		// Used only in Breast Crush atm
		float blend_x 			= 0.0f;
		float blend_y 			= 0.0f;
		float distance 			= FLT_MAX;
		Actor* victim 			= nullptr;
	};

	struct AnimationBlendInfo {
		float blendX 				= 0.0f; 	// forward (>0) /   back (<1)
		float blendY 				= 0.0f;	// right (>0)   /   left (<1)
		float outDistanceX 	 		= 0.0f;
		float outDistanceY   		= 0.0f;
		float maxDistance			= 0.0f;
		float finalDistance			= 0.0f;

		bool isInsideRectangle		= false; // Everything below is used only in crawl breast slam
		float length				= 0.0f;
		float width					= 0.0f;
		float blendOffset			= 0.0f;
		float inPercent_Directional	= 0.0f;
		float inPercent_Side		= 0.0f;

	};

    struct AimAttempt {
        bool hit = false;
        bool left = false;
        AimAssistResult result;
    };
    struct AimOutcome {
        AimZone zone = AimZone::None;
        bool left = false;
        bool hit = false;
        Actor* victim = nullptr;
    };
}
