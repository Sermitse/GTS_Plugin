#include "Managers/Animation/Grab.hpp"
#include "Managers/Animation/GrabUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"

#include "Managers/Animation/Controllers/GrabAnimationController.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"

#include "Managers/Damage/SizeHitEffects.hpp"
#include "Managers/Damage/TinyCalamity.hpp"
#include "Managers/Input/InputManager.hpp"

#include "Managers/GTSSizeManager.hpp"
#include "Managers/CrushManager.hpp"
#include "Managers/Rumble.hpp"

#include "Magic/Effects/Common.hpp"

#include "Utils/AttachPoint.hpp"
#include "Utils/Actions/InputConditions.hpp"

#include "Utils/DeathReport.hpp"

using namespace GTS;

///GTS_GrabbedTiny MUST BE 1 when we have someone in hands

/*Event used in the behaviours to transition between most behaviour states
   Grab Events
        GTSBEH_GrabStart
        GTSBEH_GrabVore
        GTSBEH_GrabAttack
        GTSBEH_GrabThrow
        GTSBEH_GrabRelease
   More Grab things we don't need to do anything with in the DLL
        GTSBeh_MT
        GTSBeh_1hm
        GTSBeh_Mag
        GTSBeh_Set
        GTSBeh_GrabVore_LegTrans
   Used to leave the grab state
        GTSBeh_GrabExit
   Grab Event to go back to vanilla
        GTSBEH_AbortGrab
 */


namespace {

	void PrintCancelReason(Actor* giant, Actor* tiny, float sizedifference, float Action_Grab) {
		log::info("Canceled Grab on {}. Reasons below", tiny->GetDisplayFullName());
		bool GiantDead = giant->IsDead();
		bool TinyDead = tiny->IsDead();
		if (GiantDead) {
			log::info("---Giant Has Died");
		}
		if (TinyDead) {
			log::info("---Tiny Has Died");
		}
		if (GetAV(tiny, ActorValue::kHealth) <= 0.0f) {
			log::info("---Tiny health is < 0: {}", GetAV(tiny, ActorValue::kHealth) <= 0.0f);
		}
		if (GetAV(giant, ActorValue::kStamina) < 2.0f) {
			log::info("---Giant stamina is < 2: {}", GetAV(giant, ActorValue::kStamina) < 2.0f);
		}
		if (sizedifference < Action_Grab) {
			log::info("---SizeDifference < Threshold: {}, Difference: {}", sizedifference < Action_Grab, sizedifference);
		}
	}

	const std::vector<std::string_view> RHAND_RUMBLE_NODES = { // used for hand rumble
		"NPC R UpperarmTwist1 [RUt1]",
		"NPC R UpperarmTwist2 [RUt2]",
		"NPC R Forearm [RLar]",
		"NPC R ForearmTwist2 [RLt2]",
		"NPC R ForearmTwist1 [RLt1]",
		"NPC R Hand [RHnd]",
	};

	const std::vector<std::string_view> LHAND_RUMBLE_NODES = { // used for hand rumble
		"NPC L UpperarmTwist1 [LUt1]",
		"NPC L UpperarmTwist2 [LUt2]",
		"NPC L Forearm [LLar]",
		"NPC L ForearmTwist2 [LLt2]",
		"NPC L ForearmTwist1 [LLt1]",
		"NPC L Hand [LHnd]",
	};

	bool Escaped(Actor* giant, Actor* tiny, float strength) {
		float tiny_chance = ((RandomFloat(0.f, 100000.f)) / 100000.0f) * get_visual_scale(tiny);
		float giant_chance = ((RandomFloat(0.f, 100000.f)) / 100000.0f) * strength * get_visual_scale(giant);
		return (tiny_chance > giant_chance);
	}

	void ApplyPitchRotation(Actor* actor, float pitch) {
		auto charCont = actor->GetCharController();
		if (charCont) {
			charCont->pitchAngle = pitch;
		}
		//log::info("Pitch: {}", charCont->pitchAngle);
	}

	void ApplyRollRotation(Actor* actor, float roll) {
		auto charCont = actor->GetCharController();
		if (!charCont) {
			return;
		}
		charCont->rollAngle = roll;
		//log::info("Roll: {}", charCont->rollAngle);
	}

	void Task_RotateActorToBreastX(Actor* giant, Actor* tiny) {
		std::string name = std::format("RotateActor_{}", giant->formID);
		ActorHandle gianthandle = giant->CreateRefHandle();
		ActorHandle tinyhandle = tiny->CreateRefHandle();
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
				return false;
			}

			float LPosX = 0.0f;
			float LPosY = 0.0f;
			float LPosZ = 0.0f;

			float RPosX = 0.0f;
			float RPosY = 0.0f;
			float RPosZ = 0.0f;

			auto BreastL = find_node(giant, "L Breast01");
			auto BreastR = find_node(giant, "R Breast01");
			if (!BreastL) {
				return false;
			}
			if (!BreastR) {
				return false;
			}

			NiMatrix3 LeftBreastRotation = BreastL->world.rotate; // get breast rotation
			NiMatrix3 RightBreastRotation = BreastR->world.rotate;

			LeftBreastRotation.ToEulerAnglesXYZ(LPosX, LPosY, LPosZ); // fill empty rotation data of breast with proper one
			RightBreastRotation.ToEulerAnglesXYZ(RPosX, RPosY, RPosZ);

			float BreastRotation_X = (LPosX + RPosX) / 2.0f;
			float BreastRotation_Y = (LPosY + RPosY) / 2.0f;

			ApplyPitchRotation(tinyref, BreastRotation_X);
			//ApplyRollRotation(tinyref, BreastRotation_Y);
			//log::info("Angle of L breast: X: {}, Y: {}, Z: {}", LPosX, LPosY, LPosZ);
			//log::info("Angle of R breast: X: {}, Y: {}, Z: {}", RPosX, RPosY, RPosZ);

			// All good try another frame
			if (!IsBetweenBreasts(tinyref)) {
				ApplyPitchRotation(tinyref, 0.0f);
				//ApplyRollRotation(tinyref, 0.0f);
				return false; // Abort it
			}
			return true;
		});
		TaskManager::ChangeUpdate(name, UpdateKind::Havok);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////G R A B
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GTSGrab_Catch_Start(AnimationEventData& data) {
		ManageCamera(&data.giant, true, CameraTracking::Grab_Left);
		auto grabbedActor = Grab::GetHeldActor(&data.giant);
		if (grabbedActor) {
			DisableCollisions(grabbedActor, &data.giant);
			SetBeingHeld(grabbedActor, true);

			StaggerActor(&data.giant, grabbedActor, 100.0f);
		}
		StartLHandRumble("GrabL", data.giant, 0.5f, 0.10f);
	}

	void GTSGrab_Catch_Actor(AnimationEventData& data) {
		AnimationVars::Grab::SetHasGrabbedTiny(&data.giant, true);

		if (auto grabbed = Grab::GetHeldActor(&data.giant)) {
			if (Config::Gameplay.ActionSettings.bGrabStartIsHostile && !IsTeammate(grabbed)) {
				Attacked(grabbed, &data.giant);
			}
			Grab::AttachActorTask(&data.giant, grabbed);
			DisableCollisions(grabbed, &data.giant); // Do it once more just in case
		}

		Rumbling::Once("GrabCatch", &data.giant, 2.0f, 0.15f);
	}

	void GTSGrab_Catch_End(AnimationEventData& data) {
		ManageCamera(&data.giant, false, CameraTracking::Grab_Left);
		StopLHandRumble("GrabL", data.giant);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////R E L E A S E
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GTSGrab_Release_FreeActor(AnimationEventData& data) {
		auto giant = &data.giant;
		
		AnimationVars::Grab::SetHasGrabbedTiny(giant, false);
		AnimationVars::Action::SetIsStoringTiny(giant, false);
		AnimationVars::Grab::SetGrabState(giant, false);
		auto grabbedActor = Grab::GetHeldActor(giant);
		ManageCamera(&data.giant, false, CameraTracking::Grab_Left);
		AnimationManager::StartAnim("TinyDied", giant);
		if (grabbedActor) {
			SetBetweenBreasts(grabbedActor, false);
			PushActorAway(giant, grabbedActor, 1.0f);
			EnableCollisions(grabbedActor);
			SetBeingHeld(grabbedActor, false);
			Anims_FixAnimationDesync(giant, grabbedActor, true);
		}
		Grab::DetachActorTask(giant);
		Grab::Release(giant);
	}

	void GTSBEH_GrabExit(AnimationEventData& data) {
		auto giant = &data.giant;
		auto grabbedActor = Grab::GetHeldActor(giant);
		if (grabbedActor) {
			EnableCollisions(grabbedActor);
			SetBetweenBreasts(grabbedActor, false);
			Anims_FixAnimationDesync(giant, grabbedActor, true);
		}
		
		AnimationVars::Grab::SetHasGrabbedTiny(giant, false);
		AnimationVars::Action::SetIsStoringTiny(giant, false);
		AnimationVars::Grab::SetGrabState(giant, false);
		AnimationManager::StartAnim("TinyDied", giant);
		DrainStamina(giant, "GrabAttack", Runtime::PERK.GTSPerkDestructionBasics, false, 0.75f);
		DrainStamina(giant, "GrabThrow", Runtime::PERK.GTSPerkDestructionBasics, false, 1.25f);
		ManageCamera(&data.giant, false, CameraTracking::Grab_Left);
		Grab::DetachActorTask(giant);
		Grab::Release(giant);
	}

	void GTSBEH_AbortGrab(AnimationEventData& data) {
		auto giant = &data.giant;
		auto grabbedActor = Grab::GetHeldActor(giant);
		if (grabbedActor) {
			EnableCollisions(grabbedActor);
			SetBeingHeld(grabbedActor, false);
			SetBetweenBreasts(grabbedActor, false);
			Anims_FixAnimationDesync(giant, grabbedActor, true);
		}
		
		AnimationVars::Grab::SetHasGrabbedTiny(giant, false);
		AnimationVars::Action::SetIsStoringTiny(giant, false);
		AnimationVars::Grab::SetGrabState(giant, false);

		AnimationManager::StartAnim("TinyDied", giant);
		DrainStamina(giant, "GrabAttack", Runtime::PERK.GTSPerkDestructionBasics, false, 0.75f);
		DrainStamina(giant, "GrabThrow", Runtime::PERK.GTSPerkDestructionBasics, false, 1.25f);
		ManageCamera(&data.giant, false, CameraTracking::Grab_Left);
		Grab::DetachActorTask(giant);
		Grab::Release(giant);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////B R E A S T S
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GTSGrab_Breast_MoveStart(AnimationEventData& data) {
		ManageCamera(&data.giant, true, CameraTracking::Grab_Left);
	}

	void GTSGrab_Breast_PutActor(AnimationEventData& data) { // Places actor between breasts
		auto giant = &data.giant;
		
		Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundBreastImpact, giant, 1.0f, "NPC L Hand [LHnd]");
		AnimationVars::Action::SetIsStoringTiny(giant, true);
		AnimationVars::Grab::SetHasGrabbedTiny(giant, false);
		auto otherActor = Grab::GetHeldActor(giant);
		if (otherActor) {
			SetBetweenBreasts(otherActor, true);
			Task_RotateActorToBreastX(giant, otherActor);
			if (IsHostile(giant, otherActor)) {
				AnimationManager::StartAnim("Breasts_Idle_Unwilling", otherActor);
			} else {
				AnimationManager::StartAnim("Breasts_Idle_Willing", otherActor);
			}
		}
	}

	void GTSGrab_Breast_TakeActor(AnimationEventData& data) { // Removes Actor
		auto giant = &data.giant;
		AnimationVars::Action::SetIsStoringTiny(giant, false);
		AnimationVars::Grab::SetHasGrabbedTiny(giant, true);
		auto otherActor = Grab::GetHeldActor(giant);
		if (otherActor) {
			SetBetweenBreasts(otherActor, false);
			AnimationManager::StartAnim("Breasts_FreeOther", otherActor);
			Anims_FixAnimationDesync(giant, otherActor, true);
		}
	}

	void GTSGrab_Breast_MoveEnd(AnimationEventData& data) {
		ManageCamera(&data.giant, false, CameraTracking::Grab_Left);
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// T R I G G E R S
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GrabOtherEvent(const ManagedInputEvent& data) { // Grab other actor
		auto player = PlayerCharacter::GetSingleton();
		auto& Grabbing = GrabAnimationController::GetSingleton();

		std::vector<Actor*> preys = Grabbing.GetGrabTargetsInFront(player, 1);
		for (auto prey: preys) {
			GrabAnimationController::StartGrab(player, prey);
		}
	}

	void GrabOtherEvent_Follower(const ManagedInputEvent& data) { // Force Follower to grab player
		Actor* player = PlayerCharacter::GetSingleton();
		ForceFollowerAnimation(player, FollowerAnimType::Grab);
	}

	void GrabAttackEvent(const ManagedInputEvent& data) { // Attack everyone in your hand
		Actor* player = GetPlayerOrControlled();
			float WasteStamina = 20.0f;
			if (Runtime::HasPerk(player, Runtime::PERK.GTSPerkDestructionBasics)) {
				WasteStamina *= 0.65f;
			}
			if (GetAV(player, ActorValue::kStamina) > WasteStamina) {
				AnimationManager::StartAnim("GrabDamageAttack", player);
			} else {
				NotifyWithSound(player, "You're too tired to perform hand attack");
			}
	}

	void GrabVoreEvent(const ManagedInputEvent& data) { // Eat everyone in hand
		Actor* player = GetPlayerOrControlled();

		if (!Grab::GetHeldActor(player)) {
			return;
		}

		AnimationManager::StartAnim("GrabEatSomeone", player);
	}

	void GrabThrowEvent(const ManagedInputEvent& data) { // Throw everyone away
		Actor* player = GetPlayerOrControlled();
			float WasteStamina = 40.0f;
			if (Runtime::HasPerk(player, Runtime::PERK.GTSPerkDestructionBasics)) {
				WasteStamina *= 0.65f;
			}
			if (GetAV(player, ActorValue::kStamina) > WasteStamina) {
				AnimationManager::StartAnim("GrabThrowSomeone", player);
			} else {
				NotifyWithSound(player, "You're too tired to throw that actor");
			}
	}

	void GrabReleaseEvent(const ManagedInputEvent& data) {
		Actor* player = GetPlayerOrControlled();

		auto grabbedActor = Grab::GetHeldActor(player);
		if (!grabbedActor) {
			return;
		}
		if (IsGtsBusy(player) && !IsUsingThighAnimations(player) || IsTransitioning(player)) {
			return;
		}
		if (!player->AsActorState()->IsWeaponDrawn()) {
			Utils_UpdateHighHeelBlend(player, false);
			AnimationManager::StartAnim("GrabReleasePunies", player);
		}
	}

	void BreastsPutEvent(const ManagedInputEvent& data) {
		Actor* player = GetPlayerOrControlled();
		AnimationManager::StartAnim("Breasts_Put", player);
	}

	void BreastsRemoveEvent(const ManagedInputEvent& data) {
		Actor* player = GetPlayerOrControlled();
		AnimationManager::StartAnim("Breasts_Pull", player);
	}

	void DelayedBreastDeattach(Actor* tiny) {
		std::string taskname = std::format("ResetBreastAttachment_{}", tiny->formID);
		double Start = Time::WorldTimeElapsed();
		auto tinyref = tiny->CreateRefHandle();
		
		TaskManager::RunFor(taskname, 1.0f, [=](auto& progressData){
			if (!tinyref) {
				return false;
			}

			double Finish = Time::WorldTimeElapsed();
			auto tinyget = tinyref.get().get();

			double timepassed = Finish - Start;
			if (timepassed > 0.50) {
				SetBetweenBreasts(tinyget, false);
				return false;
			}
			return true;
		});
	}
}

namespace GTS {

	void Utils_CrushTask(Actor* giant, Actor* grabbedActor, float bonus, bool do_sound, bool stagger, DamageSource source, QuestStage stage) {
        
        auto tinyref = grabbedActor->CreateRefHandle();
        auto giantref = giant->CreateRefHandle();

        std::string taskname = std::format("GrabCrush_{}", grabbedActor->formID);

        TaskManager::RunOnce(taskname, [=](auto& update) {
            if (!tinyref) {
                return;
            } 
            if (!giantref) {
                return;
            }
            auto tiny = tinyref.get().get();
            auto giantess = giantref.get().get();

            if (tiny && tiny->Is3DLoaded() && (GetAV(tiny, ActorValue::kHealth) <= 1.0f || tiny->IsDead())) {

                ModSizeExperience_Crush(giant, tiny, false);
				CrushManager::Crush(giantess, tiny);
				DelayedBreastDeattach(tiny);
                SetBeingHeld(tiny, false);

                Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundCrushDefault, giantess, 1.0f, "NPC L Hand [LHnd]");

				if (do_sound) {
					if (!Config::General.bLessGore) {
						Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundCrunchImpact, giantess, 1.0f, "NPC L Hand [LHnd]");
						Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundCrunchImpact, giantess, 1.0f, "NPC L Hand [LHnd]");
					} else {
						Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundSoftHandAttack, giantess, 1.0f, "NPC L Hand [LHnd]");
					}
				}

                AdjustSizeReserve(giantess, get_visual_scale(tiny)/10);
                SpawnHurtParticles(giantess, tiny, 3.0f, 1.6f);
                SpawnHurtParticles(giantess, tiny, 3.0f, 1.6f);
            
                StartCombat(tiny, giantess);
                
                AdvanceQuestProgression(giantess, tiny, stage, 1.0f, false);
                
                ReportDeath(giantess, tiny, source);
            } else {
				if (do_sound) {
					if (!Config::General.bLessGore) {
						Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundCrunchImpact, giantess, 1.0f, "NPC L Hand [LHnd]");
						SpawnHurtParticles(giantess, tiny, 1.0f, 1.0f);
					} else {
						Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundSoftHandAttack, giantess, 1.0f, "NPC L Hand [LHnd]");
					}
				}
				if (stagger) {
                	StaggerActor(giantess, tiny, 0.75f);
				}
            }
        });
    }

	std::string Grab::DebugName() {
		return "::Grab";
	}

	void Grab::DamageActorInHand(Actor* giant, float Damage) {
        Actor* grabbed = Grab::GetHeldActor(giant);
		auto& sizemanager = SizeManager::GetSingleton();

        if (grabbed) {
            Attacked(grabbed, giant); // force combat

            float bonus = 1.0f;

			float tiny_scale = get_visual_scale(grabbed) * GetSizeFromBoundingBox(grabbed);
			float gts_scale = get_visual_scale(giant) * GetSizeFromBoundingBox(giant);

			float sizeDiff = gts_scale/tiny_scale;
			float power = std::clamp(SizeManager::GetSizeAttribute(giant, SizeAttribute::Normal), 1.0f, 999999.0f);
			float additionaldamage = 1.0f + sizemanager.GetSizeVulnerability(grabbed);
			float damage = (Damage * sizeDiff) * power * additionaldamage * additionaldamage;
			float experience = std::clamp(damage/1600, 0.0f, 0.06f);
			if (HasSMT(giant)) {
				bonus = 1.65f;
			}

            if (CanDoDamage(giant, grabbed, false)) {
                if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkGrowingPressure)) {
                    auto& mgr = SizeManager::GetSingleton();
                    mgr.ModSizeVulnerability(grabbed, damage * 0.0010f);
                }

                TinyCalamity_ShrinkActor(giant, grabbed, damage * 0.10f * Config::Balance.fSizeDamageMult);

                SizeHitEffects::PerformInjuryDebuff(giant, grabbed, damage*0.15f, 6);
                InflictSizeDamage(giant, grabbed, damage);
            }
			
			Rumbling::Once("GrabAttack", giant, Rumble_Grab_Hand_Attack * bonus, 0.05f, "NPC L Hand [LHnd]", 0.0f);

			ModSizeExperience(giant, experience);
			AddSMTDuration(giant, 1.0f);

            Utils_CrushTask(giant, grabbed, bonus, false, true, DamageSource::HandCrushed, QuestStage::HandCrush);
        }
    }

	void Grab::DetachActorTask(Actor* giant) {
		std::string name = std::format("GrabAttach_{}", giant->formID);
		AnimationVars::Action::SetCleavageOverrideZ(giant, false);
		AnimationVars::Grab::SetHasGrabbedTiny(giant, false); // Tell behaviors 'we have nothing in our hands'. A must.
		AnimationVars::Grab::SetGrabState(giant, false);
		AnimationVars::Action::SetIsStoringTiny(giant, false);
		Attachment_SetTargetNode(giant, AttachToNode::None);

		TaskManager::Cancel(name);
		Grab::Release(giant);
	}

	void Grab::AttachActorTask(Actor* giant, Actor* tiny) {
		if (!giant) {
			return;
		}
		if (!tiny) {
			return;
		}
		std::string name = std::format("GrabAttach_{}", giant->formID);
		ActorHandle gianthandle = giant->CreateRefHandle();
		ActorHandle tinyhandle = tiny->CreateRefHandle();
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			if (!tinyhandle) {
				return false;
			}
			auto giantref = gianthandle.get().get();
			auto tinyref = tinyhandle.get().get();
			ReattachTiny(giantref, tinyref);
			
			if (tinyref && (tinyref->Is3DLoaded() || IsCurrentlyReattaching(giantref))) {
				return HandleGrabLogic(giantref, tinyref, gianthandle, tinyhandle);
			} else { // Else if Tiny is null or isn't 3d loaded
				return FailSafeAbort(giantref, tinyref);
			}

			// All good try another frame
			return true;
		});
		TaskManager::ChangeUpdate(name, UpdateKind::Havok);
	}

	void Grab::ExitGrabState(Actor* giant) { // Abort Grab animation
		AnimationManager::StartAnim("GrabAbort", giant); // "GTSBEH_AbortGrab"
		AnimationManager::StartAnim("TinyDied", giant); // "GTSBEH_TinyDied"
	}


	void Grab::GrabActor(Actor* giant, TESObjectREFR* tiny, float strength) {
		Grab::GetSingleton().data.try_emplace(giant, tiny, strength);
	}

	void Grab::GrabActor(Actor* giant, TESObjectREFR* tiny) {
		// Default strength 1.0: normal grab for actor of their size
		//
		Grab::GrabActor(giant, tiny, 1.0f);
	}

	void Grab::Reset() {
		this->data.clear();
	}

	void Grab::ResetActor(Actor* actor) {
		this->data.erase(actor);
	}

	void Grab::Release(Actor* giant) {
		Grab::GetSingleton().data.erase(giant);
	}

	TESObjectREFR* Grab::GetHeldObj(Actor* giant) {
		try {
			auto& me = Grab::GetSingleton();
			if (me.data.empty() || !me.data.contains(giant)) {
				return nullptr;
			}

			return me.data.at(giant).tiny;
			
		}
		catch (const std::out_of_range&) {
			return nullptr;
		}

	}
	Actor* Grab::GetHeldActor(Actor* giant) {
		auto obj = Grab::GetHeldObj(giant);
		Actor* actor = skyrim_cast<Actor*>(obj);
		if (actor) {
			return actor;
		} 
		return nullptr;
	}

	void Grab::FailSafeReset(Actor* giantref) {
		if (giantref) {
			AnimationVars::Grab::SetHasGrabbedTiny(giantref, false); // Tell behaviors 'we have nothing in our hands'. A must.
			AnimationVars::Grab::SetGrabState(giantref, false);
			AnimationVars::Action::SetIsStoringTiny(giantref, false);
			DrainStamina(giantref, "GrabAttack", Runtime::PERK.GTSPerkDestructionBasics, false, 0.75f);
			ManageCamera(giantref, false, CameraTracking::Grab_Left); // Disable any camera edits
			Grab::ExitGrabState(giantref);
			Grab::DetachActorTask(giantref);
		}
	}

	void Grab::CancelGrab(Actor* giantref, Actor* tinyref) {
		if (giantref && tinyref) {
			Anims_FixAnimationDesync(giantref, tinyref, true); // Reset anim speed override
			SetBetweenBreasts(tinyref, false);
			SetBeingEaten(tinyref, false);
			SetBeingHeld(tinyref, false);
			AnimationVars::Grab::SetHasGrabbedTiny(giantref, false); // Tell behaviors 'we have nothing in our hands'. A must.
			AnimationVars::Grab::SetGrabState(giantref, false);
			AnimationVars::Action::SetIsStoringTiny(giantref, false);
			DrainStamina(giantref, "GrabAttack", Runtime::PERK.GTSPerkDestructionBasics, false, 0.75f);
			Grab::ExitGrabState(giantref);
			ManageCamera(giantref, false, CameraTracking::Grab_Left); // Disable any camera edits
			Grab::DetachActorTask(giantref);
		}
	}

	void Grab::RegisterTriggers() {
		AnimationManager::RegisterTrigger("GrabSomeone", "Grabbing", "GTSBEH_GrabStart");
		AnimationManager::RegisterTrigger("GrabEatSomeone", "Grabbing", "GTSBEH_GrabVore");
		AnimationManager::RegisterTrigger("GrabDamageAttack", "Grabbing", "GTSBEH_GrabAttack");
		AnimationManager::RegisterTrigger("GrabThrowSomeone", "Grabbing", "GTSBEH_GrabThrow");
		AnimationManager::RegisterTrigger("GrabReleasePunies", "Grabbing", "GTSBEH_GrabRelease");
		AnimationManager::RegisterTrigger("GrabExit", "Grabbing", "GTSBEH_GrabExit");
		AnimationManager::RegisterTrigger("GrabAbort", "Grabbing", "GTSBEH_AbortGrab");
		AnimationManager::RegisterTrigger("TinyDied", "Grabbing", "GTSBEH_TinyDied");
		AnimationManager::RegisterTrigger("Breasts_Put", "Grabbing", "GTSBEH_BreastsAdd");
		AnimationManager::RegisterTrigger("Breasts_Pull", "Grabbing", "GTSBEH_BreastsRemove");
		AnimationManager::RegisterTrigger("Breasts_Idle_Unwilling", "Grabbing", "GTSBEH_T_Storage_Enemy");
		AnimationManager::RegisterTrigger("Breasts_Idle_Willing", "Grabbing", "GTSBEH_T_Storage_Ally");
		AnimationManager::RegisterTrigger("Breasts_FreeOther", "Grabbing", "GTSBEH_T_Remove");
	}

	void StartRHandRumble(std::string_view tag, Actor& actor, float power, float halflife) {
		for (auto& node_name: RHAND_RUMBLE_NODES) {
			std::string rumbleName = std::format("{}{}", tag, node_name);
			Rumbling::Start(rumbleName, &actor, power,  halflife, node_name);
		}
	}

	void StartLHandRumble(std::string_view tag, Actor& actor, float power, float halflife) {
		for (auto& node_name: LHAND_RUMBLE_NODES) {
			std::string rumbleName = std::format("{}{}", tag, node_name);
			Rumbling::Start(rumbleName, &actor, power,  halflife, node_name);
		}
	}

	void StopRHandRumble(std::string_view tag, Actor& actor) {
		for (auto& node_name: RHAND_RUMBLE_NODES) {
			std::string rumbleName = std::format("{}{}", tag, node_name);
			Rumbling::Stop(rumbleName, &actor);
		}
	}
	void StopLHandRumble(std::string_view tag, Actor& actor) {
		for (auto& node_name: RHAND_RUMBLE_NODES) {
			std::string rumbleName = std::format("{}{}", tag, node_name);
			Rumbling::Stop(rumbleName, &actor);
		}
	}

	void Grab::RegisterEvents() {
		InputManager::RegisterInputEvent("GrabPlayer", GrabOtherEvent_Follower, GrabCondition_Follower);
		InputManager::RegisterInputEvent("GrabOther", GrabOtherEvent, GrabCondition_Start);
		InputManager::RegisterInputEvent("GrabAttack", GrabAttackEvent, GrabCondition_Attack);
		InputManager::RegisterInputEvent("GrabVore", GrabVoreEvent, GrabCondition_Vore);
		InputManager::RegisterInputEvent("GrabThrow", GrabThrowEvent, GrabCondition_Throw);
		InputManager::RegisterInputEvent("GrabRelease", GrabReleaseEvent, GrabCondition_Release);
		InputManager::RegisterInputEvent("BreastsPut", BreastsPutEvent, GrabCondition_Breasts);
		InputManager::RegisterInputEvent("BreastsRemove", BreastsRemoveEvent, GrabCondition_Breasts);

		AnimationManager::RegisterEvent("GTSGrab_Catch_Start", "Grabbing", GTSGrab_Catch_Start);
		AnimationManager::RegisterEvent("GTSGrab_Catch_Actor", "Grabbing", GTSGrab_Catch_Actor);
		AnimationManager::RegisterEvent("GTSGrab_Catch_End", "Grabbing", GTSGrab_Catch_End);

		AnimationManager::RegisterEvent("GTSGrab_Breast_MoveStart", "Grabbing", GTSGrab_Breast_MoveStart);
		AnimationManager::RegisterEvent("GTSGrab_Breast_PutActor", "Grabbing", GTSGrab_Breast_PutActor);
		AnimationManager::RegisterEvent("GTSGrab_Breast_TakeActor", "Grabbing", GTSGrab_Breast_TakeActor);
		AnimationManager::RegisterEvent("GTSGrab_Breast_MoveEnd", "Grabbing", GTSGrab_Breast_MoveEnd);

		AnimationManager::RegisterEvent("GTSGrab_Release_FreeActor", "Grabbing", GTSGrab_Release_FreeActor);

		AnimationManager::RegisterEvent("GTSBEH_GrabExit", "Grabbing", GTSBEH_GrabExit);
		AnimationManager::RegisterEvent("GTSBEH_AbortGrab", "Grabbing", GTSBEH_AbortGrab);
	}

	GrabData::GrabData(TESObjectREFR* tiny, float strength) : tiny(tiny), strength(strength) {

	}
}


//Beh's:
/*
        GTSBEH_GrabStart
        GTSBEH_GrabVore
        GTSBEH_GrabAttack
        GTSBEH_GrabThrow
        GTSBEH_GrabRelease

        GTSBeh_GrabExit
        GTSBEH_AbortGrab (Similar to GTSBEH_Exit but Grab only)
 */
