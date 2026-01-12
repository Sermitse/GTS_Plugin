#include "Managers/AI/Grab/GrabAI.hpp"

#include "Config/Config.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Grab.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/CleavageState.hpp"

using namespace GTS;

namespace {

	constexpr float MINIMUM_GRAB_DISTANCE = 85.0f;
	constexpr float GRAB_ANGLE = 70;
	constexpr float PI = std::numbers::pi_v<float>;

	bool ShouldAbortGrab(Actor* giantref, Actor* tinyref, bool CanCancel, bool Dead, bool ValidPrey) {
        if (CanCancel || !ValidPrey) {
			bool ShouldCancel = (Dead || (!IsBetweenBreasts(tinyref) && GetAV(giantref, ActorValue::kStamina) < 2.0f));
            if (!ValidPrey || ShouldCancel) {
                //PrintCancelReason(giantref, tinyref, sizedifference, Action_Grab);
                // For debugging
                return true;
            }
        }
        return false;
    }

	void PreventCombat(Actor* a_actor) {

		if (a_actor->AsActorState()->actorState2.weaponState != WEAPON_STATE::kSheathed)
		a_actor->AsActorState()->actorState2.weaponState = WEAPON_STATE::kWantToSheathe;

		if (auto controller = a_actor->GetActorRuntimeData().combatController) {
			controller->ignoringCombat = true;
		}
	}

	void ResetCombat(Actor* a_actor) {
		
		if (auto controller = a_actor->GetActorRuntimeData().combatController) {
			controller->ignoringCombat = false;
		}
	}

	bool CanGrab(Actor* a_Performer, Actor* a_Prey) {

		if (a_Performer == a_Prey) {
			return false;
		}

		if (a_Prey->IsDead() || GetAV(a_Prey, ActorValue::kHealth) < 0) {
			return false;
		}

		if (!CanPerformAnimationOn(a_Performer, a_Prey, false)) {
			return false;
		}

		if (IsEquipBusy(a_Prey) || AnimationVars::General::GetIsTransitioning(a_Prey) || IsEquipBusy(a_Performer) || AnimationVars::General::GetIsTransitioning(a_Performer)) {
			return false;
		}

		const float PredScale        = get_visual_scale(a_Performer);
		const float SizeDiff         = get_scale_difference(a_Performer, a_Prey, SizeType::VisualScale, true, false);
		constexpr float MinGrabScale = Action_Grab;
		float MinDistance            = MINIMUM_GRAB_DISTANCE;

		if (HasSMT(a_Performer) || AnimationVars::Crawl::IsCrawling(a_Performer)) {
			MinDistance *= 1.5f;
		}

		const float PreyDistance = (a_Performer->GetPosition() - a_Prey->GetPosition()).Length();
		if (PreyDistance <= (MinDistance * PredScale) && SizeDiff > MinGrabScale) {
			return true;
		}
		return false;
	}

	bool GrabAI_CanAttack(Actor* a_Performer) { // Attack everyone in your hand

		float WasteStamina = 20.0f;
		if (Runtime::HasPerk(a_Performer, Runtime::PERK.GTSPerkDestructionBasics)) {
			WasteStamina *= 0.65f;
		}

		if (GetAV(a_Performer, ActorValue::kStamina) > WasteStamina) {
			return true;
		}

		return false;

	}

	bool GrabAI_CanThrow(Actor* a_Performer) { // Throw everyone away
		
		float WasteStamina = 40.0f;
		if (Runtime::HasPerk(a_Performer, Runtime::PERK.GTSPerkDestructionBasics)) {
			WasteStamina *= 0.65f;
		}

		if (GetAV(a_Performer, ActorValue::kStamina) > WasteStamina) {
			return true;
		}

		return false;
	}

	bool GrabAI_CanVore(Actor* a_Performer) { // Throw everyone away

		float WasteStamina = 10.0f;
		if (Runtime::HasPerk(a_Performer, Runtime::PERK.GTSPerkDestructionBasics)) {
			WasteStamina *= 0.65f;
		}

		if (GetAV(a_Performer, ActorValue::kStamina) > WasteStamina) {
			return true;
		}

		return false;
	}

	bool GrabAI_CanRelease(Actor* a_Performer) {

		auto grabbedActor = Grab::GetHeldActor(a_Performer);
		if (!grabbedActor) {
			return false;
		}
		if (IsGtsBusy(a_Performer) && !IsUsingThighAnimations(a_Performer) || AnimationVars::General::GetIsTransitioning(a_Performer)) {
			return false;
		}
		if (!a_Performer->AsActorState()->IsWeaponDrawn()) {
			return true;
		}

		return false;

	}

	void GrabAI_StartLogicTask(Actor* a_Performer, Actor* a_Prey) {

		if (!a_Performer || !a_Prey) {
			logger::warn("GrabAI: Actor was null before task run");
			return;
		}

		const std::string TaskName = std::format("GrabAI_{}_{}", a_Performer->formID, a_Prey->formID);
		const ActorHandle PerformerHandle = a_Performer->CreateRefHandle();
		const ActorHandle PreyHandle = a_Prey->CreateRefHandle();

		const auto& TransientData = Transient::GetActorData(a_Performer);
		if (!TransientData) {
			return;
		}

		Grab::GrabActor(a_Performer, a_Prey);
		AnimationManager::StartAnim("GrabSomeone", a_Performer);

		TaskManager::Run(TaskName, [=](auto& progressData) {

			if (!State::Live()) return false;

			const auto& Settings = Config::AI.Grab;

			if (!PerformerHandle || !PreyHandle) {
				logger::warn("GrabAI: ActorHandle was null");
				return false;
			}

			Actor* PerformerActor = PerformerHandle.get().get();
			Actor* PreyActor = PreyHandle.get().get();

			if (!PerformerActor || !PreyActor) {
				logger::warn("GrabAI: Actor was null");
				return false;
			}

			if (!TransientData) {
				logger::warn("GrabAI: Transient Bad");
				return false;
			}

			TransientData->ActionTimer.UpdateDelta(Config::AI.Grab.fInterval);
			const bool IsDead    = PreyActor->IsDead() || GetAV(PreyActor, ActorValue::kHealth) <= 0.0f || PerformerActor->IsDead();
			const bool IsBusy    = AnimationVars::Grab::GetIsGrabAttacking(PerformerActor) || AnimationVars::General::GetIsTransitioning(PerformerActor);
			const bool ValidPrey = Grab::GetHeldActor(PerformerActor) != nullptr;

			if (!IsDead && !IsBusy) {

				if (TransientData->ActionTimer.ShouldRun()) {

					PreventCombat(PerformerActor);

					if (!IsBetweenBreasts(PreyActor) && 
						!IsInCleavageState(PerformerActor) && 
						!IsInsideCleavage(PreyActor) && 
						!IsGtsBusy(PerformerActor)) {

						const int AttackChance      = GrabAI_CanAttack(PerformerActor)  ? static_cast<int>(Settings.fCrushProb)   : 0;
						const int ThrowChance       = GrabAI_CanThrow(PerformerActor)   ? static_cast<int>(Settings.fThrowProb)   : 0;
						const int EatChance         = GrabAI_CanVore(PerformerActor)    ? static_cast<int>(Settings.fVoreProb)    : 0;
						const int ReleaseChance     = GrabAI_CanRelease(PerformerActor) ? static_cast<int>(Settings.fReleaseProb) : 0;
						const int CleavageChance    = static_cast<int>(Settings.fCleavageProb);
						const int GrabPlayStartProb = static_cast<int>(Settings.fGrabPlayStartProb);

						switch (RandomIntWeighted({ AttackChance, ThrowChance, EatChance, ReleaseChance, CleavageChance, GrabPlayStartProb, 33 })) {

							//Attack
							case 0: {
								AnimationManager::StartAnim("GrabDamageAttack", PerformerActor);
								break;
							}
							//Throw
							case 1: {
								AnimationManager::StartAnim("GrabThrowSomeone", PerformerActor);
								break;
							}
							//Vore
							case 2: {
								AnimationManager::StartAnim("GrabEatSomeone", PerformerActor);
								break;
							}
							//Release
							case 3: {
								AnimationManager::StartAnim("GrabReleasePunies", PerformerActor);
								break;
							}
							//Cleavage
							case 4: {
								AnimationManager::StartAnim("Breasts_Put", PerformerActor);
								break;
							}
							//Grab Play
							case 5: {
								AnimationManager::StartAnim("GrabPlay_Enter", PerformerActor);
								AnimationManager::StartAnim("GrabPlay_Enter_T", PreyActor);
								break;
							}

							default:{}

						}

					}
					else if (IsBetweenBreasts(PreyActor) && !IsGtsBusy(PerformerActor)) {

						if (RandomBool(80)) {
							Utils_UpdateHighHeelBlend(PerformerActor, false);
							AnimationManager::StartAnim("Cleavage_EnterState", PerformerActor);
							AnimationManager::StartAnim("Cleavage_EnterState_Tiny", PreyActor);
						}

					}
					else if (AnimationVars::Cleavage::GetIsBoobsDoting(PerformerActor)) {

						//Small Chance to Stop, Basically guaranteed to happen after 30 ShouldRun Calls (100 / 3.333 = ~30)
						//Shortest Timer is 1.0 sec so after ~30s max Stop DOT.
						if (RandomBool(3.333f)) {
							// Spare tiny, return to idle breast loop
							AnimationManager::StartAnim("Cleavage_DOT_Stop", PerformerActor);
							AnimationManager::StartAnim("Cleavage_DOT_Stop_Tiny", PreyActor);
						}
					}
					//IsGtsBusy(PerformerActor) is true when in this state
					else if (IsInCleavageState(PerformerActor) && IsInsideCleavage(PreyActor)) {

						const int AttackChance    = static_cast<int>(Settings.fCleavageAttackProb);
						const int SuffocateChance = static_cast<int>(Settings.fCleavageSuffocateProb);
						const int EatChance       = static_cast<int>(Settings.fCleavageVoreProb);
						const int AbsorbChance    = static_cast<int>(Settings.fCleavageAbsorbProb);
						const int StrangleChance  = static_cast<int>(Settings.fStrangleChance);
						const int StopChance      = static_cast<int>(Settings.fCleavageStopProb);

						switch (RandomIntWeighted({ AttackChance, SuffocateChance, EatChance, AbsorbChance, StrangleChance, StopChance, 33 })) {

							//Attack
							case 0: {

								if (RandomBool(50.0f)) {
									AnimationManager::StartAnim("Cleavage_LightAttack", PerformerActor);
									AnimationManager::StartAnim("Cleavage_LightAttack_Tiny", PreyActor);
								}
								else {
									AnimationManager::StartAnim("Cleavage_HeavyAttack", PerformerActor);
									AnimationManager::StartAnim("Cleavage_HeavyAttack_Tiny", PreyActor);
								}

								break;
							}
							//Suffocate
							case 1: {
								AnimationManager::StartAnim("Cleavage_Suffocate", PerformerActor);
								AnimationManager::StartAnim("Cleavage_Suffocate_Tiny", PreyActor);
								break;
							}
							//Vore
							case 2: {
								AnimationManager::StartAnim("Cleavage_Vore", PerformerActor);
								AnimationManager::StartAnim("Cleavage_Vore_Tiny", PreyActor);
								break;
							}
							//Absorb
							case 3: {
								AnimationManager::StartAnim("Cleavage_Absorb", PerformerActor);
								AnimationManager::StartAnim("Cleavage_Absorb_Tiny", PreyActor);
								break;
							}
							//Strangle
							case 4: {
								AnimationManager::StartAnim("Cleavage_DOT_Start", PerformerActor);
								Animation_Cleavage::AttemptBreastActionOnTiny("Cleavage_DOT_Start_Tiny", PerformerActor);
								break;
							}
							//Stop
							case 5: {
								AnimationManager::StartAnim("Cleavage_ExitState", PerformerActor);
								break;
							}

							default:{}

						}
					}
					else if (AnimationVars::Action::GetIsInGrabPlayState(PerformerActor)) {

						const bool isGrinding = AnimationVars::Action::GetIsGrabPlaying(PerformerActor);
						const bool isKissing = AnimationVars::Action::GetIsKissing(PerformerActor);
						const bool isInSubState = isGrinding || isKissing;

						//Mult by the !bool so the chance is only > 0 if not in a substate
						const int HeavyCrushChance = static_cast<int>(Settings.fGrabPlayHeavyCrushProb) * !isInSubState;
						const int VoreChance       = static_cast<int>(Settings.fGrabPlayVoreProb)       * !isInSubState;
						const int KissChance       = static_cast<int>(Settings.fGrabPlayKissProb)       * !isInSubState;
						const int PokeChance       = static_cast<int>(Settings.fGrabPlayPokeProb)       * !isInSubState;
						const int FlickChance      = static_cast<int>(Settings.fGrabPlayFlickProb)      * !isInSubState;
						const int SandwichChance   = static_cast<int>(Settings.fGrabPlaySandwichProb)   * !isInSubState;
						const int GrindStartChance = static_cast<int>(Settings.fGrabPlayGrindStartProb) * !isInSubState;
						const int ExitChance       = static_cast<int>(Settings.fGrabPlayExitProb)       * !isInSubState;

						// "Sub" states, multiply by isX to only enable when in that substate
						const int KissVoreChance  = static_cast<int>(Settings.fGrabPlayKissVoreProb)  * isKissing;
						const int GrindStopChance = static_cast<int>(Settings.fGrabPlayGrindStopProb) * isGrinding;

						switch (RandomIntWeighted({ HeavyCrushChance, VoreChance, KissChance, KissVoreChance, PokeChance, 
							FlickChance, SandwichChance, GrindStartChance, GrindStopChance, ExitChance, 33 })) {

							//HeavyCrushChance - Stateless
							case 0: {
								AnimationManager::StartAnim("GrabPlay_CrushH", PerformerActor);
								AnimationManager::StartAnim("GrabPlay_CrushH_T", PreyActor);
								break;
							}
							//VoreChance - Stateless
							case 1: {
								AnimationManager::StartAnim("GrabPlay_Vore", PerformerActor);
								AnimationManager::StartAnim("GrabPlay_Vore_T", PreyActor);
								break;
							}
							//KissChance - Stateless
							case 2: {
								AnimationManager::StartAnim("GrabPlay_Kiss", PerformerActor);
								AnimationManager::StartAnim("GrabPlay_Kiss_T", PreyActor);
								break;
							}
							//KissVoreChance - Substate of Kiss
							case 3: {
								AnimationManager::StartAnim("GrabPlay_KissVore", PerformerActor);
								AnimationManager::StartAnim("GrabPlay_KissVore_T", PreyActor);
								break;
							}
							//PokeChance - Stateless
							case 4: {
								AnimationManager::StartAnim("GrabPlay_Poke", PerformerActor);
								AnimationManager::StartAnim("GrabPlay_Poke_T", PreyActor);
								break;
							}
							//FlickChance - Stateless
							case 5: {
								AnimationManager::StartAnim("GrabPlay_Flick", PerformerActor);
								AnimationManager::StartAnim("GrabPlay_Flick_T", PreyActor);
								break;
							}
							//SandwichChance - Stateless
							case 6: {
								AnimationManager::StartAnim("GrabPlay_Sandwich", PerformerActor);
								AnimationManager::StartAnim("GrabPlay_Sandwich_T", PreyActor);
								break;
							}
							//GrindStartChance - Statefull
							case 7: {
								AnimationManager::StartAnim("GrabPlay_GrindStart", PerformerActor);
								AnimationManager::StartAnim("GrabPlay_GrindStart_T", PreyActor);
								break;
							}
							//GrindStopChance - Substate of GrindStartChance
							case 8: {
								AnimationManager::StartAnim("GrabPlay_GrindStop", PerformerActor);
								AnimationManager::StartAnim("GrabPlay_GrindStop_T", PreyActor);
								break;
							}
							//ExitChance - Stateless
							case 9: {
								AnimationManager::StartAnim("GrabPlay_Exit", PerformerActor);
								AnimationManager::StartAnim("GrabPlay_Exit_T", PreyActor);
								break;
							}

							default: {}
						}
					}
				}
			}

			bool Attacking = AnimationVars::Grab::GetIsGrabAttacking(PerformerActor) || AnimationVars::Action::GetIsGrabPlaying(PerformerActor);
			bool CanCancel = (IsDead || !AnimationVars::Action::GetIsVoring(PerformerActor)) && (!Attacking || IsBeingEaten(PreyActor));
			if (ShouldAbortGrab(PerformerActor, PreyActor, CanCancel, IsDead, ValidPrey)) {
				logger::info("GrabAI: Prey Dead or Invalid");
				Utils_UpdateHighHeelBlend(PerformerActor, false);

				if (ValidPrey) { // We don't want to push/cancel grab on Null actor
					PushActorAway(PerformerActor, PreyActor, 1.0f);
					Grab::CancelGrab(PerformerActor, PreyActor);
				} else {
					Grab::FailSafeReset(PerformerActor);
				}

				ResetCombat(PerformerActor);
				return false;
			}
			
			return true;
		});
	}

}

namespace GTS {


	std::vector<Actor*> GrabAI_FilterList(Actor* a_Performer, const std::vector<Actor*>& a_PotentialPrey) {

		if (!a_Performer) {
			return {};
		}

		auto CharController = a_Performer->GetCharController();
		if (!CharController) {
			return {};
		}

		NiPoint3 PredPos = a_Performer->GetPosition();

		auto PreyList = a_PotentialPrey;

		// Sort prey by distance
		ranges::sort(PreyList,[PredPos](const Actor* a_PreyA, const Actor* a_PreyB) -> bool {
			float DistToA = (a_PreyA->GetPosition() - PredPos).Length();
			float DistToB = (a_PreyB->GetPosition() - PredPos).Length();
			return DistToA < DistToB;
		});

		// Filter out invalid targets
		std::erase_if(PreyList, [a_Performer](auto idxPrey) {
			return !CanGrab(a_Performer, idxPrey);
		});

		// Filter out actors not in front
		const float ActorAngle = a_Performer->data.angle.z;
		constexpr NiPoint3 FWDVector = { 0.f, 1.f, 0.f };
		const NiPoint3 ActorForward = RotateAngleAxis(FWDVector, -ActorAngle, { 0.f, 0.f, 1.f });

		NiPoint3 PredDirection = ActorForward;
		PredDirection = PredDirection / PredDirection.Length();
		std::erase_if(PreyList, [PredPos, PredDirection](auto idxPrey) {
			NiPoint3 PreyDirection = idxPrey->GetPosition() - PredPos;
			if (PreyDirection.Length() <= 1e-4) {
				return false;
			}
			PreyDirection = PreyDirection / PreyDirection.Length();
			const float CosineTheta = PredDirection.Dot(PreyDirection);
			return CosineTheta <= 0; // 180 degress
		});

		// Filter out actors not in a truncated cone
		// \      x   /
		//  \  x     /
		//   \______/  <- Truncated cone
		//   | pred |  <- Based on width of pred
		//   |______|

		const float PredConeWidth = 70 * get_visual_scale(a_Performer);
		const float ShiftAmount = fabs((PredConeWidth / 2.0f) / tan(GRAB_ANGLE / 2.0f));

		const NiPoint3 ConeStart = PredPos - PredDirection * ShiftAmount;
		std::erase_if(PreyList, [ConeStart, PredConeWidth, PredDirection](auto prey) {
			NiPoint3 PreyDirection = prey->GetPosition() - ConeStart;
			if (PreyDirection.Length() <= PredConeWidth * 0.4f) {
				return false;
			}
			PreyDirection = PreyDirection / PreyDirection.Length();
			const float CosineTheta = PredDirection.Dot(PreyDirection);
			return CosineTheta <= cos(GRAB_ANGLE * PI / 180.0f);
		});

		return GetMaxActionableTinyCount(a_Performer, PreyList);
	}

	void GrabAI_Start(Actor* a_Performer, Actor* a_Prey) {
		Utils_UpdateHighHeelBlend(a_Performer, false);
		GrabAI_StartLogicTask(a_Performer, a_Prey);
	}

}
 