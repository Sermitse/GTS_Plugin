#pragma once

namespace GTS {

	Actor* GetActorPtr(Actor* a_actor);
	Actor* GetActorPtr(Actor& a_actor);
	Actor* GetActorPtr(ActorHandle& a_actor);
	Actor* GetActorPtr(const ActorHandle& a_actor);
	Actor* GetActorPtr(FormID a_formID);
	std::vector<hkpRigidBody*> GetActorRB(Actor* a_actor);
	hkaRagdollInstance* GetRagdoll(Actor* a_actor);
	Actor* GetCharContActor(bhkCharacterController* a_charController);

	bool CalculateThrowDirection(RE::Actor* a_ActorGiant, RE::Actor* a_ActorTiny, RE::NiPoint3& a_From, RE::NiPoint3& a_To, float a_HorizontalAngleOffset, float a_VerticalAngleOffset);
	RE::NiPoint3 RotateAngleAxis(const RE::NiPoint3& a_vec, const float a_angle, const RE::NiPoint3& a_axis);

	void SetSneaking(Actor* a_target, bool a_doOverrideSneak, int a_sneakState);
	void SetReanimatedState(Actor* a_target);
	void SetRestrained(Actor* a_actor);
	void SetUnRestrained(Actor* a_actor);
	void SetDontMove(Actor* a_actor);
	void SetMove(Actor* a_actor);

	void KnockAreaEffect(TESObjectREFR* a_sourceRef, float a_magnitude, float a_radius);
	void ForceRagdoll(Actor* a_target, bool a_enableRagDoll);
	void ApplyManualHavokImpulse(Actor* a_target, float a_forceX, float a_forceY, float a_forceZ, float a_multiplier);

	void DisableCollisions(Actor* a_actor, TESObjectREFR* a_target);
	void EnableCollisions(Actor* a_actor);

	void StaggerActor(Actor* a_target, float a_power);
	void StaggerActor(Actor* a_source, Actor* a_target, float a_power);
	void StaggerActor_Around(Actor* a_source, const float a_radius, bool a_doLaunch);
	void StaggerOr(Actor* a_source, Actor* a_target);

	void PushActorAway(Actor* a_source, Actor* a_receiver, float a_force);
	void PushForward(Actor* a_source, Actor* a_target, float a_power);
	void PushTowards(Actor* a_source, Actor* a_target, std::string_view a_boneName, float a_power, bool a_doSizeCheck);
	void PushTowards_Task(const ActorHandle& a_sourceHandle, const ActorHandle& a_targetHandle, const NiPoint3& a_startCoords, const NiPoint3& a_endCoords, std::string_view a_taskName, float a_power, bool a_doSizeCheck);
	void PushTowards(Actor* a_source, Actor* a_target, NiAVObject* a_bone, float a_power, bool a_doSizeCheck);
	void Utils_PushCheck(Actor* giant, Actor* tiny, float force);

	void DisarmActor(Actor* a_target, bool a_dropWeapon);
	void ManageRagdoll(Actor* a_actor, float a_deltaLength, NiPoint3 a_deltaLocation, NiPoint3 a_targetLocation);
	void ChanceToScare(Actor* a_giant, Actor* a_tiny, float a_duration, int a_random, bool a_useSizeDifference);
	void ShutUp(Actor* a_target);
	void DecreaseShoutCooldown(Actor* a_target);
	void Disintegrate(Actor* a_target);

}