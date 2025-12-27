#pragma once

namespace GTS {

	Actor* GetActorPtr(Actor* actor);

	Actor* GetActorPtr(Actor& actor);

	Actor* GetActorPtr(ActorHandle& actor);

	Actor* GetActorPtr(const ActorHandle& actor);

	Actor* GetActorPtr(FormID formId);

	Actor* GetCharContActor(bhkCharacterController* charCont);

	//Function that creates a fake throw vector using Actor3D instead of bones
	bool CalculateThrowDirection(RE::Actor* a_ActorGiant, RE::Actor* a_ActorTiny, RE::NiPoint3& a_From, RE::NiPoint3& a_To, float a_HorizontalAngleOffset, float a_VerticalAngleOffset);

	RE::NiPoint3 RotateAngleAxis(const RE::NiPoint3& vec, const float angle, const RE::NiPoint3& axis);

	void SetSneaking(Actor* actor, bool override_sneak, int enable);

	void SetReanimatedState(Actor* actor);

	//Disables idle greetings by not allowing the timer to expire
	void ShutUp(Actor* actor);

	void DecreaseShoutCooldown(Actor* giant);

	void Disintegrate(Actor* actor);

	void SetRestrained(Actor* actor);

	void SetUnRestrained(Actor* actor);

	void SetDontMove(Actor* actor);

	void SetMove(Actor* actor);

	void KnockAreaEffect(TESObjectREFR* source, float afMagnitude, float afRadius);

	void ForceRagdoll(Actor* actor, bool forceOn);

	std::vector<hkpRigidBody*> GetActorRB(Actor* actor);

	void PushActorAway(Actor* source_act, Actor* receiver, float force);

	void ApplyManualHavokImpulse(Actor* target, float afX, float afY, float afZ, float Multiplier);

	float get_distance_to_actor(Actor* receiver, Actor* target);

	void DisableCollisions(Actor* actor, TESObjectREFR* otherActor);

	void EnableCollisions(Actor* actor);


	hkaRagdollInstance* GetRagdoll(Actor* actor);

	void DisarmActor(Actor* tiny, bool drop);

	void ManageRagdoll(Actor* tiny, float deltaLength, NiPoint3 deltaLocation, NiPoint3 targetLocation);

	void ChanceToScare(Actor* giant, Actor* tiny, float duration, int random, bool apply_sd);

	void StaggerActor(Actor* receiver, float power);

	void StaggerActor(Actor* giant, Actor* tiny, float power);

	void StaggerActor_Around(Actor* giant, const float radius, bool launch);

	void PushForward(Actor* giantref, Actor* tinyref, float power);

	void PushTowards(Actor* giantref, Actor* tinyref, std::string_view bone, float power, bool sizecheck);

	void PushTowards_Task(const ActorHandle& giantHandle, const ActorHandle& tinyHandle, const NiPoint3& startCoords, const NiPoint3& endCoords, std::string_view TaskName, float power, bool sizecheck);

	void PushTowards(Actor* giantref, Actor* tinyref, NiAVObject* bone, float power, bool sizecheck);

	void StaggerOr(Actor* giant, Actor* tiny, float afX, float afY, float afZ, float afMagnitude);

	void Utils_PushCheck(Actor* giant, Actor* tiny, float force);

}