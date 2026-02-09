#include "Hooks/Experiments/Experiments.hpp"
#include "Hooks/Util/HookUtil.hpp"
#include "Config/Config.hpp"


// --------------------------------------------------------------------------------
//							E X A M P L E S
//---------------------------------------------------------------------------------

/* ------------- VTABLE MULTIHOOK
struct MultiHook {

	static constexpr std::size_t funcIndex = 0x0;

	template<int ID>
	static void thunk([ARGS]) {
		func<ID>([ARGS]);
	}
	template<int ID>
	FUNCTYPE_VFUNC_UNIQUE func;
};

// ------------- VTABLE HOOK
struct VtableHook {

	static inline const std::size_t funcIndex = 0x0;

	static void thunk([ARGS]) {
		func([ARGS]);
	}

	FUNCTYPE_VFUNC func;
};

// ------------- CALL HOOK
struct CallHook {

	static void thunk([ARGS]) {
		func([ARGS]);
	}

	FUNCTYPE_CALL func;
};

// ------------- DETOUR (FUNCTION) HOOK
struct DetourHook {

	static void thunk([ARGS]) {
		func([ARGS]);
	}

	FUNCTYPE_DETOUR func;
};

// ------------- INSTALATION

//Vtable MultiHooks
stl::write_vfunc_unique<[RE::CLASS], [VTABLEARRAYINDEX], MultiHook, [MULTIHOOKID]>();
stl::write_vfunc_unique<MultiHook, [MULTIHOOKID]>(VTABLE_CLASS[VTABLEARRAYINDEX])();

//Vtable Hooks
stl::write_vfunc<[RE::CLASS], [VTABLEARRAYINDEX], VtableHook>();
stl::write_vfunc<VtableHook>(VTABLE_CLASS[VTABLEARRAYINDEX])();

//Call Hooks
stl::write_call<CallHook, [SIZE]>(REL::RelocationID(SE, AE, *[VR]), REL::VariantOffset(SE, AE, VR));
stl::write_call<CallHook, [SIZE]>(REL::VariantID(SE, AE, VR), REL::VariantOffset(SE, AE, VR));

//Jump (Branch) Hooks
stl::write_jump<CallHook>(REL::RelocationID(SE, AE, *[VR]), REL::VariantOffset(SE, AE, VR));
stl::write_jump<CallHook>(REL::VariantID(SE, AE, VR), REL::VariantOffset(SE, AE, VR));

//Detour (Function) Hooks
stl::write_detour<DetourHook>(REL::RelocationID(SE, AE, *[VR]));

NOTES:
1. Given a the same MultiHook struct [MULTIHOOKID] MUST be unique. 
2. *[VR] Means the VR Id Is optional.
3. VariantID Expects a raw offset for VR instead of an AddressLib ID.
4. [VTABLEARRAYINDEX] when not used with VTABLE_CLASS in write_vfunc_unique and write_vfunc is optional, if not given 0 is assumed.
5. [SIZE] in write_call is optional, it specifies if the call is a 5 byte relative call or a 6 byte absolute indirect call, if left empty 5 bytes is assumed.
6. Don't forget to increase the size of the allocated trampoline. Each Call Hook needs 14 bytes and Jump Hooks need 8 bytes. If not done SKSE will throw a "Couldn't handle allocation request" error.

*/


/*//True MainLoop of the game
//AE has 2 of them...
//AE ID: 36544 AE Offset: 0xd6 Location of the GetActiveWindow() Call Right before other checks.
//AE ID: 36550 AE Offset: 0x95 Location of the GetActiveWindow() Call Right before other checks.
struct MainLoop {

	//Right before a bunch of logic runs which eventually calls Main::Update
	//A call to GetActiveWindow is made to check if the game window is in focus.
	static HWND __fastcall thunk(void) {
		logger::trace("MainLoop");
		return GetActiveWindow();
	}

	FUNCTYPE_CALL func;
};*/

/*//A Very early call at near the start of Main::Update
//Call order Crt startup->MainLoop(true int main(), has while(true))->Main::Update, which is the hook below
struct MainUpdatePre {

	static void __fastcall thunk(int64_t unk_1) {
		logger::trace("MainUpdatePre");
		func(unk_1);
	}

	FUNCTYPE_CALL func;
};*/

/*//Detour of the SetMoveFlags Function,
//Intention was to force AI movement to not run.
//Result is that the player can no longer jog only walk or sprint.
struct SetMoveFlags {

	static uint8_t thunk(RE::ActorState* state, uint16_t flags) {
		constexpr uint16_t WALK = 1u << 6;
		constexpr uint16_t RUN = 1u << 7;
		constexpr uint16_t SPRINT = 1u << 8;

		const bool wantsRun = (flags & RUN) != 0;
		const bool wantsSprint = (flags & SPRINT) != 0;

		flags &= ~RUN;
		flags &= ~SPRINT;

		if (wantsRun || wantsSprint) {
			flags |= WALK;
		}

		state->actorState2.forceRun = 0;

		return func(state, flags);
	}


	FUNCTYPE_DETOUR func;
};*/

/*struct ActorStateGetMaxSpeedMult {

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
			const int offset = REL::Module::get().version() >= SKSE::RUNTIME_SSE_1_6_629 ? 0xC0 : 0xB8;
			return reinterpret_cast<Actor*>(reinterpret_cast<uintptr_t>(actorState) - offset);
		}
		return nullptr;
	}

	static float thunk(ActorState* a_this, float a1) {

		float original = func(a_this, a1);

		{

			GTS_PROFILE_ENTRYPOINT("Actor::ActorStateGetMaxSpeedMult");

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
	}

	FUNCTYPE_DETOUR func;
};*/

namespace Hooks {

	void Hook_Experiments::Install() {
		//stl::write_call<MainLoop, 6>(REL::VariantID(NULL, 36544, NULL), REL::VariantOffset(NULL, 0xd6, NULL));
		//stl::write_call<MainUpdatePre>(REL::VariantID(35565, 36564, NULL), REL::VariantOffset(0x1E, 0x3E, NULL));
	}

}

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

/*
void PatchFollowMoveSpeed() {

	logger::info("Installing AI Procedure SpeedClamp Hooks");
	

	//auto pattern_AE = REL::make_pattern<"C6 85 ?? ?? ?? ?? 01 F3 44 0F 5F 05 ?? ?? ?? ??">();
	//auto pattern_SE = REL::make_pattern<"C6 85 ?? ?? ?? ?? 01 F3 0F 10 05 ?? ?? ?? ?? 44 0F 2F C0 73 ?? 44 0F 28 C0">();


	//if (REL::Module::IsAE()) {
	//	uintptr_t address = REL::Relocation<std::uintptr_t>(REL::ID(28644), REL::Offset(0x1292)).address();
	//	pattern_AE.match_or_fail(address);
	//	REL::safe_fill(address, 0x90, 16);

	//}
	//else if (REL::Module::IsSE()) {
	//	uintptr_t address = REL::Relocation<std::uintptr_t>(REL::ID(27911), REL::Offset(0x1522)).address();
	//	pattern_SE.match_or_fail(address);
	//	REL::safe_fill(address, 0x90, 25);
	//}
	//else {
	//	logger::error("Unsuported Runtime");
	//	return;
	//}

	//14040e370 1.5.97
	//char BGSProcedureFollowExecState::sub(RE::BGSProcedureFollowExecState* this, RE::Character*** a_runningActor, char unk1) Is the caller
	//undefined8 ActorState::sub(longlong *param_1,undefined4 param_2) is the target.
	//A detour is used instead of a call hook because i saw that other follow related functions (and combat flee apparently) are the only places where this function is called.
	//Judging by their function/naming it usefull to enforce the clamping there as well.
	//stl::write_detour<ActorStateGetMaxSpeedMult>(REL::RelocationID(88499, 90925, NULL));
}
*/