#include "Managers/Animation/CleavageEvents.hpp"

#include "Managers/Animation/Controllers/VoreController.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/CleavageState.hpp"

#include "Managers/Damage/SizeHitEffects.hpp"
#include "Managers/Damage/TinyCalamity.hpp"

#include "Managers/AI/AIFunctions.hpp"
#include "Managers/Animation/Grab.hpp"
#include "Managers/Perks/PerkHandler.hpp"
#include "Managers/GtsSizeManager.hpp"
#include "Managers/Rumble.hpp"

#include "Magic/Effects/Common.hpp"
#include "Utils/Looting.hpp"
#include "API/Racemenu.hpp"

#include "Utils/KillDataUtils.hpp"

#include "Managers/Audio/MoansLaughs.hpp"

using namespace GTS;

namespace {

    void Task_FixTinyAnimation(Actor* aGiant) {

        const std::string TaskName = std::format("FixTinyCleavageTask_{}", aGiant->formID);
        auto Tiny = Grab::GetHeldActor(aGiant);

        if (!Tiny) {
            return;
        }

        const ActorHandle TinyHandle = Tiny->CreateRefHandle();
        const ActorHandle GiantHandle = aGiant->CreateRefHandle();

		TaskManager::Run(TaskName, [=](auto& progressData) {

	        const auto TinyActor = TinyHandle.get().get();
	        const auto GiantActor = GiantHandle.get().get();

	        if (!TinyActor || !GiantActor) {
	            return false;
	        }

	        const bool InCleavage = IsInCleavageState(GiantActor);
	        if (InCleavage) {
	            return true;
	        }

	        if (IsHostile(GiantActor, TinyActor)) {
	            AnimationManager::StartAnim("Breasts_Idle_Unwilling", TinyActor);
	        }
	        else {
	            AnimationManager::StartAnim("Breasts_Idle_Willing", TinyActor);
	        }

	        return false;

        });
    }

    void Absorb_GrowInSize(Actor* giant, Actor* tiny, float multiplier) {
        if (Runtime::HasPerkTeam(giant, "GTSPerkHugsGreed")) {
			multiplier *= 1.15f;
		}
        float grow_value = 0.08f * multiplier * 0.845f;
        float original = VoreController::ReadOriginalScale(tiny) + 0.875f; // Compensate 
        update_target_scale(giant, grow_value * original, SizeEffectType::kGrow);
    }

    void CancelAnimation(Actor* giant) {
        auto tiny = Grab::GetHeldActor(giant);
        giant->SetGraphVariableBool("GTS_OverrideZ", false);
        if (tiny) {
            KillActor(giant, tiny);
            PerkHandler::UpdatePerkValues(giant, PerkUpdate::Perk_LifeForceAbsorption);
            DrainStamina(giant, "GrabAttack", "GTSPerkDestructionBasics", false, 0.75f);
            tiny->SetGraphVariableBool("GTSBEH_T_InStorage", false);
            SetBetweenBreasts(tiny, false);
            SetBeingEaten(tiny, false);
            SetBeingHeld(tiny, false);
        }

        std::string name = std::format("GrabAttach_{}", giant->formID);
        TaskManager::Cancel(name);

        Grab::DetachActorTask(giant);
		Grab::Release(giant);
    }

    void RecoverAttributes(Actor* giant, ActorValue Attribute, float percentage) {
        float Percent = GetMaxAV(giant, Attribute);
        float value = Percent * percentage;

        if (Runtime::HasPerk(giant, "GTSPerkBreastsMastery2")) {
            value *= 1.5f;
        }

        DamageAV(giant, Attribute, -value);
    }

    void ShrinkTinyWithCleavage(Actor* giant, float scale_limit, float shrink_for, float stamina_damage, bool hearts, bool damage_stamina) {
        Actor* tiny = Grab::GetHeldActor(giant);
        if (tiny) {
            if (get_target_scale(tiny) > scale_limit && shrink_for < 1.0f) {
                DamageAV(giant, ActorValue::kHealth, -get_target_scale(tiny) * 10); // Heal GTS
                set_target_scale(tiny, get_target_scale(tiny) * shrink_for);
            } else {
                set_target_scale(tiny, scale_limit);
            }
            
            if (damage_stamina) {
                DamageAV(tiny, ActorValue::kStamina, stamina_damage);
            }
            if (hearts) {
                SpawnHearts(giant, tiny, 35, 0.4f, false);
            }
        }
    }

    void SuffocateTinyFor(Actor* giant, Actor* tiny, float DamageMult, float Shrink, float StaminaDrain) {
        float TotalHP = GetMaxAV(tiny, ActorValue::kHealth);
        float CurrentHP = GetAV(tiny, ActorValue::kHealth);
        float Threshold = 1.0f;
        
        float damage = TotalHP * DamageMult; // 100% hp by default
        
        if (CurrentHP > Threshold) {
            if (DamageMult > 0 && CurrentHP - damage > Threshold) {
                DamageAV(tiny, ActorValue::kHealth, damage);
            } else {
                if (DamageMult > 0) {
                    SetAV(tiny, ActorValue::kHealth, 1.0f);
                }
            }
            DamageAV(tiny, ActorValue::kStamina, StaminaDrain);
        }
    }

    void Task_RunSuffocateTask(Actor* giant, Actor* tiny) {
        std::string name = std::format("SuffoTask_{}", giant->formID);
		ActorHandle gianthandle = giant->CreateRefHandle();
		ActorHandle tinyhandle = tiny->CreateRefHandle();

        float starting_hppercentage = GetHealthPercentage(tiny);

		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			if (!tinyhandle) {
				return false;
			}
			auto giantref = gianthandle.get().get();
			auto tinyref = tinyhandle.get().get();

			if (!tinyref) {
				return false; // end task in that case
			}
            float damage = 0.0f;//0.0015f * starting_hppercentage * TimeScale();
            SuffocateTinyFor(giantref, tinyref, damage, 0.998f, 0.0035f * TimeScale());

            if (tinyref->IsDead() || GetAV(tinyref, ActorValue::kHealth) <= 0) {
                return false;
            }
			// All good try another frame
			return true;
		});
    }

    ///=================================================================== Functions
    void Deal_breast_damage(Actor* giant, float damage_mult) {
        Actor* tiny = Grab::GetHeldActor(giant);
        if (tiny) {
            if (!IsTeammate(tiny)) {
                Attacked(tiny, giant); // force combat
            }

            float bonus = 1.0f;
            auto& sizemanager = SizeManager::GetSingleton();

			float tiny_scale = get_visual_scale(tiny) * GetSizeFromBoundingBox(tiny);
			float gts_scale = get_visual_scale(giant) * GetSizeFromBoundingBox(giant);

			float sizeDiff = gts_scale/tiny_scale;
			float power = std::clamp(SizeManager::GetSizeAttribute(giant, SizeAttribute::Normal), 1.0f, 1000000.0f);
			float additionaldamage = 1.0f + sizemanager.GetSizeVulnerability(tiny);
			float damage = (Damage_Breast_Squish * damage_mult) * power * additionaldamage * additionaldamage * sizeDiff;
			float experience = std::clamp(damage/1600, 0.0f, 0.06f);

			if (HasSMT(giant)) {
				bonus = 1.65f;
			}

            if (CanDoDamage(giant, tiny, false)) {
                if (Runtime::HasPerkTeam(giant, "GTSPerkGrowingPressure")) {
                    auto& sizemanager = SizeManager::GetSingleton();
                    sizemanager.ModSizeVulnerability(tiny, damage * 0.0010f);
                }

                TinyCalamity_ShrinkActor(giant, tiny, damage * 0.20f * GetDamageSetting());

                SizeHitEffects::PerformInjuryDebuff(giant, tiny, damage * 0.15f, 6);
                if (!IsTeammate(tiny) || IsHostile(giant, tiny)) {
                    InflictSizeDamage(giant, tiny, damage);
                    DamageAV(tiny, ActorValue::kStamina, damage * 0.25f);
                } else {
                    tiny->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kHealth, damage * 5);
                }

                DamageAV(giant, ActorValue::kHealth, -damage * 0.33f);
            }
			
			Rumbling::Once("GrabAttack", tiny, Rumble_Grab_Hand_Attack * bonus * damage_mult, 0.05f, "NPC Root [Root]", 0.0f);
            Runtime::PlaySoundAtNode("GTSSoundThighSandwichImpact", tiny, 1.0f, "NPC Root [Root]");

            Utils_CrushTask(giant, tiny, bonus, false, false, DamageSource::BreastImpact, QuestStage::Crushing);
            ModSizeExperience(giant, experience);
        }
    }

    ///===================================================================

    ///=================================================================== Camera

    void GTS_BS_CamOn(const AnimationEventData& data) {
	    ManageCamera(&data.giant, true, CameraTracking::Breasts_02);
    }

    void GTS_BS_CamOff(const AnimationEventData& data) {
	    ManageCamera(&data.giant, false, CameraTracking::Breasts_02);
    }

    ///===================================================================

    ///=================================================================== Attacks

    void GTS_BS_SpringUp(const AnimationEventData& data) {
        ManageCamera(&data.giant, true, CameraTracking::ObjectB);
    }

    void GTS_BS_BoobsRelease(const AnimationEventData& data) {
    }

    void GTS_BS_DamageTiny_L(const AnimationEventData& data) {
        DamageAV(&data.giant, ActorValue::kStamina, 30 * GetWasteMult(&data.giant));

        Rumbling::Once("HandImpact_L", &data.giant, 0.3f, 0.0f, "L Breast02", 0.0f);
        Rumbling::Once("HandImpact_R", &data.giant, 0.3f, 0.0f, "R Breast02", 0.0f);

        Deal_breast_damage(&data.giant, 1.0f);
    }
    void GTS_BS_DamageTiny_H(const AnimationEventData& data) {
        DamageAV(&data.giant, ActorValue::kStamina, 45 * GetWasteMult(&data.giant));

        Rumbling::Once("HandImpactH_L", &data.giant, 0.55f, 0.0f, "L Breast02", 0.0f);
        Rumbling::Once("HandImpactH_R", &data.giant, 0.55f, 0.0f, "R Breast02", 0.0f);

        Deal_breast_damage(&data.giant, 2.25f);
    }

    void GTS_BS_Shake(const AnimationEventData& data) {
        ShrinkTinyWithCleavage(&data.giant, 0.035f, 0.980f, 25.0f, true, true);

        Rumbling::Once("BreastShake_L", &data.giant, 0.3f, 0.0f, "L Breast02", 0.0f);
        Rumbling::Once("BreastShake_R", &data.giant, 0.3f, 0.0f, "R Breast02", 0.0f);

        if (!IsActionOnCooldown(&data.giant, CooldownSource::Emotion_Laugh)) {
            Task_FacialEmotionTask_Smile(&data.giant, 6.0f, "ShakeSmile", 0.35f);
            ApplyActionCooldown(&data.giant, CooldownSource::Emotion_Laugh);
        }
    }

    ///===================================================================

    ///=================================================================== Vore

    void GTS_BS_OpenMouth(const AnimationEventData& data) {
        auto giant = &data.giant;
		auto tiny = Grab::GetHeldActor(giant);
		auto& VoreData = VoreController::GetSingleton().GetVoreData(giant);
		if (tiny) {
			SetBeingEaten(tiny, true);
			VoreController::GetSingleton().ShrinkOverTime(giant, tiny);
		}
		Task_FacialEmotionTask_OpenMouth(giant, 0.66f, "PrepareVore");
    }
    void GTS_BS_CloseMouth(const AnimationEventData& data) {
    }
    void GTS_BS_PrepareEat(const AnimationEventData& data) {
        auto tiny = Grab::GetHeldActor(&data.giant);
        ManageCamera(&data.giant, true, CameraTracking::ObjectB);
		auto& VoreData = VoreController::GetSingleton().GetVoreData(&data.giant);
        
		if (tiny) {
			VoreData.AddTiny(tiny);
		}
    }
    void GTS_BS_Swallow(const AnimationEventData& data) {
        auto tiny = Grab::GetHeldActor(&data.giant);
		auto& VoreData = VoreController::GetSingleton().GetVoreData(&data.giant);
		if (tiny) {
            if (!AllowDevourment()) {
                Runtime::PlaySoundAtNode("GTSSoundSwallow", &data.giant, 1.0f, "NPC Head [Head]"); // Play sound
            }
			for (auto& tiny: VoreData.GetVories()) {
				if (!AllowDevourment()) {
					VoreData.Swallow();
					if (IsCrawling(&data.giant)) {
						tiny->SetAlpha(0.0f); // Hide Actor
					}
				} else {
					CallDevourment(&data.giant, tiny);
				}
			}
		}

        RecoverAttributes(&data.giant, ActorValue::kHealth, 0.07f);
        RecoverAttributes(&data.giant, ActorValue::kMagicka, 0.07f);
        RecoverAttributes(&data.giant, ActorValue::kStamina, 0.07f);
    }
    void GTS_BS_KillAll(const AnimationEventData& data) {
        auto giant = &data.giant;
		auto tiny = Grab::GetHeldActor(&data.giant);
		if (tiny) {
			SetBeingEaten(tiny, false);
			auto& VoreData = VoreController::GetSingleton().GetVoreData(&data.giant);
			for (auto& tiny: VoreData.GetVories()) {
				VoreData.KillAll();
			}
			giant->SetGraphVariableInt("GTS_GrabbedTiny", 0);
			giant->SetGraphVariableInt("GTS_Grab_State", 0);

			SetBeingHeld(tiny, false);
			Grab::DetachActorTask(giant);
			Grab::Release(giant);
		}
        Animation_Cleavage::LaunchCooldownFor(&data.giant, CooldownSource::Action_Breasts_Vore);
    }

    ///=================================================================== Absorb

    void GTS_BS_AbsorbStart(AnimationEventData& data) {
        Task_FacialEmotionTask_Smile(&data.giant, 3.2f, "AbsorbStart", 0.3f);
        Task_ApplyAbsorbCooldown(&data.giant);


        data.stage = 0;
        auto tiny = Grab::GetHeldActor(&data.giant);
		if (tiny) {
            VoreController::RecordOriginalScale(tiny);
        }
    }

    void GTS_BS_AbsorbPulse(AnimationEventData& data) {
        auto giant = &data.giant;
		auto tiny = Grab::GetHeldActor(&data.giant);

        float growth = 0.0650f;
        data.stage += 1;

		if (tiny) {
            Rumbling::Once("AbsorbPulse_R", giant, 0.8f, 0.05f, "L Breast02", 0.0f, true);
            Rumbling::Once("AbsorbPulse_L", giant, 0.8f, 0.05f, "R Breast02", 0.0f, true);

            float volume = std::clamp(0.12f * get_visual_scale(giant), 0.12f, 1.0f);

            Runtime::PlaySoundAtNode("GTSSoundThighSandwichImpact", tiny, volume, "NPC Root [Root]");

            bool Blocked = IsActionOnCooldown(giant, CooldownSource::Emotion_Laugh);
            if (!Blocked && data.stage < 5) { // We don't want Laugh to happen at the end of absorption
                ApplyActionCooldown(giant, CooldownSource::Emotion_Laugh);
                Task_FacialEmotionTask_Smile(giant, 0.9f, "AbsorbSmile", 0.12f, 0.25f);
                Sound_PlayLaughs(giant, 1.0f, 0.14f, EmotionTriggerSource::Struggle);
            }

            ShrinkTinyWithCleavage(giant, 0.010f, 0.66f, 45.0f, true, true);
            RecoverAttributes(giant, ActorValue::kHealth, 0.025f);

            Absorb_GrowInSize(giant, tiny, growth);
        }
    }

    void GTS_BS_FinishAbsorb(const AnimationEventData& data) {
        Actor* giant = &data.giant;

        auto tiny = Grab::GetHeldActor(giant);
        float growth = 1.05f;

        if (RandomBool(15.0f)) {
            Task_FacialEmotionTask_SlightSmile(giant, 1.75f, "AbsorbSmile", 0.05f);
            Sound_PlayLaughs(giant, 1.0f, 0.14f, EmotionTriggerSource::Superiority);
        } else {
            Task_FacialEmotionTask_Moan(giant, 1.1f, "AbsorbMoan");
            Sound_PlayMoans(giant, 1.0f, 0.14f, EmotionTriggerSource::Absorption);
        }

        if (tiny) {

            SpawnHearts(giant, tiny, 35.0f, 1.15f, false);
            Attacked(tiny, giant);

            AdvanceQuestProgression(giant, tiny, QuestStage::HugSteal, 1.0f, false);

            Rumbling::Once("AbsorbTiny_R", giant, 0.8f, 0.05f, "L Breast02", 0.0f);
            Rumbling::Once("AbsorbTiny_L", giant, 0.8f, 0.05f, "R Breast02", 0.0f);

            ReportDeath(giant, tiny, DamageSource::BreastAbsorb);
            DamageAV(giant, ActorValue::kHealth, -30); // Heal GTS
            Absorb_GrowInSize(giant, tiny, growth);
            

            AdjustSizeReserve(giant, 0.0285f);
            AdjustMassLimit(0.0095f, giant);

            DecreaseShoutCooldown(giant);
		    const auto& MuteAbsorptionScreams = Config::Audio.bMuteBreastAbsorptionDeathScreams;

            KillActor(giant, tiny, MuteAbsorptionScreams);

            if (tiny->formID != 0x14) {
                Disintegrate(tiny);
                SendDeathEvent(giant, tiny);
            }
            else {
                DamageAV(tiny, ActorValue::kHealth, 999999);
                tiny->KillImpl(giant, 1, true, true);
            }

            std::string taskname = std::format("MergeWithTiny_{}_{}", giant->formID, tiny->formID);
            ActorHandle giantHandle = giant->CreateRefHandle();
            ActorHandle tinyHandle = tiny->CreateRefHandle();
            TaskManager::RunOnce(taskname, [=](auto& update) {
                if (!tinyHandle) {
                    return;
                }
                if (!giantHandle) {
                    return;
                }

                auto giantref = giantHandle.get().get();
                auto tinyref = tinyHandle.get().get();

                PerkHandler::UpdatePerkValues(giantref, PerkUpdate::Perk_LifeForceAbsorption);

                TransferInventory(tinyref, giantref, get_visual_scale(tinyref) * GetSizeFromBoundingBox(tinyref), false, true, DamageSource::Vored, true);
                // Actor Reset is done inside TransferInventory:StartResetTask!
                });

            RecoverAttributes(giant, ActorValue::kHealth, 0.05f);
            ModSizeExperience(giant, 0.235f);
        }
    }

    static void GrowBreastsOverTime(Actor* giant) {
    	// Should probably be capped at +50 or +100% of natural breast size
        // As a fun thing can probably even try to calculate total player weight based on morph values if we will manage to use RaceMenu functions directly
        // If RaceMenu API won't be found - it won't work. It doesn't on my end at least.
        std::string taskname = std::format("GrowBreasts_{}", giant->formID);
        ActorHandle giantHandle = giant->CreateRefHandle();

        bool AllowBreastGrow = Config::Advanced.bEnlargeBreastsOnAbsorption;  // Should be tied to UI settings
        if (!AllowBreastGrow) {
            return;
        }

        auto transient = Transient::GetActorData(giant);
        double startTime = Time::WorldTimeElapsed();

        float duration = 3.0f;
        float total_size_add = 1.0f;
        float initial_size = 0.0f;
        if (transient) {
            initial_size = transient->BreastSizeBuff;
        }

        TaskManager::Run(taskname, [=](auto& progressData) {
            if (!giantHandle) {
                return false;
            }

            double endTime = Time::WorldTimeElapsed();

            Actor* giant = giantHandle.get().get();
            float timepassed = endTime - startTime;
            float breast_buff = (initial_size)+(timepassed * 0.33f) * total_size_add;

            GTS::Racemenu::SetMorph(giant, "Breasts", breast_buff, true);

            if (timepassed >= static_cast<double>(duration)) {
                if (transient) {
                    transient->BreastSizeBuff = breast_buff;
                }
                return false;
            }
            return true;
        });
    }

    void GTS_BS_GrowBoobs(const AnimationEventData& data) {
        Animation_Cleavage::LaunchCooldownFor(&data.giant, CooldownSource::Action_Breasts_Absorb);
        //TODO Implement Propper Racemenu Scaling
        //GrowBreastsOverTime(&data.giant);
    }

    ///===================================================================

    ///=================================================================== Suffocate

    void GTS_BS_SufoStart(const AnimationEventData& data) { 
        auto tiny = Grab::GetHeldActor(&data.giant);
        if (tiny) {
            SuffocateTinyFor(&data.giant, tiny, 0.10f, 0.85f, 25.0f);
            Task_RunSuffocateTask(&data.giant, tiny);
            SpawnHearts(&data.giant, tiny, 35, 0.35f, false);
        }
        Task_FacialEmotionTask_Smile(&data.giant, 1.8f, "SufoStart", RandomFloat(0.0f, 0.75f), 0.2f);

        int rng = RandomInt(0, 3);
        if (rng >= 2) {
            Sound_PlayLaughs(&data.giant, 1.0f, 0.14f, EmotionTriggerSource::Struggle);
        }
    }
    void GTS_BS_SufoStop(const AnimationEventData& data) {}

    void GTS_BS_SufoKill(const AnimationEventData& data) {
        Animation_Cleavage::LaunchCooldownFor(&data.giant, CooldownSource::Action_Breasts_Suffocate);
        ModSizeExperience(&data.giant, 0.235f);
        CancelAnimation(&data.giant);
    }
    void GTS_BS_SufoPress(const AnimationEventData& data) {
        auto tiny = Grab::GetHeldActor(&data.giant);
        if (tiny) {
            RecoverAttributes(&data.giant, ActorValue::kMagicka, 0.035f);
            SuffocateTinyFor(&data.giant, tiny, 0.35f, 0.85f, 25.0f);
            SpawnHearts(&data.giant, tiny, 35, 0.50f, false);
        }
        Task_FacialEmotionTask_Smile(&data.giant, 1.2f, "SufoPress");
        Sound_PlayLaughs(&data.giant, 1.0f, 0.14f, EmotionTriggerSource::Struggle);
    }
    
    void GTS_BS_PullTiny(const AnimationEventData& data) {
        auto tiny = Grab::GetHeldActor(&data.giant);
        if (tiny) {
            Task_FacialEmotionTask_Smile(&data.giant, 1.6f, "SufoPullOut", 0.25f);
            Rumbling::Once("PullOut_R", &data.giant, 0.75f, 0.0f, "L Breast02", 0.0f);
            Rumbling::Once("PullOut_L", &data.giant, 0.75f, 0.0f, "R Breast02", 0.0f);

            Attachment_SetTargetNode(&data.giant, AttachToNode::ObjectL);
            IncrementKillCount(&data.giant, SizeKillType::kBreastSuffocated);

            ManageCamera(&data.giant, true, CameraTracking::ObjectB);
            SpawnHearts(&data.giant, tiny, 35, 0.50f, false);

            Sound_PlayLaughs(&data.giant, 1.0f, 0.14f, EmotionTriggerSource::Superiority);
        }
    }

    void GTS_BS_HandsLand(const AnimationEventData& data) {
        auto tiny = Grab::GetHeldActor(&data.giant);
        if (tiny) {
            RecoverAttributes(&data.giant, ActorValue::kMagicka, 0.045f);
            SuffocateTinyFor(&data.giant, tiny, 0.20f, 0.75f, 20.0f);

            Rumbling::Once("HandLand_R", &data.giant, 0.45f, 0.0f, "L Breast02", 0.0f);
            Rumbling::Once("HandLand_L", &data.giant, 0.45f, 0.0f, "R Breast02", 0.0f);

            float volume = std::clamp(0.12f * get_visual_scale(&data.giant), 0.12f, 1.0f);
            Runtime::PlaySoundAtNode("GTSSoundThighSandwichImpact", tiny, volume, "NPC Root [Root]");
        }
    }

    void GTS_BS_OverrideZ_ON(const AnimationEventData& data) { 
        data.giant.SetGraphVariableBool("GTS_OverrideZ", true);
    }
    void GTS_BS_OverrideZ_OFF(const AnimationEventData& data) { 
        data.giant.SetGraphVariableBool("GTS_OverrideZ", false);
    }

    ///===================================================================

    ///=================================================================== Utils

    void GTS_BS_SwitchToObjectB(const AnimationEventData& data) {Attachment_SetTargetNode(&data.giant, AttachToNode::ObjectB);}
    void GTS_BS_SwitchToCleavage(const AnimationEventData& data) {Attachment_SetTargetNode(&data.giant, AttachToNode::None);}

    void GTS_BS_Poke(const AnimationEventData& data) {
        auto tiny = Grab::GetHeldActor(&data.giant);
        if (tiny) {
            Task_FacialEmotionTask_Smile(&data.giant, 0.7f, "SufoPoke", RandomFloat(0.0f, 0.90f));
            SpawnHearts(&data.giant, tiny, 35, 0.4f, false);
        }
    }
    void GTS_BS_Pat(const AnimationEventData& data) {
        auto tiny = Grab::GetHeldActor(&data.giant);
        if (tiny) {
            SpawnHearts(&data.giant, tiny, 35, 0.4f, false);

            if (IsHostile(&data.giant, tiny)) {
				AnimationManager::StartAnim("Breasts_Idle_Unwilling", tiny);
			} else {
				AnimationManager::StartAnim("Breasts_Idle_Willing", tiny);
			}
        }
    }

    void GTSBEH_Boobs_StartTransition(const AnimationEventData& data) {
        Actor* giant = &data.giant;
        Actor* tiny = Grab::GetHeldActor(giant);
        if (tiny) {
            tiny->SetGraphVariableBool("GTSBEH_T_InStorage", false);
            SetBetweenBreasts(tiny, false);
            SetBeingEaten(tiny, false);
            SetBeingHeld(tiny, false);
        }

        DrainStamina(giant, "GrabAttack", "GTSPerkDestructionBasics", false, 0.75f);
        giant->SetGraphVariableInt("GTS_GrabbedTiny", 0); // Tell behaviors 'we have nothing in our hands'. A must.
        giant->SetGraphVariableInt("GTS_Grab_State", 0);
        giant->SetGraphVariableInt("GTS_Storing_Tiny", 0);
        Grab::DetachActorTask(giant);
        Grab::Release(giant);
    }

    void GTS_BS_ResetTiny(const AnimationEventData& data) {
        auto Tiny = Grab::GetHeldActor(&data.giant);
        if (!Tiny) {
            return;
        }
        if (IsHostile(&data.giant, Tiny)) {
            AnimationManager::StartAnim("Breasts_Idle_Unwilling", Tiny);
        }
        else {
            AnimationManager::StartAnim("Breasts_Idle_Willing", Tiny);
        }

    }

    ///===================================================================
}

namespace GTS
{
	void Animation_CleavageEvents::RegisterEvents() {



		AnimationManager::RegisterEvent("GTS_BS_CamOn", "Cleavage", GTS_BS_CamOn);
        AnimationManager::RegisterEvent("GTS_BS_CamOff", "Cleavage", GTS_BS_CamOff);

        AnimationManager::RegisterEvent("GTS_BS_SpringUp", "Cleavage", GTS_BS_SpringUp);
        AnimationManager::RegisterEvent("GTS_BS_BoobsRelease", "Cleavage", GTS_BS_BoobsRelease);
        AnimationManager::RegisterEvent("GTS_BS_DamageTiny_L", "Cleavage", GTS_BS_DamageTiny_L);
        AnimationManager::RegisterEvent("GTS_BS_DamageTiny_H", "Cleavage", GTS_BS_DamageTiny_H);
        AnimationManager::RegisterEvent("GTS_BS_Shake", "Cleavage", GTS_BS_Shake);

        AnimationManager::RegisterEvent("GTS_BS_OpenMouth", "Cleavage", GTS_BS_OpenMouth);
        AnimationManager::RegisterEvent("GTS_BS_CloseMouth", "Cleavage", GTS_BS_CloseMouth);

        AnimationManager::RegisterEvent("GTS_BS_PrepareEat", "Cleavage", GTS_BS_PrepareEat);
        AnimationManager::RegisterEvent("GTS_BS_Swallow", "Cleavage", GTS_BS_Swallow);
        AnimationManager::RegisterEvent("GTS_BS_KillAll", "Cleavage", GTS_BS_KillAll);

        AnimationManager::RegisterEvent("GTS_BS_AbsorbStart", "Cleavage", GTS_BS_AbsorbStart);
        AnimationManager::RegisterEvent("GTS_BS_AbsorbPulse", "Cleavage", GTS_BS_AbsorbPulse);
        AnimationManager::RegisterEvent("GTS_BS_FinishAbsorb", "Cleavage", GTS_BS_FinishAbsorb);
        AnimationManager::RegisterEvent("GTS_BS_GrowBoobs", "Cleavage", GTS_BS_GrowBoobs);

        AnimationManager::RegisterEvent("GTS_BS_SufoStart", "Cleavage", GTS_BS_SufoStart);
        AnimationManager::RegisterEvent("GTS_BS_SufoStop", "Cleavage", GTS_BS_SufoStop);
        AnimationManager::RegisterEvent("GTS_BS_SufoKill", "Cleavage", GTS_BS_SufoKill);
        AnimationManager::RegisterEvent("GTS_BS_SufoPress", "Cleavage", GTS_BS_SufoPress);
        AnimationManager::RegisterEvent("GTS_BS_PullTiny", "Cleavage", GTS_BS_PullTiny);
        AnimationManager::RegisterEvent("GTS_BS_OverrideZ_ON", "Cleavage", GTS_BS_OverrideZ_ON);
        AnimationManager::RegisterEvent("GTS_BS_OverrideZ_OFF", "Cleavage", GTS_BS_OverrideZ_OFF);

        AnimationManager::RegisterEvent("GTS_BS_SwitchToObjectB", "Cleavage", GTS_BS_SwitchToObjectB);
        AnimationManager::RegisterEvent("GTS_BS_SwitchToCleavage", "Cleavage", GTS_BS_SwitchToCleavage);
        AnimationManager::RegisterEvent("GTS_BS_ResetTiny", "Cleavage", GTS_BS_ResetTiny);

        AnimationManager::RegisterEvent("GTS_BS_Shake", "Cleavage", GTS_BS_Shake);
        AnimationManager::RegisterEvent("GTS_BS_HandsLand", "Cleavage", GTS_BS_HandsLand);
        AnimationManager::RegisterEvent("GTS_BS_Poke", "Cleavage", GTS_BS_Poke);
        AnimationManager::RegisterEvent("GTS_BS_Pat", "Cleavage", GTS_BS_Pat);

        AnimationManager::RegisterEvent("GTSBEH_Boobs_StartTransition", "Cleavage", GTSBEH_Boobs_StartTransition);
	}
} 
 
 
 