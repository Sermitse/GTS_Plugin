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

namespace Hooks {

	void Hook_Experiments::Install() {
		
	}

}