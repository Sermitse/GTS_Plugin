#include "Managers/AI/AIFunctions.hpp"
#include "config/Config.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Managers/Animation/AnimationManager.hpp"

using namespace GTS;

namespace {

	void DisableEssentialFlag(Actor* actor) {
		if (actor->IsEssential()) {
			actor->GetActorRuntimeData().boolFlags.reset(RE::Actor::BOOL_FLAGS::kEssential); // Else they respawn.
			actor->AsActorState()->actorState1.lifeState = ACTOR_LIFE_STATE::kDead;
			auto data = actor->GetActorBase()->As<TESActorBaseData>();
			if (data) {
				data->actorData.actorBaseFlags.reset(ACTOR_BASE_DATA::Flag::kEssential);
				data->actorData.actorBaseFlags.reset(ACTOR_BASE_DATA::Flag::kProtected);
			}
		}
	}

	void DisableDeathDialogueTask(Actor* tiny) {
		std::string name = std::format("MuteDeath_{}", tiny->formID);
		ActorHandle tinyRef = tiny->CreateRefHandle();

		TaskManager::RunFor(name, 0.25f, [=](auto& progressData) {
			if (!tinyRef) {
				return false;
			}
			auto actor = tinyRef.get().get();

			if (actor && actor->formID != 0x14) {
				auto process = actor->GetActorRuntimeData().currentProcess;
				if (process) {
					auto high = process->high;
					if (high) {
						high->deathDialogue = false;
						for (auto handles: {0, 1, 2}) {
							auto handle = high->soundHandles[handles];
							if (handle.soundID != BSSoundHandle::kInvalidID) {
								handle.SetVolume(0.0f);
							}
						}
					}
				}
			}
			return true;
		});
	}

	bool ShouldBeAltered(Actor* giant) {
		bool Alter = giant && giant->formID != 0x14 && IsTeammate(giant) && 
					IsHuman(giant) && IsFemale(giant, true) 
					&& get_visual_scale(giant) > 1.25f;
		return Alter;
	}

	void AlterMovementSpeed(Actor* giant, float& speed) {

		float speedMult = std::clamp(AnimationManager::GetAnimSpeed(giant), 0.01f, 1.0f);

		if (get_visual_scale(giant) > 1.75f && giant->AsActorState()->IsSprinting()) {
			giant->AsActorState()->actorState1.sprinting = 0;
			giant->AsActorState()->actorState1.walking = 1;
		}

		float multiplier = (speedMult * speedMult);

		speed *= multiplier;
		speed = std::clamp(speed, 15.0f, 10000.0f);
		
		//log::info("New Speed: {}, SpeedMult: {}, Multiplier: {}", speed, speedMult, multiplier);
	}

	void AlterRotationSpeed(Actor* giant) {
		auto AI = giant->GetActorRuntimeData().currentProcess;
		if (AI) {
			//auto high = AI->high;
			auto mid = AI->middleHigh;

			float speedMult = std::clamp(AnimationManager::GetAnimSpeed(giant), 0.02f, 1.0f);
			float multiplier = (speedMult * speedMult);

			if (mid) {
				if (mid->rotationSpeed.Length() > 0.0f) {
					mid->rotationSpeed *= multiplier;
				}
			}

			/*if (high) {
				if (high->pathingCurrentRotationSpeed.Length() > 0.0f) {
					high->pathingCurrentRotationSpeed *= multiplier * speedMult;
				}
				if (high->pathingDesiredRotationSpeed.Length() > 0.0f) {
					high->pathingDesiredRotationSpeed *= multiplier * speedMult;
				}
			}*/
		}
	}
}

namespace GTS {

	float GetNPCSpeedOverride(Actor* giant, float incoming_speed) {

		float new_speed = incoming_speed;

		if (ShouldBeAltered(giant)) {
			if (Config::AI.bSlowMovementDown) {
				AlterMovementSpeed(giant, new_speed);
			}

			if (Config::AI.bSlowRotationDown) {
				AlterRotationSpeed(giant);
			}
		}
		
		return new_speed;
	}

	float GetScareThreshold(Actor* giant) {
		float threshold = 2.5f;
		if (giant->IsSneaking()) { // If we sneak/prone/crawl = make threshold bigger so it's harder to scare actors
			threshold += 0.8f;
		}
		if (AnimationVars::Crawl::IsCrawling(giant)) {
			threshold += 1.45f;
		}
		if (AnimationVars::Prone::IsProne(giant)) {
			threshold += 1.45f;
		}
		if (giant->AsActorState()->IsWalking()) { // harder to scare if we're approaching slowly
			threshold *= 1.35f;
		}
		if (giant->AsActorState()->IsSprinting()) { // easier to scare
			threshold *= 0.75f;
		}
		return threshold;
	}

	void Task_InitHavokTask(Actor* tiny) {

		double startTime = Time::WorldTimeElapsed();
		ActorHandle tinyHandle = tiny->CreateRefHandle();
		std::string taskname = std::format("EnterRagdoll_{}", tiny->formID);

		TaskManager::RunFor(taskname, 2.0f, [=](auto& update){
			if (!tinyHandle) {
				return false;
			}
			Actor* tinyref = tinyHandle.get().get();
			if (!tinyref) {
				return false;
			}
			if (!tinyref->Is3DLoaded()) {
				return true;
			}
			if (!tinyref->IsDead()) {
				return true;
			}
			double endTime = Time::WorldTimeElapsed();

			if ((endTime - startTime) > 0.05) {
				tinyref->InitHavok(); // Hopefully will fix occasional Ragdoll issues
				return false;
			} 
			return true;
		});
	}

	void SendDeathEvent(Actor* giant, Actor* tiny) {
		auto* eventsource = ScriptEventSourceHolder::GetSingleton();
		if (eventsource) {
			TESObjectREFR* dyingTiny = skyrim_cast<TESObjectREFR*>(tiny);
			TESObjectREFR* killer = skyrim_cast<TESObjectREFR*>(giant);

			if (dyingTiny && killer) {
				ObjectRefHandle dyingRefr = dyingTiny->CreateRefHandle();
				ObjectRefHandle killerRefr = killer->CreateRefHandle();

				if (dyingRefr && killerRefr) {
					TESObjectREFRPtr dying_get = dyingRefr.get();
					TESObjectREFRPtr killer_get = killerRefr.get();
					if (dying_get && killer_get) {
						auto event = TESDeathEvent();
						event.actorDying = dying_get;
						event.actorKiller = killer_get;
						event.dead = true;
						eventsource->SendEvent(&event);
					}
				}
			}
		}
	}

	void KillActor(Actor* giant, Actor* tiny, bool silent) {
		DisableEssentialFlag(tiny); // Prevent Essentials from reappearing
		if (silent) {
			DisableDeathDialogueTask(tiny);
		}

		if (tiny && tiny->Is3DLoaded() && !tiny->IsDead()) {
			StartCombat(tiny, giant);
		}

		float hp = GetMaxAV(tiny, ActorValue::kHealth) * 9.0f;	

		InflictSizeDamage(giant, tiny, hp); // just to make sure

		if (tiny->formID == 0x14) {
			tiny->KillImpl(giant, 1, true, true);
			tiny->SetAlpha(0.0f);
		} 
		SendDeathEvent(giant, tiny);
		Task_InitHavokTask(tiny);
	}

	

	void ForceFlee(Actor* giant, Actor* tiny, float duration, bool apply_size_difference) {
		float oldConfidence = GetAV(tiny, ActorValue::kConfidence);

		double Start = Time::WorldTimeElapsed();
		std::string name = std::format("ScareAway_{}", tiny->formID);
		ActorHandle tinyHandle = tiny->CreateRefHandle();
		ActorHandle giantHandle = giant->CreateRefHandle();
		if (apply_size_difference) {
			duration *= get_scale_difference(giant, tiny, SizeType::VisualScale, false, true);
		}

		SetAV(tiny, ActorValue::kConfidence, 0.0f);

		TaskManager::Run(name, [=](auto& progressData) {
			if (!tinyHandle) {
				return false;
			}
			if (!giantHandle) {
				return false;
			}
			double Finish = Time::WorldTimeElapsed();

			auto tinyRef = tinyHandle.get().get();
			auto giantRef = giantHandle.get().get();

			if (!tinyRef || !tinyRef->Is3DLoaded()) {
				SetAV(tinyRef, ActorValue::kConfidence, oldConfidence);
				return false;
			}

			if (tinyRef->IsDead()) {
				SetAV(tinyRef, ActorValue::kConfidence, oldConfidence);
				return false; // To be safe
			}

			ApplyActionCooldown(tinyRef, CooldownSource::Action_ScareOther);

			double timepassed = Finish - Start;
			if (IsMoving(tinyRef)) {
				int FallChance = RandomInt(0, 6000);// Chance to Trip
				if (FallChance <= 2 && !IsRagdolled(tinyRef)) {
					PushActorAway(giantRef, tinyRef, 1.0f);
				}
			}
			
			if (timepassed >= static_cast<float>(duration)) {
				SetAV(tinyRef, ActorValue::kConfidence, oldConfidence);
				return false; // end it
			}
			return true;
		});
	}

	void ScareActors(Actor* giant) {
		GTS_PROFILE_SCOPE("ActorUtils: ScareActors");
		if (!Config::AI.bPanic) {
			return; // Disallow Panic if bool is false.
		}
		for (auto tiny: FindSomeActors("AiActors", 2)) {
			if (tiny != giant && tiny->formID != 0x14 && !IsTeammate(tiny)) {
				if (tiny->IsDead() || IsInSexlabAnim(tiny, giant)) {
					return;
				}
				if (IsBeingHeld(giant, tiny)) {
					return;
				}
				float get_difference = get_scale_difference(giant, tiny, SizeType::VisualScale, false, true); // Apply HH difference as well
				float sizedifference = std::clamp(get_difference, 0.10f, 12.0f);

				float distancecheck = 128.0f * GetMovementModifier(giant);
				float threshold = GetScareThreshold(giant);

				if (sizedifference >= threshold) {
					NiPoint3 GiantDist = giant->GetPosition();
					NiPoint3 ObserverDist = tiny->GetPosition();
					float distance = (GiantDist - ObserverDist).Length();

					if (distance <= distancecheck * sizedifference) {
						auto combat = tiny->GetActorRuntimeData().combatController;

						tiny->GetActorRuntimeData().currentCombatTarget = giant->CreateRefHandle();
						auto TinyRef = skyrim_cast<TESObjectREFR*>(tiny);

						if (TinyRef) {
							auto GiantRef = skyrim_cast<TESObjectREFR*>(giant);
							if (GiantRef) {
								bool SeeingOther;
								bool IsTrue = tiny->HasLineOfSight(GiantRef, SeeingOther);
								if (IsTrue || distance < (distancecheck/1.5f) * sizedifference) {
									auto cell = tiny->GetParentCell();
									if (cell) {
										if (!combat) {
											tiny->InitiateFlee(TinyRef, true, true, true, cell, TinyRef, 100.0f, 465.0f * sizedifference);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
