#include "Managers/Animation/FootTrample.hpp"
#include "Managers/Audio/Stomps.hpp"

#include "Stomp_Under.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Input/InputManager.hpp"
#include "Managers/Audio/Footstep.hpp"
#include "Managers/Rumble.hpp"

#include "Utils/InputConditions.hpp"

using namespace GTS;

namespace {

	constexpr std::string_view RNode = "NPC R Foot [Rft ]";
	constexpr std::string_view LNode = "NPC L Foot [Lft ]";

	void DeplenishStamina(Actor* giant, float WasteStamina) {
		DamageAV(giant, ActorValue::kStamina, WasteStamina * GetWasteMult(giant));
	}

	void DelayedLaunch(Actor* giant, float radius, float power, FootEvent Event) {
		std::string taskname = std::format("DelayLaunch_Trample_{}", giant->formID);
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
				LaunchTask(giant, radius, power, Event);
				return false;
			}

			return true;
		});
	}

	void FootTrample_Stage1(Actor* giant, bool right, FootEvent Event, DamageSource Source, std::string_view Node, std::string_view rumble) {
		float perk = GetPerkBonus_Basics(giant);
		float smt = 1.0f;
		float dust = 1.0f;
		
		if (HasSMT(giant)) {
			dust = 1.25f;
			smt = 1.5f;
		}

		double Start = Time::WorldTimeElapsed();
		ActorHandle giantHandle = giant->CreateRefHandle();
		std::string taskname = std::format("TrampleAttack_{}", giant->formID);

		TaskManager::RunFor(taskname, 1.0f, [=](auto& update){ // Needed because anim has a bit wrong timing(s)
			if (!giantHandle) {
				return false;
			}

			double Finish = Time::WorldTimeElapsed();
			auto GiantRef = giantHandle.get().get();

			if (Finish - Start > 0.06) { 
				DoDamageEffect(GiantRef, Damage_Trample * perk, Radius_Trample, 100, 0.10f, Event, 1.10f, Source);
				DrainStamina(GiantRef, "StaminaDrain_Trample", "GTSPerkDestructionBasics", true, 0.6f); // start stamina drain

				float shake_power = Rumble_Trample_Stage1 * smt * GetHighHeelsBonusDamage(GiantRef, true);
				
				Rumbling::Once(rumble, GiantRef, shake_power, 0.0f, Node, 0.0f);
				
				DoDustExplosion(GiantRef, dust * smt, Event, Node);
				StompManager::PlayNewOrOldStomps(GiantRef, 1.0f, Event, Node, false);

				FootGrindCheck(GiantRef, Radius_Trample, right, FootActionType::Trample_NormalOrUnder);
				DelayedLaunch(GiantRef, 0.65f * perk, 3.0f * perk, Event);

				FootStepManager::PlayVanillaFootstepSounds(GiantRef, right);

				return false;
			}
			return true;
		});
	}

	void FootTrample_Stage2(Actor* giant, bool right, FootEvent Event, DamageSource Source, std::string_view Node, std::string_view rumble) {
		float perk = GetPerkBonus_Basics(giant);
		float dust = 1.15f;
		float smt = 1.0f;
		
		if (HasSMT(giant)) {
			smt = 1.5f;
			dust *= 1.25f;
		}

		float shake_power = Rumble_Trample_Stage2 * smt * GetHighHeelsBonusDamage(giant, true);

		Rumbling::Once(rumble, giant, shake_power, 0.0f, Node, 1.1f);
		DoDamageEffect(giant, Damage_Trample_Repeat * perk, Radius_Trample_Repeat, 1, 0.12f, Event, 1.10f, Source);
		StompManager::PlayNewOrOldStomps(giant, 1.0f, Event, Node, false);
		DoDustExplosion(giant, dust * smt, Event, Node);
		DoLaunch(giant, 0.85f * perk, 3.8f * perk, Event);
		DeplenishStamina(giant, 30.0f);

		FootStepManager::PlayVanillaFootstepSounds(giant, right);
	}

	void FootTrample_Stage3(Actor* giant, bool right, FootEvent Event, DamageSource Source, std::string_view Node, std::string_view rumble) {
		float perk = GetPerkBonus_Basics(giant);
		float dust = 1.65f;
		float smt = 1.0f;
		
		if (HasSMT(giant)) {
			smt = 1.5f;
			dust *= 1.25f;
		}

		float shake_power = Rumble_Trample_Stage3 * smt * GetHighHeelsBonusDamage(giant, true);

		Rumbling::Once(rumble, giant, shake_power, 0.0f, Node, 1.2f);
		DoDamageEffect(giant, Damage_Trample_Finisher * perk, Radius_Trample_Finisher, 1, 0.25f, Event, 0.85f, Source);
		DoLaunch(giant, 1.25f * perk, 5.0f * perk, Event);
		StompManager::PlayNewOrOldStomps(giant, 1.15f, Event, Node, true);
		DoDustExplosion(giant, dust * smt, Event, Node);

		DeplenishStamina(giant, 100.0f);

		FootStepManager::DoStrongSounds(giant, 1.25f, Node);
		FootStepManager::PlayVanillaFootstepSounds(giant, right);
	}

	/////////////////////////////////////////////////////////
	// EVENTS
	////////////////////////////////////////////////////////

	void GTS_Trample_Leg_Raise_L(AnimationEventData& data) {
		data.stage = 1;
		data.canEditAnimSpeed = false;
		if (data.animSpeed == 1.0f) {
			data.animSpeed = 1.3f;
		}
		SetBusyFoot(&data.giant, BusyFoot::LeftFoot);
	}
	void GTS_Trample_Leg_Raise_R(AnimationEventData& data) {
		data.stage = 1;
		data.canEditAnimSpeed = false;
		if (data.animSpeed == 1.0f) {
			data.animSpeed = 1.3f;
		}
		SetBusyFoot(&data.giant, BusyFoot::RightFoot);
	}

	void GTS_Trample_Cam_Start_L(AnimationEventData& data) {
		ManageCamera(&data.giant, true, CameraTracking::L_Foot);
	}
	void GTS_Trample_Cam_Start_R(AnimationEventData& data) {
		ManageCamera(&data.giant, true, CameraTracking::R_Foot);
	}

	void GTS_Trample_Cam_End_L(AnimationEventData& data) {
		ManageCamera(&data.giant, false, CameraTracking::L_Foot);
		DrainStamina(&data.giant, "StaminaDrain_Trample", "GTSPerkDestructionBasics", false, 0.6f);

		data.animSpeed = 1.0f;
		data.canEditAnimSpeed = false;
		data.stage = 0;
	}
	void GTS_Trample_Cam_End_R(AnimationEventData& data) {
		ManageCamera(&data.giant, false, CameraTracking::R_Foot);
		DrainStamina(&data.giant, "StaminaDrain_Trample", "GTSPerkDestructionBasics", false, 0.6f);

		data.animSpeed = 1.0f;
		data.canEditAnimSpeed = false;
		data.stage = 0;
	}

////////////////////////////////////////////////////////////D A M A G E

	void GTS_Trample_Footstep_L(AnimationEventData& data) { // Stage 1 footsteps
		FootTrample_Stage1(&data.giant, false, FootEvent::Left, DamageSource::CrushedLeft, LNode, "TrampleL");
		SetBusyFoot(&data.giant, BusyFoot::None);
		data.animSpeed = 1.0f;
		data.canEditAnimSpeed = false;
		data.stage = 0;
	}
	void GTS_Trample_Footstep_R(AnimationEventData& data) { // stage 1 footsteps
		FootTrample_Stage1(&data.giant, true, FootEvent::Right, DamageSource::CrushedRight, RNode, "TrampleR");
		SetBusyFoot(&data.giant, BusyFoot::None);

		data.animSpeed = 1.0f;
		data.canEditAnimSpeed = false;
		data.stage = 0;
	}

	void GTS_Trample_Impact_L(AnimationEventData& data) { // Stage 2 repeating footsteps
		FootTrample_Stage2(&data.giant, false, FootEvent::Left, DamageSource::CrushedLeft, LNode, "Trample2_L");

		data.stage = 1;
		data.canEditAnimSpeed = false;
		if (data.animSpeed == 1.0f) {
			data.animSpeed = 1.15f;
		}
	}

	void GTS_Trample_Impact_R(AnimationEventData& data) { // Stage 2 repeating footsteps
		FootTrample_Stage2(&data.giant, true, FootEvent::Right, DamageSource::CrushedRight, RNode, "Trample2_R");

		data.stage = 1;
		data.canEditAnimSpeed = false;
		if (data.animSpeed == 1.00f) {
			data.animSpeed = 1.15f;
		}
	}

	void GTS_Trample_Finisher_L(AnimationEventData& data) { // last hit that deals huge chunk of damage
		//Rumbling::Stop("Trample2_L", &data.giant);
		FootTrample_Stage3(&data.giant, false, FootEvent::Left, DamageSource::CrushedLeft, LNode, "Trample2_L");
	}
	void GTS_Trample_Finisher_R(AnimationEventData& data) { // last hit that deals huge chunk of damage
		//Rumbling::Stop("Trample2_R", &data.giant);
		FootTrample_Stage3(&data.giant, true, FootEvent::Right, DamageSource::CrushedRight, RNode, "Trample2_R");
	}

	/////////////////////////////////////////////////////////// Triggers

	void TrampleLeftEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		float WasteStamina = 35.0f * GetWasteMult(player);

		bool UnderTrample = AnimationUnderStomp::ShouldStompUnder(player);
		const std::string_view TrampleType_L = UnderTrample ? "UnderTrampleL" : "TrampleL";

		if (GetAV(player, ActorValue::kStamina) > WasteStamina) {
			AnimationManager::StartAnim(TrampleType_L, player);
		} else {
			NotifyWithSound(player, "You're too tired to perform trample");
		}
	}

	void TrampleRightEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		float WasteStamina = 35.0f * GetWasteMult(player);

		bool UnderTrample = AnimationUnderStomp::ShouldStompUnder(player);
		const std::string_view TrampleType_R = UnderTrample ? "UnderTrampleR" : "TrampleR";

		if (GetAV(player, ActorValue::kStamina) > WasteStamina) {
			AnimationManager::StartAnim(TrampleType_R, player);
		} else {
			NotifyWithSound(player, "You're too tired to perform trample");
		}
	}
}

namespace GTS
{
	void AnimationFootTrample::RegisterEvents() {
		InputManager::RegisterInputEvent("TrampleLeft", TrampleLeftEvent, TrampleCondition);
		InputManager::RegisterInputEvent("TrampleRight", TrampleRightEvent, TrampleCondition);

		AnimationManager::RegisterEvent("GTS_Trample_Leg_Raise_L", "Trample", GTS_Trample_Leg_Raise_L);
		AnimationManager::RegisterEvent("GTS_Trample_Leg_Raise_R", "Trample", GTS_Trample_Leg_Raise_R);

		AnimationManager::RegisterEvent("GTS_Trample_Cam_Start_L", "Trample", GTS_Trample_Cam_Start_L);
		AnimationManager::RegisterEvent("GTS_Trample_Cam_Start_R", "Trample", GTS_Trample_Cam_Start_R);

		AnimationManager::RegisterEvent("GTS_Trample_Cam_End_L", "Trample", GTS_Trample_Cam_End_L);
		AnimationManager::RegisterEvent("GTS_Trample_Cam_End_R", "Trample", GTS_Trample_Cam_End_R);

		AnimationManager::RegisterEvent("GTS_Trample_Impact_L", "Trample", GTS_Trample_Impact_L);
		AnimationManager::RegisterEvent("GTS_Trample_Impact_R", "Trample", GTS_Trample_Impact_R);

		AnimationManager::RegisterEvent("GTS_Trample_Footstep_L", "Trample", GTS_Trample_Footstep_L);
		AnimationManager::RegisterEvent("GTS_Trample_Footstep_R", "Trample", GTS_Trample_Footstep_R);

		AnimationManager::RegisterEvent("GTS_Trample_Finisher_L", "Trample", GTS_Trample_Finisher_L);
		AnimationManager::RegisterEvent("GTS_Trample_Finisher_R", "Trample", GTS_Trample_Finisher_R);
	}

	void AnimationFootTrample::RegisterTriggers() {
		AnimationManager::RegisterTrigger("TrampleL", "Trample", "GTSBeh_Trample_L");
		AnimationManager::RegisterTrigger("TrampleR", "Trample", "GTSBeh_Trample_R");

		AnimationManager::RegisterTrigger("UnderTrampleL", "Trample", "GTSBeh_UnderTrample_StartL");
		AnimationManager::RegisterTrigger("UnderTrampleR", "Trample", "GTSBeh_UnderTrample_StartR");

		AnimationManager::RegisterTrigger("TrampleStartL", "Trample", "GTSBEH_Trample_Start_L");
		AnimationManager::RegisterTrigger("TrampleStartR", "Trample", "GTSBEH_Trample_Start_R");
	}
}