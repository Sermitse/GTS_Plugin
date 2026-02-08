#include "Hooks/Actor/Actor.hpp"

#include "Config/Config.hpp"

#include "Hooks/Util/HookUtil.hpp"
#include "Managers/AttributeManager.hpp"
#include "Managers/FurnitureManager.hpp"
#include "Managers/Damage/SizeHitEffects.hpp"


namespace {
	
	void AdjustData(RE::Actor* a_actor, RE::Movement::TypeData* a_DataToEdit) {
		if (!a_actor || !a_DataToEdit) {
			return;
		}

		RE::ActorState* state = a_actor->AsActorState();
		if (!state || state->IsSprinting()) {
			return;
		}

		const float scale = GTS::get_visual_scale(a_actor);

		constexpr float& START_CLAMP_SCALE = GTS::Config::General.fNPCMaxSpeedMultClampStartAt; // start lerp here
		constexpr float& FULL_CLAMP_SCALE = GTS::Config::General.fNPCMaxSpeedMultClampMaxAt;   // fully clamped here

		// Interp factor: 0..1
		float t = 0.0f;
		if (scale >= START_CLAMP_SCALE) {
			if (FULL_CLAMP_SCALE > START_CLAMP_SCALE) {
				t = (scale - START_CLAMP_SCALE) / (FULL_CLAMP_SCALE - START_CLAMP_SCALE);
			}
			else {
				t = 1.0f;
			}
			t = std::clamp(t, 0.0f, 1.0f);
		}

		// Percent-based lerp ceiling (run-speed floor)
		const float maxLerp = std::clamp(GTS::Config::General.fNPCMaxSpeedMultLerpTargetPercent * 0.01f, 0.0f, 1.0f);

		// Never allow full walk if maxLerp < 1
		t = std::min(t, maxLerp);

		auto lerpRunTowardWalk = [&](RE::Movement::SPEED_DIRECTION dir) {
			float& run = a_DataToEdit->defaultData.speeds[dir][RE::Movement::MaxSpeeds::kRun];
			float  walk = a_DataToEdit->defaultData.speeds[dir][RE::Movement::MaxSpeeds::kWalk];
			run = std::lerp(run, walk, t); // at t=1, run becomes walk
		};

		lerpRunTowardWalk(RE::Movement::SPEED_DIRECTION::kForward);
		lerpRunTowardWalk(RE::Movement::SPEED_DIRECTION::kBack);
		lerpRunTowardWalk(RE::Movement::SPEED_DIRECTION::kLeft);
		lerpRunTowardWalk(RE::Movement::SPEED_DIRECTION::kRight);
	}
}

namespace Hooks {

	struct HandleHealthDamage {

		static inline const std::size_t funcIndex = REL::Relocate(0x104, 0x104, 0x106);

		template<int ID>
		static void thunk(RE::Actor* a_this, RE::Actor* a_attacker, float a_damage) {

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("Actor::HandleHealthDamage", ID);

				if (a_attacker) {

					SizeHitEffects::ApplyEverything(a_attacker, a_this, a_damage); // Apply bonus damage, overkill, stagger resistance

					if (Runtime::HasPerkTeam(a_this, Runtime::PERK.GTSPerkSizeReserveAug1)) { // Size Reserve Augmentation
						auto Cache = Persistent::GetActorData(a_this);
						if (Cache) {
							Cache->fSizeReserve += -a_damage / 3000;
						}
					}
				}
			}

			func<ID>(a_this, a_attacker, a_damage);  // Just reports the value, can't override it.
		}
		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;
	};

	struct Move {

		static inline const std::size_t funcIndex = REL::Relocate(0x0C8, 0x0C8, 0x0CA);

		// Override Movement Speed
		template<int ID>
		static bhkCharacterController* thunk(RE::Actor* a_this, float a_arg2, const RE::NiPoint3& a_position) {

			float bonus = 1.0f;

			{
				//This function is responsible for the resulting movement vector and applies to all actors.
				//It does not change the actual visible animation/walk speed, just the movement in the world.
				GTS_PROFILE_ENTRYPOINT_UNIQUE("Actor::Move", ID);
				if (a_this && a_this->Get3D1(false) && !a_this->IsInKillMove()) {
					bonus = AttributeManager::AlterMovementSpeed(a_this);
				}
			}

			return func<ID>(a_this, a_arg2, a_position * bonus);

		}
		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;
	};

	struct Update {

		static constexpr std::size_t funcIndex = 0xAD;
		template<int ID>
		static void thunk(RE::Actor* a_this, float a_deltaTime) {

			func<ID>(a_this, a_deltaTime);

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("Actor::Update", ID);
				EventDispatcher::DoActorUpdate(a_this);
			}

		}
		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;
	};

	struct Load3D {

		static constexpr std::size_t funcIndex = 0x6A;

		template<int ID>
		static NiAVObject* thunk(Actor* actor, bool a_backgroundLoading) {

			NiAVObject* Res = func<ID>(actor, a_backgroundLoading);

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("Actor::Load3D", ID);
				const auto& intfc = SKSE::GetTaskInterface();
				//From my understanding skse tasks run as jobs on the main thread at the end of the frame.
				//Which feels like the safest place to do this.
				intfc->AddTask([actor] {
					GTS_PROFILE_ENTRYPOINT_UNIQUE("Actor::Load3D::Task", ID);
					//Moved this event dispatch here, The game events one runs before peristent data load
					//plus it fires when an actor "unloads" as well.
					if (State::InGame()) {
						EventDispatcher::DoActorLoaded(actor);
					}
				});
			}

			return Res;
		}

		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;
	};

	

	struct ApplyMovementDelta {

		static void thunk(RE::Actor* a_actor, float a_delta) {

			{
				GTS_PROFILE_ENTRYPOINT("Actor::ApplyMovementDelta");

				if (!a_actor) return;

				if (FurnitureManager::ValidActor(a_actor)) {
					if (TransientActorData* data = Transient::GetActorData(a_actor)) {
						if (data->BlockMovementTimer.Gate()) {
							return;
						}
					}
				}
			}

			func(a_actor, a_delta);
		}

		FUNCTYPE_CALL func;
	};

	//FUN_14065b900 -> SE
	//FUN_1406ee680 -> AE
	//Intercept and modify Movement::TypeData when its copied from the AI process data to anywhere else
	struct MovementTypeData_Copy_1 {

		static bool thunk(RE::AIProcess* a_process_src, RE::Movement::TypeData* a_typeData_dst) {

			if (RE::HighProcessData* hi = a_process_src->high) {
				if (RE::Actor* actor = a_process_src->GetUserData()) {
					if (!actor->IsPlayerRef()) {
						AdjustData(actor, reinterpret_cast<RE::Movement::TypeData*>(reinterpret_cast<std::byte*>(hi) + 0xF0));
					}
				}
			}
			return func(a_process_src, a_typeData_dst);
		}

		FUNCTYPE_DETOUR func;
	};

	//FUN_14065ca40 -> SE
	//FUN_1406ef990 -> AE
	//Intercept and modify Movement::TypeData when its copied a given typedata* to the value held by AIProcess->HighData
	struct MovementTypeData_Copy_2 {

		static void thunk(RE::AIProcess* a_process_dst, RE::Movement::TypeData* a_typeData_src) {

			if (RE::Actor* actor = a_process_dst->GetUserData()) {
				if (!actor->IsPlayerRef()) {
					AdjustData(actor, a_typeData_src);
				}
			}

			return func(a_process_dst, a_typeData_src);
		}

		FUNCTYPE_DETOUR func;
	};

	void Hook_Actor::Install() {

		logger::info("Installing Actor VTABLE MultiHooks...");

		stl::write_vfunc_unique<HandleHealthDamage, 1>(VTABLE_Character[0]);
		stl::write_vfunc_unique<HandleHealthDamage, 2>(VTABLE_PlayerCharacter[0]);

		stl::write_vfunc_unique<Move, 1>(VTABLE_Character[0]);
		stl::write_vfunc_unique<Move, 2>(VTABLE_PlayerCharacter[0]);

		stl::write_vfunc_unique<Update, 1>(VTABLE_Character[0]);
		stl::write_vfunc_unique<Update, 2>(VTABLE_PlayerCharacter[0]);

		stl::write_vfunc_unique<Load3D, 1>(VTABLE_Character[0]);
		stl::write_vfunc_unique<Load3D, 2>(VTABLE_PlayerCharacter[0]);

		logger::info("Installing AIProcess MovementSpeed Clamp Detours...");

		//Movementspeed Clamp hooks
		stl::write_detour<MovementTypeData_Copy_1>(REL::RelocationID(38543, 39554, NULL));
		stl::write_detour<MovementTypeData_Copy_2>(REL::RelocationID(38576, 39601, NULL));

		logger::info("Installing MovementDelta Furniture Pause Hook...");
		
		stl::write_call<ApplyMovementDelta>(REL::RelocationID(36359, 37350, NULL), REL::VariantOffset(0xF0, 0xFB, NULL));
	}

	
}
