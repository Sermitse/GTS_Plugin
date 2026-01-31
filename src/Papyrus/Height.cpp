#include "Papyrus/Height.hpp"

using namespace GTS;
using namespace RE::BSScript;

namespace {

	constexpr std::string_view PapyrusClass = "GTSHeight";

	// Target Scales
	void SetTargetHeight(StaticFunctionTag*, Actor* actor, float height) {
		set_target_height(actor, height);
	}

	float GetTargetHeight(StaticFunctionTag*, Actor* actor) {
		return get_target_height(actor);
	}

	void ModTargetHeight(StaticFunctionTag*, Actor* actor, float amt) {
		mod_target_height(actor, amt);
	}

	void SetMaxHeight(StaticFunctionTag*, Actor* actor, float height) {
		set_max_height(actor, height);
	}

	float GetMaxHeight(StaticFunctionTag*, Actor* actor) {
		return get_max_height(actor);
	}

	void ModMaxHeight(StaticFunctionTag*, Actor* actor, float amt) {
		mod_max_height(actor, amt);
	}

	float GetVisualHeight(StaticFunctionTag*, Actor* actor) {
		return get_visual_height(actor);
	}

}

namespace GTS {

	bool register_papyrus_height(IVirtualMachine* vm) {

		//Target Height
		vm->RegisterFunction("SetTargetHeight", PapyrusClass, SetTargetHeight);
		vm->RegisterFunction("GetTargetHeight", PapyrusClass, GetTargetHeight);
		vm->RegisterFunction("ModTargetHeight", PapyrusClass, ModTargetHeight);

		//Target Max Height
		vm->RegisterFunction("SetMaxHeight", PapyrusClass, SetMaxHeight);
		vm->RegisterFunction("GetMaxHeight", PapyrusClass, GetMaxHeight);
		vm->RegisterFunction("ModMaxHeight", PapyrusClass, ModMaxHeight);

		//Visual Height
		vm->RegisterFunction("GetVisualHeight", PapyrusClass, GetVisualHeight);

		return true;
	}
}
