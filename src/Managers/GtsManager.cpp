#include "Managers/GtsManager.hpp"

#include "Managers/Animation/Utils/CrawlUtils.hpp"
#include "Managers/Gamemode/GameModeManager.hpp"
#include "Managers/Damage/CollisionDamage.hpp"
#include "Managers/Damage/TinyCalamity.hpp"
#include "Managers/Audio/PitchShifter.hpp"
#include "Managers/RipClothManager.hpp"
#include "Managers/MaxSizeManager.hpp"
#include "Managers/Animation/Grab.hpp"

#include "Magic/Effects/Common.hpp"
#include "Utils/DynamicScale.hpp"
#include "AI/AIFunctions.hpp"
#include "Utils/MovementForce.hpp"
#include "Config/Config.hpp"

#include "Hooks/Engine/Settings.hpp"
#include "UI/DebugAPI.hpp"

using namespace GTS;

namespace {

	const auto& GameplaySettings = Config::GetGameplay();

	constexpr float ini_adjustment = 65535.f; //High Value
	constexpr float vanilla_interaction_range = 180.0f;
	constexpr float vanilla_radius_range = 16.0f;

	void FixEmotionsRange() {

		// Makes facial emotions always enabled at any size
		*Hooks::LOD::fTalkingDistance = ini_adjustment;
		*Hooks::LOD::fLodDistance = ini_adjustment;
	}

	void UpdateInterractionDistance() {
		if (Config::GetGeneral().bOverrideInteractionDist) {
			float player_scale = std::clamp(get_visual_scale(PlayerCharacter::GetSingleton()), 1.0f, 999999.0f);
			float new_dist_value = vanilla_interaction_range * player_scale;
			float new_radius_value = vanilla_radius_range * player_scale;

			*Hooks::Distance::fActivatePickRadius = new_radius_value;
			*Hooks::Distance::fActivatePickLength = new_dist_value;
		}
	}

	void UpdateCameraINIs() {

		auto& CamSettings = Config::GetCamera();

		if (!CamSettings.bEnableSkyrimCameraAdjustments) return;

		*Hooks::Camera::fVanityModeMinDist = CamSettings.fCameraDistMin;
		*Hooks::Camera::fVanityModeMaxDist = CamSettings.fCameraDistMax;
		*Hooks::Camera::fMouseWheelZoomIncrement = CamSettings.fCameraIncrement;
		*Hooks::Camera::fMouseWheelZoomSpeed = CamSettings.fCameraZoomSpeed;

	}

	void Foot_PerformIdleEffects_Main(Actor* actor) {
		if (actor) {
			if (GetBusyFoot(actor) != BusyFoot::RightFoot) { // These are needed to get rid of annoying pushing away during stomps, Does Right Leg Idle Damage Over Time
				float FootDamage = std::clamp(Get_Bone_Movement_Speed(actor, NodeMovementType::Movement_RightLeg), 0.0f, 1.0f);
				CollisionDamage::DoFootCollision(actor, Damage_Default_Underfoot * FootDamage * TimeScale(), Radius_Default_Idle, 0, 0.0f, Minimum_Actor_Crush_Scale_Idle, DamageSource::FootIdleR, true, false, false, false);
			} 
			if (GetBusyFoot(actor) != BusyFoot::LeftFoot) { // Does Left Leg Idle Damage Over Time
				float FootDamage = std::clamp(Get_Bone_Movement_Speed(actor, NodeMovementType::Movement_LeftLeg), 0.0f, 1.0f);
				CollisionDamage::DoFootCollision(actor, Damage_Default_Underfoot * FootDamage * TimeScale(), Radius_Default_Idle, 0, 0.0f, Minimum_Actor_Crush_Scale_Idle, DamageSource::FootIdleL, false, false, false, false);
			}
		}
	}

	void Foot_PerformIdle_Headtracking_Effects_Others(Actor* actor) {
		if (actor && Config::GetGeneral().bAllActorSizeEffects) {
			if (actor->formID != 0x14 && !IsTeammate(actor)) {
				if (GetBusyFoot(actor) != BusyFoot::RightFoot) {
					CollisionDamage::DoFootCollision(actor, Damage_Default_Underfoot * TimeScale(), Radius_Default_Idle, 0, 0.0f, Minimum_Actor_Crush_Scale_Idle, DamageSource::FootIdleR, true, false, false, false);
				}
				if (GetBusyFoot(actor) != BusyFoot::LeftFoot) {
					CollisionDamage::DoFootCollision(actor, Damage_Default_Underfoot * TimeScale(), Radius_Default_Idle, 0, 0.0f, Minimum_Actor_Crush_Scale_Idle, DamageSource::FootIdleL, false, false, false, false);
				}
			}
		}
	}

	void ManageActorControl() { // Rough control other fix

		GTS_PROFILE_SCOPE("GTSManager: ManageActorControl");

		Actor* target = GetPlayerOrControlled();
		if (target->formID != 0x14) {
			auto grabbed = Grab::GetHeldActor(target);
			if (grabbed && grabbed->formID == 0x14) {
				return;
			}
			if (!IsGtsBusy(target) && !IsBeingHeld(target, PlayerCharacter::GetSingleton())) {
				ControlAnother(target, true);
			}
		}
	}

	void UpdateFalling() {
		Actor* player = PlayerCharacter::GetSingleton(); 
		if (player && player->IsInMidair()) {
			auto transient = Transient::GetSingleton().GetData(player);
			if (Runtime::HasPerkTeam(player, "GTSPerkCruelFall")) {
				auto charCont = player->GetCharController();
				if (charCont) {
					if (transient) {
						float scale = std::clamp(get_visual_scale(player), 0.06f, 2.0f);
						float CalcFall = 1.0f + (charCont->fallTime * (4.0f / scale) - 4.0f);
						float FallTime = std::clamp(CalcFall, 1.0f, 3.0f);
						transient->FallTimer = FallTime;
					}
				}
			} else {
				transient->FallTimer = 1.0f;
			}
		}
	}

	void FixActorFade() {
		GTS_PROFILE_SCOPE("GTSManager: FixActorFade");
		// No fix: 
		// -Followers fade away at ~x1000 scale, may even fade earlier than that
		// -Proteus Player gets disabled at ~x2200 scale

		// With fix:
		// -Followers render even at x26000 scale
		// -Proteus Player is also rendered even at x26000 scale
		// -At this point only draw distance limit of the game hides the characters at such gigantic scales

		static Timer ApplyTimer = Timer(1.00);
		bool reset = false;

		if (ApplyTimer.ShouldRunFrame()) {
			for (auto actor: find_actors()) {
				if (actor) {
					for (auto fp : {true, false}) {
						auto model = actor->Get3D(fp);
						if (model) {
							get_visual_scale(actor) < 1.5f ? reset = true : reset = false; 

							if (!reset || IsGtsBusy(actor) || actor->formID == 0x14) {
								model->GetFlags().set(RE::NiAVObject::Flag::kIgnoreFade);
							} else {
								model->GetFlags().reset(RE::NiAVObject::Flag::kIgnoreFade);
							}
						}
					}
				}
			}
		}
	}

	void PerformRoofRaycastAdjustments(Actor* actor, float& target_scale, float currentOtherScale) {

		const auto& Settings = Config::GetGeneral();
		const bool DoRayCast = (actor->formID == 0x14) ? Settings.bDynamicSizePlayer : Settings.bDynamicSizeFollowers;
		/*auto data = Transient::GetSingleton().GetActorData(actor);
		if (data->UsingFurniture) { 
			target_scale = data->FurnitureScale;
			return;
		}*/ 
		// ^ To Arial: Uncommend it when you'll be experimenting with furniture, this part scales actors down

		if (DoRayCast && !actor->IsDead() && target_scale > 1.025f) {

			float room_scale = GetMaxRoomScale(actor);
			if (room_scale > (currentOtherScale - 0.05f)) {
				// Only apply room scale if room_scale > natural_scale
				//   This stops it from working when room_scale < 1.0
				target_scale = min(target_scale, room_scale);
			}
			else {
				// Else we just scale to natural
				target_scale = 1.0f;
			}
		}
	}

	void update_height(Actor* actor, ActorData* persi_actor_data, TempActorData* trans_actor_data) {
		GTS_PROFILE_SCOPE("GTSManager: UpdateHeight");

		if (!actor) {
			return;
		}

		if (!trans_actor_data) {
			//log::info("!Upate_height: Trans Data not found for {}", actor->GetDisplayFullName());
			return;
		}

		if (!persi_actor_data) {
			//log::info("!Upate_height: Pers Data not found for {}", actor->GetDisplayFullName());
			return;
		}

		const float currentOtherScale = Get_Other_Scale(actor);
		trans_actor_data->OtherScales = currentOtherScale;

		const float natural_scale = get_natural_scale(actor, false);
		float target_scale = persi_actor_data->target_scale;
		const float max_scale = persi_actor_data->max_scale / natural_scale;

		

		float ScaleMult = 1.0f;
		VisualScale_CheckForSizeAdjustment(actor, ScaleMult); // Updates ScaleMult value based on Actor Type (Player/Follower/Others)
	
		// Smooth target_scale towards max_scale if target_scale > max_scale
		if (target_scale > max_scale && get_target_scale(actor) > get_natural_scale(actor, true) * ScaleMult) {
			constexpr float minimum_scale_delta = 0.000005f;

			if (fabs(target_scale - max_scale) < minimum_scale_delta) {
				float target = max_scale;
				persi_actor_data->target_scale = target;
				persi_actor_data->target_scale_v = 0.0f;
			} else {
				critically_damped(
					persi_actor_data->target_scale,
					persi_actor_data->target_scale_v,
					max_scale,
					persi_actor_data->half_life*1.5f,
					Time::WorldTimeDelta()
				);
			}
		}
		else {
			persi_actor_data->target_scale_v = 0.0f;
		}

		// Room Size adjustments
		// We only do this if they are bigger than 1.05x their natural scale (currentOtherScale)
		// and if enabled in the mcm
		PerformRoofRaycastAdjustments(actor, target_scale, currentOtherScale);
		
		if (fabs(target_scale - persi_actor_data->visual_scale) > 1e-5) {
			float minimum_scale_delta = 0.000005f; // 0.00005f
			if (fabs(target_scale - persi_actor_data->visual_scale) < minimum_scale_delta) {
				persi_actor_data->visual_scale = target_scale;
				persi_actor_data->visual_scale_v = 0.0f;
			} else {
				critically_damped(
					persi_actor_data->visual_scale,
					persi_actor_data->visual_scale_v,
					target_scale,
					persi_actor_data->half_life / TimeScale(),
					Time::WorldTimeDelta()
				);
			}
		}
	}

	void apply_height(Actor* actor, ActorData* persi_actor_data, TempActorData* trans_actor_data, bool force = false) {

		GTS_PROFILE_SCOPE("GTSManager: ApplyHeight");

		if (!actor) {
			return;
		}
		if (!actor->Get3D1(false)) {
			return;
		}
		if (!actor->Is3DLoaded()) {
			return;
		}
		if (!trans_actor_data) {
			log::info("!Height: Trans Data not found for {}", actor->GetDisplayFullName());
			return;
		}
		if (!persi_actor_data) {
			log::info("!Height: Pers Data not found for {}", actor->GetDisplayFullName());
			return;
		}
		float scale = get_scale(actor);
		if (scale < 0.0f) {
			return;
		}
		float visual_scale = persi_actor_data->visual_scale;

		if(actor->formID == 0x14) {
			if (IsFirstPerson()) {
				visual_scale *= GetProneAdjustment();
			}
		}

		// Is scale correct already?
		if (fabs(visual_scale - scale) <= 1e-5 && !force) {
			return;
		}
		// Is scale too small
		if (visual_scale <= 1e-5) {
			return;
		}

		float initialScale = GetInitialScale(actor); // Incorperate the NIF scale into our edits
		float GameScale = game_getactorscale(actor); // * by GetScale
		
		update_model_visuals(actor, visual_scale * initialScale * GameScale); // We've set the values, now update model size based on them
	}

	void apply_speed(Actor* actor, ActorData* persi_actor_data, TempActorData* trans_actor_data, bool force = false) {
		GTS_PROFILE_SCOPE("GTSManager: ApplySpeed");
		if (!Config::GetGeneral().bDynamicAnimspeed) {
			return;
		}
		if (!actor) {
			return;
		}
		if (!actor->Is3DLoaded()) {
			return;
		}
		if (!trans_actor_data) {
			return;
		}
		if (!persi_actor_data) {
			return;
		}
		if (actor->IsDead()) {
			return;
		}
		float scale = get_visual_scale(actor);
		if (scale < 1e-5) {
			return;
		}

		if (Runtime::IsSexlabInstalled()) {

			const auto Player = PlayerCharacter::GetSingleton();

			// Copy player speed onto the actor
			if (IsInSexlabAnim(actor, Player)) {
				persi_actor_data->anim_speed = GetAnimationSlowdown(Player);
				return;
			}
		}
		
		persi_actor_data->anim_speed = GetAnimationSlowdown(actor); // else behave as usual
	}

	void update_actor(Actor* actor) {

		GTS_PROFILE_SCOPE("GTSManager: UpdateActor");

		auto temp_data = Transient::GetSingleton().GetActorData(actor);
		auto saved_data = Persistent::GetSingleton().GetActorData(actor);
		update_height(actor, saved_data, temp_data);
	}

	void apply_actor(Actor* actor, bool force = false) {

		GTS_PROFILE_SCOPE("GTSManager: ApplyActor");

		auto temp_data = Transient::GetSingleton().GetData(actor);
		auto saved_data = Persistent::GetSingleton().GetData(actor);
		apply_height(actor, saved_data, temp_data, force);
		apply_speed(actor, saved_data, temp_data, force);
	}
}

GtsManager& GtsManager::GetSingleton() noexcept {
	static GtsManager instance;

	static std::atomic_bool initialized;
	static std::latch latch(1);
	if (!initialized.exchange(true)) {
		latch.count_down();
	}
	latch.wait();

	return instance;
}

std::string GtsManager::DebugName() {
	return "::GTSManager";
}

void GtsManager::Start() {
	FixEmotionsRange();
}

// Poll for updates
void GtsManager::Update() {

	UpdateInterractionDistance(); // Player exclusive
	UpdateGlobalSizeLimit();
	ShiftAudioFrequency();
	ManageActorControl(); // Sadly have to call it non stop since im unsure how to easily fix it otherwise :(
	UpdateCameraINIs();
	ApplyTalkToActor();
	UpdateMaxScale(); // Update max scale of each actor in the scene
	UpdateFalling(); // Update player size damage when falling down
	CheckTalkPerk();
	FixActorFade(); // Self explanatory

	const auto& ActorList = find_actors();

#ifdef GTS_PROFILER_ENABLED
	GtsManager::LoadedActorCount = static_cast<uint32_t>(ActorList.size());
#endif

	for (auto actor : ActorList) {

		if (actor) {

			if (actor->formID == 0x14 || IsTeammate(actor)) {

				ClothManager::GetSingleton().CheckClothingRip(actor);
				GameModeManager::GameMode(actor); // Handle Game Modes

				Foot_PerformIdleEffects_Main(actor); // Just idle zones for pushing away/dealing minimal damage
				UpdateBoneMovementData(actor); // Records movement force of Player/Follower Legs/Hands
				TinyCalamity_SeekActors(actor); // Active only on Player
				SpawnActionIcon(actor); // Icons for interactions with others, Player only
				ScareActors(actor);

				//Ported from papyrus
				UpdateCrawlState(actor);
				UpdateFootStompType(actor);
				UpdateSneakTransition(actor);

				if (IsCrawling(actor)) {
					ApplyAllCrawlingDamage(actor, 1000, 0.25f);
				}
			}

			Foot_PerformIdle_Headtracking_Effects_Others(actor); // Just idle zones for pushing away/dealing minimal damage, but this one is for others as well
			update_actor(actor);
			apply_actor(actor);
		}
	}
}

void GtsManager::DragonSoulAbsorption() {
	DragonAbsorptionBonuses(); 
}

void GtsManager::FurnitureEvent(RE::Actor* activator, TESObjectREFR* object, bool enter) {
	if (activator&& object) {
		auto data = Transient::GetSingleton().GetActorData(activator);
		if (data) {
			data->FurnitureScale = object->GetScale() / get_natural_scale(activator, true);
			data->UsingFurniture = enter;
		}
	}
}

void GtsManager::reapply(bool force) {
	// Get everyone in loaded AI data and reapply

	GTS_PROFILE_SCOPE("GTSManager: ReApply");

	for (auto actor: find_actors()) {
		if (actor) {
		   	if (actor->Is3DLoaded()) {
				reapply_actor(actor, force);
			}
		}
	}
}
void GtsManager::reapply_actor(Actor* actor, bool force) {

	GTS_PROFILE_SCOPE("GTSManager: ReApplyActor");

	// Reapply just this actor
	if (actor) {
		if (actor->Is3DLoaded()) {
			apply_actor(actor, force);
		}
	}
}