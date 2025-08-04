#include "Managers/GameMode/GameModeManager.hpp"
#include "config/Config.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/GtsSizeManager.hpp"
#include "Managers/Rumble.hpp"

#include "Managers/Audio/MoansLaughs.hpp"

using namespace GTS;

namespace {

	constexpr float EPS = 1e-7f;

	float GetShrinkPenalty(float size) {
		// https://www.desmos.com/calculator/pqgliwxzi2

		SoftPotential launch{
			.k = 0.98f,
			.n = 0.82f,
			.s = 0.70f,
			.a = 0.0f
		};

		const float balance = Config::GetBalance().fBMShrinkRate * 1.6f;
		float power = soft_power(size, launch) * balance;
		return power;
	}

	float Aspect_GetEfficiency(float aspect) {

		float k = 0.75f;
		float a = 0.0f;
		float n = 0.85f;
		float s = 1.0f;

		// https://www.desmos.com/calculator/ygoxbe7hjg
		float result = k*pow(s*(aspect-a), n);

		return result;
	}

	// ---------- Quest Shrink

	void QuestShrink(RE::Actor* a_Actor, const float a_ShrinkRate, const float a_TargetScale, const float a_NaturalScale) {

		const float ModAmmount = -a_ShrinkRate * Time::WorldTimeDelta();
		if (fabs(a_ShrinkRate) < EPS) {
			return;
		}

		float EnchantmentPower = Ench_Aspect_GetPower(a_Actor);
		float Gigantism = Aspect_GetEfficiency(EnchantmentPower) * 0.5f;
		float DefaultScale = a_NaturalScale * (1.0f + Gigantism);

		if ((a_TargetScale + ModAmmount) > DefaultScale) {
			update_target_scale(a_Actor, ModAmmount, SizeEffectType::kShrink);
		}
		else if (a_TargetScale > DefaultScale) {
			set_target_scale(a_Actor, DefaultScale);
		}
	}


	// ---------- Grow

	void Grow(RE::Actor* a_Actor, const float a_CurrentScale, const float a_GrowthRate, const float a_TargetScale, const float a_MaxScale, const float a_Mult = 1.0f) {

		const float ModAmmount = a_CurrentScale * (0.00010f + a_GrowthRate * 0.25f) * 60 * Time::WorldTimeDelta() * a_Mult;

		if (fabs(a_GrowthRate) < EPS) {
			return;
		}

		if (a_TargetScale + ModAmmount < a_MaxScale) {
			update_target_scale(a_Actor, ModAmmount, SizeEffectType::kGrow);
		}

		else if (a_TargetScale < a_MaxScale) {
			set_target_scale(a_Actor, a_MaxScale);
		} 

	}

	// ---------- Shrink

	void Shrink(RE::Actor* a_Actor, const float a_CurrentScale, const float a_ShrinkRate, const float a_TargetScale, const float a_NaturalScale, const float a_Mult = 1.0f) {

		const float ModAmmount = -(0.00025f + (a_ShrinkRate * 0.25f) * a_CurrentScale) * 60 * Time::WorldTimeDelta() * a_Mult;

		if (fabs(a_ShrinkRate) < EPS) {
			return;
		}

		if (a_TargetScale + ModAmmount > a_NaturalScale) {
			update_target_scale(a_Actor, ModAmmount, SizeEffectType::kShrink);
		}

		else if (a_TargetScale > a_NaturalScale || a_TargetScale < a_NaturalScale) {
			set_target_scale(a_Actor, a_NaturalScale);
		} 
	}

	// ---------- Combat Growth

	void CombatGrowth(RE::Actor* a_Actor, const float a_CurrentScale, const float a_GrowthRate, const float a_ShrinkRate, const float a_TargetScale, const float a_NaturalScale, const float a_MaxScale) {
		constexpr float Mult = 1.5f;
		if (a_Actor->IsInCombat()) {
			Grow(a_Actor, a_CurrentScale, a_GrowthRate, a_TargetScale, a_MaxScale, Mult);
		}
		else {
			Shrink(a_Actor, a_CurrentScale, a_ShrinkRate, a_TargetScale, a_NaturalScale, Mult);
		}
	}

	// ---------- Slow Combat Growth

	void SlowCombatGrowth(RE::Actor* a_Actor, const float a_CurrentScale, const float a_GrowthRate, const float a_TargetScale, const float a_MaxScale) {
		constexpr float Mult = 0.6f;
		if (a_Actor->IsInCombat()) {
			Grow(a_Actor, a_CurrentScale, a_GrowthRate, a_TargetScale, a_MaxScale, Mult);
		}
	}

	// ---------- Curse Of Growth

	void CurseOfGrowth(RE::Actor* a_Actor,  const float a_CurrentTargetScale, const float a_MaxScale) {

		// Slider that determines max size cap.
		float CurseOfGrowthMaxSize;
		constexpr float SkillLevel = 100.f; //Its locked behind a level 100 perk anyways
		//Get the actor's Gamemode Timer From Transient and set a random value to it.

		auto ActorData = Transient::GetSingleton().GetData(a_Actor);
		if (!ActorData) return;

		Timer* IntervalTimer = &ActorData->ActionTimer;
		if (!IntervalTimer) return;

		//Set Values based on Settings and actor type.
		if (a_Actor->formID == 0x14) {
			const auto& Settings =  Config::GetGameplay().GamemodePlayer;

			//SkillLevel = GetGtsSkillLevel(a_Actor);
			CurseOfGrowthMaxSize = Settings.fCurseGrowthSizeLimit;
			const float RandomDelay = Settings.fGameModeUpdateInterval;
			ActorData->ActionTimer.UpdateDelta(RandomDelay + RandomFloat(-RandomDelay/10, RandomDelay / 10));
		}
		else if (IsTeammate(a_Actor)) {
			const auto& Settings = Config::GetGameplay().GamemodeFollower;

			//NPC's Set it trough alteration
			//SkillLevel = GetAV(a_Actor, ActorValue::kAlteration);
			CurseOfGrowthMaxSize = Settings.fCurseGrowthSizeLimit;
			const float RandomDelay = Settings.fGameModeUpdateInterval;
			ActorData->ActionTimer.UpdateDelta(RandomDelay + RandomFloat(-RandomDelay / 10, RandomDelay / 10));
		}
		else {
			return;
		}

		const float CurseTargetScale = std::clamp(1.0f * (SkillLevel / 100.0f * CurseOfGrowthMaxSize), 1.0f, CurseOfGrowthMaxSize);

		//10/5% Chance
		const bool DoStrongGrowth = RandomBool(10.0f);
		const int DoMegaGrowth = RandomBool(5.0f);
		const int Random = RandomInt(1, 20);
		float GrowthPower = SkillLevel * 0.00220f / static_cast<float>(Random);                                           

		if (IntervalTimer->ShouldRun() && RandomBool(66.0f)) {

			//If the target scale > than the actors max scale return
			if (a_CurrentTargetScale >= a_MaxScale) {
				return;
			}

			if (DoStrongGrowth && DoMegaGrowth) {
				GrowthPower *= 4.0f;
			}

			if (DoStrongGrowth) {
				GrowthPower *= 4.0f;
			}

			Rumbling::Once("CurseOfGrowth", a_Actor, GrowthPower * 20, 0.10f);
			Runtime::PlaySoundAtNode("GTSSoundGrowth", a_Actor, GrowthPower * 2, "NPC Pelvis [Pelv]");

			//If the Growth event would make the actor larger than the target scale set it to target scale.
			if (a_CurrentTargetScale + GrowthPower >= CurseTargetScale) {
				set_target_scale(a_Actor, CurseTargetScale);
				return;
			}

			update_target_scale(a_Actor, GrowthPower, SizeEffectType::kGrow);

			if (((DoStrongGrowth && Random >= 15) || (DoStrongGrowth && DoMegaGrowth)) && Config::GetAudio().bSlowGrowMoans) {
				Sound_PlayMoans(a_Actor, a_CurrentTargetScale / 4, 0.14f, EmotionTriggerSource::Growth);
				Task_FacialEmotionTask_Moan(a_Actor, 2.0f, "GameMode");
			}
		}
	}

	// ---------- Curse Of The Giantess

	void CurseOfTheGiantess(Actor* a_Actor, const float a_CurrentTargetScale, const float a_MaxScale) {

		// Slider that determines max size cap.
		float CurseTargetScale;
		float PowerMult;

		//Get the actor's Gamemode Timer From Transient and set a random value to it.
		auto ActorData = Transient::GetSingleton().GetData(a_Actor);
		if (!ActorData) return;

		Timer* IntervalTimer = &ActorData->ActionTimer;
		if (!IntervalTimer) return;

		//Set Values based on Settings and actor type.
		if (a_Actor->formID == 0x14) {
			const auto& Settings = Config::GetGameplay().GamemodePlayer;

			CurseTargetScale = Settings.fCurseTargetScale;
			const float RandomDelay = Settings.fGameModeUpdateInterval;
			PowerMult = Settings.fGrowthRate + 0.030f;
			ActorData->ActionTimer.UpdateDelta(RandomDelay + RandomFloat(-RandomDelay / 10, RandomDelay / 10));
		}
		else if (IsTeammate(a_Actor)) {
			const auto& Settings = Config::GetGameplay().GamemodeFollower;
			CurseTargetScale = Settings.fCurseTargetScale;
			const float RandomDelay = Settings.fGameModeUpdateInterval;
			PowerMult = Settings.fGrowthRate + 0.030f;
			ActorData->ActionTimer.UpdateDelta(RandomDelay + RandomFloat(-RandomDelay / 10, RandomDelay / 10));
		}
		else {
			return;
		}

		//If the target scale > than the actors max scale return
		if (a_CurrentTargetScale >= a_MaxScale) {
			return;
		}

		if (IntervalTimer->ShouldRun()) {

			if (a_CurrentTargetScale >= CurseTargetScale) {
				return;
			}
			const float ScaleMult = abs(a_CurrentTargetScale - CurseTargetScale) + PowerMult;
			float ModAmmount = (PowerMult * (RandomFloat(1.f, 4.5f) * ScaleMult));
			//const float GrowthPower = std::clamp(RandomFloatGauss(0.30f, 0.1f), 0.1f, 0.5f);

			if (a_CurrentTargetScale + ModAmmount >= CurseTargetScale) {
				set_target_scale(a_Actor, CurseTargetScale);
				return;
			}

			if (RandomBool(12.0f)) {
				Sound_PlayMoans(a_Actor, a_CurrentTargetScale / 4, 0.14f, EmotionTriggerSource::Growth);
				Task_FacialEmotionTask_Moan(a_Actor, 2.0f, "CurseOfTheGiantess");
			}

			update_target_scale(a_Actor, ModAmmount, SizeEffectType::kGrow);
			Runtime::PlaySoundAtNode("GTSSoundGrowth", a_Actor, ModAmmount * 2.0f, "NPC Pelvis [Pelv]");
		}
	}

	// ---------- Curse Of Diminishing

	void CurseOfDiminishing(Actor* a_Actor, const float a_CurrentTargetScale) {

		if (GetCombatState(a_Actor) > 0 || IsGtsBusy(a_Actor)) return;

		// Slider that determines max size cap.
		float CurseTargetScale;
		float PowerMult;

		//Get the actor's Gamemode Timer From Transient and set a random value to it.
		auto ActorData = Transient::GetSingleton().GetData(a_Actor);
		if (!ActorData) return;

		Timer* IntervalTimer = &ActorData->ActionTimer;
		if (!IntervalTimer) return;

		//Set Values based on Settings and actor type.
		if (a_Actor->formID == 0x14) {
			const auto& Settings = Config::GetGameplay().GamemodePlayer;

			PowerMult = Settings.fShrinkRate + 0.030f;
			CurseTargetScale = Settings.fCurseTargetScale;
			//The larger the actor the faster they shrink
			const float RandomDelay = Settings.fGameModeUpdateInterval / RandomFloat(1.0, a_CurrentTargetScale / 2.0f);
			ActorData->ActionTimer.UpdateDelta(RandomDelay + RandomFloat(-RandomDelay / 10, RandomDelay / 10));
		}
		else if (IsTeammate(a_Actor)) {
			const auto& Settings = Config::GetGameplay().GamemodeFollower;

			PowerMult = Settings.fShrinkRate + 0.030f;
			CurseTargetScale = Settings.fCurseTargetScale;
			//The larger the actor the faster they shrink
			const float RandomDelay = Settings.fGameModeUpdateInterval / RandomFloat(1.0, a_CurrentTargetScale / 2.0f);
			ActorData->ActionTimer.UpdateDelta(RandomDelay + RandomFloat(-RandomDelay / 10, RandomDelay / 10));
		}
		else {
			return;
		}

		if (a_CurrentTargetScale <= CurseTargetScale) {
			return;
		}

		const float ScaleMult = abs(a_CurrentTargetScale - CurseTargetScale) + PowerMult;

		if (IntervalTimer->ShouldRun()) {

			float ModAmmount = (PowerMult * (RandomFloat(1.f, 4.5f) * ScaleMult));

			Runtime::PlaySoundAtNode("GTSSoundShrink", a_Actor, ModAmmount * 2.0f, "NPC Pelvis [Pelv]");

			if (a_CurrentTargetScale - ModAmmount <= CurseTargetScale) {
				set_target_scale(a_Actor, CurseTargetScale);
				return;
			}

			update_target_scale(a_Actor, -ModAmmount, SizeEffectType::kShrink);

		}
		
	}

}

namespace GTS {

	GameModeManager& GameModeManager::GetSingleton() noexcept {
		static GameModeManager instance;
		return instance;
	}

	std::string GameModeManager::DebugName() {
		return "::GameModeManager";
	}

	void GameModeManager::ApplyGameMode(Actor* a_Actor, const SelectedGameMode& a_SelectedGameMode, const float& a_GrowthRate, const float& a_ShrinkRate)  {

		GTS_PROFILE_SCOPE("GameModeManager: ApplyGameMode");


		if (a_SelectedGameMode == SelectedGameMode::kNone) {
			return;
		}

		auto& Settings = Config::GetGameplay();
		float NaturalScale = get_natural_scale(a_Actor, true);
		float TargetScale = get_target_scale(a_Actor);
		float MaxScale = get_max_scale(a_Actor);
		float RateMultiplier = 1.0f;

		if (IsFemale(a_Actor, true)) {

			if (Settings.GamemodePlayer.bMultiplyGrowthrate && a_Actor->formID == 0x14) {
				RateMultiplier = clamp(get_visual_scale(a_Actor) * 0.25f, 1.0f, 10.0f);
			}
			else if (Settings.GamemodeFollower.bMultiplyGrowthrate && IsTeammate(a_Actor)) {
				RateMultiplier = clamp(get_visual_scale(a_Actor) * 0.25f, 1.0f, 10.0f);
			}

			switch (a_SelectedGameMode) {

				case SelectedGameMode::kNone: {
					return;
				}

				case SelectedGameMode::kGrow: {
					Grow(a_Actor,RateMultiplier,a_GrowthRate, TargetScale, MaxScale);
					return;
				}

				case SelectedGameMode::kShrink: {
					Shrink(a_Actor, RateMultiplier, a_ShrinkRate, TargetScale, NaturalScale);
					return;
				}

				case SelectedGameMode::kSlowCombatGrowth: {
					SlowCombatGrowth(a_Actor, RateMultiplier, a_GrowthRate, TargetScale, MaxScale);
					return;
				}

				case SelectedGameMode::kCombatGrowth: {
					CombatGrowth(a_Actor, RateMultiplier, a_GrowthRate, a_ShrinkRate, TargetScale, NaturalScale, MaxScale);
					return;
				}

				case SelectedGameMode::kCurseOfGrowth: {
					CurseOfGrowth(a_Actor, TargetScale, MaxScale);
					return;
				}

				case SelectedGameMode::kCurseOfTheGiantess: {
					CurseOfTheGiantess(a_Actor, TargetScale, MaxScale);
					return;
				}

				case SelectedGameMode::kCurseOfDiminishing: {
					CurseOfDiminishing(a_Actor, TargetScale);
					return;
				}

				case SelectedGameMode::kSizeLocked: {
					CurseOfTheGiantess(a_Actor, TargetScale, MaxScale);
					CurseOfDiminishing(a_Actor, TargetScale);
					break;
				}
			}
		}
	}

	void GameModeManager::GameMode(Actor* actor)  {
		GTS_PROFILE_SCOPE("GameModeManager: GameMode");

		if (!actor) {
			return;
		}

		if (!Plugin::Live()) {
			return;
		}

		if (!IsVisible(actor)) {
			return;
		}

		float BaseGrowhtRate = 0.0f;
		float BaseShrinkRate = 0.0f;
		float BonusShrink = 7.4f;
		float BonusGrowth = 1.0f;
		uint16_t QuestStage = Runtime::GetStage("GTSQuestProgression");

		const bool BalanceModeEnabled = Config::GetBalance().bBalanceMode;

		float CurrentScale = get_visual_scale(actor);
		bool DoQuestShrink = false;
		SelectedGameMode GameMode = SelectedGameMode::kNone;
		
		if (BalanceModeEnabled) {
			BonusShrink *= GetShrinkPenalty(CurrentScale);
		}

		if (QuestStage < 100 || BalanceModeEnabled) {

			if (actor->formID == 0x14 || IsTeammate(actor)) {

				DoQuestShrink = true;

				if (QuestStage < 40) {
					BaseShrinkRate = 0.00086f * BonusShrink * 2.0f;
				}
				else if (QuestStage >= 40 && QuestStage < 80) {
					BaseShrinkRate = 0.00086f * BonusShrink * 1.6f;
				}
				else if (BalanceModeEnabled && QuestStage >= 80) {
					BaseShrinkRate = 0.00086f * BonusShrink * 1.40f;
				}

				BaseShrinkRate *= Potion_GetShrinkResistance(actor);
				BaseShrinkRate /= 1.0f + Ench_Aspect_GetPower(actor);

				if (Potion_IsUnderGrowthPotion(actor)) {
					BaseShrinkRate *= 0.0f; // prevent shrinking in that case
				} 
				if (HasGrowthSpurt(actor)) {
					BaseShrinkRate *= 0.25f;
				}
				if (actor->IsInCombat() && !BalanceModeEnabled) {
					BaseShrinkRate = 0.0f;
				}
				else if (SizeManager::GetSingleton().GetGrowthSpurt(actor) > 0.01f) {
					BaseShrinkRate = 0.0f;
				}
				else if (actor->IsInCombat() && BalanceModeEnabled) {
					BaseShrinkRate *= Config::GetBalance().fBMShrinkRateCombat;
				}

				if (fabs(BaseShrinkRate) <= 1e-6) {
					DoQuestShrink = false; // Nothing to do
				}
			}
		}

		else if (QuestStage > 100 && !BalanceModeEnabled) {

			const auto& Settings = Config::GetGameplay();

			if (actor->formID == 0x14) {

				if (Runtime::HasMagicEffect(actor, "GTSPotionEffectSizeAmplify")) {
					BonusGrowth = CurrentScale * 0.25f + 0.75f;
				}

				BaseGrowhtRate = Settings.GamemodePlayer.fGrowthRate * BonusGrowth;
				BaseShrinkRate = Settings.GamemodePlayer.fShrinkRate;
				GameMode = StringToEnum<SelectedGameMode>(Settings.GamemodePlayer.sGameMode);
			}
			else if (IsTeammate(actor)) {

				if (Runtime::HasMagicEffect(actor, "GTSPotionEffectSizeAmplify")) {
					BonusGrowth = CurrentScale * 0.25f + 0.75f;
				}

				BaseGrowhtRate = Settings.GamemodeFollower.fGrowthRate * BonusGrowth;
				BaseShrinkRate = Settings.GamemodeFollower.fShrinkRate;
				GameMode = StringToEnum<SelectedGameMode>(Settings.GamemodeFollower.sGameMode);
			}
		}

		BaseShrinkRate *= Perk_GetSprintShrinkReduction(actor); // up to 20% reduction

		if (DoQuestShrink) {
			QuestShrink(actor, BaseShrinkRate, get_target_scale(actor), get_natural_scale(actor, true));
			return;
		}

		if (Runtime::HasPerk(PlayerCharacter::GetSingleton(), "GTSPerkColossalGrowth")) {
            ApplyGameMode(actor, GameMode, BaseGrowhtRate / 2, BaseShrinkRate);
        }
	}
}
