#include "scale/modscale.hpp"
#include "data/transient.hpp"
#include "data/runtime.hpp"
#include "spring.hpp"
#include "node.hpp"

using namespace SKSE;
using namespace RE;


namespace Gts {
	Transient& Transient::GetSingleton() noexcept {
		static Transient instance;
		return instance;
	}

	TempActorData* Transient::GetData(TESObjectREFR* object) {
		if (!object) {
			return nullptr;
		}
		auto key = object->formID;
		TempActorData* result = nullptr;
		try {
			result = &this->_actor_data.at(key);
		} catch (const std::out_of_range& oor) {
			return nullptr;
		}
		return result;
	}

	TempActorData* Transient::GetActorData(Actor* actor) {
		std::unique_lock lock(this->_lock);
		if (!actor) {
			return nullptr;
		}
		auto key = actor->formID;
		try {
			auto no_discard = this->_actor_data.at(key);
		} catch (const std::out_of_range& oor) {
			// Try to add
			if (!actor) {
				return nullptr;
			}
			TempActorData result;
			auto bound_values = get_bound_values(actor);
			auto scale = get_scale(actor);
			if (scale < 0.0) {
				log::info("Scale of {} is < 0", actor->GetDisplayFullName());
				return nullptr;
			}
			float base_height_unit = bound_values[2] * scale;
			float base_height_meters = unit_to_meter(base_height_unit);
			float fall_start = actor->GetPosition()[2];
			float last_set_fall_start = fall_start;
			float carryweight_boost = 0.0;
			float health_boost = 0.0;
			float SMT_Bonus_Duration = 0.0;
			float SMT_Penalty_Duration = 0.0;
			float FallTimer = 1.0;
			float Hug_AnimSpeed = 1.0;
			float Throw_Speed = 0.0;

			float potion_max_size = 0.0;
			float buttcrush_max_size = 0.0;
			float buttcrush_start_scale = 0.0;

			float SizeVulnerability = 0.0;

			float push_force = 1.0;
			
			bool Throw_WasThrown = false;

			bool can_do_vore = true;
			bool can_be_crushed = true;
			bool dragon_was_eaten = false;
			bool can_be_vored = true;
			bool being_held = false;
			bool is_between_breasts = false;
			bool about_to_be_eaten = false;
			bool being_foot_grinded = false;
			bool SMT_ReachedFullSpeed = false;
			bool OverrideCamera = false;
			bool WasReanimated = false;
			bool FPCrawling = false;
			bool FPProning = false;
			bool Overkilled = false;
			bool Protection = false;
			bool GrowthPotion = false;

			bool Devourment_Devoured = false;
			bool Devourment_Eaten = false;

			bool disable_collision = false;
			bool was_sneaking = false;

			float IsNotImmune = 1.0;

			float rip_lastScale = 1.0;

			NiPoint3 POS_Last_Leg_L = NiPoint3(0.0, 0.0, 0.0);
			NiPoint3 POS_Last_Leg_R = NiPoint3(0.0, 0.0, 0.0);
			NiPoint3 POS_Last_Hand_L = NiPoint3(0.0, 0.0, 0.0);
			NiPoint3 POS_Last_Hand_R = NiPoint3(0.0, 0.0, 0.0);

			float shrink_until = 0.0;

			Actor* IsInControl = nullptr;

			std::vector<Actor*> shrinkies = {};

			TESObjectREFR* disable_collision_with = nullptr;
			TESObjectREFR* Throw_Offender = nullptr;

			AttachToNode AttachmentNode = AttachToNode::None;

			float otherScales = 1.0;
			float vore_recorded_scale = 1.0;
			float WorldFov_Default = 0;
			float FpFov_Default = 0;
			float ButtCrushGrowthAmount = 0;
			float MovementSlowdown = 1.0;
			float ShrinkResistance = 0.0;
			float MightValue = 0.0;
			float Shrink_Ticks = 0.0;
			float Shrink_Ticks_Calamity = 0.0;
			

			float Perk_BonusActionSpeed = 1.0;
			float Perk_lifeForceStolen = 0.0;
			int Perk_lifeForceStacks = 0;

			int CrushedTinies = 0;

			NiPoint3 BoundingBox_Cache = get_bound_values(actor); // Default Human Height

			// Volume scales cubically
			float base_volume = bound_values[0] * bound_values[1] * bound_values[2] * scale * scale * scale;
			float base_volume_meters = unit_to_meter(base_volume);

			const float rip_initScale = -1.0;

			result.base_height = base_height_meters;
			result.base_volume = base_volume_meters;

			auto shoe = actor->GetWornArmor(BGSBipedObjectForm::BipedObjectSlot::kFeet);
			float shoe_weight = 1.0;
			if (shoe) {
				shoe_weight = shoe->weight;
			}
			result.shoe_weight = shoe_weight;
			result.char_weight = actor->GetWeight();
			result.fall_start = fall_start;
			result.last_set_fall_start = last_set_fall_start;
			result.carryweight_boost = carryweight_boost;
			result.health_boost = health_boost;
			result.SMT_Bonus_Duration = SMT_Bonus_Duration;
			result.SMT_Penalty_Duration = SMT_Penalty_Duration;
			result.FallTimer = FallTimer;
			result.Hug_AnimSpeed = Hug_AnimSpeed;
			result.Throw_Speed = Throw_Speed;
			result.potion_max_size = potion_max_size;
			result.buttcrush_max_size = buttcrush_max_size;
			result.buttcrush_start_scale = buttcrush_start_scale;
			result.SizeVulnerability = SizeVulnerability;

			result.push_force = push_force;
			result.can_do_vore = can_do_vore;
			result.Throw_WasThrown = Throw_WasThrown;
			result.can_be_crushed = can_be_crushed;
			result.being_held = being_held;
			result.is_between_breasts = is_between_breasts;
			result.about_to_be_eaten = about_to_be_eaten;
			result.being_foot_grinded = being_foot_grinded;
			result.SMT_ReachedFullSpeed = SMT_ReachedFullSpeed;
			result.dragon_was_eaten = dragon_was_eaten;
			result.can_be_vored = can_be_vored;
			result.disable_collision_with = disable_collision_with;
			result.Throw_Offender = Throw_Offender;
			result.AttachmentNode = AttachmentNode;
			result.otherScales = otherScales;
			result.vore_recorded_scale = vore_recorded_scale;
			result.WorldFov_Default = WorldFov_Default;
			result.FpFov_Default = FpFov_Default;
			result.ButtCrushGrowthAmount = ButtCrushGrowthAmount;
			result.MovementSlowdown = MovementSlowdown;
			result.ShrinkResistance = ShrinkResistance;
			result.MightValue = MightValue;
			result.Shrink_Ticks = Shrink_Ticks;
			result.Shrink_Ticks_Calamity = Shrink_Ticks_Calamity;

			result.Perk_BonusActionSpeed = Perk_BonusActionSpeed;
			result.Perk_lifeForceStolen = Perk_lifeForceStolen;
			result.Perk_lifeForceStacks = Perk_lifeForceStacks;

			result.CrushedTinies = CrushedTinies;

			result.BoundingBox_Cache = BoundingBox_Cache;

			result.OverrideCamera = OverrideCamera;
			result.WasReanimated = WasReanimated;
			result.FPCrawling = FPCrawling;
			result.FPProning = FPProning;
			result.Overkilled = Overkilled;
			result.Protection = Protection;
			result.GrowthPotion = GrowthPotion;

			result.Devourment_Devoured = Devourment_Devoured;
			result.Devourment_Eaten = Devourment_Eaten;

			result.disable_collision = disable_collision;
			result.was_sneaking = was_sneaking;

			result.IsNotImmune = IsNotImmune;

			result.POS_Last_Leg_L = POS_Last_Leg_L;
			result.POS_Last_Leg_R = POS_Last_Leg_R;
			result.POS_Last_Hand_L = POS_Last_Hand_L;
			result.POS_Last_Hand_R = POS_Last_Hand_R;

			result.shrinkies = shrinkies;
			result.shrink_until = shrink_until;

			result.IsInControl = IsInControl;
			
			result.is_teammate = actor->formID != 0x14 && actor->IsPlayerTeammate();

			result.rip_lastScale = rip_initScale;
			result.rip_offset = rip_initScale;

			this->_actor_data.try_emplace(key, result);
		}
		return &this->_actor_data[key];
	}

	std::vector<FormID> Transient::GetForms() {
		std::vector<FormID> keys;
		keys.reserve(this->_actor_data.size());
		for(auto kv : this->_actor_data) {
			keys.push_back(kv.first);
		}
		return keys;
	}


	std::string Transient::DebugName() {
		return "Transient";
	}

	void Transient::Update() {
		for (auto actor: find_actors()) {
			if (!actor) {
				continue;
			}
			if (!actor->Is3DLoaded()) {
				continue;
			}

			auto key = actor->formID;
			std::unique_lock lock(this->_lock);
			try {
				auto data = this->_actor_data.at(key);
				auto shoe = actor->GetWornArmor(BGSBipedObjectForm::BipedObjectSlot::kFeet);
				float shoe_weight = 1.0;
				if (shoe) {
					shoe_weight = shoe->weight;
				}
				data.shoe_weight = shoe_weight;

				data.char_weight = actor->GetWeight();

				data.is_teammate = actor->formID != 0x14 && actor->IsPlayerTeammate();


			} catch (const std::out_of_range& oor) {
				continue;
			}
		}
	}
	void Transient::Reset() {
		log::info("Transient was reset");
		std::unique_lock lock(this->_lock);
		this->_actor_data.clear();
	}
	void Transient::ResetActor(Actor* actor) {
		std::unique_lock lock(this->_lock);
		if (actor) {
			auto key = actor->formID;
			this->_actor_data.erase(key);
		}
	}
}
