#pragma once

namespace GTS {

	struct GrabData {
		GrabData(TESObjectREFR* tiny, float strength);;
		TESObjectREFR* tiny;
		bool holding = false;
		float strength;
	};

	void Utils_CrushTask(Actor* giant, Actor* grabbedActor, float bonus, bool do_sound, bool stagger, DamageSource source, QuestStage stage);

	class Grab : public EventListener, public CInitSingleton <Grab> {
		public:
		virtual std::string DebugName() override;
		virtual void Reset() override;
		virtual void ResetActor(Actor* actor) override;

		static void RegisterEvents();
		static void RegisterTriggers();
		static void DamageActorInHand(Actor* giant, float Damage);
		static void DetachActorTask(Actor* giant);
		static void AttachActorTask(Actor* giant, Actor* tiny);

		// Streangth is meant to be for a calculation of
		// escape chance currently unused
		static void ExitGrabState(Actor* giant);
		static void GrabActor(Actor* giant, TESObjectREFR* tiny, float strength);
		static void GrabActor(Actor* giant, TESObjectREFR* tiny);
		static void Release(Actor* giant);

		// Get object being held
		static TESObjectREFR* GetHeldObj(Actor* giant);
		// Same as `GetHeldObj` but with a conversion to actor if possible
		static Actor* GetHeldActor(Actor* giant);
		static void FailSafeReset(Actor* giantref);
		static void CancelGrab(Actor* giantref, Actor* tinyref);
		
		std::unordered_map<Actor*, GrabData> data;
	};

	void StartRHandRumble(std::string_view tag, Actor& actor, float power, float halflife);
	void StartLHandRumble(std::string_view tag, Actor& actor, float power, float halflife);
	void StopRHandRumble(std::string_view tag, Actor& actor);
	void StopLHandRumble(std::string_view tag, Actor& actor);
}
