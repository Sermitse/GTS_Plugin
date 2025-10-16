#pragma once

namespace GTS {

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
		Thigh_Crush,
		Thigh_Sandwich,
		Hand_Right,
		Hand_Left,
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
		float Duration;
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

}
