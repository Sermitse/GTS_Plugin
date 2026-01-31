#pragma once

// Module that handles high heels

namespace GTS {

	struct HHData {
		Spring multiplier = Spring(1.0f, 0.5f); // Used to smoothly disable/enable model .z offset of heels
		bool wasWearingHh = false;
		NiPoint3 lastBaseHHOffset;
		float InitialHeelHeight; // initial heel height, used in calculating damage bonus
	};

	class HighHeelManager : public EventListener, public CInitSingleton <HighHeelManager> {
		public:
		virtual std::string DebugName() override;
		virtual void HavokUpdate() override;
		virtual void ActorEquip(Actor* actor) override;
		virtual void ActorLoaded(Actor* actor) override;
		virtual void OnAddPerk(const AddPerkEvent& evt) override;

		static bool IsWearingHH(Actor* actor); // Checks if GetBaseHHOffset().Length() > 1e-4
		static void UpdateHHOffset(Actor* actor);
		static NiPoint3 GetBaseHHOffset(Actor* actor); // Unscaled HH as read from the shoe data
		static NiPoint3 GetHHOffset(Actor* actor); // Scaled HH
		static float GetHHMultiplier(Actor* actor); // get current 'height' multiplier of HH for dll to smoothly enable/disable hh visual .z offset
		static float GetInitialHeelHeight(Actor* actor); // Get base heel height value, used for calculating damage bonus

		void ApplyHH(Actor* actor, bool force);

		std::unordered_map<Actor*, HHData> data;
	};
}
