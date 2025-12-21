#include "RE/A/Actor.hpp"

namespace RE {

	void SetCriticalStage(Actor* a_actor, RE::ACTOR_CRITICAL_STAGE a_stage) {
		if (a_stage < ACTOR_CRITICAL_STAGE::kTotal && a_stage >= ACTOR_CRITICAL_STAGE::kNone) {
			typedef void (*DefSetCriticalStage)(Actor* actor, RE::ACTOR_CRITICAL_STAGE stage);
			static const REL::Relocation<DefSetCriticalStage> func{ REL::RelocationID(36607, 37615, NULL) };
			func(a_actor, a_stage);
		}
	}

	void Attacked(Actor* a_victim, Actor* a_aggressor) {
		typedef void (*DefAttacked)(Actor*, Actor*);
		static const REL::Relocation<DefAttacked> SkyrimAttacked{ REL::RelocationID(37672, 38626, NULL) };
		SkyrimAttacked(a_victim, a_aggressor);
	}

	// This function starts combat and adds the assault bounty.
	void StartCombat(Actor* a_victim, Actor* a_aggressor) {
		typedef void (*DefStartCombat)(Actor*, Actor*, Actor*, Actor*);
		REL::Relocation<DefStartCombat> func{ REL::RelocationID(36430, 37425, NULL) }; // sub_1405DE870 : 36430  (SE) ; 1406050c0 : 37425 (AE)
		func(a_victim, a_aggressor, a_aggressor, a_aggressor);                         // Called from Attacked above at some point
	}

	// Applies the correct amount of damage and kills actors properly.
	void ApplyDamage(Actor* a_source, Actor* a_target, float damage) { 
		typedef void (*DefApplyDamage)(Actor*, float, Actor*, HitData*, TESObjectREFR*);
		REL::Relocation<DefApplyDamage> func{ REL::RelocationID(36345, 37335, NULL) };
		func(a_target, damage, a_source, nullptr, nullptr);
	}

	void StaggerActor_Directional(Actor* a_source, float a_power, Actor* a_target) {
		//SE: 1405FA1B0 : 36700 (Character *param_1,float param_2,Actor *param_3)
		//AE: 140621d80 : 37710
		typedef void (*DefStaggerActor_Directional)(Actor*, float, Actor*);
		REL::Relocation<DefStaggerActor_Directional> func{ REL::RelocationID(36700, 37710, NULL) };
		func(a_target, a_power, a_source);
	}


	ACTOR_COMBAT_STATE GetCombatState(Actor* a_actor) {
		using func_t = decltype(GetCombatState);
		REL::Relocation<func_t> func{ REL::RelocationID(37603, 38556, NULL) };
		return func(a_actor);
	}

	bool IsMoving(Actor* a_actor) {
		using func_t = decltype(&IsMoving);
		REL::Relocation<func_t> func{ REL::RelocationID(36928, 37953, NULL) };
		return func(a_actor);
	}

	std::int16_t GetItemCount(InventoryChanges* a_changes, RE::TESBoundObject* a_obj){
		using func_t = decltype(&GetItemCount);
		REL::Relocation<func_t> func{REL::RelocationID(15868, 16108, NULL) };
		return func(a_changes, a_obj);
	}

}