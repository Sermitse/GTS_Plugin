#include "Scale/Scale.hpp"

using namespace GTS;
using namespace RE::BSScript;

namespace {
	constexpr std::string_view PapyrusClass = "GTSScale";

	bool SetScale(StaticFunctionTag*, Actor* actor, float scale) {
		bool result = false;
		auto actor_data = Persistent::GetActorData(actor);
		if (actor_data) {
			result = update_model_visuals(actor, scale);
			actor_data->fVisualScale = scale;
			actor_data->fVisualScaleV = 0.0f;
			actor_data->fTargetScale = scale;
		}
		return result;
	}

	float GetScale(StaticFunctionTag*, Actor* actor) {
		if (!actor) {
			return 0.0f;
		}
		auto result = get_scale(actor);
		return result;
	}

	bool ModScale(StaticFunctionTag*, Actor* actor, float amt) {
		bool result = false;
		auto actor_data = Persistent::GetActorData(actor);
		if (actor_data) {
			auto scale = get_scale(actor) + amt;
			result = update_model_visuals(actor, scale);
			actor_data->fVisualScale = scale;
			actor_data->fVisualScaleV = 0.0f;
			actor_data->fTargetScale = scale;
		}
		return result;
	}

	// Target Scales
	void SetTargetScale(StaticFunctionTag*, Actor* actor, float scale) {
		set_target_scale(actor, scale);
	}

	float GetTargetScale(StaticFunctionTag*, Actor* actor) {
		return get_target_scale(actor);
	}

	void ModTargetScale(StaticFunctionTag*, Actor* actor, float amt) {
		mod_target_scale(actor, amt);
	}

	void SetMaxScale(StaticFunctionTag*, Actor* actor, float scale) {
		set_max_scale(actor, scale);
	}

	float GetMaxScale(StaticFunctionTag*, Actor* actor) {
		return get_max_scale(actor); 
	}

	void ModMaxScale(StaticFunctionTag*, Actor* actor, float amt) {
		mod_max_scale(actor, amt);
	}

	float GetVisualScale(StaticFunctionTag*, Actor* actor) {
		return get_visual_scale(actor);
	}

	float GetGiantessScale(StaticFunctionTag*, Actor* actor) {
		return get_giantess_scale(actor);
	}

	float GetNaturalScale(StaticFunctionTag*, Actor* actor) {
		return get_natural_scale(actor, true);
	}
}

namespace GTS {
	bool register_papyrus_scale(IVirtualMachine* vm) {

		//Scale
		vm->RegisterFunction("SetScale", PapyrusClass, SetScale);
		vm->RegisterFunction("GetScale", PapyrusClass, GetScale);
		vm->RegisterFunction("ModScale", PapyrusClass, ModScale);

		//Target Scale
		vm->RegisterFunction("SetTargetScale", PapyrusClass, SetTargetScale);
		vm->RegisterFunction("GetTargetScale", PapyrusClass, GetTargetScale);
		vm->RegisterFunction("ModTargetScale", PapyrusClass, ModTargetScale);

		//Max Scale
		vm->RegisterFunction("SetMaxScale", PapyrusClass, SetMaxScale);
		vm->RegisterFunction("GetMaxScale", PapyrusClass, GetMaxScale);
		vm->RegisterFunction("ModMaxScale", PapyrusClass, ModMaxScale);

		//Visual Scale
		vm->RegisterFunction("GetVisualScale", PapyrusClass, GetVisualScale);

		//Gts Scale
		vm->RegisterFunction("GetGiantessScale", PapyrusClass, GetGiantessScale);

		//Natural Scale
		vm->RegisterFunction("GetNaturalScale", PapyrusClass, GetNaturalScale);

		return true;
	}
}
