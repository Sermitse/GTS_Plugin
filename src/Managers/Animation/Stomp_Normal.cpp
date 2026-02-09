#include "Managers/Animation/Stomp_Normal.hpp"
#include "Managers/Animation/Stomp_Under.hpp"
#include "Managers/Animation/AnimationManager.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Audio/Footstep.hpp"
#include "Managers/Audio/Stomps.hpp"
#include "Managers/Input/InputManager.hpp"
#include "Managers/Rumble.hpp"

#include "Utils/Actions/InputConditions.hpp"
#include "Utils/AttachPoint.hpp"

using namespace GTS;

namespace {

	constexpr std::string_view RNode = "NPC R Foot [Rft ]";
	constexpr std::string_view LNode = "NPC L Foot [Lft ]";
	std::random_device rd;
	std::mt19937 e2(rd());

	void DoStompOrUnderStomp(Actor* giant, const std::string_view name) {
		float WasteStamina = 25.0f;
		if (Runtime::HasPerk(giant, Runtime::PERK.GTSPerkDestructionBasics)) {
			WasteStamina *= 0.65f;
		}
		if (GetAV(giant, ActorValue::kStamina) > WasteStamina) {
			AnimationManager::StartAnim(name, giant);
		} else {
			NotifyWithSound(giant, "You're too tired to perform stomp");
		}
	}

	std::vector<Actor*> FindSquished(Actor* giant) {
		/*
		Find actor that are being pressed underfoot
		*/
		std::vector<Actor*> result = {};
		if (!giant) {
			return result;
		}
		float giantScale = get_visual_scale(giant);
		auto giantLoc = giant->GetPosition();
		for (auto tiny: find_actors()) {
			if (tiny) {
				float tinyScale = get_visual_scale(tiny);
				float scaleRatio = giantScale / tinyScale;
				
				float actorRadius = 35.0f;
				auto bounds = get_bound_values(tiny);
				actorRadius = (bounds.x + bounds.y + bounds.z) / 6.0f;
				

				actorRadius *= tinyScale;
				
				if (scaleRatio > 3.5f) {
					// 3.5 times bigger
					auto tinyLoc = tiny->GetPosition();
					auto distance = (giantLoc - tinyLoc).Length() - actorRadius;
					if (distance < giantScale * 15.0f) {
						// About 1.5 the foot size
						result.push_back(tiny);
					}
				}
			}
		}
		return result;
	}

	void StopLoopRumble(Actor* giant) {
		Rumbling::Stop("StompR_Loop", giant);
		Rumbling::Stop("StompL_Loop", giant);
	}

	void Stomp_ResetAnimSpeed(AnimationEventData& data) {
		data.stage = 0;
		data.canEditAnimSpeed = false;
		data.animSpeed = 1.0f;
	}

	void Stomp_IncreaseAnimSpeed(AnimationEventData& data) {
		data.stage = 1;
		data.canEditAnimSpeed = true;
		data.animSpeed = 1.35f;
		if (!data.giant.IsPlayerRef()) {
			data.animSpeed = 1.35f + GetRandomBoost()/2;
		}
	}

	void MoveUnderFoot(Actor* giant, std::string_view node) {
		auto footNode = find_node(giant, RNode);
		if (footNode) {
			auto footPos = footNode->world.translate;
			for (auto tiny: FindSquished(giant)) {
				if (tiny) {
					std::uniform_real_distribution<float> dist(-10.f, 10.f);
					float dx = dist(e2);
					float dy = dist(e2);
					auto randomOffset = NiPoint3(dx, dy, 0.0f);
					tiny->SetPosition(footPos + randomOffset, true);
				}
			}
		}
	}

	/*
	Will keep the tiny in place for a second
	*/
	void KeepInPlace(Actor* giant, float duration) {
		for (auto tiny: FindSquished(giant)) {
			if (tiny) {
				auto giantRef = giant->CreateRefHandle();
				auto tinyRef = tiny->CreateRefHandle();
				auto currentPos = tiny->GetPosition();
				TaskManager::RunFor(duration, [=](const auto& data){
					if (!tinyRef) {
						return false;	
					}
					if (!giantRef) {
						return false;	
					}
					AttachTo(giantRef, tinyRef, currentPos);
					return true;
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

	void Stomp_Footsteps_DoEverything(Actor* giant, bool right, float animSpeed, FootEvent Event, DamageSource Source, std::string_view Node, std::string_view rumble) {
		float perk = GetPerkBonus_Basics(giant);
		float smt = 1.0f;
		float dust = 1.25f;
		if (TinyCalamityActive(giant)) {
			smt = 1.5f;
			dust = 1.45f;
		}

		// TO ANDY: i commented it out for tests
		//MoveUnderFoot(giant, Node); 
		float hh = GetHighHeelsBonusDamage(giant, true);
		float shake_power = Rumble_Stomp_Normal * smt * hh;

		std::string taskname = std::format("StompAttack_{}", giant->formID);
		ActorHandle giantHandle = giant->CreateRefHandle();

		double Start = Time::WorldTimeElapsed();
		
		TaskManager::RunFor(taskname, 1.0f, [=](auto& update){ // Needed because anim has a wrong timing
			if (!giantHandle) {
				return false;
			}

			double Finish = Time::WorldTimeElapsed();
			auto giantref = giantHandle.get().get();
		
			if (Finish - Start > 0.02) { 

				Rumbling::Once(rumble, giant, shake_power, 0.0f, Node, 1.10f);
				DoDamageEffect(giantref, Damage_Stomp * perk, Radius_Stomp, 10, 0.25f, Event, 1.0f, Source);
				DoDustExplosion(giantref, dust + (animSpeed * 0.05f), Event, Node);
				StompManager::PlayNewOrOldStomps(giantref, 1.0f, Event, Node, false);
				
				DrainStamina(giantref, "StaminaDrain_Stomp", Runtime::PERK.GTSPerkDestructionBasics, false, 1.8f); // cancel stamina drain

				FootGrindCheck(giantref, Radius_Stomp, right, FootActionType::Grind_Normal);

				DelayedLaunch(giantref, 0.80f * perk, 2.0f* animSpeed, Event);

				FootStepManager::PlayVanillaFootstepSounds(giantref, right);

				SetBusyFoot(giantref, BusyFoot::None);

				return false;
			}
			return true;
		});
	}

	void Stomp_Land_DoEverything(Actor* giant, float animSpeed, bool right, FootEvent Event, DamageSource Source, std::string_view Node, std::string_view rumble) {
		float perk = GetPerkBonus_Basics(giant);
		float smt = 1.0f;
		float dust = 0.85f;
		
		if (TinyCalamityActive(giant)) {
			dust = 1.35f;
			smt = 1.5f;
		}
		float hh = GetHighHeelsBonusDamage(giant, true);
		float shake_power = Rumble_Stomp_Land_Normal * smt * hh;

		std::string taskname = std::format("StompLand_{}_{}", giant->formID, Time::WorldTimeElapsed());
		ActorHandle giantHandle = giant->CreateRefHandle();

		double Start = Time::WorldTimeElapsed();
		
		TaskManager::RunFor(taskname, 1.0f, [=](auto& update){ // Needed because anim has a bit too early timings
			if (!giantHandle) {
				return false;
			}

			double Finish = Time::WorldTimeElapsed();

			if (Finish - Start > 0.075) { 
				auto giant = giantHandle.get().get();

				Rumbling::Once(rumble, giant, shake_power, 0.0f, Node, 0.0f);
				DoDamageEffect(giant, Damage_Stomp * perk, Radius_Stomp, 25, 0.25f, Event, 1.0f, DamageSource::CrushedRight);
				DoDustExplosion(giant, dust + (animSpeed * 0.05f), Event, Node);
				StompManager::PlayNewOrOldStomps(giant, 1.0f + animSpeed/14, Event, RNode, false);

				LaunchTask(giant, 0.90f * perk, 3.2f + animSpeed/2, Event);

				FootStepManager::PlayVanillaFootstepSounds(giant, right);

				return false;
			}
			return true;
		});
		
		//KeepInPlace(giant, 1.5);
	}

///////////////////////////////////////////////////////////////////////////////////////////////////// Events

	void GTSstompstartR(AnimationEventData& data) {
		DrainStamina(&data.giant, "StaminaDrain_Stomp", Runtime::PERK.GTSPerkDestructionBasics, true, 1.8f);
		Rumbling::Start("StompR_Loop", &data.giant, 0.25f, 0.15f, RNode);
		ManageCamera(&data.giant, true, CameraTracking::R_Foot);
		Stomp_IncreaseAnimSpeed(data);

		SetBusyFoot(&data.giant, BusyFoot::RightFoot);
	}

	void GTSstompstartL(AnimationEventData& data) {
		
		DrainStamina(&data.giant, "StaminaDrain_Stomp", Runtime::PERK.GTSPerkDestructionBasics, true, 1.8f);
		Rumbling::Start("StompL_Loop", &data.giant, 0.25f, 0.15f, LNode);
		ManageCamera(&data.giant, true, CameraTracking::L_Foot);
		Stomp_IncreaseAnimSpeed(data);

		SetBusyFoot(&data.giant, BusyFoot::LeftFoot);
	}

	void GTSstompimpactR(AnimationEventData& data) {
		Stomp_Footsteps_DoEverything(&data.giant, true, data.animSpeed, FootEvent::Right, DamageSource::CrushedRight, RNode, "StompR");
		StopLoopRumble(&data.giant);
	}

	void GTSstompimpactL(AnimationEventData& data) {
		Stomp_Footsteps_DoEverything(&data.giant, false, data.animSpeed, FootEvent::Left, DamageSource::CrushedLeft, LNode, "StompL");
		StopLoopRumble(&data.giant);
	}

	void GTSstomplandR(AnimationEventData& data) {
		//Rumbling::Stop("StompLandL", &data.giant);
		Stomp_Land_DoEverything(&data.giant, data.animSpeed, true, FootEvent::Right, DamageSource::CrushedRight, RNode, "StompLand");
		StopLoopRumble(&data.giant);
	}

	void GTSstomplandL(AnimationEventData& data) {
		//Rumbling::Stop("StompLandR", &data.giant);
		Stomp_Land_DoEverything(&data.giant, data.animSpeed, false, FootEvent::Left, DamageSource::CrushedLeft, LNode, "StompLand");
		StopLoopRumble(&data.giant);
	}

	void GTSStompendR(AnimationEventData& data) {
		Stomp_ResetAnimSpeed(data);
	}

	void GTSStompendL(AnimationEventData& data) {
		Stomp_ResetAnimSpeed(data);
	}

	void GTS_Next(AnimationEventData& data) {
		StopLoopRumble(&data.giant);
	}

	void GTSBEH_Exit(AnimationEventData& data) {
		DrainStamina(&data.giant, "StaminaDrain_Stomp", Runtime::PERK.GTSPerkDestructionBasics, false, 1.8f);
		DrainStamina(&data.giant, "StaminaDrain_StrongStomp", Runtime::PERK.GTSPerkDestructionBasics, false, 2.8f);
		ManageCamera(&data.giant, false, CameraTracking::L_Foot);
		ManageCamera(&data.giant, false, CameraTracking::R_Foot);
		StopLoopRumble(&data.giant);
	}

	void RightStompEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		bool UnderStomp = AnimationUnderStomp::ShouldStompUnder(player);
		const std::string_view StompType = UnderStomp ? "UnderStompRight" : "StompRight";
		DoStompOrUnderStomp(player, StompType);
	}

	void LeftStompEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		bool UnderStomp = AnimationUnderStomp::ShouldStompUnder(player);
		const std::string_view StompType = UnderStomp ? "UnderStompLeft" : "StompLeft";
		DoStompOrUnderStomp(player, StompType);
	}
}

namespace GTS
{
	void AnimationStomp::RegisterEvents() {
		AnimationManager::RegisterEvent("GTSstompimpactR", "Stomp", GTSstompimpactR);
		AnimationManager::RegisterEvent("GTSstompimpactL", "Stomp", GTSstompimpactL);
		AnimationManager::RegisterEvent("GTSstomplandR", "Stomp", GTSstomplandR);
		AnimationManager::RegisterEvent("GTSstomplandL", "Stomp", GTSstomplandL);
		AnimationManager::RegisterEvent("GTSstompstartR", "Stomp", GTSstompstartR);
		AnimationManager::RegisterEvent("GTSstompstartL", "Stomp", GTSstompstartL);
		AnimationManager::RegisterEvent("GTSStompendR", "Stomp", GTSStompendR);
		AnimationManager::RegisterEvent("GTSStompendL", "Stomp", GTSStompendL);
		AnimationManager::RegisterEvent("GTS_Next", "Stomp", GTS_Next);
		AnimationManager::RegisterEvent("GTSBEH_Exit", "Stomp", GTSBEH_Exit);

		InputManager::RegisterInputEvent("RightStomp", RightStompEvent, StompCondition);
		InputManager::RegisterInputEvent("LeftStomp", LeftStompEvent, StompCondition);
	}

	void AnimationStomp::RegisterTriggers() {
		AnimationManager::RegisterTrigger("StompRight", "Stomp", "GtsModStompAnimRight");
		AnimationManager::RegisterTrigger("StompLeft", "Stomp", "GtsModStompAnimLeft");
	}
}