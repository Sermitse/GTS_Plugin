#pragma once
// Module that holds data that is not persistent across saves
#include "events.hpp"
#include "spring.hpp"

using namespace std;
using namespace SKSE;
using namespace RE;

namespace Gts {
	struct TempActorData {
		float base_height;
		float base_volume;
		float char_weight;
		float shoe_weight;
		float fall_start;
		float last_set_fall_start;
		float health_boost;
		float SMT_Bonus_Duration;
		float SMT_Penalty_Duration;
		float carryweight_boost;
		float FallTimer;
		float Hug_AnimSpeed;
		float Throw_Speed;
		float potion_max_size;
		float buttcrush_max_size;
		float buttcrush_start_scale;
		float SizeVulnerability;
		float push_force;
		float otherScales;
		float vore_recorded_scale;
		float WorldFov_Default;
		float FpFov_Default;
		float ButtCrushGrowthAmount;
		float MovementSlowdown;
		float ShrinkResistance;
		float MightValue;

		float Shrink_Ticks;
		float Shrink_Ticks_Calamity;

		float Perk_BonusActionSpeed;
		float Perk_lifeForceStolen;
		int   Perk_lifeForceStacks;

		int CrushedTinies;

		NiPoint3 BoundingBox_Cache;
		
		bool Throw_WasThrown;
		bool is_teammate;
		bool can_do_vore;
		bool can_be_crushed;
		bool being_held;
		bool is_between_breasts;
		bool about_to_be_eaten;
		bool dragon_was_eaten;
		bool can_be_vored;
		bool being_foot_grinded;
		bool SMT_ReachedFullSpeed;
		bool OverrideCamera;
		bool WasReanimated;
		bool FPCrawling;
		bool FPProning;
		bool Overkilled;
		bool Protection;
		bool GrowthPotion;

		bool Devourment_Devoured;
		bool Devourment_Eaten;
		

		bool disable_collision;
		bool was_sneaking;

		float IsNotImmune;

		NiPoint3 POS_Last_Leg_L;
		NiPoint3 POS_Last_Leg_R;
		NiPoint3 POS_Last_Hand_L;
		NiPoint3 POS_Last_Hand_R;

		float shrink_until;

		Actor* IsInControl;
		std::vector<Actor*> shrinkies;

		TESObjectREFR* disable_collision_with;
		TESObjectREFR* Throw_Offender;

		AttachToNode AttachmentNode;
		
		float rip_lastScale;
		float rip_offset;
	};

	class Transient : public EventListener {
		public:
			[[nodiscard]] static Transient& GetSingleton() noexcept;

			TempActorData* GetData(TESObjectREFR* object);
			TempActorData* GetActorData(Actor* actor);
			std::vector<FormID> GetForms();

			virtual std::string DebugName() override;
			virtual void Update() override;
			virtual void Reset() override;
			virtual void ResetActor(Actor* actor) override;
		private:

			mutable std::mutex _lock;
			std::unordered_map<FormID, TempActorData> _actor_data;
	};
}
