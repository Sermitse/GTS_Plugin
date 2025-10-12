#pragma once
#include "Utils/Singleton.hpp"

namespace GTS {
	
	struct BoneTarget {
		std::vector<std::string> boneNames = {};
		float zoomScale = 1.0f;
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

	class EventListener {
		public:
			EventListener() = default;
			virtual ~EventListener() = default;
			EventListener(EventListener const&) = delete;
			EventListener& operator=(EventListener const&) = delete;

			// Get name used for debug prints
			virtual std::string DebugName() = 0;

			// Called on Live (non paused) gameplay
			virtual void Update();

			// Called on Papyrus OnUpdate
			virtual void PapyrusUpdate();

			// Called on Havok update (when processing hitjobs)
			virtual void HavokUpdate();

			// Called when the camera update event is fired (in the TESCameraState)
			virtual void CameraUpdate();

			// Called on game load started (not yet finished)
			// and when new game is selected
			virtual void Reset();

			// Called when game is enabled (while not paused)
			virtual void Enabled();

			// Called when game is disabled (while not paused)
			virtual void Disabled();

			// Called when a game is started after a load/newgame
			virtual void Start();

			// Called when all forms are loaded (during game load before mainmenu)
			virtual void DataReady();

			// Called when an actor is reset
			virtual void ResetActor(RE::Actor* actor);

			// Called when an actor has an item equipped
			virtual void ActorEquip(RE::Actor* actor);

			// Called when Player absorbs dragon soul
			virtual void DragonSoulAbsorption();

			// Called when an actor has is fully loaded
			virtual void ActorLoaded(RE::Actor* actor);

			// Called when a papyrus hit event is fired
			virtual void HitEvent(const RE::TESHitEvent* evt);

			// Called when an actor is squashed underfoot
			virtual void UnderFootEvent(const UnderFoot& evt);

			// Fired when a foot lands
			virtual void OnImpact(const Impact& impact);

			// Fired when a highheel is (un)equiped or when an actor is loaded with HH
			virtual void OnHighheelEquip(const HighheelEquip& evt);

			// Fired when a perk is added
			virtual void OnAddPerk(const AddPerkEvent& evt);

			// Fired when a perk about to be removed
			virtual void OnRemovePerk(const RemovePerkEvent& evt);

			// Fired when a skyrim menu event occurs
			virtual void MenuChange(const RE::MenuOpenCloseEvent* menu_event);

			// Fired when a actor animation event occurs
			virtual void ActorAnimEvent(RE::Actor* actor, const std::string_view& tag, const std::string_view& payload);

			// Fired when actor uses furniture
			virtual void FurnitureEvent(RE::Actor* user, TESObjectREFR* object, bool enter);

			// Fired during skse cosave save start
			virtual void OnGameSave();

			// Fired during skse cosave load end
			virtual void OnGameLoaded();

			// Fired during config reset
			virtual void OnConfigReset();

			// Fired during config import
			virtual void OnConfigRefresh();
	};

	class EventDispatcher {

			public:

			static void AddListener(EventListener* a_listener);
			static void RemoveListener(EventListener* a_listener);
			static void DoUpdate();
			static void DoPapyrusUpdate();
			static void DoHavokUpdate();
			static void DoCameraUpdate();
			static void DoReset();
			static void DoEnabled();
			static void DoDisabled();
			static void DoStart();
			static void DoDataReady();
			static void DoResetActor(RE::Actor* actor);
			static void DoActorEquip(RE::Actor* actor);
			static void DoDragonSoulAbsorption();
			static void DoActorLoaded(RE::Actor* actor);
			static void DoHitEvent(const RE::TESHitEvent* evt);
			static void DoUnderFootEvent(const UnderFoot& evt);
			static void DoOnImpact(const Impact& impact);
			static void DoHighheelEquip(const HighheelEquip& impact);
			static void DoAddPerk(const AddPerkEvent& evt);
			static void DoRemovePerk(const RemovePerkEvent& evt);
			static void DoMenuChange(const RE::MenuOpenCloseEvent* menu_event);
			static void DoActorAnimEvent(RE::Actor* actor, const RE::BSFixedString& a_tag, const RE::BSFixedString& a_payload);
			static void DoSerdePreSaveEvent();
			static void DoSerdePostLoadEvent();
			static void DoConfigResetEvent();
			static void DoConfigRefreshEvent();
			static void DoFurnitureEvent(const TESFurnitureEvent* a_event);
		private:
			static inline std::mutex m_lock;
			static inline std::vector<EventListener*> m_listeners;
	};
}
