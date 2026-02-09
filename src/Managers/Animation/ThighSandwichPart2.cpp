#include "Managers/Animation/ThighSandwichPart2.hpp"

#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Controllers/ThighSandwichController.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Audio/GoreAudio.hpp"
#include "Managers/Input/InputManager.hpp"
#include "Managers/CrushManager.hpp"
#include "Managers/GtsSizeManager.hpp"
#include "Managers/Rumble.hpp"
#include "Magic/Effects/Common.hpp"
#include "Utils/Actions/ButtCrushUtils.hpp"
#include "Utils/Actions/InputConditions.hpp"
#include "Managers/Audio/MoansLaughs.hpp"

#include "Managers/Animation/Controllers/VoreController.hpp"

#include "Utils/Actions/VoreUtils.hpp"
#include "Config/Config.hpp"

#include "Managers/Perks/PerkHandler.hpp"
#include "Utils/DifficultyUtils.hpp"
#include "Managers/AI/AIFunctions.hpp"
#include "Utils/DeathReport.hpp"
#include "Utils/Looting.hpp"

using namespace GTS;

namespace AnimLogic {
	void PerformAnimations(std::string_view owner_anim, std::string_view receiver_anim = "") {
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(PlayerCharacter::GetSingleton());
		AnimationManager::StartAnim(owner_anim, PlayerCharacter::GetSingleton());

		for (auto tiny : sandwichdata.GetActors()) {
			if (tiny && receiver_anim.size() > 1) {
				AnimationManager::StartAnim(receiver_anim, tiny);
			}
		}
	}

	void AbsorbTinies(Actor* giant) {
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(giant);
		const auto& MuteVore = Config::Audio.bMuteVoreDeathScreams;

		for (auto tiny: sandwichdata.GetActors()) {
			ModSizeExperience(giant, 0.08f + (get_natural_scale(tiny)*0.025f));
			Vore_AdvanceQuest(giant, tiny, IsDragon(tiny), IsGiant(tiny)); // Progress quest
			ReportDeath(giant, tiny, DamageSource::Vored, true);
			SetBeingHeld(tiny, false);

			sandwichdata.Remove(tiny);

			if (!tiny->IsPlayerRef()) {
				KillActor(giant, tiny, MuteVore);
				PerkHandler::UpdatePerkValues(giant, PerkUpdate::Perk_LifeForceAbsorption);
			} else {
				InflictSizeDamage(giant, tiny, 900000);
				KillActor(giant, tiny, MuteVore);
				TriggerScreenBlood(50);
				tiny->SetAlpha(0.0f); // Player can't be disintegrated: simply nothing happens. So we Just make player Invisible instead.
			}
			
			DecreaseShoutCooldown(giant);

			std::string taskname = std::format("VoreAbsorb_{}", tiny->formID);
			auto tinyref = tiny->CreateRefHandle();
			auto giantref = giant->CreateRefHandle();

			TaskManager::RunOnce(taskname, [=](auto& update) {
				if (!tinyref) {
					return;
				}
				if (!giantref) {
					return;
				}
				auto g = giantref.get().get();
				auto t = tinyref.get().get();

				if (!t->IsPlayerRef()) {
					Disintegrate(t);
				}
				TransferInventory(t, g, 1.0f, false, true, DamageSource::Vored, true);
			});
		}
	}
}

namespace DamageLogic {
	bool DoButtDamage(Actor* giant, float damage, bool CanStartAnim, float mult = 1.0f) {
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(giant);
		auto& sizemanager = SizeManager::GetSingleton();
		bool DealDamage = true;
		float threshold = 0.15f;

		if (TinyCalamityActive(giant)) {
			mult *= 1.5f;
		}

		for (auto tiny: sandwichdata.GetActors()) {
			if (tiny && tiny->Is3DLoaded()) {
				float sizedifference = get_visual_scale(giant)/(get_visual_scale(tiny) * GetSizeFromBoundingBox(tiny));
				float additionaldamage = 1.0f + sizemanager.GetSizeVulnerability(tiny); // Get size damage debuff from enemy
				float normaldamage = std::clamp(SizeManager::GetSizeAttribute(giant, SizeAttribute::Normal), 1.0f, 999.0f);
				damage *= mult * sizedifference * normaldamage * GetPerkBonus_Thighs(giant);

				float max_tiny_hp = GetMaxAV(tiny, ActorValue::kHealth) * threshold;
				float damage_Setting = GetDifficultyMultiplier(giant, tiny);
				float tiny_health = GetAV(tiny, ActorValue::kHealth);
				float tiny_health_perc = GetHealthPercentage(tiny);
			
				auto& sizemanager = SizeManager::GetSingleton();
				
				if (CanStartAnim && (tiny_health_perc <= threshold || tiny_health - (damage * damage_Setting) <= max_tiny_hp)) {
					logger::info("Starting Finisher animation");
					DealDamage = false;
				}
				
				if (CanDoDamage(giant, tiny, false)) {
					InflictSizeDamage(giant, tiny, damage);
				}

				float experience = std::clamp(damage/200, 0.0f, 0.20f);
				ModSizeExperience(giant, experience);

				float hp = GetAV(tiny, ActorValue::kHealth);

				if (damage > hp || hp <= 0 || tiny->IsDead()) {
					ModSizeExperience_Crush(giant, tiny, true);
					CrushManager::Crush(giant, tiny);
					
					ReportDeath(giant, tiny, DamageSource::Crushed);
					AdvanceQuestProgression(giant, tiny, QuestStage::Crushing, 1.0f, false);
					auto node = find_node(giant, "AnimObjectA");
					
					PlayCrushSound(giant, node, false, get_corrected_scale(tiny));
					sandwichdata.Remove(tiny);

					if (sandwichdata.GetActors().empty()) {
						AnimationManager::StartAnim("TinyDied", giant);
					}
				}
			}
		}

		if (!DealDamage) {
			AnimationManager::StartAnim("Sandwich_Finisher", giant);
			auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(giant);
			sandwichdata.EnableSuffocate(false);
			sandwichdata.SetSuffocateMult(1.0f);
			for (auto tiny: sandwichdata.GetActors()) {
				if (tiny && tiny->Is3DLoaded()) {
					AnimationManager::StartAnim("Sandwich_Finisher_T", tiny);
				}
			}
		}

		return DealDamage;
	}

	void DoButtDamage_DOT(Actor* giant) {
		std::string name = std::format("ButtDOT_TS_{}", giant->formID);
		ActorHandle gianthandle = giant->CreateRefHandle();

		double Start = Time::WorldTimeElapsed();

		TaskManager::Run(name, [=](auto& progressData) { // There's no event to toggle it on/off yet, need Nick to add these
			if (!gianthandle) {
				return false;
			}
			double Finish = Time::WorldTimeElapsed();
			double timepassed = Finish - Start;
			if (timepassed < 0.1) { // Give it some time so it passes check below when they're true
				return true;
			}
			auto giantref = gianthandle.get().get();
			if (!AnimationVars::Action::IsInSecondSandwichBranch(giantref) || !AnimationVars::Action::IsThighGrinding(giantref)) {
				return false;
			}
			if (!DoButtDamage(giantref, Damage_ThighSandwich_Butt_Grind, true)) {
				return false;
			}
			return true;
		});
	}
}


namespace AnimEvents {

	//Used in the Intro to Butt State when the GTS sits back down softly on the Tiny
	void GTS_TSB_SitDownSoft(AnimationEventData& data) {
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(&data.giant);
		sandwichdata.EnableSuffocate(true);
		sandwichdata.SetSuffocateMult(1.5f);

		ManageCamera(&data.giant, true, CameraTracking::Butt);
	}

	//Used in the exit of the butt state when the gts's sits down onto the rune with no tiny underneath
	void GTS_TSB_SitDown(AnimationEventData& data) {
		
	}

	//Used in the exit when the Tiny falls back onto the GTS's thigh
	void GTS_TSB_TinyThigh(AnimationEventData& data) {
		
	}

	//Used when the GTS starts to lift her self up from sitting down mostly for sfx
	//Also fired when we initially grab the tiny for some reason...
	void GTS_TSB_Stand(AnimationEventData& data) {
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(&data.giant);
		sandwichdata.ReleaseAll();

		if (GetGrowthCount(&data.giant) > 0) {
			ModGrowthCount(&data.giant, 0, true); // Reset growth count
			SetButtCrushSize(&data.giant, 0, true);
		}
	}

	//Used when the GTS starts to fall down to butt crush
	void GTS_TSB_Fall(AnimationEventData& data) {
		
	}

	//Used when the GTS lands the light butt attack
	void GTS_TSB_LandSmall(AnimationEventData& data) {
		logger::info("GTS_TSB_LandSmall triggered");
		DamageLogic::DoButtDamage(&data.giant, Damage_ThighSandwich_Butt_Light, false);
		//Do light damage
	}

	//Used when the GTS lands the first two hits during the Butt state finisher, doesn't do damage
	void GTS_TSB_LandMid(AnimationEventData& data) {
		Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundFootstepHighHeels_2x, &data.giant, 1.0f, "AnimObjectA"); 
		Rumbling::Once("ButtImpact", &data.giant, Rumble_ThighSandwich_ButtImpact, 0.15f, "AnimObjectA", 0.0f);
	}

	//Used when the GTS lands the heavy butt attack
	void GTS_TSB_LandHeavy(AnimationEventData& data) {
		logger::info("GTS_TSB_LandHeavy triggered");
		DamageLogic::DoButtDamage(&data.giant, Damage_ThighSandwich_Butt_Heavy, false);
		Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundFootstepHighHeels_2x, &data.giant, 1.0f, "AnimObjectA"); 
		Rumbling::Once("ButtImpact", &data.giant, Rumble_ThighSandwich_ButtImpact_Heavy, 0.15f, "AnimObjectA", 0.0f);
		//Do heavy damage
	}

	//Used for the final hit when the GTS deals most damage and cracks the butt rune
	void GTS_TSB_LandFinisher(AnimationEventData& data) {
		logger::info("GTS_TSB_LandFinisher triggered");
		DamageLogic::DoButtDamage(&data.giant, Damage_ThighSandwich_Butt_Heavy, false, 4.0f); //Deal Extra damage
		//Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundTinyCalamity_Impact, &data.giant, 1.0f, "AnimObjectA");
		Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundFootstepHighHeels_2x, &data.giant, 1.0f, "AnimObjectA"); 
		Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundTinyCalamity_ReachedSpeed, &data.giant, 1.0f, "AnimObjectA");
		Rumbling::Once("ButtImpactFinisher", &data.giant, Rumble_ThighSandwich_ButtImpact_Finisher, 0.15f, "AnimObjectA", 0.0f);

		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(&data.giant);
		sandwichdata.ReleaseAll();
		
		if (GetGrowthCount(&data.giant) > 0) {
			ModGrowthCount(&data.giant, 0, true); // Reset growth count
			SetButtCrushSize(&data.giant, 0, true);
		}
	}

	//Used when the GTS lands on the Floor after cracking the butt rune
	void GTS_TSB_LandFloor(AnimationEventData& data) {
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(&data.giant);
		sandwichdata.ReleaseAll();
	}

	void GTS_TSB_DOT_Start(AnimationEventData& data) {
		DamageLogic::DoButtDamage_DOT(&data.giant);
	}
	void GTS_TSB_DOT_Stop(AnimationEventData& data) {
		// The check inside DoButtDamage_DOT should stop it automacially
	}

	void GTSSandwich_DisableRune(AnimationEventData& data) {
		Actor* caster = &data.giant;
		bool HasEffect = Runtime::HasMagicEffect(caster, Runtime::MGEF.GTSEffectThighRune);
		SpellItem* Spell = Runtime::GetSpell(Runtime::SPEL.GTSSpellThighRune);

		if (HasEffect && Spell) {
			ActorHandle handle = caster->CreateRefHandle();
			caster->AsMagicTarget()->DispelEffect(Spell, handle);
		}
	}

	//Used when the GTS dust off her butt after standing up from the Finisher mostly for SFX
	void GTS_TSB_DustButt(AnimationEventData& data) {}

	//Triggered when the Tiny is initially inserted
	void GTS_TSB_TinyInserted(AnimationEventData& data) {
		Task_FacialEmotionTask_Smile(&data.giant, 0.75f, "UB_Smile_Slight", 0.15f);
	}

	//Triggered when the GTS Pushes the Tiny inside
	void GTS_TSB_TinyPushStart(AnimationEventData& data) {
	}

	//Triggered when the tiny is fully inside
	void GTS_TSB_TinyFullyIn(AnimationEventData& data) {
	}

	//Triggered at the end when the tiny is fully inside Kills the Tiny
	void GTS_TSB_TinyKill(AnimationEventData& data) {
		Sound_PlayMoans(&data.giant, 1.0f, 0.14f, EmotionTriggerSource::Absorption, CooldownSource::Emotion_Voice_Long);
		Task_FacialEmotionTask_Moan(&data.giant, 1.75f, "UB_Moan", 0.15f);
		
		AnimLogic::AbsorbTinies(&data.giant);
	}

	//Triggered When the Tiny is being pushed in (a frame later than Pushstart) this was here for the devourment mod but I guess I just made the event an official one *shruging asci art*
	void GTS_TSB_UBStart(AnimationEventData& data) {}

	//Triggered when the Tiny is fully in and the GTS's head is back and she's praising Diabella's blessings(O facing)
	void GTS_TSB_UBEnjoy(AnimationEventData& data) {}

	//Triggered when the ub is over nad the GTS has stopped enjoying her self and is returning to Idle
	void GTS_TSB_UBEnd(AnimationEventData& data) {}

	//Triggered after TinyKill, an event for the Devourment mod for when the tiny can be deleted
	void GTS_TSB_UBAbsorb(AnimationEventData& data) {
		logger::info("GTS_TSB_UBAbsorb triggered");
	}


}

namespace {
	void ButtStart(const ManagedInputEvent& data) {
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(PlayerCharacter::GetSingleton());
		if (sandwichdata.GetActors().size() > 0) {
			AnimLogic::PerformAnimations("Sandwich_ButtStart", "Sandwich_ButtStart_T");
		}
	}

	void ButtStop(const ManagedInputEvent& data) {
		AnimLogic::PerformAnimations("Sandwich_ButtStop", "Sandwich_ButtStop_T");
	}

	void ButtLightAttack(const ManagedInputEvent& data) {
		AnimLogic::PerformAnimations("Sandwich_LightAttack", "Sandwich_LightAttack_T");
	}

	void ButtHeavyAttack(const ManagedInputEvent& data) {
		AnimLogic::PerformAnimations("Sandwich_HeavyAttack", "Sandwich_HeavyAttack_T");
	}

	void ButtFinisher(const ManagedInputEvent& data) {
		AnimLogic::PerformAnimations("Sandwich_Finisher", "Sandwich_Finisher_T");
	}

	void ButtGrowth(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		bool CanGrow = ButtCrush_IsAbleToGrow(player, GetGrowthLimit(player));
		bool HasPerk = Runtime::HasPerkTeam(player, Runtime::PERK.GTSPerkButtCrushAug2);
		if (HasPerk && CanGrow) {
			AnimLogic::PerformAnimations("Sandwich_Grow", "Sandwich_Grow_T");
		} else {
			if (HasPerk) {
				NotifyWithSound(player, "Your body can't grow any further");
			}
		}
	}

	void ButtGrindStart(const ManagedInputEvent& data) {
		AnimLogic::PerformAnimations("Sandwich_GrindStart", "Sandwich_GrindStart_T");
	}

	void ButtGrindStop(const ManagedInputEvent& data) {
		AnimLogic::PerformAnimations("Sandwich_GrindStop", "Sandwich_GrindStop_T");
	}

	void ButtGrindAbort(const ManagedInputEvent& data) {
		AnimLogic::PerformAnimations("Sandwich_GrindAbort", "Sandwich_GrindAbort_T");
	}

	void ButtUB(const ManagedInputEvent& data) {
		AnimLogic::PerformAnimations("Sandwich_UB", "Sandwich_UB_T");
	}

}

namespace GTS
{
	void AnimationThighSandwich_P2::RegisterEvents() {
		
		//Input Events
		InputManager::RegisterInputEvent("SandwichButtStart", ButtStart, SecondThighSandwichBranch_Start);
		InputManager::RegisterInputEvent("SandwichButtStop", ButtStop, SecondThighSandwichBranch);
		InputManager::RegisterInputEvent("SandwichLightAttack", ButtLightAttack, SecondThighSandwichBranch);
		InputManager::RegisterInputEvent("SandwichHeavyAttack", ButtHeavyAttack, SecondThighSandwichBranch);
		InputManager::RegisterInputEvent("SandwichGrindStart", ButtGrindStart, ThighSandwichGrind_Start);
		InputManager::RegisterInputEvent("SandwichGrindStop", ButtGrindStop, ThighSandwichGrind);
		InputManager::RegisterInputEvent("SandwichGrindAbort", ButtGrindAbort, ThighSandwichGrind);
		InputManager::RegisterInputEvent("SandwichUB", ButtUB, UBCondition);
		InputManager::RegisterInputEvent("SandwichFinisher", ButtFinisher, SecondThighSandwichBranch);
		InputManager::RegisterInputEvent("SandwichGrowth", ButtGrowth, SecondThighSandwichBranch);

		//Animation Events
		AnimationManager::RegisterEvent("GTS_TSB_SitDownSoft", "ThighSandwich", AnimEvents::GTS_TSB_SitDownSoft);
		AnimationManager::RegisterEvent("GTS_TSB_SitDown", "ThighSandwich", AnimEvents::GTS_TSB_SitDown);
		AnimationManager::RegisterEvent("GTS_TSB_TinyThigh", "ThighSandwich", AnimEvents::GTS_TSB_TinyThigh);
		AnimationManager::RegisterEvent("GTS_TSB_Stand", "ThighSandwich", AnimEvents::GTS_TSB_Stand);
		AnimationManager::RegisterEvent("GTS_TSB_Fall", "ThighSandwich", AnimEvents::GTS_TSB_Fall);
		AnimationManager::RegisterEvent("GTS_TSB_LandSmall", "ThighSandwich", AnimEvents::GTS_TSB_LandSmall);
		AnimationManager::RegisterEvent("GTS_TSB_LandMid", "ThighSandwich", AnimEvents::GTS_TSB_LandMid);
		AnimationManager::RegisterEvent("GTS_TSB_LandHeavy", "ThighSandwich", AnimEvents::GTS_TSB_LandHeavy);
		AnimationManager::RegisterEvent("GTS_TSB_LandFinisher", "ThighSandwich", AnimEvents::GTS_TSB_LandFinisher);
		AnimationManager::RegisterEvent("GTS_TSB_LandFloor", "ThighSandwich", AnimEvents::GTS_TSB_LandFloor);
		AnimationManager::RegisterEvent("GTS_TSB_DOT_Start", "ThighSandwich", AnimEvents::GTS_TSB_DOT_Start);
		AnimationManager::RegisterEvent("GTS_TSB_DOT_Stop", "ThighSandwich", AnimEvents::GTS_TSB_DOT_Stop);
		AnimationManager::RegisterEvent("GTSSandwich_DisableRune", "ThighSandwich", AnimEvents::GTSSandwich_DisableRune);
		AnimationManager::RegisterEvent("GTS_TSB_DustButt", "ThighSandwich", AnimEvents::GTS_TSB_DustButt);
		AnimationManager::RegisterEvent("GTS_TSB_TinyInserted", "ThighSandwich", AnimEvents::GTS_TSB_TinyInserted);
		AnimationManager::RegisterEvent("GTS_TSB_TinyPushStart", "ThighSandwich", AnimEvents::GTS_TSB_TinyPushStart);
		AnimationManager::RegisterEvent("GTS_TSB_TinyFullyIn", "ThighSandwich", AnimEvents::GTS_TSB_TinyFullyIn);
		AnimationManager::RegisterEvent("GTS_TSB_TinyKill", "ThighSandwich", AnimEvents::GTS_TSB_TinyKill);
		AnimationManager::RegisterEvent("GTS_TSB_UBStart", "ThighSandwich", AnimEvents::GTS_TSB_UBStart);
		AnimationManager::RegisterEvent("GTS_TSB_UBEnjoy", "ThighSandwich", AnimEvents::GTS_TSB_UBEnjoy);
		AnimationManager::RegisterEvent("GTS_TSB_UBEnd", "ThighSandwich", AnimEvents::GTS_TSB_UBEnd);
		AnimationManager::RegisterEvent("GTS_TSB_UBAbsorb", "ThighSandwich", AnimEvents::GTS_TSB_UBAbsorb);
	}

	void AnimationThighSandwich_P2::RegisterTriggers() {
		AnimationManager::RegisterTrigger("Sandwich_ButtStart", "ThighSandwich", "GTSBEH_ButtState_Start");
		AnimationManager::RegisterTrigger("Sandwich_ButtStop", "ThighSandwich", "GTSBEH_ButtState_Stop");
		AnimationManager::RegisterTrigger("Sandwich_LightAttack", "ThighSandwich", "GTSBEH_ButtState_LightAttack");
		AnimationManager::RegisterTrigger("Sandwich_HeavyAttack", "ThighSandwich", "GTSBEH_ButtState_HeavyAttack");
		AnimationManager::RegisterTrigger("Sandwich_Finisher", "ThighSandwich", "GTSBEH_ButtState_Finisher");
		AnimationManager::RegisterTrigger("Sandwich_Grow", "ThighSandwich", "GTSBEH_ButtState_Grow");
		AnimationManager::RegisterTrigger("Sandwich_GrindStart", "ThighSandwich", "GTSBEH_ButtState_GrindStart");
		AnimationManager::RegisterTrigger("Sandwich_GrindStop", "ThighSandwich", "GTSBEH_ButtState_GrindStop");
		AnimationManager::RegisterTrigger("Sandwich_GrindAbort", "ThighSandwich", "GTSBEH_ButtState_Abort");
		AnimationManager::RegisterTrigger("Sandwich_UB", "ThighSandwich", "GTSBEH_Sandwich_UB");
		AnimationManager::RegisterTrigger("Sandwich_ButtStart_T", "ThighSandwich", "GTSBEH_T_ButtState_Start");
		AnimationManager::RegisterTrigger("Sandwich_ButtStop_T", "ThighSandwich", "GTSBEH_T_ButtState_Stop");
		AnimationManager::RegisterTrigger("Sandwich_LightAttack_T", "ThighSandwich", "GTSBEH_T_ButtState_LightAttack");
		AnimationManager::RegisterTrigger("Sandwich_HeavyAttack_T", "ThighSandwich", "GTSBEH_T_ButtState_HeavyAttack");
		AnimationManager::RegisterTrigger("Sandwich_Finisher_T", "ThighSandwich", "GTSBEH_T_ButtState_Finisher");
		AnimationManager::RegisterTrigger("Sandwich_Grow_T", "ThighSandwich", "GTSBEH_T_ButtState_Grow");
		AnimationManager::RegisterTrigger("Sandwich_GrindStart_T", "ThighSandwich", "GTSBEH_T_ButtState_GrindStart");
		AnimationManager::RegisterTrigger("Sandwich_GrindStop_T", "ThighSandwich", "GTSBEH_T_ButtState_GrindStop");
		AnimationManager::RegisterTrigger("Sandwich_GrindAbort_T", "ThighSandwich", "GTSBEH_T_ButtState_Abort");
		AnimationManager::RegisterTrigger("Sandwich_UB_T", "ThighSandwich", "GTSBEH_T_Sandwich_UB");
	}
}