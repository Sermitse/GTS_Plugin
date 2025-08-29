#pragma once

namespace GTS {

	enum class AnimationCondition {
		kHugs = 0,
		kStompsAndKicks = 1,
		kGrabAndSandwich = 2,
		kVore = 3,
		kOthers = 4,
	};

	float GetPerkBonus_OnTheEdge(Actor* giant, float amt);

	[[nodiscard]] NiPoint3 RotateAngleAxis(const NiPoint3& vec, const float angle, const NiPoint3& axis);

	Actor* GetActorPtr(Actor* actor);

	Actor* GetActorPtr(Actor& actor);

	Actor* GetActorPtr(ActorHandle& actor);

	Actor* GetActorPtr(const ActorHandle& actor);

	Actor* GetActorPtr(FormID formId);

	Actor* GetCharContActor(bhkCharacterController* charCont);

	void Task_AdjustHalfLifeTask(Actor* tiny, float halflife, double revert_after);

	void StartResetTask(Actor* tiny);

	// GTS State Bools
	void Potion_SetMightBonus(Actor* giant, float value, bool add);
	float Potion_GetMightBonus(Actor* giant);

	float Potion_GetSizeMultiplier(Actor* giant);

	void Potion_ModShrinkResistance(Actor* giant, float value);
	void Potion_SetShrinkResistance(Actor* giant, float value);
	float Potion_GetShrinkResistance(Actor* giant);

	void Potion_SetUnderGrowth(Actor* actor, bool set);
	bool Potion_IsUnderGrowthPotion(Actor* actor);

	bool CalculateThrowDirection(RE::Actor* a_ActorGiant, RE::Actor* a_ActorTiny, RE::NiPoint3& a_From, RE::NiPoint3& a_To, float a_HorizontalAngleOffset = 0.0f, float a_VerticalAngleOffset = 0.0f);
	bool IsInsect(Actor* actor, bool performcheck);
	bool IsFemale(Actor* a_Actor, bool AllowOverride = false);
	bool IsDragon(Actor* actor);
	bool IsGiant(Actor* actor);
	bool IsMammoth(Actor* actor);
	bool IsLiving(Actor* actor);
	bool IsUndead(Actor* actor, bool PerformCheck);
	bool WasReanimated(Actor* actor);
	bool IsFlying(Actor* actor);
	bool IsHostile(Actor* giant, Actor* tiny);
	bool CanPerformAnimationOn(Actor* giant, Actor* tiny, bool HugCheck);
	bool IsEssential(Actor* giant, Actor* actor);
	bool IsHeadtracking(Actor* giant);
	bool AnimationsInstalled(Actor* giant);
	bool IsInGodMode(Actor* giant);
	bool IsFreeCameraEnabled();
	bool IsDebugEnabled();
	bool CanDoDamage(Actor* giant, Actor* tiny, bool HoldCheck);

	void Attachment_SetTargetNode(Actor* giant, AttachToNode Node);
	AttachToNode Attachment_GetTargetNode(Actor* giant);

	void SetBusyFoot(Actor* giant, BusyFoot Foot);
	BusyFoot GetBusyFoot(Actor* giant);
	
	void ControlAnother(Actor* target, bool reset);
	Actor* GetPlayerOrControlled();

	void RecordSneaking(Actor* actor);
	void SetSneaking(Actor* actor, bool override_sneak, int enable);

	void SetWalking(Actor* actor, int enable);

	// Gts Bools end

	// GTS Actor Functions
	float GetDamageSetting();
	float GetFallModifier(Actor* giant);

	std::vector<Actor*> GetMaxActionableTinyCount(Actor* giant, const std::vector<Actor*>& actors);

	float Ench_Aspect_GetPower(Actor* giant);
	float Ench_Hunger_GetPower(Actor* giant);

	float Perk_GetSprintShrinkReduction(Actor* actor);

	float GetDamageResistance(Actor* actor);
	float GetDamageMultiplier(Actor* actor);

	float GetSizeDifference(Actor* giant, Actor* tiny, SizeType Type, bool Check_SMT, bool HH);
	float GetActorGTSWeight(Actor* giant);
	float GetActorGTSHeight(Actor* giant);
	float GetSizeFromBoundingBox(Actor* tiny);
	float GetRoomStateScale(Actor* giant);
	float GetProneAdjustment();

	float GetPerkBonus_OnTheEdge(Actor* giant, float amt);

	void override_actor_scale(Actor* giant, float amt, SizeEffectType type);
	void update_target_scale(Actor* giant, float amt, SizeEffectType type);
	float get_update_target_scale(Actor* giant, float amt, SizeEffectType type);

	void SpawnActionIcon(Actor* giant);
	// End

	// GTS State Controllers
	void SetBeingHeld(Actor* tiny, bool enable);
	void SetProneState(Actor* giant, bool enable);
	void SetBetweenBreasts(Actor* actor, bool enable);
	void SetBeingEaten(Actor* tiny, bool enable);
	void SetBeingGrinded(Actor* tiny, bool enable);
	void SetCameraOverride(Actor* actor, bool enable);

	bool IsUsingAlternativeStomp(Actor* giant);

	void SetReanimatedState(Actor* actor);
	void ShutUp(Actor* actor);

	// GTS State Controllers end
	void PlayAnimation(Actor* actor, std::string_view animName);

	void DecreaseShoutCooldown(Actor* giant);

	void Disintegrate(Actor* actor);
	void UnDisintegrate(Actor* actor);

	void SetRestrained(Actor* actor);
	void SetUnRestrained(Actor* actor);

	void SetDontMove(Actor* actor);
	void SetMove(Actor* actor);

	void ForceRagdoll(Actor* actor, bool forceOn);

	std::vector<hkpRigidBody*> GetActorRB(Actor* actor);
	void PushActorAway(Actor* source_act, Actor* receiver, float force);
	void KnockAreaEffect(TESObjectREFR* source, float afMagnitude, float afRadius);
	void ApplyManualHavokImpulse(Actor* target, float afX, float afY, float afZ, float Multiplier);

	void CompleteDragonQuest(Actor* tiny, ParticleType Type, bool dead);

	float get_distance_to_actor(Actor* receiver, Actor* target);
	float GetHighHeelsBonusDamage(Actor* actor, bool multiply);
	float GetHighHeelsBonusDamage(Actor* actor, bool multiply, float adjust);

	void EnableFreeCamera();

	bool DisallowSizeDamage(Actor* giant, Actor* tiny);
	bool AllowDevourment();
	bool AllowCameraTracking();
	bool LessGore();

	bool IsTeammate(Actor* actor);
	bool EffectsForEveryone(Actor* giant);
	
	bool BehaviorGraph_DisableHH(Actor* actor);
	bool IsEquipBusy(Actor* actor);
	
	bool IsRagdolled(Actor* actor);
	bool IsGrowing(Actor* actor);
	bool IsChangingSize(Actor* actor);

	bool IsFootGrinding(Actor* actor);
	bool IsProning(Actor* actor);
	bool IsCrawling(Actor* actor);
	bool IsHugCrushing(Actor* actor);
	bool IsHugHealing(Actor* actor);
	bool IsVoring(Actor* giant);
	bool IsHuggingFriendly(Actor* actor);
	bool IsTransitioning(Actor* actor);
	bool IsJumping(Actor* actor);
	bool IsBeingHeld(Actor* giant, Actor* tiny);
	bool IsBetweenBreasts(Actor* actor);
	bool IsTransferingTiny(Actor* actor);
	bool IsUsingThighAnimations(Actor* actor);
	bool IsSynced(Actor* actor);
	bool CanDoPaired(Actor* actor);
	bool IsThighCrushing(Actor* actor);
	bool IsThighSandwiching(Actor* actor);
	bool IsBeingEaten(Actor* tiny);
	bool IsGtsBusy(Actor* actor);

	bool IsStomping(Actor* actor);
	bool IsInCleavageState(Actor* actor);
	bool IsCleavageZIgnored(Actor* actor);
	bool IsInsideCleavage(Actor* actor);
	bool IsKicking(Actor* actor);
	bool IsTrampling(Actor* actor);

	bool CanDoCombo(Actor* actor);
	bool IsCameraEnabled(Actor* actor);
	bool IsCrawlVoring(Actor* actor);
	bool IsButtCrushing(Actor* actor);
	bool ButtCrush_IsAbleToGrow(Actor* actor, float limit);
	bool IsBeingGrinded(Actor* actor);
	bool IsHugging(Actor* actor);
	bool IsBeingHugged(Actor* actor); 
	bool CanDoButtCrush(Actor* actor, bool apply_cooldown);
	bool GetCameraOverride(Actor* actor);
	// GTS State Bools End

	// Gts Bools
	bool IsGrowthSpurtActive(Actor* actor);
	bool HasGrowthSpurt(Actor* actor);
	bool InBleedout(Actor* actor);
	bool AllowStagger(Actor* giant, Actor* tiny);
	bool IsMechanical(Actor* actor);
	bool IsHuman(Actor* actor);
	bool IsBlacklisted(Actor* actor);

    bool IsGtsTeammate(Actor* actor);

	void ResetCameraTracking(Actor* actor);
	void CallDevourment(Actor* a_Pred, Actor* a_Prey);
	void GainWeight(Actor* giant, float value);
	void CallVampire();
	void AddCalamityPerk();
	void RemoveCalamityPerk();
	void AddPerkPoints(float a_Level);

	float GetStolenAttributeCap(Actor* giant);
	void AddStolenAttributes(Actor* giant, float value);
	void AddStolenAttributesTowards(Actor* giant, ActorValue type, float value);
	float GetStolenAttributes_Values(Actor* giant, ActorValue type);
	float GetStolenAttributes(Actor* giant);
	void DistributeStolenAttributes(Actor* giant, float value);

	float GetRandomBoost();
	
	float GetButtCrushCost(Actor* actor, bool DoomOnly);
	float Perk_GetCostReduction(Actor* giant);
	float GetAnimationSlowdown(Actor* giant);

	void DoFootstepSound(Actor* giant, float modifier, FootEvent kind, std::string_view node);
	void DoDustExplosion(Actor* giant, float modifier, FootEvent kind, std::string_view node);
	void SpawnParticle(Actor* actor, float lifetime, const char* modelName, const NiMatrix3& rotation, const NiPoint3& position, float scale, std::uint32_t flags, NiAVObject* target);
	void SpawnDustParticle(Actor* giant, Actor* tiny, std::string_view node, float size);

	void Utils_PushCheck(Actor* giant, Actor* tiny, float force);

	void StaggerOr(Actor* giant, Actor* tiny, float afX, float afY, float afZ, float afMagnitude);
	void DoDamageEffect(Actor* giant, float damage, float radius, int random, float bonedamage, FootEvent kind, float crushmult, DamageSource Cause);
	void DoDamageEffect(Actor* giant, float damage, float radius, int random, float bonedamage, FootEvent kind, float crushmult, DamageSource Cause, bool ignore_rotation);

	void PushTowards(Actor* giantref, Actor* tinyref, std::string_view bone, float power, bool sizecheck);
	void PushTowards_Task(const ActorHandle& giantHandle, const ActorHandle& tinyHandle, const NiPoint3& startCoords, const NiPoint3& endCoords, std::string_view TaskName, float power, bool sizecheck);
	void PushTowards(Actor* giantref, Actor* tinyref, NiAVObject* bone, float power, bool sizecheck);
	void PushForward(Actor* giantref, Actor* tinyref, float power);
	void TinyCalamityExplosion(Actor* giant, float radius);
	void ShrinkOutburst_Shrink(Actor* giant, Actor* tiny, float shrink, float gigantism);
	void ShrinkOutburstExplosion(Actor* giant, bool WasHit);

	void Utils_ProtectTinies(bool Balance);
	void LaunchImmunityTask(Actor* giant, bool Balance);

	bool HasSMT(Actor* giant);
	void NotifyWithSound(Actor* actor, std::string_view message);

	hkaRagdollInstance* GetRagdoll(Actor* actor);

	void DisarmActor(Actor* tiny, bool drop);
	void ManageRagdoll(Actor* tinyref, float deltaLength, NiPoint3 deltaLocation, NiPoint3 targetLocation);
	void ChanceToScare(Actor* giant, Actor* tiny, float duration, int random, bool apply_sd);
	void StaggerActor(Actor* receiver, float power);
	void StaggerActor(Actor* giant, Actor* tiny, float power);
	void StaggerActor_Around(Actor* giant, const float radius, bool launch);
	

	float GetMovementModifier(Actor* giant);
	float GetGtsSkillLevel(Actor* giant);
	float GetLegendaryLevel(Actor* giant);
	float GetXpBonus();

	void DragonAbsorptionBonuses();

	void AddSMTDuration(Actor* actor, float duration, bool perk_check = true);
	void AddSMTPenalty(Actor* actor, float penalty);

	void ShrinkUntil(Actor* giant, Actor* tiny, float expected, float halflife, bool animation);
	void DisableCollisions(Actor* actor, TESObjectREFR* otherActor);
	void EnableCollisions(Actor* actor);

	void ApplyTalkToActor();
	void CheckTalkPerk();

	void UpdateCrawlState(Actor* actor);
	void UpdateFootStompType(RE::Actor* a_actor);
	void UpdateSneakTransition(RE::Actor* a_actor);

	void SpringGrow(Actor* actor, float amt, float halfLife, std::string_view naming, bool drain);
	void SpringShrink(Actor* actor, float amt, float halfLife, std::string_view naming);

	void ResetGrab(Actor* giant);
	void FixAnimationsAndCamera();

	bool CanPerformAnimation(Actor* giant, AnimationCondition type);
	void AdvanceQuestProgression(Actor* giant, Actor* tiny, QuestStage stage, float value, bool vore);
	float GetQuestProgression(int stage);
	void ResetQuest();

	void SpawnHearts(Actor* giant, Actor* tiny, float Z, float scale, bool hugs, NiPoint3 CustomPos = NiPoint3(0,0,0));
	void SpawnCustomParticle(Actor* actor, ParticleType Type, NiPoint3 spawn_at_point, std::string_view spawn_at_node, float scale_mult);

	void InflictSizeDamage(Actor* attacker, Actor* receiver, float value);

	float Sound_GetFallOff(NiAVObject* source, float mult);

	// RE Fun:
	void SetCriticalStage(Actor* actor, int stage);
	void Attacked(Actor* victim, Actor* agressor);
	void StartCombat(Actor* victim, Actor* agressor);
  	void ApplyDamage(Actor* giant, Actor* tiny, float damage);
	void SetObjectRotation_X(TESObjectREFR* ref, float X);
	void StaggerActor_Directional(Actor* giant, float power, Actor* tiny);
	void SetLinearImpulse(bhkRigidBody* body, const hkVector4& a_impulse);
	void SetAngularImpulse(bhkRigidBody* body, const hkVector4& a_impulse);

	std::int16_t GetItemCount(InventoryChanges* changes, TESBoundObject* a_obj);
	int GetCombatState(Actor* actor);
	bool IsMoving(Actor* giant);

	bool IsPlayerFirstPerson(Actor* a_actor);

	float get_corrected_scale(Actor* a_actor);
	std::tuple<float, float> CalculateVoicePitch(Actor* a_actor);
	float CalculateGorePitch(float TinyScale);
}
