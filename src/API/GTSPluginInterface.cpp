#include "API/GTSPluginInterface.hpp"

namespace GTS {

	APIResult GTSPluginIntfc::GetVisualScale(RE::ActorHandle a_actorHandle, float& a_out) noexcept {
		
		if (const auto& actor = a_actorHandle.get().get(); actor) {
			a_out = get_visual_scale(actor);
			return APIResult::kOk;
		}

		return APIResult::kFail;
	}

	APIResult GTSPluginIntfc::GetMaxScale(RE::ActorHandle a_actorHandle, float& a_out) noexcept {
		
		if (const auto& actor = a_actorHandle.get().get(); actor) {
			a_out = get_max_scale(actor);
			return APIResult::kOk;
		}

		return APIResult::kFail;
	}

	APIResult GTSPluginIntfc::GetNaturalScale(RE::ActorHandle a_actorHandle, float& a_out) noexcept {
		
		if (const auto& actor = a_actorHandle.get().get(); actor) {
			a_out = get_natural_scale(actor);
			return APIResult::kOk;
		}
		
		return APIResult::kFail;
	}

	APIResult GTSPluginIntfc::GetTargetScale(RE::ActorHandle a_actorHandle, float& a_out) noexcept {
		
		if (const auto& actor = a_actorHandle.get().get(); actor) {
			a_out = get_target_scale(actor);
			return APIResult::kOk;
		}
		
		return APIResult::kFail;
	}

	APIResult GTSPluginIntfc::SetTargetScale(RE::ActorHandle a_actorHandle, float a_scale) noexcept {
		
		if (const auto& actor = a_actorHandle.get().get(); actor) {
			set_target_scale(actor, a_scale);
			return APIResult::kOk;
		}
		
		
		return APIResult::kFail;
	}

	APIResult GTSPluginIntfc::ModTargetScale(RE::ActorHandle a_actorHandle, float a_modAmount) noexcept {
		
		if (const auto& actor = a_actorHandle.get().get(); actor) {
			set_target_scale(actor, a_modAmount);
			return APIResult::kOk;
		}

		return APIResult::kFail;
	}
}

extern "C" __declspec(dllexport)
void* RequestPluginAPI(GTSPluginAPI::InterfaceVersion a_version) {
	using namespace GTSPluginAPI;

	switch (a_version) {
		case InterfaceVersion::V1:
		//case InterfaceVersion::V2:
		{
			logger::info("Plugin Interface V{} requested", static_cast<int>(a_version));
			return GTS::GTSPluginIntfc::GetSingleton();
		}
			
		default: 
		{
			logger::info("Invalid Plugin Interface {} Requested", static_cast<int>(a_version));
			return nullptr;
		}
	}
}
