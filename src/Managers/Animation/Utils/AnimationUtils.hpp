#pragma once

namespace GTS {

	enum class FollowerAnimType {
		ThighSandwich,
		ButtCrush,
		Grab,
		Hugs,
		Vore,
	};

	void Task_ApplyAbsorbCooldown(Actor* giant);
	void RestoreBreastAttachmentState(Actor* giant, Actor* tiny);
	void Anims_FixAnimationDesync(Actor* giant, Actor* tiny, bool reset);
	void ForceFollowerAnimation(Actor* giant, FollowerAnimType Type);
	void Vore_AttachToRightHandTask(Actor* giant, Actor* tiny);

	void UpdateFriendlyHugs(Actor* giant, Actor* tiny, bool force);

	bool Vore_ShouldAttachToRHand(Actor* giant, Actor* tiny);

	void HugCrushOther(Actor* giant, Actor* tiny);

	void AbortHugAnimation(Actor* giant, Actor* tiny);
	void Utils_UpdateHugBehaviors(Actor* giant, Actor* tiny);
	void Utils_UpdateHighHeelBlend(Actor* giant, bool reset);
	void Task_HighHeel_SyncVoreAnim(Actor* giant);
	void StartHealingAnimation(Actor* giant, Actor* tiny);

	void AllowToDoVore(Actor* actor, bool toggle);
	void AllowToBeCrushed(Actor* actor, bool toggle);
	void ManageCamera(Actor* giant, bool enable, CameraTracking type);
	void ApplyButtCrushCooldownTask(Actor* giant);

	void LaunchTask(Actor* actor, float radius, float power, FootEvent kind);

	void DoLaunch(Actor* giant, float radius, float power, FootEvent kind);
	void DoLaunch(Actor* giant, float radius, float power, NiAVObject* node);

	void GrabStaminaDrain(Actor* giant, Actor* tiny, float sizedifference);
	void DrainStamina(Actor* giant, std::string_view TaskName, const RuntimeData::RuntimeEntry<RE::BGSPerk>& perk, bool enable, float power);

	void SpawnHurtParticles(Actor* giant, Actor* grabbedActor, float mult, float dustmult);

	void AdjustFacialExpression(Actor* giant, int ph, float target, CharEmotionType Type, float phenome_halflife = 0.08f, float modifier_speed = 0.25f);

	float GetWasteMult(Actor* giant);
	float GetPerkBonus_Basics(Actor* Giant);
	float GetPerkBonus_Thighs(Actor* Giant);

	void DoFootGrind(Actor* giant, Actor* tiny, bool Right);
	void DoFingerGrind(Actor* giant, Actor* tiny);
	void FingerGrindCheck(Actor* giant, CrawlEvent kind, bool Right, float radius);
	void FootGrindCheck(Actor* actor, float radius, bool Right, FootActionType Type);

	void DoDamageAtPoint_Cooldown(Actor* giant, float radius, float damage, NiAVObject* node, NiPoint3 NodePosition, float random, float bbmult, float crushmult, float pushpower, DamageSource Cause);
	void ApplyThighDamage(Actor* actor, bool right, bool CooldownCheck, float radius, float damage, float bbmult, float crush_threshold, int random, DamageSource Cause);
	void ApplyFingerDamage(Actor* giant, float radius, float damage, NiAVObject* node, float random, float bbmult, float crushmult, float Shrink, DamageSource Cause);

	void GetThighCoordinates(Actor* giant, std::string_view calf, std::string_view feet, std::string_view thigh, std::vector<NiPoint3>& outCoordinates);
	std::vector<NiPoint3> GetFootCoordinates(Actor* actor, bool Right, bool ignore_rotation);
	NiPoint3 GetHeartPosition(Actor* giant, Actor* tiny, bool hugs);

	void AbsorbShout_BuffCaster(Actor* giantref, Actor* tinyref);
	void Task_TrackSizeTask(Actor* giant, Actor* tiny, std::string_view naming, bool check_ticks, float time_mult);
	void Task_FacialEmotionTask_OpenMouth(Actor* giant, float duration, std::string_view naming, float duration_add = 0.0f, float speed_mult = 1.0f);
	void Task_FacialEmotionTask_Moan(Actor* giant, float duration, std::string_view naming, float duration_add = 0.0f);
	void Task_FacialEmotionTask_SlightSmile(Actor* giant, float duration, std::string_view naming, float duration_add);
	void Task_FacialEmotionTask_Smile(Actor* giant, float duration, std::string_view naming, float duration_add = 0.0f, float open_mouth = 0.0f);
	void Task_FacialEmotionTask_Anger(Actor* giant, float duration, std::string_view naming, float duration_add);
	void Task_FacialEmotionTask_Kiss(Actor* giant, float duration, std::string_view naming, float duration_add = 0.0f);

	void Laugh_Chance(Actor* giant, Actor* otherActor, float multiply, std::string_view name);
	void Laugh_Chance(Actor* giant, float multiply, std::string_view name);

	float GetHugStealRate(Actor* actor);
	float GetHugShrinkThreshold(Actor* actor);
	float GetHugCrushThreshold(Actor* giant, Actor* tiny, bool check_size);

	bool SetCrawlAnimation(Actor* a_actor, const bool a_state);
	void UpdateCrawlAnimations(Actor* a_actor, bool a_state);

	void SetAltFootStompAnimation(RE::Actor* a_actor, const bool a_state);
	void SetEnableSneakTransition(RE::Actor* a_actor, const bool a_state);
	
}
