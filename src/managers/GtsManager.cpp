#include "managers/animation/AnimationManager.hpp"
#include "managers/animation/Utils/CrawlUtils.hpp"
#include "managers/gamemode/GameModeManager.hpp"
#include "managers/damage/CollisionDamage.hpp"
#include "managers/damage/TinyCalamity.hpp"
#include "managers/audio/PitchShifter.hpp"
#include "magic/effects/TinyCalamity.hpp"
#include "managers/cameras/camutil.hpp"
#include "managers/ai/headtracking.hpp"
#include "managers/RipClothManager.hpp"
#include "managers/MaxSizeManager.hpp"
#include "managers/animation/Grab.hpp"
#include "managers/GtsSizeManager.hpp"
#include "managers/InputManager.hpp"
#include "magic/effects/common.hpp"
#include "managers/GtsManager.hpp"
#include "managers/Attributes.hpp"
#include "managers/hitmanager.hpp"
#include "utils/MovementForce.hpp"
#include "utils/DynamicScale.hpp"
#include "managers/highheel.hpp"
#include "utils/actorUtils.hpp"
#include "data/persistent.hpp"
#include "managers/Rumble.hpp"
#include "scale/modscale.hpp"
#include "data/transient.hpp"
#include "ActionSettings.hpp"
#include "rays/raycast.hpp"
#include "data/runtime.hpp"
#include "utils/camera.hpp"
#include "utils/debug.hpp"
#include "UI/DebugAPI.hpp"
#include "scale/scale.hpp"
#include "data/time.hpp"
#include "profiler.hpp"
#include "Config.hpp"
#include "timer.hpp"
#include "node.hpp"
#include <vector>
#include <string>

using namespace Gts;
using namespace RE;
using namespace SKSE;
using namespace std;

namespace {
	const float ini_adjustment = 1000000; //1 million units distance

	void FixEmotionsRange() { // Makes facial emotions always enabled at any size
		EnsureINIFloat("fTalkingDistance:LOD", ini_adjustment);
		EnsureINIFloat("fLodDistance:LOD", ini_adjustment);
	}

	void ManageActorControl() { // Rough control other fix
		auto profiler = Profilers::Profile("Manager: Actor Control");
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
			if (Runtime::HasPerkTeam(player, "MightyLegs")) {
				auto charCont = player->GetCharController();
				if (charCont) {
					auto transient = Transient::GetSingleton().GetData(player);
					if (transient) {
						float scale = std::clamp(get_visual_scale(player), 0.06f, 2.0f);
						float CalcFall = 1.0 + (charCont->fallTime * (4.0 / scale) - 4.0);
						float FallTime = std::clamp(CalcFall, 1.0f, 3.0f);
						transient->FallTimer = FallTime;
					}
				}
			}
		}
	}

	void FixActorFade(Actor* actor) {
		auto profiler = Profilers::Profile("Manager: Fade Fix");

		static Timer ApplyTimer = Timer(2.00);

		if (!ApplyTimer.ShouldRunFrame()) {
			return;
		}

		bool reset = false;
		NiAVObject* node = find_node(actor, "skeleton_female.nif");
		
		if (get_visual_scale(actor) < 1.5) {
			reset = true;
		}
		if (node) {
			if (!reset) {
				node->GetFlags().set(RE::NiAVObject::Flag::kIgnoreFade);
				node->GetFlags().set(RE::NiAVObject::Flag::kAlwaysDraw);
				node->GetFlags().set(RE::NiAVObject::Flag::kHighDetail);
			} else {
				node->GetFlags().reset(RE::NiAVObject::Flag::kIgnoreFade);
				node->GetFlags().reset(RE::NiAVObject::Flag::kAlwaysDraw);
				node->GetFlags().reset(RE::NiAVObject::Flag::kHighDetail);
			}
		}
	}

	void PerformRoofRaycastAdjustments(Actor* actor, float& target_scale, float currentOtherScale) {
		if (SizeRaycastEnabled() && !actor->IsDead() && target_scale > 1.025) {
			float room_scale = GetMaxRoomScale(actor);
			if (room_scale > (currentOtherScale - 0.05)) {
				// Only apply room scale if room_scale > natural_scale
				//   This stops it from working when room_scale < 1.0
				target_scale = min(target_scale, room_scale);
			} else {
				// Else we just scale to natural
				target_scale = 1.0;
			}
		}
	}

	void update_height(Actor* actor, ActorData* persi_actor_data, TempActorData* trans_actor_data) {
		auto profiler = Profilers::Profile("Manager: update_height");
		if (!actor) {
			return;
		}
		if (!trans_actor_data) {
			log::info("!Upate_height: Trans Data not found for {}", actor->GetDisplayFullName());
			return;
		}
		if (!persi_actor_data) {
			log::info("!Upate_height: Pers Data not found for {}", actor->GetDisplayFullName());
			return;
		}
		float currentOtherScale = Get_Other_Scale(actor);
		trans_actor_data->otherScales = currentOtherScale;

		float target_scale = persi_actor_data->target_scale;
		
		// Smooth target_scale towards max_scale if target_scale > max_scale
		float max_scale = persi_actor_data->max_scale;
		if (target_scale > max_scale) {
			float minimum_scale_delta = 0.000005; // 0.00005%
			if (fabs(target_scale - max_scale) < minimum_scale_delta) {
				float target = max_scale;
				persi_actor_data->target_scale = target;
				persi_actor_data->target_scale_v = 0.0;
			} else {
				critically_damped(
					persi_actor_data->target_scale,
					persi_actor_data->target_scale_v,
					max_scale,
					persi_actor_data->half_life*1.5,
					Time::WorldTimeDelta()
					);
			}
		} else {
			persi_actor_data->target_scale_v = 0.0;
		}

		// Room Size adjustments
		// We only do this if they are bigger than 1.05x their natural scale (currentOtherScale)
		// and if enabled in the mcm
		PerformRoofRaycastAdjustments(actor, target_scale, currentOtherScale);
		
		if (fabs(target_scale - persi_actor_data->visual_scale) > 1e-5) {
			float minimum_scale_delta = 0.000005; // 0.00005%
			if (fabs(target_scale - persi_actor_data->visual_scale) < minimum_scale_delta) {
				persi_actor_data->visual_scale = target_scale;
				persi_actor_data->visual_scale_v = 0.0;
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
		auto profiler = Profilers::Profile("Manager: apply_height");
		if (!actor) {
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
		if (scale < 0.0) {
			return;
		}
		float visual_scale = persi_actor_data->visual_scale;
		if (actor->formID == 0x14) {
			float scaleOverride = get_fp_scale(actor);
			if (IsFirstPerson() && scaleOverride >= 1e-4) {
				if (scaleOverride > 1.0) {
					visual_scale *= GetProneAdjustment(); // In normal case we * it for compatibility with crawling/proning.
				} else {
					visual_scale = scaleOverride; // In Loot/Combat mode case, we override it with flat value (such as 0.6).
				}
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
		auto profiler = Profilers::Profile("Manager: apply_speed");
		if (!Persistent::GetSingleton().is_speed_adjusted) {
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
		float speedmultcalc = GetAnimationSlowdown(actor); // For all other movement types
		persi_actor_data->anim_speed = speedmultcalc;
	}

	void update_actor(Actor* actor) {
		auto profiler = Profilers::Profile("Manager: update_actor");
		auto temp_data = Transient::GetSingleton().GetActorData(actor);
		auto saved_data = Persistent::GetSingleton().GetActorData(actor);
		update_height(actor, saved_data, temp_data);
	}

	void apply_actor(Actor* actor, bool force = false) {
		auto profiler = Profilers::Profile("Manager: apply_actor");
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
	return "GtsManager";
}

void GtsManager::Start() {
	FixEmotionsRange();
}

// Poll for updates
void GtsManager::Update() {
	auto profiler = Profilers::Profile("Manager: Update()");

	UpdateFalling();
	UpdateMaxScale(); // Update max scale of each actor in the scene
	ManageActorControl(); // Sadly have to call it non stop since im unsure how to easily fix it otherwise :(
	ShiftAudioFrequency();

	for (auto actor: find_actors()) {
		if (actor) {
			auto& CollisionDamage = CollisionDamage::GetSingleton();
			auto& sizemanager = SizeManager::GetSingleton();

			if (actor->formID == 0x14 || IsTeammate(actor)) {

				ScareActors(actor);
				FixActorFade(actor);

				CollisionDamage.DoFootCollision(actor, Damage_Default_Underfoot * TimeScale(), Radius_Default_Idle, 0, 0.0, Minimum_Actor_Crush_Scale_Idle, DamageSource::FootIdleL, false, false, false, false);
				CollisionDamage.DoFootCollision(actor, Damage_Default_Underfoot * TimeScale(), Radius_Default_Idle, 0, 0.0, Minimum_Actor_Crush_Scale_Idle, DamageSource::FootIdleR, true, false, false, false);
				
				ClothManager::GetSingleton().CheckClothingRip(actor);
				TinyCalamity_SeekActors(actor);
				SpawnActionIcon(actor);

				if (IsCrawling(actor)) {
					ApplyAllCrawlingDamage(actor, 1000, 0.25);
				}

				GameModeManager::GetSingleton().GameMode(actor); // Handle Game Modes
			}
			if (Runtime::GetBool("PreciseDamageOthers")) {
				if (actor->formID != 0x14 && !IsTeammate(actor)) {
					CollisionDamage.DoFootCollision(actor, Damage_Default_Underfoot * TimeScale(), Radius_Default_Idle, 0, 0.0, Minimum_Actor_Crush_Scale_Idle, DamageSource::FootIdleL, false, false, false, false);
					CollisionDamage.DoFootCollision(actor, Damage_Default_Underfoot * TimeScale(), Radius_Default_Idle, 0, 0.0, Minimum_Actor_Crush_Scale_Idle, DamageSource::FootIdleR, true, false, false, false);
				}
			}
			update_actor(actor);
			apply_actor(actor);
		}
	}
}

void GtsManager::DragonSoulAbsorption() {
	DragonAbsorptionBonuses(); 
}

void GtsManager::reapply(bool force) {
	// Get everyone in loaded AI data and reapply
	auto profiler = Profilers::Profile("Manager: reapply");
	for (auto actor: find_actors()) {
		if (actor) {
		   	if (actor->Is3DLoaded()) {
				reapply_actor(actor, force);
			}
		}
	}
}
void GtsManager::reapply_actor(Actor* actor, bool force) {
	auto profiler = Profilers::Profile("Manager: reapply_actor");
	// Reapply just this actor
	if (actor) {
		if (actor->Is3DLoaded()) {
			apply_actor(actor, force);
		}
	}
}