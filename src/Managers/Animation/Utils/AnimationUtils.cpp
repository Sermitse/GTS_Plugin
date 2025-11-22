#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Managers/Animation/Utils/AttachPoint.hpp"

#include "Managers/SpectatorManager.hpp"

#include "Managers/Animation/Controllers/ThighSandwichController.hpp"
#include "Managers/Animation/Controllers/GrabAnimationController.hpp"
#include "Managers/Animation/Controllers/ButtCrushController.hpp"
#include "Managers/Animation/Controllers/VoreController.hpp"
#include "Managers/Animation/Controllers/HugController.hpp"

#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/HugShrink.hpp"

#include "Managers/Emotions/EmotionManager.hpp"
#include "Managers/Perks/PerkHandler.hpp"

#include "Managers/Damage/CollisionDamage.hpp"
#include "Managers/Damage/LaunchPower.hpp"
#include "Managers/Damage/LaunchActor.hpp"

#include "Managers/AI/AIFunctions.hpp"

#include "Managers/Rumble.hpp"

#include "Managers/GTSSizeManager.hpp"
#include "Managers/HighHeel.hpp"

#include "Magic/Effects/Common.hpp"

#include "Utils/MovementForce.hpp"
#include "Utils/DeathReport.hpp"
#include "Utils/Looting.hpp"

#include "Managers/Audio/MoansLaughs.hpp"

using namespace GTS;

namespace {

	std::string_view GetImpactNode(CrawlEvent kind) {
		if (kind == CrawlEvent::RightKnee) {
			return "NPC R Calf [RClf]";
		}
		else if (kind == CrawlEvent::LeftKnee) {
			return "NPC L Calf [LClf]";
		}
		else if (kind == CrawlEvent::RightHand) {
			return "NPC R Finger20 [RF20]";
		}
		else if (kind == CrawlEvent::LeftHand) {
			return "NPC L Finger20 [LF20]";
		}
		else {
			return "NPC L Finger20 [LF20]";
		}
	}

	float GetStaggerThreshold(DamageSource Cause) {
		float StaggerThreshold = 1.0f;
		if (Cause == DamageSource::HandSwipeRight || Cause == DamageSource::HandSwipeLeft) {
			StaggerThreshold = 1.4f; // harder to stagger with hand swipes
		}
		return StaggerThreshold;
	}
}

namespace GTS {

	constexpr std::string_view leftFootLookup = "NPC L Foot [Lft ]";
	constexpr std::string_view rightFootLookup = "NPC R Foot [Rft ]";

	constexpr std::string_view leftCalfLookup = "NPC L Calf [LClf]";
	constexpr std::string_view rightCalfLookup = "NPC R Calf [RClf]";

	constexpr std::string_view leftCalfLookup_failed = "NPC L Calf [LClf]";
	constexpr std::string_view rightCalfLookup_failed = "NPC R Calf [RClf]";

	constexpr std::string_view leftToeLookup_failed = "NPC L Toe0 [LToe]";
	constexpr std::string_view rightToeLookup_failed = "NPC R Toe0 [RToe]";

	constexpr std::string_view leftToeLookup = "NPC L Joint 3 [Lft ]";
	constexpr std::string_view rightToeLookup = "NPC R Joint 3 [Rft ]";

	void RestoreBreastAttachmentState(Actor* giant, Actor* tiny) { // Fixes tiny going under our foot if someone suddenly ragdolls us during breast anims such as Absorb
		if (IsRagdolled(giant) && Attachment_GetTargetNode(giant) != AttachToNode::None) {
			Attachment_SetTargetNode(giant, AttachToNode::None);
			giant->SetGraphVariableBool("GTS_OverrideZ", false);

			if (IsHostile(giant, tiny)) {
				AnimationManager::StartAnim("Breasts_Idle_Unwilling", tiny);
			}
			else {
				AnimationManager::StartAnim("Breasts_Idle_Willing", tiny);
			}
		}
	}

	void Task_ApplyAbsorbCooldown(Actor* giant) {
        std::string taskname = std::format("ManageCooldown_{}", giant->formID);
        ActorHandle giantHandle = giant->CreateRefHandle();
        TaskManager::Run(taskname, [=](auto& update){
            if (!giantHandle) {
                return false;
            }

            auto giant_get = giantHandle.get().get();
            if (giant_get) {
                if (IsInCleavageState(giant_get) || IsHugCrushing(giant_get)) {
                    ApplyActionCooldown(giant_get, CooldownSource::Action_AbsorbOther);
                    return true; // reapply
                }
                return false;
            }
            return false;
        });
    }

	void Anims_FixAnimationDesync(Actor* giant, Actor* tiny, bool reset) {
		auto transient = Transient::GetActorData(tiny);
		if (transient) {
			float& animspeed = transient->HugAnimationSpeed;
			if (!reset) {
				animspeed = AnimationManager::GetAnimSpeed(giant); 
				// Make DLL use animspeed of GTS on Tiny
			} else {
				animspeed = 1.0f; // 1.0 makes dll use GetAnimSpeed of tiny
			}
			// Fixes hug and boob attack states anim de-sync
		}
	}

	void ForceFollowerAnimation(Actor* giant, FollowerAnimType Type) {
		std::size_t numberOfPrey = 1000;

		auto& Vore =        VoreController::GetSingleton();
		auto& ButtCrush = 	ButtCrushController::GetSingleton();
		auto& Hugs = 		HugAnimationController::GetSingleton();
		auto& Grabs = 		GrabAnimationController::GetSingleton();
		auto& Sandwich =    ThighSandwichController::GetSingleton();

		switch (Type) {
			// xxx.AllowMessage(true/false) are used to allow info messages when Follower can't do something with player
			// They're all false by default
			case FollowerAnimType::ButtCrush: {
				for (auto new_gts: FindTeammates()) {
					if (IsTeammate(new_gts)) {
						ButtCrush.AllowMessage(true);
						for (auto new_tiny: ButtCrush.GetButtCrushTargets(new_gts, numberOfPrey)) { 
							if (new_tiny->formID == 0x14) {
								ButtCrush.StartButtCrush(new_gts, new_tiny);
								ControlAnother(new_gts, false);
							}
						}
					}
				}
				ButtCrush.AllowMessage(false);
				break;	
			}
		 	case FollowerAnimType::Hugs: {
				for (auto new_gts: FindTeammates()) {
					if (IsTeammate(new_gts)) {
						Hugs.AllowMessage(true);
						for (auto new_tiny: Hugs.GetHugTargetsInFront(new_gts, numberOfPrey)) { 
							if (new_tiny->formID == 0x14) {
								Hugs.StartHug(new_gts, new_tiny);
								ControlAnother(new_gts, false);
							}
						}
					}
				}
				Hugs.AllowMessage(false);
				break;
			}
		 	case FollowerAnimType::Grab: {
				for (auto new_gts: FindTeammates()) {
					if (IsTeammate(new_gts)) {
						Grabs.AllowMessage(true);
						std::vector<Actor*> FindTiny = Grabs.GetGrabTargetsInFront(new_gts, numberOfPrey);
						for (auto new_tiny: FindTiny) { 
							if (new_tiny->formID == 0x14) {
								Grabs.StartGrab(new_gts, new_tiny);
								ControlAnother(new_gts, false);
							}
						}
					}
				}
				Grabs.AllowMessage(false);
				break;	
			}
		 	case FollowerAnimType::Vore: {	
				for (auto new_gts: FindTeammates()) {
					if (IsTeammate(new_gts)) {
						Vore.AllowMessage(true);
						for (auto new_tiny: Vore.GetVoreTargetsInFront(new_gts, numberOfPrey)) { 
							if (new_tiny->formID == 0x14) {
								Vore.StartVore(new_gts, new_tiny);
								ControlAnother(new_gts, false);
							}
						}
					}
				}
				Vore.AllowMessage(false);
				break;
			} 
		 	case FollowerAnimType::ThighSandwich: {
				for (auto new_gts: FindTeammates()) {
					if (IsTeammate(new_gts)) {
						Sandwich.AllowMessage(true);
						for (auto new_tiny: Sandwich.GetSandwichTargetsInFront(new_gts, numberOfPrey)) { 
							if (new_tiny->formID == 0x14) {
								Sandwich.StartSandwiching(new_gts, new_tiny);
								ControlAnother(new_gts, false);
							}
						}
					}
				} 
				Sandwich.AllowMessage(false);
				break;
			}
		}
	}

	void Vore_AttachToRightHandTask(Actor* giant, Actor* tiny) {
		std::string name = std::format("CrawlVore_{}_{}", giant->formID, tiny->formID);
		ActorHandle giantHandle = giant->CreateRefHandle();
		ActorHandle tinyHandle = tiny->CreateRefHandle();
		TaskManager::Run(name, [=](auto& progressData) {
			if (!giantHandle) {
				return false;
			} 
			if (!tinyHandle) {
				return false;
			}
			auto giantref = giantHandle.get().get();
			auto tinyref = tinyHandle.get().get();

			auto FingerA = find_node(giant, "NPC R Finger02 [RF02]");
			if (!FingerA) {
				Notify("R Finger 02 node not found");
				return false;
			}
			auto FingerB = find_node(giant, "NPC R Finger30 [RF30]");
			if (!FingerB) {
				Notify("R Finger 30 node not found");
				return false;
			}
			NiPoint3 coords = (FingerA->world.translate + FingerB->world.translate) / 2.0f;
			coords.z -= 3.0f;

			if (tinyref->IsDead()) {
				Notify("Vore Task ended");
				return false;
			}

			return AttachTo(giantref, tinyref, coords);
		});
	}
	
	bool Vore_ShouldAttachToRHand(Actor* giant, Actor* tiny) {
		if (IsTransferingTiny(giant)) {
			Vore_AttachToRightHandTask(giant, tiny); // start "attach to hand" task outside of vore.cpp
			return true;
		} else {
			return false;
		}
	}

	void UpdateFriendlyHugs(Actor* giant, Actor* tiny, bool force) {
		bool hostile = IsHostile(tiny, giant);
		bool teammate = IsTeammate(tiny) || tiny->formID == 0x14;
		bool perk = Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkHugsLovingEmbrace);

		if (perk && !hostile && teammate && !force) {
			tiny->SetGraphVariableBool("GTS_IsFollower", true);
			giant->SetGraphVariableBool("GTS_HuggingTeammate", true);
		} else {
			tiny->SetGraphVariableBool("GTS_IsFollower", false);
			giant->SetGraphVariableBool("GTS_HuggingTeammate", false);
		}
		// This function determines the following:
		// Should the Tiny play "willing" or "Unwilling" hug idle?
	}

	void HugCrushOther(Actor* giant, Actor* tiny) {
		AdvanceQuestProgression(giant, tiny, QuestStage::Crushing, 1.0f, false);
		Attacked(tiny, giant);

		ModSizeExperience(giant, 0.22f); // Adjust Size Matter skill

		auto Node = find_node(giant, "NPC Spine2 [Spn2]"); 
		if (!Node) {
			Notify("Error: Spine2 [Spn2] node not found");
			return;
		}

		Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundShrinkToNothing, giant, 1.0f, "NPC Spine2 [Spn2]");

		if (!IsLiving(tiny)) {
			SpawnDustParticle(tiny, tiny, "NPC Root [Root]", 3.6f);
		} else {
			if (!LessGore()) {
				auto root = find_node(tiny, "NPC Root [Root]");
				if (root) {
					SpawnParticle(tiny, 1.20f, "GTS/Damage/Explode.nif", NiMatrix3(), root->world.translate, 2.0f, 7, root);
					SpawnParticle(tiny, 1.20f, "GTS/Damage/Explode.nif", NiMatrix3(), root->world.translate, 2.0f, 7, root);
					SpawnParticle(tiny, 1.20f, "GTS/Damage/Explode.nif", NiMatrix3(), root->world.translate, 2.0f, 7, root);
					SpawnParticle(tiny, 1.20f, "GTS/Damage/ShrinkOrCrush.nif", NiMatrix3(), root->world.translate, get_visual_scale(tiny) * 10, 7, root);
				}
				Runtime::CreateExplosion(tiny, get_visual_scale(tiny)/4, Runtime::EXPL.GTSExplosionBlood);
				Runtime::PlayImpactEffect(tiny, Runtime::IDTS.GTSBloodSprayImpactSetVoreMedium, "NPC Root [Root]", NiPoint3{0, 0, -1}, 512, false, true);
			} else {
				Runtime::PlaySound("SKSoundBloodGush", tiny, 1.0f, 0.5f);
			}
		}

		AddSMTDuration(giant, 5.0f);
		ApplyShakeAtNode(tiny, 3, "NPC Root [Root]");

		const auto& MuteHugCrush = Config::Audio.bMuteHugCrushDeathScreams;

		DecreaseShoutCooldown(giant);
		KillActor(giant, tiny, MuteHugCrush);

		if (tiny->formID != 0x14) {
			Disintegrate(tiny); // Set critical stage 4 on actor
            SendDeathEvent(giant, tiny);
		} else {
			TriggerScreenBlood(50);
			tiny->SetAlpha(0.0f); // Player can't be disintegrated, so we make player Invisible
		}

		ActorHandle giantHandle = giant->CreateRefHandle();
		ActorHandle tinyHandle = tiny->CreateRefHandle();
		std::string taskname = std::format("HugCrush_{}", tiny->formID);

		TaskManager::RunOnce(taskname, [=](auto& update){
			if (!tinyHandle) {
				return;
			}
			if (!giantHandle) {
				return;
			}
			auto giantref = giantHandle.get().get();
			auto tinyref = tinyHandle.get().get();

			float scale = get_visual_scale(tinyref) * GetSizeFromBoundingBox(tinyref);
			PerkHandler::UpdatePerkValues(giantref, PerkUpdate::Perk_LifeForceAbsorption);
			TransferInventory(tinyref, giantref, scale, false, true, DamageSource::Crushed, true);
		});
	}

	// Cancels all hug-related things
	void AbortHugAnimation(Actor* giant, Actor* tiny) {
		
		bool Friendly;
		giant->GetGraphVariableBool("GTS_HuggingTeammate", Friendly);

		SetSneaking(giant, false, 0);

		AdjustFacialExpression(giant, 0, 0.0f, CharEmotionType::Phenome);
		AdjustFacialExpression(giant, 0, 0.0f, CharEmotionType::Modifier);
		AdjustFacialExpression(giant, 1, 0.0f, CharEmotionType::Modifier);

		AnimationManager::StartAnim("Huggies_Spare", giant); // Start "Release" animation on Giant

		if (Friendly && !IsCrawling(giant) && tiny) { // If friendly, we don't want to push/release actor
			EnableCollisions(tiny);
			SetBeingHeld(tiny, false);
			UpdateFriendlyHugs(giant, tiny, true); // set GTS_IsFollower (tiny) and GTS_HuggingTeammate (GTS) bools to false
			Anims_FixAnimationDesync(giant, tiny, true); // reset anim speed override so .dll won't use it
			AnimationManager::StartAnim("Huggies_Spare", tiny);
			return; // GTS_Hug_Release event (HugHeal.cpp) handles it instead.
		}

		if (tiny) {
			EnableCollisions(tiny);
			SetBeingHeld(tiny, false);
			PushActorAway(giant, tiny, 1.0f);
			UpdateFriendlyHugs(giant, tiny, true); // set GTS_IsFollower (tiny) and GTS_HuggingTeammate (GTS) bools to false
			Anims_FixAnimationDesync(giant, tiny, true); // reset anim speed override so .dll won't use it
		}
		HugShrink::Release(giant);
	}

	void Utils_UpdateHugBehaviors(Actor* giant, Actor* tiny) { // blend between two anims: send value to behaviors
        float tinySize = get_visual_scale(tiny);
        float giantSize = get_visual_scale(giant);
        float size_difference = std::clamp(giantSize/tinySize, 1.0f, 3.0f);

		float OldMin = 1.0f;
		float OldMax = 3.0f;

		float NewMin = 0.0f;
		float NewMax = 1.0f;

		float OldValue = size_difference;
		float NewValue = (((OldValue - OldMin) * (NewMax - NewMin)) / (OldMax - OldMin)) + NewMin;

		tiny->SetGraphVariableFloat("GTS_SizeDifference", NewValue); // pass Tiny / Giant size diff POV to Tiny
		giant->SetGraphVariableFloat("GTS_SizeDifference", NewValue); // pass Tiny / Giant size diff POV to GTS
    }

	void Utils_UpdateHighHeelBlend(Actor* giant, bool reset) { // needed to blend between 2 animations so hand will go lower
		if (!reset) {
			float max_heel_height = 0.215f; // All animations are configured with this value in mind. Blending isn't configured for heels bigger than this value.
			float hh_value = HighHeelManager::GetBaseHHOffset(giant)[2]/100;
			float hh_offset = std::clamp(hh_value / max_heel_height, 0.0f, 1.0f); // reach max HH at 0.215 offset (highest i've seen and the max that we support)
		
			giant->SetGraphVariableFloat("GTS_HHoffset", hh_offset);
		} else {
			giant->SetGraphVariableFloat("GTS_HHoffset", 0.0f); // reset it
		}
	}

	void Task_HighHeel_SyncVoreAnim(Actor* giant) {
		// Purpose of this task is to blend between 2 animations based on value.
		// The problem: hand that grabs the tiny is becomming offset if we equip High Heels
		// This task fixes that (by, again, blending with anim that has hand placed lower).
		std::string name = std::format("Vore_AdjustHH_{}", giant->formID);
		ActorHandle gianthandle = giant->CreateRefHandle();
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			Actor* giantref = gianthandle.get().get();

			Utils_UpdateHighHeelBlend(giantref, false);
			// make behaviors read the value to blend between anims

			if (!IsVoring(giantref)) {
				Utils_UpdateHighHeelBlend(giantref, true);
				return false; // just a fail-safe to cancel the task if we're outside of Vore anim
			}
			
			return true;
		});
    }

	void StartHealingAnimation(Actor* giant, Actor* tiny) {
		UpdateFriendlyHugs(giant, tiny, false);
		AnimationManager::StartAnim("Huggies_Heal", giant);

		if (IsFemale(tiny)) {
			AnimationManager::StartAnim("Huggies_Heal_Victim_F", tiny);
		}
		else {
			AnimationManager::StartAnim("Huggies_Heal_Victim_M", tiny);
		}
	}

	void AllowToDoVore(Actor* actor, bool toggle) {
		auto transient = Transient::GetActorData(actor);
		if (transient) {
			transient->CanDoVore = toggle;
		}
	}

	void AllowToBeCrushed(Actor* actor, bool toggle) {
		auto transient = Transient::GetActorData(actor);
		if (transient) {
			transient->CanBeCrushed = toggle;
		}
	}

	void ManageCamera(Actor* giant, bool enable, CameraTracking type) {
		if (AllowCameraTracking()) {
			auto& sizemanager = SizeManager::GetSingleton();
			sizemanager.SetTrackedBone(giant, enable, type);
		}
	}

	void ApplyButtCrushCooldownTask(Actor* giant) {
		std::string name = std::format("CooldownTask_{}", giant->formID);
		auto gianthandle = giant->CreateRefHandle();
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			
			auto giantref = gianthandle.get().get();

			ApplyActionCooldown(giant, CooldownSource::Action_ButtCrush); // Set butt crush on the cooldown
			if (!IsGtsBusy(giantref)) {
				return false;
			}
			return true;
		});
	}

	void LaunchTask(Actor* actor, float radius, float power, FootEvent kind) {
		std::string taskname = std::format("LaunchTask_{}", actor->formID);
		ActorHandle giantref = actor->CreateRefHandle();
		TaskManager::RunOnce(taskname, [=](auto& update) {
			if (!giantref) {
				return;
			}
			auto giant = giantref.get().get();

			//giant->SetGraphVariableBool("GTS_IsStomping", false);

			DoLaunch(giant, radius, power, kind);
		});
	}

	void DoLaunch(Actor* giant, float radius, float power, FootEvent kind) {
		float smt_power = 1.0f;
		float smt_radius = 1.0f;
		if (HasSMT(giant)) {
			smt_power *= 2.0f;
			smt_radius *= 1.25f;
		}
		LaunchActor::ApplyLaunch_At(giant, radius * smt_radius, power * smt_power, kind);
	}

	void DoLaunch(Actor* giant, float radius, float power, NiAVObject* node) {
		float smt_power = 1.0f;
		float smt_radius = 1.0f;
		if (HasSMT(giant)) {
			smt_power *= 2.0f;
			smt_radius *= 1.25f;
		}
		LaunchActor::LaunchAtCustomNode(giant, radius * smt_radius, 0.0f, power * smt_power, node);
	}

	void GrabStaminaDrain(Actor* giant, Actor* tiny, float sizedifference) {
		float WasteMult = 1.0f;
		if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkDestructionBasics)) {
			WasteMult *= 0.65f;
		}
		WasteMult *= Perk_GetCostReduction(giant);

		if (giant->formID != 0x14) {
			WasteMult *= 0.33f; // less drain for non-player
		}

		float WasteStamina = (1.40f * WasteMult)/sizedifference * TimeScale();
		DamageAV(giant, ActorValue::kStamina, WasteStamina);
	}

	void DrainStamina(Actor* giant, std::string_view TaskName, const RuntimeData::RuntimeEntry<RE::BGSPerk>& perk, bool enable, float power) {
		float WasteMult = 1.0f;
		if (Runtime::HasPerkTeam(giant, perk)) {
			WasteMult -= 0.35f;
		}
		WasteMult *= Perk_GetCostReduction(giant);

		std::string name = std::format("StaminaDrain_{}_{}", TaskName, giant->formID);
		if (enable) {
			ActorHandle GiantHandle = giant->CreateRefHandle();
			TaskManager::Run(name, [=](auto& progressData) {
				if (!GiantHandle) {
					return false;
				}
				auto GiantRef = GiantHandle.get().get();
				float stamina = GetAV(giant, ActorValue::kStamina);
				if (stamina <= 1.0f) {
					return false; // Abort if we don't have stamina so it won't drain it forever. Just to make sure.
				}
				float multiplier = AnimationManager::GetAnimSpeed(giant);
				float WasteStamina = 0.50f * power * multiplier;
				DamageAV(giant, ActorValue::kStamina, WasteStamina * WasteMult * TimeScale());
				return true;
			});
		} else {
			TaskManager::Cancel(name);
		}
	}

	void SpawnHurtParticles(Actor* giant, Actor* grabbedActor, float mult, float dustmult) {
		auto hand = find_node(giant, "NPC L Hand [LHnd]");
		if (hand) {
			if (IsLiving(grabbedActor)) {
				if (!LessGore()) {
					SpawnParticle(giant, 25.0f, "GTS/Damage/Explode.nif", hand->world.rotate, hand->world.translate, get_visual_scale(grabbedActor) * 3* mult, 4, hand);
					SpawnParticle(giant, 25.0f, "GTS/Damage/Crush.nif", hand->world.rotate, hand->world.translate, get_visual_scale(grabbedActor) * 3 *  mult, 4, hand);
				} else if (LessGore()) {
					Runtime::PlaySound("SKSoundBloodGush", grabbedActor, 1.0f, 0.5f);
				}
			} else {
				SpawnDustParticle(giant, grabbedActor, "NPC L Hand [LHnd]", dustmult);
			}
		}
	}

	void AdjustFacialExpression(Actor* giant, int ph, float target, CharEmotionType Type, float phenome_halflife, float modifier_speed) {
		auto fgen = giant->GetFaceGenAnimationData();
		switch (Type) {
			case CharEmotionType::Phenome:
				EmotionManager::OverridePhenome(giant, ph, phenome_halflife, target);
			break;
			case CharEmotionType::Expression:
				if (fgen) {
					if (target > 0.01f) {
						fgen->exprOverride = false;
						fgen->SetExpressionOverride(ph, target);
						fgen->expressionKeyFrame.SetValue(ph, target); // Expression doesn't need Spring since it is already smooth by default
						fgen->exprOverride = true;
					} else {
						fgen->exprOverride = false;
						fgen->Reset(0.50f, true, false, false, false);
					}
				}
			break;
			case CharEmotionType::Modifier:
				EmotionManager::OverrideModifier(giant, ph, modifier_speed, target);
			break;
		}
	}

	float GetWasteMult(Actor* giant) {
		float WasteMult = 1.0f;
		if (Runtime::HasPerk(giant, Runtime::PERK.GTSPerkDestructionBasics)) {
			WasteMult *= 0.65f;
		}
		WasteMult *= Perk_GetCostReduction(giant);
		return WasteMult;
	}

	float GetPerkBonus_Basics(Actor* Giant) {
		if (Runtime::HasPerkTeam(Giant, Runtime::PERK.GTSPerkDestructionBasics)) {
			return 1.25f;
		} else {
			return 1.0f;
		}
	}

	float GetPerkBonus_Thighs(Actor* Giant) {
		if (Runtime::HasPerkTeam(Giant, Runtime::PERK.GTSPerkThighAbilities)) {
			return 1.25f;
		} else {
			return 1.0f;
		}
	}

	void DoFootTrample(Actor* giant, Actor* tiny, bool Right) {
		auto gianthandle = giant->CreateRefHandle();
		auto tinyhandle = tiny->CreateRefHandle();

		ShrinkUntil(giant, tiny, 8.0f, 0.22f, false);

		std::string name = std::format("FootTrample_{}", tiny->formID);
		auto FrameA = Time::FramesElapsed();

		auto coordinates = AttachToUnderFoot(giant, tiny, Right); // get XYZ;
		
		SetBeingGrinded(tiny, true);
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			if (!tinyhandle) {
				return false;
			}

			auto giantref = gianthandle.get().get();
			auto tinyref = tinyhandle.get().get();

			auto FrameB = Time::FramesElapsed() - FrameA;
			if (FrameB <= 4.0f) {
				return true;
			}

			if (!IsTrampling(giantref) || coordinates.Length() <= 0.0f) {
				SetBeingGrinded(tinyref, false);
				return false;
			}
			
			AttachTo(giantref, tinyref, coordinates);

			if (tinyref->IsDead()) {
				SetBeingGrinded(tinyref, false);
				return false;
			}
			return true;
		});
	}

	void DoFootGrind(Actor* giant, Actor* tiny, bool Right) {
		auto gianthandle = giant->CreateRefHandle();
		auto tinyhandle = tiny->CreateRefHandle();

		ShrinkUntil(giant, tiny, 8.0f, 0.16f, false);
		
		std::string name = std::format("FootGrind_{}", tiny->formID);
		auto FrameA = Time::FramesElapsed();

		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			if (!tinyhandle) {
				return false;
			}

			auto giantref = gianthandle.get().get();
			auto tinyref = tinyhandle.get().get();
			auto FrameB = Time::FramesElapsed() - FrameA;
			if (FrameB <= 4.0f) {
				return true;
			}

			auto coordinates = AttachToUnderFoot(giant, tiny, Right);
				
			if (coordinates == NiPoint3(0,0,0)) {
				return true;
			}

			AttachTo(giantref, tinyref, coordinates);
			if (!IsFootGrinding(giantref)) {
				SetBeingGrinded(tinyref, false);
				return false;
			}
			if (tinyref->IsDead()) {
				SetBeingGrinded(tinyref, false);
				return false;
			}
			return true;
		});

		TaskManager::ChangeUpdate(name, UpdateKind::Havok);

	}

	void DoFingerGrind(Actor* giant, Actor* tiny) {
		auto gianthandle = giant->CreateRefHandle();
		auto tinyhandle = tiny->CreateRefHandle();

		ShrinkUntil(giant, tiny, 10.0f, 0.18f, false);
		
		std::string name = std::format("FingerGrind_{}_{}", giant->formID, tiny->formID);
		AnimationManager::StartAnim("Tiny_Finger_Impact_S", tiny);
		auto FrameA = Time::FramesElapsed();
		auto coordinates = AttachToObjectB_GetCoords(giant, tiny);
		if (coordinates == NiPoint3(0,0,0)) {
			return;
		}
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			if (!tinyhandle) {
				return false;
			}

			auto giantref = gianthandle.get().get();
			auto tinyref = tinyhandle.get().get();
			auto FrameB = Time::FramesElapsed() - FrameA;
			if (FrameB <= 3) {
				return true;
			}

			AttachTo(giantref, tinyref, coordinates);
			if (!IsFootGrinding(giantref)) {
				SetBeingGrinded(tinyref, false);
				return false;
			}
			if (tinyref->IsDead()) {
				SetBeingGrinded(tinyref, false);
				return false;
			}
			return true;
		});
	}

	void FingerGrindCheck(Actor* giant, CrawlEvent kind, bool Right, float radius) {
		std::string_view name = GetImpactNode(kind);

		auto node = find_node(giant, name);
		if (!node) {
			return; // Make sure to return if node doesn't exist, no CTD in that case
		}

		if (!node) {
			return;
		}
		if (!giant) {
			return;
		}
		float giantScale = get_visual_scale(giant);

		float SCALE_RATIO = Action_FingerGrind;
		bool SMT = HasSMT(giant);
		if (SMT) {
			SCALE_RATIO = 0.9f;
		}

		NiPoint3 NodePosition = node->world.translate;

		float maxDistance = radius * giantScale;
		float CheckDistance = 220 * giantScale;
		// Make a list of points to check
		std::vector<NiPoint3> points = {
			NiPoint3(0.0f, 0.0f, 0.0f), // The standard position
		};
		std::vector<NiPoint3> CrawlPoints = {};

		for (NiPoint3 point: points) {
			CrawlPoints.push_back(NodePosition);
		}
		if (IsDebugEnabled() && (giant->formID == 0x14 || IsTeammate(giant) || EffectsForEveryone(giant))) {
			for (auto &point : CrawlPoints) {
				DebugDraw::DrawSphere(glm::vec3(point.x, point.y, point.z), maxDistance);
			}
		}

		NiPoint3 giantLocation = giant->GetPosition();
		for (auto otherActor: find_actors()) {
			if (otherActor != giant) {
				float tinyScale = get_visual_scale(otherActor);
				if (giantScale / tinyScale > SCALE_RATIO) {
					NiPoint3 actorLocation = otherActor->GetPosition();
					for (auto &point : CrawlPoints) {
						if ((actorLocation-giantLocation).Length() <= CheckDistance) {
							int nodeCollisions = 0;

							auto model = otherActor->GetCurrent3D();
							if (model) {
								VisitNodes(model, [&nodeCollisions, NodePosition, maxDistance](NiAVObject& a_obj) {
									float distance = (NodePosition - a_obj.world.translate).Length() - Collision_Distance_Override;
		
									if (distance <= maxDistance) {
										nodeCollisions += 1;
										return false;
									}
									return true;
								});
							}
							if (nodeCollisions > 0 && !otherActor->IsDead()) {
								SetBeingGrinded(otherActor, true);
								if (Right) {
									DoFingerGrind(giant, otherActor);
									AnimationManager::StartAnim("GrindRight", giant);
								} else {
									DoFingerGrind(giant, otherActor);
									AnimationManager::StartAnim("GrindLeft", giant);
								}
							}
						}
					}
				}
			}
		}
	}

	void FootGrindCheck(Actor* actor, float radius, bool Right, FootActionType Type) {  // Check if we hit someone with Trample/Grind. If we did - start Grind/Trample.
		if (actor) {
			float giantScale = get_visual_scale(actor);
			constexpr float BASE_CHECK_DISTANCE = 180.0f;
			float SCALE_RATIO = 3.0f;

			float maxFootDistance = radius * giantScale;

			if (HasSMT(actor)) {
				SCALE_RATIO = 0.8f;
			}
			std::vector<NiPoint3> CoordsToCheck = GetFootCoordinates(actor, Right, false);
			if (!CoordsToCheck.empty()) {
				if (IsDebugEnabled() && (actor->formID == 0x14 || IsTeammate(actor))) {
					for (const auto& footPoints : CoordsToCheck) {
						DebugDraw::DrawSphere(glm::vec3(footPoints.x, footPoints.y, footPoints.z), maxFootDistance, 800, { 0.0f, 1.0f, 0.0f, 1.0f });
					}
				}

				NiPoint3 giantLocation = actor->GetPosition();
				for (auto otherActor : find_actors()) {
					if (otherActor != actor) {
						float tinyScale = get_visual_scale(otherActor);
						if (giantScale / tinyScale > SCALE_RATIO) {
							NiPoint3 actorLocation = otherActor->GetPosition();

							if ((actorLocation - giantLocation).Length() < BASE_CHECK_DISTANCE * giantScale) {
								// Check the tiny's nodes against the giant's foot points
								int nodeCollisions = 0;

								auto model = otherActor->GetCurrent3D();

								if (model) {
									for (auto& point : CoordsToCheck) {
										VisitNodes(model, [&nodeCollisions, point, maxFootDistance](NiAVObject& a_obj) {
											float distance = (point - a_obj.world.translate).Length() - Collision_Distance_Override;

											if (distance <= maxFootDistance) {
												nodeCollisions += 1;
												return false;
											}
											return true;
											});
									}
								}
								if (nodeCollisions > 0) {
									ActorHandle giantHandle = actor->CreateRefHandle();
									ActorHandle tinyHandle = otherActor->CreateRefHandle();

									double Start = Time::WorldTimeElapsed();

									std::string taskname = std::format("GrindCheck_{}_{}", actor->formID, otherActor->formID);
									TaskManager::RunFor(taskname, 1.0f, [=](auto& update) {
										if (!tinyHandle) {
											return false;
										}
										if (!giantHandle) {
											return false;
										}

										double Finish = Time::WorldTimeElapsed();

										auto giant = giantHandle.get().get();
										auto tiny = tinyHandle.get().get();

										if (Finish - Start > 0.02) {
											if (CanDoDamage(giant, tiny, false)) {
												if (!tiny->IsDead() && GetAV(tiny, ActorValue::kHealth) > 0.0f) {
													SetBeingGrinded(tiny, true);
													std::string_view action;
													switch (Type) {
														case FootActionType::Grind_Normal:
															Right ? action = "GrindRight" : action = "GrindLeft";
															AnimationManager::StartAnim(action, giant);
															DoFootGrind(giant, tiny, Right);
															break;
														case FootActionType::Grind_UnderStomp: // Used for both standing and sneaking
															Right ? action = "UnderGrindR" : action = "UnderGrindL";
															AnimationManager::StartAnim(action, giant);
															DoFootGrind(giant, tiny, Right);
															break;
														case FootActionType::Trample_NormalOrUnder:
															Right ? action = "TrampleStartR" : action = "TrampleStartL";
															AnimationManager::StartAnim(action, giant);
															DoFootTrample(giant, tiny, Right);
															break;
													}
												}
											}
											return false;
										}
										return true;
									});
								}
							}
						}
					}
				}
			}
		}
	}

	void DoDamageAtPoint_Cooldown(Actor* giant, float radius, float damage, NiAVObject* node, NiPoint3 NodePosition, float random, float bbmult, float crushmult, float pushpower, DamageSource Cause) { // Apply crawl damage to each bone individually
		GTS_PROFILE_SCOPE("AnimUtils: DoDamageAtPoint");
		if (node) {
			if (!giant) {
				return;
			}
			auto& sizemanager = SizeManager::GetSingleton();
			float giantScale = get_visual_scale(giant);

			float SCALE_RATIO = 1.0f;
			bool SMT = false;

			if (NodePosition.Length() < 1) {
				NodePosition = node->world.translate;
			}

			if (HasSMT(giant)) {
				giantScale += 2.40f; // enough to push giants around, but not mammoths/dragons
				SMT = true; // set SMT to true
			}

			float maxDistance = radius * giantScale;
			float CheckDistance = 220 * giantScale;

			if (IsDebugEnabled() && (giant->formID == 0x14 || IsTeammate(giant))) {
				DebugDraw::DrawSphere(glm::vec3(NodePosition.x, NodePosition.y, NodePosition.z), maxDistance, 400);
			}

			NiPoint3 giantLocation = giant->GetPosition();

			for (auto otherActor: find_actors()) {
				if (otherActor != giant) {
					float tinyScale = get_visual_scale(otherActor);
					NiPoint3 actorLocation = otherActor->GetPosition();
					if ((actorLocation - giantLocation).Length() < CheckDistance) {
						tinyScale *= GetSizeFromBoundingBox(otherActor); // take Giant/Dragon scale into account

						int nodeCollisions = 0;
						float force = 0.0f;

						auto model = otherActor->GetCurrent3D();

						if (model) {
							VisitNodes(model, [&nodeCollisions, &force, NodePosition, maxDistance](NiAVObject& a_obj) {
								float distance = (NodePosition - a_obj.world.translate).Length() - Collision_Distance_Override;
								if (distance <= maxDistance) {
									nodeCollisions += 1;
									force = GetForceFromDistance(distance, maxDistance);
									return false;
								}
								return true;
							});
						}
						if (nodeCollisions > 0) {
							bool allow = IsActionOnCooldown(otherActor, CooldownSource::Damage_Hand);
							if (!allow) {
								float aveForce = std::clamp(force, 0.16f, 0.70f);
								float pushForce = std::clamp(force, 0.04f, 0.10f);
								float audio = 1.0f;
								if (SMT) {
									pushForce *= 1.5f;
									audio = 3.0f;
								}
								if (otherActor->IsDead()) {
									tinyScale *= 0.6f;
								}

								float difference = giantScale / tinyScale;
								float Threshold = GetStaggerThreshold(Cause);

								int Random = RandomInt(0, 100);
								int RagdollChance = static_cast<int>(-32 + (32 / Threshold) * difference);
								bool roll = RagdollChance > Random;
								//log::info("Roll: {}, RandomChance {}, Threshold: {}", roll, RagdollChance, Random);
								//eventually it reaches 100% chance to ragdoll an actor (at ~x3.0 size difference)

								if (otherActor->formID == 0x14 && !Config::Gameplay.ActionSettings.bEnablePlayerPushBack) {
									continue;
								}

								if (difference > 1.35f && (roll || otherActor->IsDead())) {
									PushTowards(giant, otherActor, node, pushForce * pushpower, true);
								}
								else if (difference > 0.88f * Threshold) {
									float push = std::clamp(0.25f * (difference - 0.25f), 0.25f, 1.0f);
									StaggerActor(giant, otherActor, push);
								}

								float Volume = std::clamp(difference*pushForce, 0.15f, 1.0f);

								auto targetNode = find_node(giant, GetDeathNodeName(Cause));
								if (targetNode) {
									Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundSwingImpact, Volume, targetNode); // play swing impact sound
									ApplyShakeAtPoint(giant, 1.8f * pushpower * audio, targetNode->world.translate, 0.0f);
								}

								ApplyActionCooldown(otherActor, CooldownSource::Damage_Hand);
								CollisionDamage::DoSizeDamage(giant, otherActor, damage, bbmult, crushmult, static_cast<int>(random), Cause, true);
							}
						}
					}
				}
			}
		}
	}

	void ApplyThighDamage(Actor* actor, bool right, bool CooldownCheck, float radius, float damage, float bbmult, float crush_threshold, int random, DamageSource Cause) {
		GTS_PROFILE_SCOPE("AnimUtils: ApplyThighDamage");

		if (actor) {
			auto& sizemanager = SizeManager::GetSingleton();
			float giantScale = get_visual_scale(actor);
			float perk = GetPerkBonus_Thighs(actor);
			constexpr float BASE_CHECK_DISTANCE = 90.0f;
			float SCALE_RATIO = 1.75f;

			if (HasSMT(actor)) {
				giantScale += 0.20f;
				SCALE_RATIO = 0.90f;
			}

			std::string_view leg = "NPC R Foot [Rft ]";
			std::string_view knee = "NPC R Calf [RClf]";
			std::string_view thigh = "NPC R Thigh [RThg]";

			if (!right) {
				leg = "NPC L Foot [Lft ]";
				knee = "NPC L Calf [LClf]";
				thigh = "NPC L Thigh [LThg]";
			}


			std::vector<NiPoint3> ThighPoints = GetThighCoordinates(actor, knee, leg, thigh);

			float speed = AnimationManager::GetBonusAnimationSpeed(actor);
			crush_threshold *= (1.10f - speed*0.10f);

			float feet_damage = (Damage_ThighCrush_CrossLegs_FeetImpact * perk * speed);
			
			if (CooldownCheck) {
				CollisionDamage::DoFootCollision(actor, feet_damage, radius, random, bbmult, crush_threshold, DamageSource::ThighCrushed, true, true, false, false);
				CollisionDamage::DoFootCollision(actor, feet_damage, radius, random, bbmult, crush_threshold, DamageSource::ThighCrushed, false, true, false, false);
			}

			float maxFootDistance = radius * giantScale;

			if (!ThighPoints.empty()) {
				if (IsDebugEnabled() && (actor->formID == 0x14 || IsTeammate(actor) || EffectsForEveryone(actor))) {
					for (auto &point : ThighPoints) {
						DebugDraw::DrawSphere(glm::vec3(point.x, point.y, point.z), maxFootDistance);
					}
				}
			
				NiPoint3 giantLocation = actor->GetPosition();
				for (auto otherActor: find_actors()) {
					if (otherActor != actor) {
						float tinyScale = get_visual_scale(otherActor);
						if (giantScale / tinyScale > SCALE_RATIO) {
							NiPoint3 actorLocation = otherActor->GetPosition();

							if ((actorLocation-giantLocation).Length() < BASE_CHECK_DISTANCE*giantScale) {
								int nodeCollisions = 0;
								float force = 0.0f;

								auto model = otherActor->GetCurrent3D();
								
								if (model) {
									for (auto &point : ThighPoints) {
										VisitNodes(model, [&nodeCollisions, &force, point, maxFootDistance](NiAVObject& a_obj) {
											float distance = (point - a_obj.world.translate).Length() - Collision_Distance_Override;
											if (distance <= maxFootDistance) {
												nodeCollisions += 1;
												force = GetForceFromDistance(distance, maxFootDistance);
												return false;
											}
											return true;
										});
									}
								}
								if (nodeCollisions > 0) {
									//damage /= nodeCollisions;
									if (CooldownCheck) {
										float pushForce = std::clamp(force, 0.04f, 0.10f);
										bool OnCooldown = IsActionOnCooldown(otherActor, CooldownSource::Damage_Thigh);
										if (!OnCooldown) {
											float pushCalc = 0.06f * pushForce * speed;
											Laugh_Chance(actor, otherActor, 1.35f, "ThighCrush");
											float difference = giantScale / (tinyScale * GetSizeFromBoundingBox(otherActor));
											PushTowards(actor, otherActor, leg, pushCalc * difference, true);
											CollisionDamage::DoSizeDamage(actor, otherActor, damage * speed * perk, bbmult, crush_threshold, random, Cause, true);
											ApplyActionCooldown(otherActor, CooldownSource::Damage_Thigh);
										}
									} else {
										Utils_PushCheck(actor, otherActor, Get_Bone_Movement_Speed(actor, Cause)); // pass original un-altered force
										CollisionDamage::DoSizeDamage(actor, otherActor, damage, bbmult, crush_threshold, random, Cause, true);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	void ApplyFingerDamage(Actor* giant, float radius, float damage, NiAVObject* node, float random, float bbmult, float crushmult, float Shrink, DamageSource Cause) { // Apply crawl damage to each bone individually
		GTS_PROFILE_SCOPE("AnimUtils: ApplyFingerDamage");
		if (!node) {
			return;
		}
		if (!giant) {
			return;
		}
		float giantScale = get_visual_scale(giant);

		float SCALE_RATIO = 1.25f;
		if (HasSMT(giant)) {
			SCALE_RATIO = 0.8f;
			giantScale *= 1.3f;
		}
		NiPoint3 NodePosition = node->world.translate;

		float maxDistance = radius * giantScale;
		float CheckDistance = 220 * giantScale;
		// Make a list of points to check
		std::vector<NiPoint3> points = {
			NiPoint3(0.0f, 0.0f, 0.0f), // The standard position
		};
		std::vector<NiPoint3> FingerPoints = {};

		for (NiPoint3 point: points) {
			FingerPoints.push_back(NodePosition);
		}
		if (IsDebugEnabled() && (giant->formID == 0x14 || IsTeammate(giant) || EffectsForEveryone(giant))) {
			for (auto &point : FingerPoints) {
				DebugDraw::DrawSphere(glm::vec3(point.x, point.y, point.z), maxDistance, 400);
			}
		}

		Utils_UpdateHighHeelBlend(giant, false);
		NiPoint3 giantLocation = giant->GetPosition();
		
		for (auto otherActor: find_actors()) {
			if (otherActor != giant) {
				float tinyScale = get_visual_scale(otherActor);
				if (giantScale / tinyScale > SCALE_RATIO) {
					NiPoint3 actorLocation = otherActor->GetPosition();
					for (auto &point : FingerPoints) {
						if ((actorLocation-giantLocation).Length() <= CheckDistance) {

							int nodeCollisions = 0;

							auto model = otherActor->GetCurrent3D();

							if (model) {
								VisitNodes(model, [&nodeCollisions, NodePosition, maxDistance](NiAVObject& a_obj) {
									float distance = (NodePosition - a_obj.world.translate).Length() - Collision_Distance_Override;
									if (distance <= maxDistance) {
										nodeCollisions += 1;
										return false;
									}
									return true;
								});
							}
							if (nodeCollisions > 0) {
								if (get_target_scale(otherActor) > 0.08f / GetSizeFromBoundingBox(otherActor)) {
									update_target_scale(otherActor, Shrink, SizeEffectType::kShrink);
								} else {
									set_target_scale(otherActor, 0.08f / GetSizeFromBoundingBox(otherActor));
								}
								Laugh_Chance(giant, otherActor, 1.0f, "FingerGrind"); 

								Utils_PushCheck(giant, otherActor, 1.0f);

								CollisionDamage::DoSizeDamage(giant, otherActor, damage, bbmult, crushmult, static_cast<int>(random), Cause, true);
							}
						}
					}
				}
			}
		}
	}

	std::vector<NiPoint3> GetThighCoordinates(Actor* giant, std::string_view calf, std::string_view feet, std::string_view thigh) {
		NiAVObject* Knee = find_node(giant, calf);
		NiAVObject* Foot = find_node(giant, feet);
		NiAVObject* Thigh = find_node(giant, thigh);

		if (!Knee) {
			return std::vector<NiPoint3>{};
		}
		if (!Foot) {
			return std::vector<NiPoint3>{};
		}
		if (!Thigh) {
			return std::vector<NiPoint3>{};
		}

		NiPoint3 Knee_Point = Knee->world.translate;
		NiPoint3 Foot_Point = Foot->world.translate;
		NiPoint3 Thigh_Point = Thigh->world.translate;

		NiPoint3 Knee_Pos_Middle = (Knee_Point + Foot_Point) / 2.0f; 				// middle  |-----|-----|
		NiPoint3 Knee_Pos_Up = (Knee_Point + Knee_Pos_Middle) / 2.0f;				//         |--|--|-----|
		NiPoint3 Knee_Pos_Down = (Knee_Pos_Middle + Foot_Point) / 2.0f; 				//         |-----|--|--|

		NiPoint3 Thigh_Pos_Middle = (Thigh_Point + Knee_Point) / 2.0f;               // middle  |-----|-----|
		NiPoint3 Thigh_Pos_Up = (Thigh_Pos_Middle + Thigh_Point) / 2.0f;            	//         |--|--|-----|
		NiPoint3 Thigh_Pos_Down = (Thigh_Pos_Middle + Knee_Point) / 2.0f;        	//         |-----|--|--|

		NiPoint3 Knee_Thigh_Middle = (Thigh_Pos_Down + Knee_Pos_Up) / 2.0f;          // middle between two

		std::vector<NiPoint3> coordinates = { 	
			Knee_Pos_Middle,
			Knee_Pos_Up,
			Knee_Pos_Down,
			Thigh_Pos_Middle,
			Thigh_Pos_Up,
			Thigh_Pos_Down,
			Knee_Thigh_Middle,
		};

		return coordinates;
	}

	std::vector<NiPoint3> GetFootCoordinates(Actor* actor, bool Right, bool ignore_rotation) {
		// Get world HH offset
		NiPoint3 hhOffsetbase = HighHeelManager::GetBaseHHOffset(actor);
		std::vector<NiPoint3> footPoints = {};
		std::string_view FootLookup = leftFootLookup;
		std::string_view CalfLookup = leftCalfLookup;
		std::string_view ToeLookup = leftToeLookup;

		std::string_view ToeFailed = leftToeLookup_failed;
		if (Right) {
			FootLookup = rightFootLookup;
			CalfLookup = rightCalfLookup;
			ToeLookup = rightToeLookup;

			ToeFailed = rightToeLookup_failed;
		}

		auto Foot = find_node(actor, FootLookup);
		auto Calf = find_node(actor, CalfLookup);
		auto Toe = find_node(actor, ToeLookup);
		if (!Foot) {
			//log::info("Missing Foot node on {}", actor->GetDisplayFullName());
			return footPoints;
		}
		if (!Calf) {
			return footPoints;
		}
		if (!Toe) {
			Toe = find_node(actor, ToeFailed);
			if (!Toe) {
				return footPoints;
			}
		}
		NiMatrix3 RotMat;
		{
			NiAVObject* foot = Foot;
			NiAVObject* calf = Calf;
			NiAVObject* toe = Toe;
			NiTransform inverseFoot = foot->world.Invert();
			NiPoint3 forward = inverseFoot*toe->world.translate;
			forward = forward / forward.Length();

			NiPoint3 up_1 = ((calf->world.translate + foot->world.translate) / 2);

			NiPoint3 up = inverseFoot*((up_1 + foot->world.translate) / 2);

			if (!actor->IsSneaking()) { // So foot zones face straigth, a very rough fix
				if (!IsCrawling(actor)) {
					bool ignore = (IsStomping(actor) || IsVoring(actor) || IsTrampling(actor) || IsThighSandwiching(actor));
					if (ignore_rotation || ignore) {
						up = (toe->world.translate + foot->world.translate) / 2;
						up.z += 35.0f * get_visual_scale(actor);
						up = inverseFoot*up;
					}
				}
			}

			up = up / up.Length();

			NiPoint3 side = forward.UnitCross(up);
			forward = up.UnitCross(side); // Reorthonalize

			RotMat = NiMatrix3(NiPoint3(0,0,0), forward, up);
		}

		float hh = hhOffsetbase[2] / get_npcparentnode_scale(actor);
		// Make a list of points to check
		std::vector<NiPoint3> points = {
			// x = side, y = forward, z = up/down      
			NiPoint3(0.0f, hh/10, -(1.0f + hh * 0.25f)), 	// basic foot pos
			// ^ Point 1: ---()  
			NiPoint3(0.0f, 8.0f + hh/10, -(0.35f + hh)), // Toe point		
			// ^ Point 2: ()---   
			NiPoint3(0.0f, hh/70, -(1.25f + hh)), // Underheel point 
			//            -----
			// ^ Point 3: ---()  
		};
		std::tuple<NiAVObject*, NiMatrix3> CoordResult(Foot, RotMat);

		for (const auto& [foot, rotMat]: {CoordResult}) {
			if (!foot) {
				return footPoints;
			}
			for (NiPoint3 point: points) {
				footPoints.push_back(foot->world*(rotMat*point));
			}
		}
		return footPoints;
	}

	NiPoint3 GetHeartPosition(Actor* giant, Actor* tiny, bool hugs) { // It is used to spawn Heart Particles during healing hugs

		NiPoint3 TargetA = NiPoint3();
		NiPoint3 TargetB = NiPoint3();
		std::vector<std::string_view> bone_names = {
			"L Breast03",
			"R Breast03"
		};
		std::uint32_t bone_count = static_cast<uint32_t>(bone_names.size());
		for (auto &bone_name_A : bone_names) {
			auto bone = find_node(giant, bone_name_A);
			if (!bone) {
				Notify("Error: Breast Nodes could not be found.");
				Notify("Actor without nodes: {}", giant->GetDisplayFullName());
				Notify("Suggestion: install XP32 skeleton.");
				return NiPoint3();
			}
			TargetA += (bone->world.translate) * (1.0f/bone_count);
		}

		TargetB += tiny->GetPosition();

		auto targetPoint = TargetA;
		float adjustment = 45.0f * get_visual_scale(giant);

		if (hugs) {
			if (IsCrawling(giant)) { // if doing healing crawl hugs
				targetPoint = TargetA; // just target the breasts
			} else {
				adjustment = 85 * get_visual_scale(giant);
				targetPoint = (TargetA + TargetB) / 2; // else Breasts + TinyPos
			}
		}
		targetPoint.z += adjustment;
		return targetPoint;
	}

	void AbsorbShout_BuffCaster(Actor* giantref, Actor* tinyref) {
		static Timer MoanTimer = Timer(10.0);
		auto random = RandomInt(0, 8);
		if (random <= 4) {
			if (MoanTimer.ShouldRunFrame()) {
				ApplyShakeAtNode(giantref, 6.0f, "NPC COM [COM ]");
				ModSizeExperience(giantref, 0.14f);
				Sound_PlayMoans(giantref, 1.0f, 0.14f, EmotionTriggerSource::Absorption);

				Grow(giantref, 0, 0.016f * (1 + random));

				Runtime::CastSpell(giantref, giantref, Runtime::SPEL.GTSSpellFear);

				SpawnCustomParticle(giantref, ParticleType::Blue, NiPoint3(), "NPC COM [COM ]", get_visual_scale(giantref));
				Task_FacialEmotionTask_Moan(giantref, 2.0f, "Absorb");
			}	
		}
	}

	void Task_TrackSizeTask(Actor* giant, Actor* tiny, std::string_view naming, bool check_ticks, float time_mult) { 
		// A fail-safe task. The goal of it is to kill actor
		// if half-life puts actor below shrink to nothing threshold, so we won't have < x0.01 actors
		ActorHandle giantHandle = giant->CreateRefHandle();
		ActorHandle tinyHandle = tiny->CreateRefHandle();
		
		float task_duration = 3.0f;
		std::string name = std::format("{}_STN_Check_{}_{}", naming, giant->formID, tiny->formID);

		TaskManager::RunFor(name, task_duration, [=](auto& progressData) {
			if (!giantHandle) {
				return false;
			}
			if (!tinyHandle) {
				return false;
			}
			auto giantref = giantHandle.get().get();
			auto tinyref = tinyHandle.get().get();

			float size = get_visual_scale(tinyref);
			if (ShrinkToNothing(giantref, tinyref, check_ticks, time_mult)) {
				if (naming == "Absorb") {
					AbsorbShout_BuffCaster(giantref, tinyref);
				}
				return false;
			}

			return true;
		});
	}

	void Task_FacialEmotionTask_OpenMouth(Actor* giant, float duration, std::string_view naming, float duration_add, float speed_mult) {
		ActorHandle giantHandle = giant->CreateRefHandle();

		double start = Time::WorldTimeElapsed();
		std::string name = std::format("{}_Facial_{}", naming, giant->formID);

		float open_speed = duration/6.0f * speed_mult;

		AdjustFacialExpression(giant, 0, 1.0f, CharEmotionType::Phenome, open_speed, open_speed); // Start opening mouth
		AdjustFacialExpression(giant, 1, 0.5f, CharEmotionType::Phenome, open_speed, open_speed); // Open it wider

		AdjustFacialExpression(giant, 0, 0.80f, CharEmotionType::Modifier, open_speed, open_speed); // blink L
		AdjustFacialExpression(giant, 1, 0.80f, CharEmotionType::Modifier, open_speed, open_speed); // blink R

		EmotionManager::SetEmotionBusy(giant, CharEmotionType::Phenome, true);
		EmotionManager::SetEmotionBusy(giant, CharEmotionType::Modifier, true);

		TaskManager::Run(name, [=](auto& progressData) {
			if (!giantHandle) {
				return false;
			}
			double finish = Time::WorldTimeElapsed();
			auto giantref = giantHandle.get().get();

			float timepassed = static_cast<float>(finish - start) * AnimationManager::GetAnimSpeed(giantref);
			bool FullEmotion = EmotionManager::GetEmotionValue(giantref, CharEmotionType::Phenome, 0) >= 1.0f;

			bool ShouldRevert = FullEmotion && timepassed >= duration + duration_add;
			
			if (ShouldRevert) {
				float close_speed = duration / 6.0f;
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Phenome, false);
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Modifier, false);

				AdjustFacialExpression(giantref, 0, 0.0f, CharEmotionType::Phenome, close_speed, close_speed); // Start opening mouth
				AdjustFacialExpression(giantref, 1, 0.0f, CharEmotionType::Phenome, close_speed, close_speed); // Open it wider

				AdjustFacialExpression(giantref, 0, 0.0f, CharEmotionType::Modifier, close_speed, close_speed); // blink L
				AdjustFacialExpression(giantref, 1, 0.0f, CharEmotionType::Modifier, close_speed, close_speed); // blink R

				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Phenome, true);
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Modifier, true);
				return false;
			}
			return true;
		});
	}

	void Task_FacialEmotionTask_Moan(Actor* giant, float duration, std::string_view naming, float duration_add) {
		ActorHandle giantHandle = giant->CreateRefHandle();

		double start = Time::WorldTimeElapsed();
		std::string name = std::format("{}_Facial_{}", naming, giant->formID);

		AdjustFacialExpression(giant, 0, 1.0f, CharEmotionType::Modifier); // blink L
		AdjustFacialExpression(giant, 1, 1.0f, CharEmotionType::Modifier); // blink R
		AdjustFacialExpression(giant, 0, 1.0f, CharEmotionType::Phenome); // open mouth

		EmotionManager::SetEmotionBusy(giant, CharEmotionType::Phenome, true);
		EmotionManager::SetEmotionBusy(giant, CharEmotionType::Modifier, true);

		TaskManager::Run(name, [=](auto& progressData) {
			if (!giantHandle) {
				return false;
			}
			double finish = Time::WorldTimeElapsed();
			auto giantref = giantHandle.get().get();
			float timepassed = static_cast<float>(finish - start) * AnimationManager::GetAnimSpeed(giantref);

			bool ShouldRevert = timepassed >= duration + duration_add;

			if (ShouldRevert) {
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Phenome, false);
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Modifier, false);

				AdjustFacialExpression(giantref, 0, 0.0f, CharEmotionType::Modifier); // blink L
				AdjustFacialExpression(giantref, 1, 0.0f, CharEmotionType::Modifier); // blink R
				AdjustFacialExpression(giantref, 0, 0.0f, CharEmotionType::Phenome); // close mouth

				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Phenome, true);
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Modifier, true);
				return false;
			}
			return true;
		});
	}

	void Task_FacialEmotionTask_Smile(Actor* giant, float duration, std::string_view naming, float duration_add, float open_mouth) {
		ActorHandle giantHandle = giant->CreateRefHandle();

		double start = Time::WorldTimeElapsed();
		std::string name = std::format("{}_Facial_{}", naming, giant->formID);

		AdjustFacialExpression(giant, 0, 0.1f, CharEmotionType::Phenome); // Start opening mouth

		AdjustFacialExpression(giant, 0, 0.40f, CharEmotionType::Modifier); // blink L
		AdjustFacialExpression(giant, 1, 0.40f, CharEmotionType::Modifier); // blink R

		float random = open_mouth > 0 ? (random = open_mouth): (random = (RandomInt(5, 25)) * 0.01f);

		AdjustFacialExpression(giant, 3, random, CharEmotionType::Phenome); // Slightly open mouth
		AdjustFacialExpression(giant, 5, 0.5f, CharEmotionType::Phenome); // Actual smile but leads to opening mouth 
		AdjustFacialExpression(giant, 7, 1.0f, CharEmotionType::Phenome); // Close mouth stronger to counter opened mouth from smiling

		EmotionManager::SetEmotionBusy(giant, CharEmotionType::Phenome, true);
		EmotionManager::SetEmotionBusy(giant, CharEmotionType::Modifier, true);
		
		// Emotion guide:
		// https://steamcommunity.com/sharedfiles/filedetails/?id=187155077

		TaskManager::Run(name, [=](auto& progressData) {
			if (!giantHandle) {
				return false;
			}
			double finish = Time::WorldTimeElapsed();
			auto giantref = giantHandle.get().get();

			float timepassed = static_cast<float>(finish - start) * AnimationManager::GetAnimSpeed(giantref);
			bool ShouldRevert = timepassed >= duration + duration_add;
			
			if (ShouldRevert) {
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Phenome, false);
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Modifier, false);

				AdjustFacialExpression(giantref, 0, 0.0f, CharEmotionType::Phenome); // Start closing mouth

				AdjustFacialExpression(giantref, 0, 0.0f, CharEmotionType::Modifier); // blink L
				AdjustFacialExpression(giantref, 1, 0.0f, CharEmotionType::Modifier); // blink R

				AdjustFacialExpression(giantref, 3, 0.0f, CharEmotionType::Phenome); // Smile a bit (Mouth)
				AdjustFacialExpression(giantref, 5, 0.0f, CharEmotionType::Phenome); // Smile a bit (Mouth)
				AdjustFacialExpression(giantref, 7, 0.0f, CharEmotionType::Phenome); // Smile a bit (Mouth)

				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Phenome, true);
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Modifier, true);
				return false;
			}
			return true;
		});
	}
	

	void Task_FacialEmotionTask_SlightSmile(Actor* giant, float duration, std::string_view naming, float duration_add) {
		ActorHandle giantHandle = giant->CreateRefHandle();

		double start = Time::WorldTimeElapsed();
		std::string name = std::format("{}_Facial_{}", naming, giant->formID);

		AdjustFacialExpression(giant, 0, 0.30f, CharEmotionType::Modifier); // blink L
		AdjustFacialExpression(giant, 1, 0.30f, CharEmotionType::Modifier); // blink R

		AdjustFacialExpression(giant, 5, 0.5f, CharEmotionType::Phenome); 
		AdjustFacialExpression(giant, 6, 0.25f, CharEmotionType::Phenome); 

		EmotionManager::SetEmotionBusy(giant, CharEmotionType::Phenome, true);
		EmotionManager::SetEmotionBusy(giant, CharEmotionType::Modifier, true);
		

		// Emotion guide:
		// https://steamcommunity.com/sharedfiles/filedetails/?id=187155077

		TaskManager::Run(name, [=](auto& progressData) {
			if (!giantHandle) {
				return false;
			}
			double finish = Time::WorldTimeElapsed();
			auto giantref = giantHandle.get().get();

			float timepassed = static_cast<float>(finish - start) * AnimationManager::GetAnimSpeed(giantref);
			bool ShouldRevert = timepassed >= duration + duration_add;
			
			if (ShouldRevert) {
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Phenome, false);
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Modifier, false);

				AdjustFacialExpression(giantref, 0, 0.0f, CharEmotionType::Modifier);
				AdjustFacialExpression(giantref, 1, 0.0f, CharEmotionType::Modifier); 

				AdjustFacialExpression(giantref, 5, 0.0f, CharEmotionType::Phenome);
				AdjustFacialExpression(giantref, 6, 0.0f, CharEmotionType::Phenome);

				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Phenome, true);
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Modifier, true);
				return false;
			}
			return true;
		});
	}
	

	void Task_FacialEmotionTask_Anger(Actor* giant, float duration, std::string_view naming, float duration_add) {
		ActorHandle giantHandle = giant->CreateRefHandle();

		double start = Time::WorldTimeElapsed();
		std::string name = std::format("{}_Facial_{}", naming, giant->formID);

		float random = (RandomInt(70, 90)) * 0.01f;
        // Expression 0 70 or 90
		// Phoneme 2 75

		AdjustFacialExpression(giant, 0, random, CharEmotionType::Expression);
		AdjustFacialExpression(giant, 2, 0.75f, CharEmotionType::Phenome);

		EmotionManager::SetEmotionBusy(giant, CharEmotionType::Expression, true);
		EmotionManager::SetEmotionBusy(giant, CharEmotionType::Phenome, true);
		
		// Emotion guide:
		// https://steamcommunity.com/sharedfiles/filedetails/?id=187155077

		TaskManager::Run(name, [=](auto& progressData) {
			if (!giantHandle) {
				return false;
			}
			double finish = Time::WorldTimeElapsed();
			auto giantref = giantHandle.get().get();

			float timepassed = static_cast<float>(finish - start) * AnimationManager::GetAnimSpeed(giantref);
			bool ShouldRevert = timepassed >= duration + duration_add;
			
			if (ShouldRevert) {
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Expression, false);
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Phenome, false);

				AdjustFacialExpression(giantref, 0, 0.0f, CharEmotionType::Expression);
				AdjustFacialExpression(giantref, 2, 0.0f, CharEmotionType::Phenome); 
				
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Expression, true);
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Phenome, true);
				
				return false;
			}
			return true;
		});
	}

	void Task_FacialEmotionTask_Kiss(Actor* giant, float duration, std::string_view naming, float duration_add) {
		ActorHandle giantHandle = giant->CreateRefHandle();

		double start = Time::WorldTimeElapsed();
		std::string name = std::format("{}_Facial_{}", naming, giant->formID);

		AdjustFacialExpression(giant, 1, 1.0f, CharEmotionType::Expression); // Brows up, a bit gentle expression

		AdjustFacialExpression(giant, 0, 1.0f, CharEmotionType::Modifier); // blink L
		AdjustFacialExpression(giant, 1, 1.0f, CharEmotionType::Modifier); // blink R

		AdjustFacialExpression(giant, 3, 1.0f, CharEmotionType::Phenome); 
		AdjustFacialExpression(giant, 7, 1.0f, CharEmotionType::Phenome); 
		AdjustFacialExpression(giant, 12, 1.0f, CharEmotionType::Phenome); 
		// 3 = ~100
		// 7 = ~100
		// 12 = 100

		EmotionManager::SetEmotionBusy(giant, CharEmotionType::Phenome, true);
		EmotionManager::SetEmotionBusy(giant, CharEmotionType::Modifier, true);
		
		// Emotion guide:
		// https://steamcommunity.com/sharedfiles/filedetails/?id=187155077

		TaskManager::Run(name, [=](auto& progressData) {
			if (!giantHandle) {
				return false;
			}
			double finish = Time::WorldTimeElapsed();
			auto giantref = giantHandle.get().get();

			float timepassed = static_cast<float>(finish - start) * AnimationManager::GetAnimSpeed(giantref);
			bool ShouldRevert = timepassed >= duration + duration_add;
			
			if (ShouldRevert) {
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Phenome, false);
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Modifier, false);

				AdjustFacialExpression(giantref, 0, 0.0f, CharEmotionType::Modifier, 0.32f, 0.075f); // blink L
				AdjustFacialExpression(giantref, 1, 0.0f, CharEmotionType::Modifier, 0.32f, 0.075f); // blink R

				AdjustFacialExpression(giantref, 3, 0.0f, CharEmotionType::Phenome); 
				AdjustFacialExpression(giantref, 7, 0.0f, CharEmotionType::Phenome); 
				AdjustFacialExpression(giantref, 12, 0.0f, CharEmotionType::Phenome); 

				AdjustFacialExpression(giantref, 1, 0.0f, CharEmotionType::Expression); 

				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Phenome, true);
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Modifier, true);
				return false;
			}
			return true;
		});
	}

	void Laugh_Chance(Actor* giant, Actor* otherActor, float multiply, std::string_view name) {
		bool Blocked = IsActionOnCooldown(giant, CooldownSource::Emotion_Laugh);
		if (!Blocked) {
			int rng = RandomInt(0, 3);
			if (rng <= 1) {
				float duration = 1.5f + ((RandomInt(0, 100)) * 0.01f);
				duration *= multiply;

				ApplyActionCooldown(giant, CooldownSource::Emotion_Laugh);
				
				if (!otherActor->IsDead()) {
					Sound_PlayLaughs(giant, 1.0f, 0.14f, EmotionTriggerSource::Superiority);
					Task_FacialEmotionTask_Smile(giant, duration, name);
				}
			}
		}
	}

	void Laugh_Chance(Actor* giant, float multiply, std::string_view name) {
		bool Blocked = IsActionOnCooldown(giant, CooldownSource::Emotion_Laugh);
		if (!Blocked) {
			int rng = RandomInt(0, 3);
			if (rng <= 1) {
				float duration = 1.5f + ((RandomInt(1, 100)) * 0.01f);
				duration *= multiply;

				Sound_PlayLaughs(giant, 1.0f, 0.14f, EmotionTriggerSource::Superiority);
				Task_FacialEmotionTask_Smile(giant, duration, name);
				ApplyActionCooldown(giant, CooldownSource::Emotion_Laugh);
			}
		}
	}

	float GetHugStealRate(Actor* actor) {
		float steal = 0.18f;
		if (Runtime::HasPerkTeam(actor, Runtime::PERK.GTSPerkHugsToughGrip)) {
			steal += 0.072f;
		}
		if (Runtime::HasPerkTeam(actor, Runtime::PERK.GTSPerkHugs)) {
			steal *= 1.35f;
		}
		return steal;
	}

	float GetHugShrinkThreshold(Actor* actor) {
		float threshold = 2.5f;
		if (Runtime::HasPerkTeam(actor, Runtime::PERK.GTSPerkHugs)) {
			threshold *= 1.25f;
		}
		if (Runtime::HasPerkTeam(actor, Runtime::PERK.GTSPerkHugsGreed)) {
			threshold *= 1.35f;
		}
		if (HasGrowthSpurt(actor)) {
			threshold *= 2.0f;
		}
		return threshold;
	}

	float GetHugCrushThreshold(Actor* giant, Actor* tiny, bool check_size) {
		float hp = 0.12f;
		if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkHugMightyCuddles)) {
			hp += 0.08f;
		}
		if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkHugsOfDeath)) {
			hp += 0.10f;
		}

		if (!check_size) {
			return hp;
		}

		float difference = GetSizeDifference(giant, tiny, SizeType::GiantessScale, false, false);
		float clamped_diff = std::clamp(difference, 1.0f, 100.0f);

		return hp * clamped_diff;
	}


	void SetAltFootStompAnimation(RE::Actor* a_actor, const bool a_state) {


		if (!a_actor) {
			return;
		}

		bool PrevState = false;
		if (a_actor->GetGraphVariableBool("GTS_EnableAlternativeStomp", PrevState)) {

			if (PrevState == a_state) {
				return;
			}

			a_actor->SetGraphVariableBool("GTS_EnableAlternativeStomp", a_state);
		}
	}

	void SetEnableSneakTransition(RE::Actor* a_actor, const bool a_state) {

		if (!a_actor) {
			return;
		}

		bool PrevState = false;
		if (a_actor->GetGraphVariableBool("GTS_DisableSneakTrans", PrevState)) {

			if (PrevState == a_state) {
				return;
			}

			a_actor->SetGraphVariableBool("GTS_DisableSneakTrans", a_state);
		}
	}


	bool SetCrawlAnimation(Actor* a_actor, const bool a_state) {

		if (!a_actor) {
			return false;
		}

		bool PrevState = false;
		if (auto transient = Transient::GetActorData(a_actor)) {
			transient->FPCrawling = a_state;
		}

		if (a_actor->GetGraphVariableBool("GTS_CrawlEnabled", PrevState)) {

			if (PrevState == a_state) {
				return false;
			}

			a_actor->SetGraphVariableBool("GTS_CrawlEnabled", a_state);


			if (a_actor->IsSneaking() && !IsProning(a_actor) && !IsGtsBusy(a_actor) && !IsTransitioning(a_actor)) {
				return PrevState != a_state;
			}
			return false;
		}
		return false;
	}

	void UpdateCrawlAnimations(Actor* a_actor, bool a_state) {

		if (a_actor) {
			AnimationManager::StartAnim(a_state ? "CrawlON" : "CrawlOFF", a_actor);
		}
	}
}
