#include "Hooks/Actor/Actor.hpp"

#include "Config/Config.hpp"

#include "Hooks/Util/HookUtil.hpp"
#include "Managers/AttributeManager.hpp"
#include "Managers/Damage/SizeHitEffects.hpp"

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

	struct ActorStateGetMaxSpeedMult {

		static inline Actor* GetOwningActor(const ActorState* actorState) noexcept {

			if (actorState) {
				// 0xB8 for versions up to 1.6.629 (SSE/older AE)
				// 0xC0 for versions 1.6.640+ (newer AE)
				// It's so funny that this just works.
				// This basically hinges on the fact that the actor state only actually exists as a member of Actor in the game.
				// So to be able to pass an actor state pointer you need to have an actor as well.
				// Also fun fact for SE And probably AE too. If you subtract 0x1B8 ontop of this you can get the owning MovementControllerNPC Pointer.
				// Fenix has RE'd a bunch of the AI/Movement Related classes in his SSE clib fork which is how i found this out.
				// https://github.com/fenix31415/CommonLibSSE/blob/b4a2fc70d5709f8a6bb9023bb942ab65867ae8ab/include/RE/M/MovementControllerNPC.h#L17
				const int offset = REL::Module::get().version() >= RUNTIME_SSE_1_6_629 ? 0xC0 : 0xB8;
				return reinterpret_cast<Actor*>(reinterpret_cast<uintptr_t>(actorState) - offset);
			}
			return nullptr;
		}
	
		static float thunk(ActorState* a_this, float a1) {

			float original = func(a_this, a1);

			if (Actor* const owner = GetOwningActor(a_this)) {
				const float scale = get_visual_scale(owner);
				constexpr float& START_CLAMP_SCALE = Config::General.fNPCMaxSpeedMultClampStartAt;  // Scale at which clamping begins
				constexpr float& FULL_CLAMP_SCALE = Config::General.fNPCMaxSpeedMultClampMaxAt;    // Scale at which speed is fully clamped to target

				if (scale >= START_CLAMP_SCALE) {
					// Calculate interpolation factor (0.0 to 1.0)
					float t = (scale - START_CLAMP_SCALE) / (FULL_CLAMP_SCALE - START_CLAMP_SCALE);
					t = std::clamp(t, 0.0f, 1.0f);

					// Lerp from original speed to fNPCMaxSpeedMultClampTarget
					const float clampedSpeed = std::lerp(original, Config::General.fNPCMaxSpeedMultClampTarget, t);

					//logger::trace("Scale: {}, Original: {}, Clamped: {}", scale, original, clampedSpeed);
					return clampedSpeed;
				}
			}

			return original;
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

		logger::info("Installing AI Procedure SpeedClamp Hooks");
		/*
		 
			The speed clamp fix is made out of 2 parts.
			one is detouring what appears to be an actorstate member subroutine that is only used by The BGSProcedure* system.
			I think its just the GetMaxspeed Function.
			The second is NOPing out the assembly that forces the speed clamp in the BGSProcedureFollowExecState function.
			The follow procedure forcibly sets the speed to 2.0 and sets the movement state to sprint if the distance between the player and follower is too large.
			This distance check use a gamesetting value and by default is at around 500.
			This distance check is only relevant when the vanilla ai follow package runs.
			NFF has a custom follow package that is actually made out of 3 state where each state has its own distance check and prefferered movement speed set as an oveeride.
			The too far check happens right after the call to the GetMaxSpeed function. Its easier to just nop out this check than to inject an xbyak block with extra logic.
		 
			//1.6.1170

			14046a1a2 c6 85 78        MOV        byte ptr [RBP + TooFar],0x1
					  02 00 00 01

			14046a1a9 f3 44 0f        MAXSS      Speedmultiplier,dword ptr [DAT_141ad288c]  = 40000000h
					  5f 05 da
					  86 66 01

			//1.5.97

			14040f892 c6 85 58        MOV        byte ptr [RBP + TooFar],0x1
					  02 00 00 01
			14040f899 f3 0f 10        MOVSS      XMM0,dword ptr [DAT_141525c5c]             = 40000000h
					  05 bb 63
					  11 01
			14040f8a1 44 0f 2f c0     COMISS     XMM8,XMM0
			14040f8a5 73 0d           JNC        LAB_14040f8b4
			14040f8a7 44 0f 28 c0     MOVAPS     XMM8,XMM0

		*/

		auto pattern_AE = REL::make_pattern<"C6 85 ?? ?? ?? ?? 01 F3 44 0F 5F 05 ?? ?? ?? ??">();
		auto pattern_SE = REL::make_pattern<"C6 85 ?? ?? ?? ?? 01 F3 0F 10 05 ?? ?? ?? ?? 44 0F 2F C0 73 ?? 44 0F 28 C0">();


		if (REL::Module::IsAE()) {
			uintptr_t address = REL::Relocation<std::uintptr_t>(REL::ID(28644), REL::Offset(0x1292)).address();
			pattern_AE.match_or_fail(address);
			REL::safe_fill(address, 0x90, 16);
			
		}
		else if (REL::Module::IsSE()) {
			uintptr_t address = REL::Relocation<std::uintptr_t>(REL::ID(27911), REL::Offset(0x1522)).address();
			pattern_SE.match_or_fail(address);
			REL::safe_fill(address, 0x90, 25);
		}
		else {
			logger::error("Unsuported Runtime");
			return;
		}

		//14040e370 1.5.97
		//char BGSProcedureFollowExecState::sub(RE::BGSProcedureFollowExecState* this, RE::Character*** a_runningActor, char unk1) Is the caller
		//undefined8 ActorState::sub(longlong *param_1,undefined4 param_2) is the target.
		//A detour is used instead of a call hook because i saw that other follow related functions (and combat flee apparently) are the only places where this function is called.
		//Judging by their function/naming it usefull to enforce the clamping there as well.
		stl::write_detour<ActorStateGetMaxSpeedMult>(REL::RelocationID(88499, 90925, NULL));
	}
}
