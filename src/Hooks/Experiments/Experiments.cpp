#include "Hooks/Experiments/Experiments.hpp"
#include "Hooks/Util/HookUtil.hpp"

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


//True MainLoop of the game
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
};

//A Very early call at near the start of Main::Update
//Call order Crt startup->MainLoop(true int main(), has while(true))->Main::Update, which is the hook below
struct MainUpdatePre {

	static void __fastcall thunk(int64_t unk_1) {
		logger::trace("MainUpdatePre");
		func(unk_1);
	}

	FUNCTYPE_CALL func;
};

//Detour of the SetMoveFlags Function,
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
};

//A Random Character member func. The follow procedure uses it to calculate the distance between Character and the person being followed (i think)
//Or actually i think its the move function??? maybe? forcing 0 causes a follower to slide in place sometimes?
struct Sub {

	static float thunk(RE::Character* a_this) {
		return 0.0f;
	}

	FUNCTYPE_DETOUR func;
};


namespace Hooks {

	void Hook_Experiments::Install() {
		//stl::write_call<MainLoop, 6>(REL::VariantID(NULL, 36544, NULL), REL::VariantOffset(NULL, 0xd6, NULL));
		//stl::write_call<MainUpdatePre>(REL::VariantID(35565, 36564, NULL), REL::VariantOffset(0x1E, 0x3E, NULL));



		//SE: 38026
		//AE: 38982
		//stl::write_detour<SetMoveFlags>(REL::RelocationID(38026, 38982));

		//SE: 36460
		//AE : 37458
		//stl::write_detour<Sub>(REL::RelocationID(36460, 37458));

	}

}