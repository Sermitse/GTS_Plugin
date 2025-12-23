#include "Utils/Actions/VoreUtils.hpp"

#include "Config/Config.hpp"

#include "Utils/SurvivalMode.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Rumble.hpp"
#include "Managers/Animation/Controllers/VoreController.hpp"
#include "Magic/Effects/Common.hpp"

#include "Utils/DeathReport.hpp"
#include "Utils/KillDataUtils.hpp"

#include "Managers/Audio/MoansLaughs.hpp"
#include "Managers/Morphs/MorphManager.hpp"
#include "Managers/Perks/PerkHandler.hpp"

using namespace GTS;

namespace {

    void BuffAttributes(Actor* giant) {
		if (giant) {
			if (Runtime::HasPerk(giant, Runtime::PERK.GTSPerkFullAssimilation)) { // Permamently increases random AV after eating someone
				float TotalMod = 0.33f;
				int Boost = RandomInt(0, 2);
				if (Boost == 0) {
					AddStolenAttributesTowards(giant, ActorValue::kHealth, TotalMod);
				} else if (Boost == 1) {
					AddStolenAttributesTowards(giant, ActorValue::kStamina, TotalMod);
				} else if (Boost >= 2) {
					AddStolenAttributesTowards(giant, ActorValue::kMagicka, TotalMod);
				}
			}
		}
	}

    void UpdateVoreValues(Actor* giant, Actor* tiny, float& Health_Regeneration, float& growth, float& duration) { // Updates values 
        if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkVoreAbility)) {
			Health_Regeneration = GetMaxAV(tiny, ActorValue::kHealth) * 0.2f; // Default hp/sp regen
		}
		if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkVoreHeal)) {
			Health_Regeneration = GetMaxAV(tiny, ActorValue::kHealth) * 0.8f; // 4 times stronger hp/sp regen
			duration *= 0.5f; // 50% faster vore, means -50% duration
            growth *= 2.0f; // but 100% more growth
		}
		if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkAdditionalGrowth)) {
			growth *= 1.25f; // 25% stronger growth
		}
    }
}

namespace GTS {

	VoreInformation GetVoreInfo(Actor* giant, Actor* tiny, float growth_mult) {
		float recorded_scale = VoreController::ReadOriginalScale(tiny);
		float Health_Regeneration = 0.0f; // No hp regen by default
		float duration = 80.0f; // 80 seconds duration by default
		float growth = 0.275f; // Default power of gaining size

		UpdateVoreValues(giant, tiny, Health_Regeneration, growth, duration);

		float bounding_box = GetSizeFromBoundingBox(tiny);
		float Vore_Power = growth * growth_mult * bounding_box; // power of most buffs that we start

		VoreInformation VoreInfo = VoreInformation { // Create Vore Info, used for Task functions
			.giantess = giant,
			.WasLiving = IsLiving(tiny),
			.Scale = recorded_scale,
			.Vore_Power = Vore_Power,
			.Health_Regeneration = Health_Regeneration,
			.Box_Scale = bounding_box,
			.Duration = duration,
			.Tiny_Name = tiny->GetDisplayFullName(),
		};

		return VoreInfo;
	}

	float GetGrowthFormula(float a_giantScale, float a_tinyScale, bool a_devourment) {
		const float g = a_giantScale;
		const float t = a_tinyScale;
		constexpr float b = 5.0f; //Dampening factor for GTS formula, change to whatever you think looks good
		constexpr float u = 0.1f; //Dampening factor for DV formula, change to whatever you think looks good
		const float a = g / (g * u);

		if (a_devourment) {
		//https://www.desmos.com/calculator/5abytasrni
			const float out = (t / sqrt(2 * a * sqrt(g * g))) * 1.1f;
			//Notify("DV Gained {} Scale", out);
			return out;
		}

		//https://www.desmos.com/calculator/tpklrq4pxn
		const float out = pow(t, 2.0f / b) / (sqrt(2 * b * sqrt(g * g)));
		//Notify("GTS Gained {} Scale", out);
		return out;
        
	}

    void CantVorePlayerMessage(Actor* giant, Actor* tiny, float sizedifference) {
		if (sizedifference < Action_Vore) {
			std::string message = std::format("{} is too big to be eaten: x{:.2f}/{:.2f}", PlayerCharacter::GetSingleton()->GetName(), sizedifference, Action_Vore);
			NotifyWithSound(tiny, message);
		}
	}

    void Vore_AdvanceQuest(Actor* pred, Actor* tiny, bool WasDragon, bool WasGiant) {
		if (pred->formID == 0x14 && WasDragon) {
			CompleteDragonQuest(tiny, ParticleType::Blue, false);
			return;
		}
		if (WasGiant) {
			AdvanceQuestProgression(pred, tiny, QuestStage::Giant, 1, true);
		} else {
			AdvanceQuestProgression(pred, tiny, QuestStage::Vore, 1, true);
		}
	}

	void Task_Vore_FinishVoreBuff(const VoreInformation& VoreInfo, Actor* tiny, int amount_of_tinies, bool Devourment) {

		float sizePower = VoreInfo.Vore_Power;
        float Box_Scale = VoreInfo.Box_Scale; // Base scale of Bounding Box
        float tinySize = VoreInfo.Scale; // Pre-Shrink scale of actor (before entering mouth)
		
		bool WasLiving = VoreInfo.WasLiving;

		Actor* giant = VoreInfo.giantess;

		float multiplier = 1.0f;
		if (Devourment) {
			multiplier = 0.5f; // Because Devourment does it 2 times. Affects Attribute and Weight Gain
		}

        float size_gain = sizePower * 0.5f * GetGrowthFormula(get_visual_scale(giant), tinySize, Devourment);

		//Multiply vore gain
		size_gain *= Config::Gameplay.ActionSettings.fVoreGainMult;

        if (giant) {
			if (tiny) {
            	ReportDeath(giant, tiny, DamageSource::Vored, true);
			}
			PerkHandler::Perks_Cataclysmic_ManageStacks(giant, PerkAction::Increase);

            GainWeight(giant, 3.0f * tinySize * amount_of_tinies * multiplier); // Self explanatory
            ModSizeExperience(giant, 0.20f * multiplier + (tinySize * 0.02f)); // Gain Size Mastery XP

			update_target_scale(giant, size_gain, SizeEffectType::kGrow); // Give GTS Size
			
            AdjustSizeReserve(giant, size_gain);
            BuffAttributes(giant);

            if (giant->formID == 0x14) {
				SurvivalMode_AdjustHunger(giant, tinySize * Box_Scale * multiplier, WasLiving, true);
                AdjustMassLimit(0.0106f * multiplier, giant);
            }
            if (VoreController::GetSingleton().GetVoreData(giant).GetTimer() == true) {
				Task_FacialEmotionTask_Moan(giant, 1.0f, "Vore", RandomFloat(0.0f, 0.35f));
                Sound_PlayMoans(giant, 1.0f, 0.14f, EmotionTriggerSource::Vore);
            }

			//Remove tiny from belly scale
			if (!AllowDevourment()) {
				MorphManager::AlterMorph(VoreInfo.giantess, MorphManager::Category::kBelly, MorphManager::Action::kModify, -Config::Gameplay.ActionSettings.fBellyAbsorbIncrementBy, MorphManager::UpdateKind::kGradual, 1.0f);
				if (auto data = Transient::GetActorData(VoreInfo.giantess)) {
					data->VoreCurrentlyAbsorbingCount -= 1;
					data->VoreCurrentlyAbsorbingCount = std::max(data->VoreCurrentlyAbsorbingCount, 0); //Clamp to 0
				}
			}

            Rumbling::Once("GrowthRumble", giant, 1.75f, 0.30f);
        }
	}

	void Task_Vore_StartVoreBuff(Actor* giant, Actor* tiny, int amount_of_tinies) {

		if (!AllowDevourment()) { // ONLY when using default GTS vore logic, not Devourment one
			std::string name = std::format("Vore_Buff_{}_{}", giant->formID, tiny->formID);
			VoreInformation VoreInfo = GetVoreInfo(giant, tiny, 1.0f);
			ActorHandle gianthandle = giant->CreateRefHandle();
			ActorHandle tinyhandle = tiny->CreateRefHandle();

			double start_time = Time::WorldTimeElapsed();

			float giantSize = get_visual_scale(giant);

			float Regeneration = VoreInfo.Health_Regeneration;
			float sizePower = VoreInfo.Vore_Power;
			float Duration = VoreInfo.Duration;
            float tinySize = VoreInfo.Scale;

			float formula = GetGrowthFormula(giantSize, tinySize, false);
			float size_gain = (sizePower * formula * TimeScale()) / 5000;

			if (Config::Gameplay.ActionSettings.bEnableBellyMorph) {
				MorphManager::AlterMorph(VoreInfo.giantess, MorphManager::Category::kBelly, MorphManager::Action::kModify, Config::Gameplay.ActionSettings.fBellyAbsorbIncrementBy, MorphManager::UpdateKind::kGradual, 1.0f);
			}

			if (auto data = Transient::GetActorData(VoreInfo.giantess)) {
				data->VoreCurrentlyAbsorbingCount += 1;
			}

			TaskManager::Run(name, [=](auto& progressData) {
				if (!gianthandle) {
					return false;
				}
				if (!tinyhandle) {
					return false;
				}
				double timepassed = Time::WorldTimeElapsed() - start_time;
				auto giantref = gianthandle.get().get();
				
				float regen_attributes = GetMaxAV(giantref, ActorValue::kHealth) * 0.0006f;
				float health = std::clamp(Regeneration/4000.0f, 0.0f, regen_attributes);

				DamageAV(giantref, ActorValue::kHealth, -health * TimeScale());
				DamageAV(giantref, ActorValue::kStamina, -health * TimeScale()); 
				// Restore HP and Stamina for GTS

				if (get_target_scale(giantref) < get_max_scale(giantref)) { // For some reason likes to surpass size limit by ~0.03 (multiplies by race scale)
					update_target_scale(giantref, size_gain, SizeEffectType::kGrow);
					AddStolenAttributes(giantref, size_gain * TimeScale());
				}

				if (timepassed >= Duration) {
					Task_Vore_FinishVoreBuff(VoreInfo, tinyhandle.get().get(), amount_of_tinies, false);
					if (giantref->formID == 0x14) {
						shake_camera(giantref, 0.50f, 0.75f);
					}
					return false; // Finish Task
				}

				return true;
			});
		}
	}

	void DevourmentBonuses(Actor* Pred, Actor* Prey, bool Digested, float mult) {
		VoreInformation VoreInfo = GetVoreInfo(Pred, Prey, mult);

		if (Digested) {
			Vore_AdvanceQuest(Pred, Prey, IsDragon(Prey), IsGiant(Prey)); // Progress quest
			if (Pred->formID == 0x14) {
				shake_camera(Pred, 0.50f, 0.75f);
			}
		}

		Task_Vore_FinishVoreBuff(VoreInfo, Prey, 1, true);
	}

	void Devourment_Compatibility(Actor* Pred, Actor* Prey, bool Digested) { // Called from the GTSManagerQuest script, takes priority over GTS Vore
		if (Pred && Prey) {
			auto Data = Transient::GetActorData(Prey);
			if (Data) {
				bool& Devoured = Data->DevourmentDevoured;
				bool& Eaten = Data->DevourmentEaten;
				if (!Eaten) { // Stage 1: Health Bar is depleted (actor is dead)
					DevourmentBonuses(Pred, Prey, false, 0.5f); // Value is multiplier of growth power.
					Eaten = true;
				} else if (Digested && !Devoured) { // Stage 2: actor is fully absorbed 
					Notify("{} was devoured by {}", Prey->GetDisplayFullName(), Pred->GetDisplayFullName());
					Cprint("{} was devoured by {}", Prey->GetDisplayFullName(), Pred->GetDisplayFullName());
					DevourmentBonuses(Pred, Prey, true, 1.5f); // Value is multiplier of growth power.
					RecordKill(Pred, Prey, DeathType::kEaten);
					Devoured = true;
				}
			}
		}
	}
}