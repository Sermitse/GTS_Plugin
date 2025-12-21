#pragma once

#include "RE/A/Actor.h"
#include "RE/T/TESCombatEvent.h"
#include "RE/T/TESObjectREFR.h"

namespace RE {
	
	void SetCriticalStage(Actor* a_actor, ACTOR_CRITICAL_STAGE a_stage);
	void Attacked(Actor* a_victim, Actor* a_aggressor);
	void StartCombat(Actor* a_victim, Actor* a_aggressor);
	void ApplyDamage(Actor* a_source, Actor* a_target, float damage);
	void SetObjectRotation_X(TESObjectREFR* ref, float X);
	void StaggerActor_Directional(Actor* a_source, float a_power, Actor* a_target);
	ACTOR_COMBAT_STATE GetCombatState(Actor* a_actor);
	bool IsMoving(Actor* a_actor);
	std::int16_t GetItemCount(InventoryChanges* a_changes, RE::TESBoundObject* a_obj);

}
