#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Controllers/VoreController.hpp"
#include "Managers/Animation/Grab.hpp"
#include "Managers/Animation/Grab_Play_Events.hpp"

#include "Config/Config.hpp"

#include "Managers/Animation/Grab_Throw.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"

#include "Managers/Damage/SizeHitEffects.hpp"
#include "Managers/Damage/TinyCalamity.hpp"

#include "Managers/GTSSizeManager.hpp"
#include "Managers/Rumble.hpp"

#include "Magic/Effects/Common.hpp"

#include "Utils/Actions/VoreUtils.hpp"

using namespace GTS;
using namespace std;

namespace Grab_Fixes {

	void FixKissVoreTinyOffset(Actor* giant, bool Fix) {
		auto Transient = Transient::GetActorData(giant);
		if (Transient) {
			Transient->KissVoring = Fix; // Fix slight .Z offset when kiss voring, else Tiny will be below mouth
		}
	}

	void ResetTinyAnimSpeed(Actor* giant) {
		auto Tiny = Grab::GetHeldActor(giant);
		if (Tiny) {
			TaskManager::Cancel(std::format("CopyAnimSpeed_{}", Tiny->formID));
			Anims_FixAnimationDesync(giant, Tiny, true);
		}
	}

	void Task_CopyAnimationSpeed(Actor* giant, Actor* tiny) {
		auto gianthandle = giant->CreateRefHandle();
		auto tinyhandle = tiny->CreateRefHandle();
		if (tiny) {
			std::string name = std::format("CopyAnimSpeed_{}", tiny->formID);

			TaskManager::Run(name, [=](auto& update){
				if (!gianthandle || !tinyhandle) {
					return false;
				}
				Actor* giantref = gianthandle.get().get();
				Actor* tinyref = tinyhandle.get().get();
				if (!AnimationVars::Action::IsInGrabPlayState(giantref)) { // Exit Grab Play state as soon as attack bool is false
					Anims_FixAnimationDesync(giantref, tinyref, true);
					return false;
				} else {
					Anims_FixAnimationDesync(giantref, tinyref, false);
					return true;
				}
				return true; // Else continue to wait for it
			});
		}
	}

	void GTSGrab_KickTiny(Actor* a_giant) {
		auto otherActor = Grab::GetHeldActor(a_giant);

		Grab::DetachActorTask(a_giant);
		Grab::Release(a_giant);

		AnimationVars::Grab::SetHasGrabbedTiny(a_giant, false);
		AnimationVars::Grab::SetGrabState(a_giant, false);

		if (otherActor) {

			if (auto charcont = otherActor->GetCharController()) {
				charcont->SetLinearVelocityImpl(0.0f); // Needed so Actors won't fall down.
			}

			if (const auto bone = find_node(a_giant, "NPC R Finger22 [RF22]")) {

				NiPoint3 startCoords = bone->world.translate;

				ActorHandle gianthandle = a_giant->CreateRefHandle();
				ActorHandle tinyhandle = otherActor->CreateRefHandle();

				const std::string name = std::format("Throw_{}_{}", a_giant->formID, otherActor->formID);
				const std::string pass_name = std::format("ThrowOther_{}_{}", a_giant->formID, otherActor->formID);

				// Run task that will actually launch the Tiny
				TaskManager::Run(name, [=](auto& update){

					if (!gianthandle || !tinyhandle) {
						return false;
					}

					Actor* giant = gianthandle.get().get();
					Actor* tiny = tinyhandle.get().get();
					
					// Wait for 3D to be ready
					if (!giant->Is3DLoaded()   || 
						!giant->GetCurrent3D() || 
						!tiny->Is3DLoaded()    || 
						!tiny->GetCurrent3D()) {
						return true;
					}

					NiPoint3 endCoords = bone->world.translate;

					Anims_FixAnimationDesync(giant, tiny, true);

					SetBeingHeld(tiny, false);
					EnableCollisions(tiny);

					PushActorAway(giant, tiny, 1.0f);

					const float Flick_Power = HasSMT(giant) ? 9.0f : 4.0f;

					Animation_GrabThrow::Throw_Actor(gianthandle, tinyhandle, startCoords, endCoords, pass_name, Flick_Power, 20.0f);
					
					return false;
				});
			}
		}
	}

	void GTSGrab_SpawnHeartsAtHead(Actor* giant, float Z_Offset, float Heart_Scale) {
		auto tiny = Grab::GetHeldActor(giant);
		if (tiny) {
			log::info("Tiny True");
			auto node = find_node(tiny, "NPC Head [Head]");
			if (node) {
				log::info("Trying to spawn hearts");
				SpawnHearts(giant, tiny, Z_Offset, Heart_Scale, false, node->world.translate);
			}
		}
	}

	void GTSGrab_AddToVoreData(Actor* giant) {
		auto& VoreData = VoreController::GetSingleton().GetVoreData(giant);
		auto otherActor = Grab::GetHeldActor(giant);
		if (otherActor) {
			VoreData.AddTiny(otherActor);
		}
	}

	void GTSGrab_SetBeingEaten(Actor* giant) {
		auto otherActor = Grab::GetHeldActor(giant);
		if (otherActor) {
			SetBeingEaten(otherActor, true);
		}
	}

	void GTSGrab_SwallowTiny(Actor* giant) {
		auto& VoreData = VoreController::GetSingleton().GetVoreData(giant);
		auto otherActor = Grab::GetHeldActor(giant);
		if (otherActor) {
			for (auto& tiny: VoreData.GetVories()) {
				if (!IsDevourmentEnabled()) {
					VoreData.Swallow();
					if (AnimationVars::Crawl::IsCrawling(giant)) {
						otherActor->SetAlpha(0.0f); // Hide Actor
					}
				} else {
					CallDevourment(giant, otherActor);
				}
			}
		}
	}

	void GTSGrab_FullyEatTiny(Actor* giant) {
		auto otherActor = Grab::GetHeldActor(giant);
		if (otherActor) {
			SetBeingEaten(otherActor, false);
			auto& VoreData = VoreController::GetSingleton().GetVoreData(giant);
			for (auto& tiny: VoreData.GetVories()) {
				VoreData.KillAll();
			}
			AnimationVars::Grab::SetHasGrabbedTiny(giant, false);
			AnimationVars::Grab::SetGrabState(giant, false);
			
			AnimationManager::StartAnim("TinyDied", giant);
			ManageCamera(giant, false, CameraTracking::Grab_Left);
			SetBeingHeld(otherActor, false);
			Grab::CancelGrab(giant, otherActor);
			Grab::DetachActorTask(giant);
			Grab::Release(giant);
		}
	}

	void GTSGrab_Do_Damage(Actor* giant, float base_damage) {
		auto& sizemanager = SizeManager::GetSingleton();
		float bonus = HasSMT(giant) ? 1.65f : 1.0f;
		auto grabbedActor = Grab::GetHeldActor(giant);

		if (grabbedActor) {
			grabbedActor->Attacked(giant); // force combat

			float tiny_scale = get_visual_scale(grabbedActor) * GetSizeFromBoundingBox(grabbedActor);
			float gts_scale = get_visual_scale(giant) * GetSizeFromBoundingBox(giant);

			float sizeDiff = gts_scale/tiny_scale;
			float power = std::clamp(sizemanager.GetSizeAttribute(giant, SizeAttribute::Normal), 1.0f, 1000000.0f);
			float additionaldamage = 1.0f + sizemanager.GetSizeVulnerability(grabbedActor);
			float damage = (base_damage * sizeDiff) * power * additionaldamage * additionaldamage;
			float experience = std::clamp(damage/1600, 0.0f, 0.06f);

            if (CanDoDamage(giant, grabbedActor, false)) {
                if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkGrowingPressure)) {
                    auto& mgr = SizeManager::GetSingleton();
                    mgr.ModSizeVulnerability(grabbedActor, damage * 0.0010f);
                }

                TinyCalamity_ShrinkActor(giant, grabbedActor, damage * 0.10f * Config::Balance.fSizeDamageMult);

                SizeHitEffects::PerformInjuryDebuff(giant, grabbedActor, damage*0.15f, 6);
                InflictSizeDamage(giant, grabbedActor, damage);
            }
			
			Rumbling::Once("GrabAttack", giant, Rumble_Grab_Hand_Attack * bonus, 0.05f, "NPC L Hand [LHnd]", 0.0f);

			ModSizeExperience(giant, experience);
			AddSMTDuration(giant, 1.0f);

            Utils_CrushTask(giant, grabbedActor, bonus, true, true, DamageSource::HandCrushed, QuestStage::HandCrush);
		}
	}

	void GTSGrab_DelayedSmile(Actor* a_giant, float delay) {
		std::string name = std::format("DelayedKiss_{}", a_giant->formID);

		ActorHandle giantHandle = a_giant->CreateRefHandle();
		double start = Time::WorldTimeElapsed();

		TaskManager::Run(name, [=](auto& progressData) {
			if (!giantHandle) {
				return false;
			}
			double finish = Time::WorldTimeElapsed();
			auto giantref = giantHandle.get().get();

			float timepassed = static_cast<float>(finish - start) * AnimationManager::GetAnimSpeed(giantref);

			bool ShouldRevert = timepassed >= (delay * GetAnimationSlowdown(giantref));
			
			if (ShouldRevert) {
				Task_FacialEmotionTask_Smile(giantref, 1.25f, "GrabSmile", 0.125f);
				return false;
			}
			return true;
		});
	}

	void GTSGrab_DelayedVore(Actor* a_giant, float delay) {
		std::string name = std::format("DelayedKiss_{}", a_giant->formID);
		auto a_tiny = Grab::GetHeldActor(a_giant);

		ActorHandle giantHandle = a_giant->CreateRefHandle();
		ActorHandle tinyHandle = a_tiny->CreateRefHandle();
		double start = Time::WorldTimeElapsed();

		TaskManager::Run(name, [=](auto& progressData) {
			if (!giantHandle) {
				return false;
			}
			if (!tinyHandle) {
				return false;
			}
			double finish = Time::WorldTimeElapsed();
			auto giantref = giantHandle.get().get();
			auto tinyref = tinyHandle.get().get();

			float timepassed = static_cast<float>(finish - start) * AnimationManager::GetAnimSpeed(giantref);

			bool ShouldRevert = timepassed >= (delay * GetAnimationSlowdown(giantref));
			
			if (ShouldRevert) {
				VoreController::GetSingleton().ShrinkOverTime(giantref, tinyref, 0.05f, 6.0f);
				Task_FacialEmotionTask_OpenMouth(giantref, 0.65f, "GrabVoreOpenMouth", 0.1f, 2.15f);
				GTSGrab_AddToVoreData(giantref);
				return false;
			}
			return true;
		});
	}
}

namespace {

	// [ S M I L E ]
	void GTS_HS_SmileOn(AnimationEventData& data) {
		Task_FacialEmotionTask_SlightSmile(&data.giant, RandomFloat(0.6f, 0.8f), "Grab_Smile", 0.125f);
	}

	void GTS_HS_SmileOff(AnimationEventData& data) {} // unused
	
	// [ C A M E R A ]
	void GTS_HS_CamOn(AnimationEventData& data) {
		ManageCamera(&data.giant, true, CameraTracking::ObjectL);
		auto tiny = Grab::GetHeldActor(&data.giant);
		if (tiny) {
			Grab_Fixes::Task_CopyAnimationSpeed(&data.giant, tiny);
		}
	}

	void GTS_HS_CamOff(AnimationEventData& data) {
		ManageCamera(&data.giant, false, CameraTracking::ObjectL);
		Attachment_SetTargetNode(&data.giant, AttachToNode::None);
	}

	// [ T O S S ]
	void GTS_HS_TossTiny(AnimationEventData& data) {
		Rumbling::Once("TossTiny", &data.giant, 1.25f, 0.05f, "NPC L Hand [LHnd]", true);
	}

	void GTS_HS_TinyLand(AnimationEventData& data) {
		Rumbling::Once("TossTiny", &data.giant, 1.65f, 0.05f, "NPC L Hand [LHnd]", true);
		Grab_Fixes::GTSGrab_SpawnHeartsAtHead(&data.giant, 0.0f, 0.45f);
	}

	// [ P O K E ]
	void GTS_HS_Poke_WindUp(AnimationEventData& data) {
		Rumbling::Once("PokeTiny", &data.giant, 1.0f, 0.05f, "NPC L Hand [LHnd]", true);
	}

	void GTS_HS_Poke_Contact(AnimationEventData& data) {
		Rumbling::Once("PokeTiny_C", &data.giant, 1.75f, 0.05f, "NPC L Hand [LHnd]", true);
		Grab_Fixes::GTSGrab_SpawnHeartsAtHead(&data.giant, 0.0f, 0.45f);
	}

	void GTS_HS_Poke_TinyLand(AnimationEventData& data) {
		Rumbling::Once("PokeTiny_L", &data.giant, 1.25f, 0.05f, "NPC L Hand [LHnd]", true);
	}

	// [ F L I C K ]
	void GTS_HS_Flick_Launch(AnimationEventData& data) {
		Rumbling::Once("FlichTiny_Launch", &data.giant, 0.75f, 0.05f, "NPC L Hand [LHnd]", true);
		Grab_Fixes::GTSGrab_SpawnHeartsAtHead(&data.giant, 0.0f, 0.45f);
		Grab_Fixes::GTSGrab_KickTiny(&data.giant);
		Grab_Fixes::ResetTinyAnimSpeed(&data.giant);
	}

	void GTS_HS_Flick_Ragdoll(AnimationEventData& data) {
		Rumbling::Once("FlichTiny_Land", &data.giant, 2.1f, 0.05f, "NPC L Hand [LHnd]", true);
		Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundSwingImpact, &data.giant, 0.75f, "NPC L Hand [LHnd]"); // play swing impact sound
	}

	// [ L I G H T  C R U S H ]
	void GTS_HS_Sand_WindUp(AnimationEventData& data) {}
	void GTS_HS_Sand_Lower(AnimationEventData& data) {}
	void GTS_HS_Sand_Hit(AnimationEventData& data) {
		Grab_Fixes::GTSGrab_Do_Damage(&data.giant, Damage_Grab_Play_Light);
		Rumbling::Once("SmashTiny_L", &data.giant, 1.65f, 0.05f, "NPC L Hand [LHnd]", true);
	}

	void GTS_HS_Sand_Release(AnimationEventData& data) {}

	// [ H E A V Y  C R U S H ]
	void GTS_HS_Fist_WindUp(AnimationEventData& data) {}
	void GTS_HS_Fist_Lower(AnimationEventData& data) {}

	void GTS_HS_Fist_Hit(AnimationEventData& data) {
		Grab_Fixes::GTSGrab_Do_Damage(&data.giant, Damage_Grab_Play_Heavy);
		Rumbling::Once("SmashTiny_H", &data.giant, 2.25f, 0.05f, "NPC L Hand [LHnd]", true);
	}

	void GTS_HS_Fist_Release(AnimationEventData& data) {}

	void GTS_HS_Exit_NoTiny(AnimationEventData& data) { 
		// If tiny has died at the end - cancel grab state
	}

	// [ K I S S + K I S S V O R E ]
	void GTS_HS_Kiss_Start(AnimationEventData& data) {
		Rumbling::Once("KissTiny", &data.giant, 2.25f, 0.05f, "NPC Head [Head]", true);
		Task_FacialEmotionTask_Kiss(&data.giant, 1.25f, "KissTiny", 0.325f);
		Grab_Fixes::GTSGrab_SpawnHeartsAtHead(&data.giant, 0.0f, 0.45f);
		if (auto tiny = Grab::GetHeldActor(&data.giant)) {
			Task_FacialEmotionTask_Kiss(tiny, 1.25f, "KissedByGts", 0.325f);
		}
	}

	void GTS_HS_Kiss_Stop(AnimationEventData& data) {}
	void GTS_HS_KissSound_Play(AnimationEventData& data) {
		Runtime::PlaySoundAtNode_FallOff(Runtime::SNDR.GTSSoundKissing, &data.giant, 1.0f, "NPC Head [Head]", 0.11f * get_visual_scale(&data.giant));
		auto tiny = Grab::GetHeldActor(&data.giant);
		if (tiny) {
			tiny->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kHealth, GetMaxAV(tiny, ActorValue::kHealth) * 0.1f);
		}
	}
	void GTS_HS_KissSound_Stop(AnimationEventData& data) {} // Unused

	
	// [ K I S S  V O R E]
	void GTS_HS_K_Vore_OpenMouth(AnimationEventData& data) {
		Task_FacialEmotionTask_OpenMouth(&data.giant, 1.8f, "GrabVoreOpenMouth", 0.65f);
		
		Grab_Fixes::GTSGrab_AddToVoreData(&data.giant);

		ManageCamera(&data.giant, true, CameraTracking::ObjectB);
		Grab_Fixes::FixKissVoreTinyOffset(&data.giant, true);

		auto tiny = Grab::GetHeldActor(&data.giant);
		if (tiny) {
			VoreController::GetSingleton().ShrinkOverTime(&data.giant, tiny, 0.05f, 6.0f);
		}
	}

	void GTS_HS_K_Vore_CloseMouth(AnimationEventData& data) {
		Grab_Fixes::GTSGrab_SetBeingEaten(&data.giant);
	}

	void GTS_HS_K_Vore_TinyYell(AnimationEventData& data) {}
	void GTS_HS_K_Vore_TinyMuffle(AnimationEventData& data) {}
	void GTS_HS_K_Vore_SlurpTiny(AnimationEventData& data) {}

	void GTS_HS_K_Vore_SlurpTiny_End(AnimationEventData& data) {
		Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundSwallow, &data.giant, 1.0f, "NPC Head [Head]"); // Play sound
	}

	void GTS_HS_K_Vore_TinyInMouth(AnimationEventData& data) {
		Grab_Fixes::GTSGrab_SwallowTiny(&data.giant);
	}

	void GTS_HS_K_Vore_SwallowTiny(AnimationEventData& data) {
		Grab_Fixes::FixKissVoreTinyOffset(&data.giant, false);
		Grab_Fixes::GTSGrab_DelayedSmile(&data.giant, 1.2f);
		Grab_Fixes::GTSGrab_FullyEatTiny(&data.giant);
		//Grab::FailSafeReset(&data.giant);
	}

	// [ V O R E]
	void GTS_HS_Vore_OpenMouth(AnimationEventData& data) {
		Grab_Fixes::GTSGrab_DelayedVore(&data.giant, 0.2f);
	}

	void GTS_HS_Vore_CloseMouth(AnimationEventData& data) {}
	void GTS_HS_Vore_TinyInMouth(AnimationEventData& data) {
		Grab_Fixes::GTSGrab_SwallowTiny(&data.giant);
	}

	void GTS_HS_Vore_SwallowTiny(AnimationEventData& data) {
		Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundSwallow, &data.giant, 1.0f, "NPC Head [Head]"); // Play sound
		Grab_Fixes::GTSGrab_FullyEatTiny(&data.giant);
		//Grab::FailSafeReset(&data.giant);
	}
}

namespace GTS {
    void Animation_GrabPlay_Events::RegisterEvents() {
		AnimationManager::RegisterEvent("GTS_HS_SmileOn", "GrabPlay", GTS_HS_SmileOn);
		AnimationManager::RegisterEvent("GTS_HS_SmileOff", "GrabPlay", GTS_HS_SmileOff);

		AnimationManager::RegisterEvent("GTS_HS_CamOn", "GrabPlay", GTS_HS_CamOn);
		AnimationManager::RegisterEvent("GTS_HS_CamOff", "GrabPlay", GTS_HS_CamOff);

		AnimationManager::RegisterEvent("GTS_HS_TossTiny", "GrabPlay", GTS_HS_TossTiny);
		AnimationManager::RegisterEvent("GTS_HS_TinyLand", "GrabPlay", GTS_HS_TinyLand);

		AnimationManager::RegisterEvent("GTS_HS_Poke_WindUp", "GrabPlay", GTS_HS_Poke_WindUp);
		AnimationManager::RegisterEvent("GTS_HS_Poke_Contact", "GrabPlay", GTS_HS_Poke_Contact);
		AnimationManager::RegisterEvent("GTS_HS_Poke_TinyLand", "GrabPlay", GTS_HS_Poke_TinyLand);

		AnimationManager::RegisterEvent("GTS_HS_Flick_Launch", "GrabPlay", GTS_HS_Flick_Launch);
		AnimationManager::RegisterEvent("GTS_HS_Flick_Ragdoll", "GrabPlay", GTS_HS_Flick_Ragdoll);

		AnimationManager::RegisterEvent("GTS_HS_Sand_WindUp", "GrabPlay", GTS_HS_Sand_WindUp);
		AnimationManager::RegisterEvent("GTS_HS_Sand_Lower", "GrabPlay", GTS_HS_Sand_Lower);

		AnimationManager::RegisterEvent("GTS_HS_Sand_Hit", "GrabPlay", GTS_HS_Sand_Hit);
		AnimationManager::RegisterEvent("GTS_HS_Sand_Release", "GrabPlay", GTS_HS_Sand_Release);

		AnimationManager::RegisterEvent("GTS_HS_Fist_WindUp", "GrabPlay", GTS_HS_Fist_WindUp);
		AnimationManager::RegisterEvent("GTS_HS_Fist_Lower", "GrabPlay", GTS_HS_Fist_Lower);

		AnimationManager::RegisterEvent("GTS_HS_Fist_Hit", "GrabPlay", GTS_HS_Fist_Hit);
		AnimationManager::RegisterEvent("GTS_HS_Fist_Release", "GrabPlay", GTS_HS_Fist_Release);
		AnimationManager::RegisterEvent("GTS_HS_Exit_NoTiny", "GrabPlay", GTS_HS_Exit_NoTiny);

		AnimationManager::RegisterEvent("GTS_HS_Kiss_Start", "GrabPlay", GTS_HS_Kiss_Start);
		AnimationManager::RegisterEvent("GTS_HS_Kiss_Stop", "GrabPlay", GTS_HS_Kiss_Stop);
		AnimationManager::RegisterEvent("GTS_HS_KissSound_Play", "GrabPlay", GTS_HS_KissSound_Play);
		AnimationManager::RegisterEvent("GTS_HS_KissSound_Stop", "GrabPlay", GTS_HS_KissSound_Stop);
		AnimationManager::RegisterEvent("GTS_HS_K_Vore_OpenMouth", "GrabPlay", GTS_HS_K_Vore_OpenMouth);
		AnimationManager::RegisterEvent("GTS_HS_K_Vore_CloseMouth", "GrabPlay", GTS_HS_K_Vore_CloseMouth);
		AnimationManager::RegisterEvent("GTS_HS_K_Vore_TinyYell", "GrabPlay", GTS_HS_K_Vore_TinyYell);
		AnimationManager::RegisterEvent("GTS_HS_K_Vore_TinyMuffle", "GrabPlay", GTS_HS_K_Vore_TinyMuffle);
		AnimationManager::RegisterEvent("GTS_HS_K_Vore_SlurpTiny", "GrabPlay", GTS_HS_K_Vore_SlurpTiny);
		AnimationManager::RegisterEvent("GTS_HS_K_Vore_SlurpTiny_End", "GrabPlay", GTS_HS_K_Vore_SlurpTiny_End);
		AnimationManager::RegisterEvent("GTS_HS_K_Vore_TinyInMouth", "GrabPlay", GTS_HS_K_Vore_TinyInMouth);
		AnimationManager::RegisterEvent("GTS_HS_K_Vore_SwallowTiny", "GrabPlay", GTS_HS_K_Vore_SwallowTiny);

		AnimationManager::RegisterEvent("GTS_HS_Vore_OpenMouth", "GrabPlay", GTS_HS_Vore_OpenMouth);
		AnimationManager::RegisterEvent("GTS_HS_Vore_CloseMouth", "GrabPlay", GTS_HS_Vore_CloseMouth);
		AnimationManager::RegisterEvent("GTS_HS_Vore_TinyInMouth", "GrabPlay", GTS_HS_Vore_TinyInMouth);
		AnimationManager::RegisterEvent("GTS_HS_Vore_SwallowTiny", "GrabPlay", GTS_HS_Vore_SwallowTiny);

	}
}

/* EVENTS
GTS_HS_Exit_NoTiny - Triggered when we want to exit an anim after the Tiny is dead
##Facial Anims
GTS_HS_SmileOn - Makes the GTS smile
GTS_HS_SmileOff - Stops the GTS Smiling

###SFX/Cam Stuff
GTS_HS_CamOn - Triggered when we enter the Hand state to change the camera
GTS_HS_CamOff - Triggered when we exit the Hand state to change the camera
GTS_HS_TossTiny - Intro Anim when we throw the tiny upwards
GTS_HS_TinyLand - Intro Anim when the tiny lands

###Poke -- No damage
GTS_HS_Poke_WindUp - When the GTS pulls her arm back to poke
GTS_HS_Poke_Contact - When the poke conects with the tiny
GTS_HS_Poke_TinyLand - When the Tiny falls down in the gts hand

###Flick -- Launched the Tiny out of the GTS's hand
GTS_HS_Flick_Launch - Triggered when the GTS flicks the tiny
GTS_HS_Flick_Ragdoll - Triggered when the tiny is meant to ragdoll after being flicked

###Sandwich  - Used as a Light crush Does Damage
GTS_HS_Sand_WindUp - Triggered when the GTS Raise's her hand
GTS_HS_Sand_Lower - Triggered when the GTS lower the hand to crush
GTS_HS_Sand_Hit - Triggered when the GTS Hits the Tiny
GTS_HS_Sand_Release - Triggered when the GTS lifts her hand from the Tiny

###Fist Crush -- Does Damage, Heavy Crush attack
GTS_HS_Fist_WindUp - Triggered when the GTS Raise's her hand
GTS_HS_Fist_Lower - Triggered when the GTS lower the hand to crush
GTS_HS_Fist_Hit - Triggered when the GTS Hits the Tiny
GTS_HS_Fist_Release - Triggered when the GTS lifts her hand from the Tiny   
 
###Kiss
GTS_HS_Kiss_Start - Used to trigger the Facial anims for kissing
GTS_HS_Kiss_Stop - Used to end the kissing facial anims
GTS_HS_KissSound_Play - Plays the Kiss Sound
GTS_HS_KissSound_Stop - Stops the Kiss Sound - Unused
GTS_HS_K_Vore_OpenMouth - Used to open the GTS's mouth
GTS_HS_K_Vore_CloseMouth - Used to close the GTS's mouth
GTS_HS_K_Vore_TinyYell - ideally makes the tiny yell frightenedly
GTS_HS_K_Vore_TinyMuffle - ideally muffles the tiny's voice when yelling
GTS_HS_K_Vore_SlurpTiny - ideally used to play a sound when slurping the Tiny up
GTS_HS_K_Vore_SlurpTiny_End - ideally used to play a sound  when the gts finishes slurping the tiny
GTS_HS_K_Vore_TinyInMouth - Triggered when the Tiny is fully in the GTS mouth
GTS_HS_K_Vore_SwallowTiny - Triggered when the GTS swallows the tiny

###Vore -- NOT the Kiss Vore
GTS_HS_Vore_OpenMouth - Used to open the GTS's mouth
GTS_HS_Vore_CloseMouth - Used to close the GTS's mouth
GTS_HS_Vore_TinyInMouth - Triggered when the Tiny is fully in the GTS mouth
GTS_HS_Vore_SwallowTiny - Triggered when the GTS swallows the tiny
*/