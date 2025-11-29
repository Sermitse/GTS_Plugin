#pragma once

namespace GTS {

	struct TransientActorData {

		float BaseHeight = 0.0f;
		float SMTBonusDuration = 0.0f;
		float SMTPenaltyDuration = 0.0f;
		float CarryWeightBoost = 0.0f;
		float HealthBoost = 0.0f;
		float FallTimer = 1.0f;
		float HugAnimationSpeed = 1.0f;
		float ThrowSpeed = 0.0f;
		float PotionMaxSize = 0.0f;
		float ButtCrushMaxSize = 0.0f;
		float ButtCrushStartScale = 0.0f;
		float SizeVulnerability = 0.0f;
		float PushForce = 1.0f;
		float OtherScales = 1.0f;
		float VoreRecordedScale = 1.0f;
		float WorldFOVDefault = 0.0f;
		float FPFOVDefault = 0.0f;
		float ButtCrushGrowthAmount = 0.0f;
		float MovementSlowdown = 1.0f;
		float ShrinkResistance = 0.0f;
		float MightValue = 0.0f;
		float ShrinkTicks = 0.0f;
		float ShrinkTicksCalamity = 0.0f;
		float PerkBonusSpeed = 1.0f;
		float PerkLifeForceStolen = 0.0f;
		float ClothRipLastScale = -1.0f;
		float ClothRipOffset = -1.0f;
		float ShrinkUntil = 0.0f;
		float BreastSizeBuff = 0.0f;
		float OverkillSizeBonus = 1.0f;
		float Overkills = 0.0f;
		float FootVelocity_R = 0.0f;
		float FootVelocity_L = 0.0f;
		float HandVelocity_R = 0.0f;
		float HandVelocity_L = 0.0f;

		int Stacks_Perk_CataclysmicStomp = 0;
		int Stacks_Perk_LifeForce = 0;
		int CrushSound_Calc_CrushedTinies = 0;

		bool ThrowWasThrown = false;
		bool CanDoVore = true;
		bool CanBeCrushed = true;
		bool CanBeVored = true;
		bool BeingHeld = false;
		bool BetweenBreasts = false;
		bool AboutToBeEaten = false;
		bool DragonWasEaten = false;
		bool BeingFootGrinded = false;
		bool SMTReachedMaxSpeed = false;
		bool OverrideCamera = false;
		bool WasReanimated = false;
		bool FPCrawling = false;
		bool FPProning = false;
		bool Protection = false;
		bool GrowthPotion = false;
		bool DevourmentDevoured = false;
		bool DevourmentEaten = false;
		bool WasSneaking = false;
		bool EmotionModifierBusy = false;
		bool EmotionPhonemeBusy = false;
		bool ImmuneToBreastOneShot = true;
		bool IsSlowGrowing = false;
		bool TemporaryDamageImmunity = false;
		bool ReattachingTiny = false;
		bool KissVoring = false;

		NiPoint3 BoundingBoxCache = { 0.0f, 0.0f, 0.0f };

		NiPoint3 POSCurrentLegL = { 0.0f, 0.0f, 0.0f };
		NiPoint3 POSCurrentLegR = { 0.0f, 0.0f, 0.0f };
		NiPoint3 POSCurrentHandL = { 0.0f, 0.0f, 0.0f };
		NiPoint3 POSCurrentHandR = { 0.0f, 0.0f, 0.0f };

		NiPoint3 POSLastLegL = { 0.0f, 0.0f, 0.0f };
		NiPoint3 POSLastLegR = { 0.0f, 0.0f, 0.0f };
		NiPoint3 POSLastHandL = { 0.0f, 0.0f, 0.0f };
		NiPoint3 POSLastHandR = { 0.0f, 0.0f, 0.0f };

		Actor* IsInControl = nullptr;

		TESObjectREFR* DisableColissionWith = nullptr;
		TESObjectREFR* ThrowOffender = nullptr;

		AttachToNode AttachmentNode = AttachToNode::None;
		BusyFoot FootInUse = BusyFoot::None;

		Timer GameModeIntervalTimer = Timer(0);
		Timer ActionTimer = Timer(0);

		std::vector<Actor*> shrinkies;

		explicit TransientActorData(Actor* a_Actor) {
			const auto _BoundValues = get_bound_values(a_Actor);
			const auto _Scale = get_scale(a_Actor);

			BaseHeight = GameUnitToMeter(_BoundValues[2] * _Scale);
			BoundingBoxCache = _BoundValues;
		}
	};
}