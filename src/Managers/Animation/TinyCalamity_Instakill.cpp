#include "Managers/Animation/TinyCalamity_Instakill.hpp"
#include "Managers/Animation/TinyCalamity_Shrink.hpp"
#include "Managers/Animation/AnimationManager.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/TurnTowards.hpp"

#include "Managers/Perks/PerkHandler.hpp"
#include "Managers/AI/aifunctions.hpp"
#include "Managers/Audio/Footstep.hpp"
#include "Managers/Rumble.hpp"

#include "Magic/Effects/Common.hpp"

#include "Utils/AttachPoint.hpp"
#include "Utils/KillDataUtils.hpp"

#include "Managers/Audio/MoansLaughs.hpp"

using namespace GTS;

namespace {
	void FingerSnap_Execute(Actor* giant, Actor* tiny) {
		ReportDeath(giant, tiny, DamageSource::EraseFromExistence);

		AdvanceQuestProgression(giant, tiny, QuestStage::ShrinkToNothing, 0.25f, false);
		ModSizeExperience(giant, 0.24f * 0.25f); // Adjust Size Matter skill
		ShrinkToNothingManager::SpawnDeathEffects(tiny);
		Attacked(tiny, giant);

		const auto& MuteSnapDeath = Config::Audio.bMuteFingerSnapDeathScreams;
		
		KillActor(giant, tiny, MuteSnapDeath);
		AddSMTDuration(giant, 5.0f);

		DecreaseShoutCooldown(giant);

		PerkHandler::UpdatePerkValues(giant, PerkUpdate::Perk_LifeForceAbsorption);
		ShrinkToNothingManager::TransferInventoryTask(giant, tiny); // Also plays STN sound
	}

	void SpawnRuneOnTiny(Actor* tiny, float mult) {
		auto node = find_node(tiny, "AnimObjectA");
		if (node) {
			SpawnParticle(tiny, 3.00f, "GTS/gts_calamityrune.nif", NiMatrix3(), node->world.translate, 1.0f * mult, 7, node); 
		}
	}

	void SpawnEffectsOverTime(Actor* tiny) {
		std::string name = std::format("CalamityEffects_{}", tiny->formID);
		ActorHandle tinyhandle = tiny->CreateRefHandle();
		TaskManager::Run(name, [=](auto& progressData) {
			if (!tinyhandle) {
				return false;
			}
			auto tinyref = tinyhandle.get().get();

			bool IsDead = tinyref->IsDead();
			bool OnCooldown = IsActionOnCooldown(tinyref, CooldownSource::Misc_ShrinkParticle);
			if (IsDead || !IsBeingKilledWithMagic(tinyref)) {
				return false;
			}
			if (!OnCooldown) {
				SpawnCustomParticle(tinyref, ParticleType::Red, NiPoint3(), "NPC Root [Root]", get_visual_scale(tinyref) * 0.85f);
				ApplyActionCooldown(tinyref, CooldownSource::Misc_ShrinkParticle);
			}

			// All good try another frame
			return true;
		});
	}

	void AttachToObjectBTask(Actor* giant) {
		auto Tinies = Animation_TinyCalamity::GetShrinkActors(giant);
		for (auto tiny: Tinies) {
			if (tiny) {
				std::string name = std::format("CalamityKill_{}_{}", giant->formID, tiny->formID);
				ActorHandle gianthandle = giant->CreateRefHandle();
				ActorHandle tinyhandle = tiny->CreateRefHandle();
				TaskManager::Run(name, [=](auto& progressData) {
					if (!gianthandle) {
						return false;
					}
					if (!tinyhandle) {
						return false;
					}
					auto tinyref = tinyhandle.get().get();
					auto giantref = gianthandle.get().get();
					
					ShutUp(tinyref);
					ShutUp(giantref);

					Anims_FixAnimationDesync(giantref, tinyref, false); // Share GTS Animation Speed with hugged actor to avoid de-sync

					bool IsDead = (giantref->IsDead() || tinyref->IsDead());
					if (IsDead || !IsGtsBusy(giantref)) {
						return false;
					}
					// Ensure they are NOT in ragdoll
					AttachToObjectB(giantref, tinyref);
					ForceRagdoll(tinyref, false);
					
					if (!FaceOpposite(giant, tiny)) {
						return false;
					}

					// All good try another frame
					return true;
				});
			}
		}
	}

	void ShrinkTinyUntil(Actor* giant, float to, float magnitude) {
		auto tinies = Animation_TinyCalamity::GetShrinkActors(giant);
		for (auto tiny: tinies) {
			if (tiny) {
				float shrink_magnitude = -magnitude;

				ActorHandle tinyHandle = tiny->CreateRefHandle();

				std::string name = std::format("ShrinkTowards_{}_{}", tiny->formID, to);

				TaskManager::Run(name, [=](auto& progressData) {
					Actor* actor = tinyHandle.get().get();
					if (!actor) {
						return false;
					}

					float scale = get_visual_scale(actor);

					if (scale > to) {
						override_actor_scale(actor, shrink_magnitude * 0.225f * TimeScale(), SizeEffectType::kNeutral);
						//log::info("Visual Scale of {} is {}, to: {}", actor->GetDisplayFullName(), scale, to);
						if (get_target_scale(actor) < to) {
							set_target_scale(actor, to);
							
							return false;
						}
						return true;
					} else {
						return false;
					}

					return false;
				});
			}
		}
	}
	

	void DelayedLaunch(Actor* giant, float radius, float power, FootEvent Event) {
		std::string taskname = std::format("DelayLaunch_{}", giant->formID);
		ActorHandle giantHandle = giant->CreateRefHandle();

		double Start = Time::WorldTimeElapsed();

		TaskManager::Run(taskname, [=](auto& update){ // Needed to prioritize grind over launch
			if (!giantHandle) {
				return false;
			}
			Actor* giantref = giantHandle.get().get();
			double Finish = Time::WorldTimeElapsed();

			double timepassed = Finish - Start;

			if (timepassed > 0.03) {
				LaunchTask(giantref, radius, power, Event);
				return false;
			}

			return true;
		});
	}

	void DoFootstep(Actor* giant, bool right, float animSpeed, FootEvent Event, DamageSource Source, std::string_view Node, std::string_view rumble) {
		float perk = GetPerkBonus_Basics(giant);
		float smt = 1.0f;
		float dust = 1.25f;
		if (HasSMT(giant)) {
			smt = 1.5f;
			dust = 1.45f;
		}

		float hh = GetHighHeelsBonusDamage(giant, true);
		float shake_power = Rumble_Stomp_Normal * smt * hh;

		Rumbling::Once(rumble, giant, shake_power, 0.0f, Node, 1.10f);
		DoDamageEffect(giant, Damage_Walk_Defaut * perk, Radius_Stomp, 10, 0.25f, Event, 1.0f, Source);
		DoDustExplosion(giant, dust + (animSpeed * 0.05f), Event, Node);
		DoFootstepSound(giant, 1.0f, Event, Node);
		
		DrainStamina(giant, "StaminaDrain_Stomp", Runtime::PERK.GTSPerkDestructionBasics, false, 1.8f); // cancel stamina drain

		DelayedLaunch(giant, 0.90f * perk, 2.2f, Event);

		FootStepManager::PlayVanillaFootstepSounds(giant, right);
	}

    void GTS_LC_Start(AnimationEventData& data) { 
		auto tinies = Animation_TinyCalamity::GetShrinkActors(&data.giant);
		for (auto tiny: tinies) {
			if (tiny) {
				Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundTinyCalamity_SpawnRune, tiny, 1.0f, "NPC Root [Root]");
				SpawnCustomParticle(tiny, ParticleType::Red, NiPoint3(), "NPC Root [Root]", 0.75f);
				SpawnRuneOnTiny(&data.giant, 1.0f);
			}
		}
    }

	void GTS_LC_FootstepL(AnimationEventData& data) { 
		DoFootstep(&data.giant, false, 1.0f, FootEvent::Left, DamageSource::CrushedLeft, "NPC L Foot [Lft ]", "NormalRumbleL");
    }
	void GTS_LC_FootstepR(AnimationEventData& data) { 
		DoFootstep(&data.giant, true, 1.0f, FootEvent::Right, DamageSource::CrushedRight, "NPC R Foot [Rft ]", "NormalRumbleR");
    }
	void GTS_LC_AttachToObject(AnimationEventData& data) { 
		auto tinies = Animation_TinyCalamity::GetShrinkActors(&data.giant);
		for (auto tiny: tinies) {
			if (tiny) {
				ChanceToScare(&data.giant, tiny, 6.0f, 1, false); // force actor to flee 
				AttachToObjectBTask(&data.giant);
				StartCombat(tiny, &data.giant);
			}
		}
    }

	void GTS_LC_Liftup(AnimationEventData& data) {
		Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundTinyCalamity_RuneReady, &data.giant, 1.0f, "NPC R Hand [RHnd]");
		Task_FacialEmotionTask_Anger(&data.giant, 1.25f + RandomFloat(0.01f, 0.25f), "LiftUpSmile", 0.125f);
		auto tinies = Animation_TinyCalamity::GetShrinkActors(&data.giant);
		for (auto tiny: tinies) { 
			if (tiny) {
				SpawnCustomParticle(tiny, ParticleType::Red, NiPoint3(), "AnimObjectA", 1.50f);
				SpawnCustomParticle(tiny, ParticleType::Red, NiPoint3(), "AnimObjectA", 1.25f);
				SpawnCustomParticle(tiny, ParticleType::Red, NiPoint3(), "AnimObjectA", 1.00f);
				SpawnEffectsOverTime(tiny);
			}
		}
		
		Rumbling::Once("LiftUp", &data.giant, 6.0f, 0.025f, true);
	}

	void GTS_LC_CameraON(AnimationEventData& data) { 
		ManageCamera(&data.giant, true, CameraTracking::Hand_Right);
    }
	void GTS_LC_CameraOFF(AnimationEventData& data) { 
		ManageCamera(&data.giant, false, CameraTracking::Hand_Right);
    }
	void GTS_LC_Shrink_1(AnimationEventData& data) { 
		ShrinkTinyUntil(&data.giant, 0.28f, 0.0420f);
    }
	void GTS_LC_Shrink_2(AnimationEventData& data) { 
		ShrinkTinyUntil(&data.giant, 0.20f, 0.0175f);
    }
	void GTS_LC_Shrink_3(AnimationEventData& data) { 
		Task_FacialEmotionTask_Smile(&data.giant, 1.25f + RandomFloat(0.01f, 0.25f), "KillSmile", 0.12f);
		/*for (auto tiny: Animation_TinyCalamity::GetShrinkActors(&data.giant)) { 
			if (tiny) {
				SpawnRuneOnTiny(&data.giant, 0.8f);
			}
		}*/
		ShrinkTinyUntil(&data.giant, 0.012f, 0.00525f);
    }
	void GTS_LC_FingerSnap(AnimationEventData& data) { 
		Task_FacialEmotionTask_SlightSmile(&data.giant, 1.525f + RandomFloat(0.35f, 0.75f), "SnapSmile", 0.25f);
		int laugh_rng = RandomInt(1, 4);
		if (laugh_rng >= 1) {
			Sound_PlayLaughs(&data.giant, 1.0f, 0.14f, EmotionTriggerSource::Superiority);
		}
		
		Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundTinyCalamity_FingerSnap, &data.giant, 1.0f, "NPC R Hand [RHnd]");
		auto tinies = Animation_TinyCalamity::GetShrinkActors(&data.giant);
		Rumbling::Once("FingerSnap", &data.giant, 7.25f, 0.025f, true);
		for (auto tiny: tinies) {
			if (tiny) {
				Animation_TinyCalamity::GetSingleton().ResetActors(&data.giant);
				FingerSnap_Execute(&data.giant, tiny);
			}
		}
    }
}

namespace GTS
{
	void Animation_TinyCalamity_InstaKill::RegisterEvents() {
		AnimationManager::RegisterEvent("GTS_LC_Start", "CalamityIC", GTS_LC_Start); 
		AnimationManager::RegisterEvent("GTS_LC_FootstepL", "CalamityIC", GTS_LC_FootstepL);
        AnimationManager::RegisterEvent("GTS_LC_FootstepR", "CalamityIC", GTS_LC_FootstepR);
        AnimationManager::RegisterEvent("GTS_LC_AttachToObject", "CalamityIC", GTS_LC_AttachToObject);
		AnimationManager::RegisterEvent("GTS_LC_Liftup", "CalamityIC", GTS_LC_Liftup);
        AnimationManager::RegisterEvent("GTS_LC_CameraON", "CalamityIC", GTS_LC_CameraON);
		AnimationManager::RegisterEvent("GTS_LC_CameraOFF", "CalamityIC", GTS_LC_CameraOFF);
		AnimationManager::RegisterEvent("GTS_LC_Shrink_1", "CalamityIC", GTS_LC_Shrink_1);
		AnimationManager::RegisterEvent("GTS_LC_Shrink_2", "CalamityIC", GTS_LC_Shrink_2);
		AnimationManager::RegisterEvent("GTS_LC_Shrink_3", "CalamityIC", GTS_LC_Shrink_3);
		AnimationManager::RegisterEvent("GTS_LC_FingerSnap", "CalamityIC", GTS_LC_FingerSnap);
	}

	void Animation_TinyCalamity_InstaKill::RegisterTriggers() {
		AnimationManager::RegisterTrigger("InstaKill_Start_GTS", "CalamityIC", "GTSBeh_Shrink_Magic_Fatal_Start");
		AnimationManager::RegisterTrigger("InstaKill_Start_Tiny", "CalamityIC", "GTSBeh_T_Shrink_Magic_Fatal_Start");
		// GTS_isCastingShrink
		// GTS_isBeingShrunk (Not added yet)
	}
}