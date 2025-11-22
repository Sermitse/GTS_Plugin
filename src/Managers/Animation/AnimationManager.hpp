#pragma once

namespace GTS {
	// This data is passed to an animation that is in progress
	//   It is created when StartAnim is called
	//   It is destroyed after ActorAnimEvent if stage == 0
	//    Therefore when an anim is finished set stage = 0 to tell
	//    the anim system that we are done
	struct AnimationEventData {
		// Giant must exists and is the one the animation is happening on
		//   Maybe rename this to caster
		Actor& giant;
		// This is the thing being grabbed. It is an TESObjectREFR so we could also
		//   pick up other objects
		// Set to nullptr for no object
		TESObjectREFR* tiny;
		// This is used to keep track of which trigger should be run the next time
		// Next is called
		std::size_t currentTrigger = 0;
		// Stage a value of 0 means finished and will be cleaned up
		std::size_t stage = 0;
		// Anim speed, can be adjusted via the animation itself or via AdjustAnimSpeed
		float animSpeed = 1.0f;
		// Make true during a stage of the anim to allow animSpeed adjust via AdjustAnimSpeed
		bool canEditAnimSpeed = false;
		// If true then hhs are disabled
		bool disableHH = false;
		// Speed of adjusting high heels
		float HHspeed = 1.0f;
		AnimationEventData(Actor& giant, TESObjectREFR* tiny);
	};

	// Holds data that links a animation tag to a callback and group
	struct AnimationEvent {
		// The callback to run
		std::function<void(AnimationEventData&)> callback;
		// This is a tag used to link animation data from one call to another
		//  All animations of the same kind i.e Stomp has GTSstompimpactR, GTSstompimpactL etc
		//  will have the same group name.
		// When an animation is started data with this group name is created for this actor
		//  At every stage this data will be passed to any animation registered with this groupname for an actor
		std::string group;

		AnimationEvent(const std::function<void(AnimationEventData&)>& a_callback, const std::string& a_group);
	};

	// Holds data that links a trigger to a behaviour and group
	//
	// These are just friendly names to start an animation
	struct TriggerData {
		// Name to send to the animation graph
		std::vector<std::string> behavors;
		// The name of the data to be created
		std::string group;

		TriggerData(const std::vector<std::string_view>& behavors, std::string_view group);
	};

	class AnimationManager : public EventListener, public CInitSingleton <AnimationManager> {
		public:
		virtual std::string DebugName() override;
		virtual void DataReady() override;
		virtual void Update() override;
		virtual void ActorAnimEvent(Actor* actor,  const std::string_view& tag, const std::string_view& payload) override;
		virtual void Reset() override;
		virtual void ResetActor(Actor* actor) override;

		static float GetHighHeelSpeed(Actor* actor);
		static float GetBonusAnimationSpeed(Actor* actor);


		// Change speed of all animations that are currently playing on the PLAYER
		// that have the data.canEditAnimSpeed == true
		//
		// Each anim gets it's own adjustable speed
		static void AdjustAnimSpeed(float bonus);

		// Get total animation speed of an actor
		static float GetAnimSpeed(Actor* actor);

		// Register an animation event to a function callback
		//
		// Should be of the form:
		// ```cpp
		// AnimationManager::RegisterEvent("GTSstompimpactR", "Stomp", GTSstompimpactR);
		// AnimationManager::RegisterEvent("GTSstompimpactL", "Stomp", GTSstompimpactL);
		// ```
		//
		// Where group is name of the collection to share data for
		static void RegisterEvent( std::string_view name,  std::string_view group, std::function<void(AnimationEventData&)> func);


		// Register a trigger to a behavior to start
		//
		// This is used to link the friendly anim name to the complex behaviour name
		//  while also creating any associated animation data that matches the group name
		static void RegisterTrigger( std::string_view trigger,  std::string_view group,  std::string_view behavior);

		// Similar to RegisterTrigger but takes a list of stages.
		//
		// You can trigger the next stage with
		// ``cpp
		// AnimationManager::NextAnim("ThighCrush", giant)
		// ```
		static void RegisterTriggerWithStages( std::string_view trigger,  std::string_view group,  std::vector< std::string_view> behaviors);

		// Start an anuimation with NO object to carry
		static void StartAnim(std::string_view trigger, Actor& giant);
		static void StartAnim(std::string_view trigger, Actor* giant);
		// Start the animation WITH an object to carry. We use TESObjectREFR over Actor so that we can pick up anything
		static void StartAnim(std::string_view trigger, Actor& giant, TESObjectREFR* tiny);
		static void ResetAnimationSpeedData(Actor* actor);
		static void StartAnim(std::string_view trigger, Actor* giant, TESObjectREFR* tiny);

		// Advance an animation to the next stage if possible
		static void NextAnim(std::string_view trigger, Actor& giant);
		static void NextAnim(std::string_view trigger, Actor* giant);

		// Get the current stage of an animation group
		static std::size_t GetStage(Actor& actor,  std::string_view group);
		static std::size_t GetStage(Actor* actor,  std::string_view group);

		// Check if any currently playing anim disabled the HHs
		static bool HHDisabled(Actor& actor);
		static bool HHDisabled(Actor* actor);

		static void UpdateGravity(Actor* actor);

		protected:
		std::unordered_map<Actor*, std::unordered_map<std::string, AnimationEventData> > data;
		std::unordered_map<std::string, AnimationEvent> eventCallbacks;
		std::unordered_map<std::string, TriggerData> triggers;
	};

	void ShakeAndSound(Actor* caster, float volume,  std::string_view& node);
}
