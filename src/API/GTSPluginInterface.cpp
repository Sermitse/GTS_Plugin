#include "API/GTSPluginInterface.hpp"
#include "Config/Config.hpp"

namespace GTS {

	APIResult GTSPluginIntfc::GetVisualScale(RE::Actor* a_actor, float& a_out) noexcept {
		
		if (a_actor && a_actor->Is3DLoaded()) {
			a_out = get_visual_scale(a_actor);
			return APIResult::kOk;
		}

		return APIResult::kFail;
	}

	APIResult GTSPluginIntfc::GetMaxScale(RE::Actor* a_actor, float& a_out) noexcept {
		
		if (a_actor && a_actor->Is3DLoaded()) {
			a_out = get_max_scale(a_actor);
			return APIResult::kOk;
		}

		return APIResult::kFail;
	}

	APIResult GTSPluginIntfc::GetNaturalScale(RE::Actor* a_actor, float& a_out) noexcept {
		
		if (a_actor && a_actor->Is3DLoaded()) {
			a_out = get_natural_scale(a_actor);
			return APIResult::kOk;
		}
		
		return APIResult::kFail;
	}

	APIResult GTSPluginIntfc::GetTargetScale(RE::Actor* a_actor, float& a_out) noexcept {
		
		if (a_actor && a_actor->Is3DLoaded()) {
			a_out = get_target_scale(a_actor);
			return APIResult::kOk;
		}
		
		return APIResult::kFail;
	}

	APIResult GTSPluginIntfc::SetTargetScale(RE::Actor* a_actor, float a_scale) noexcept {
		
		if (a_actor && a_actor->Is3DLoaded()) {
			set_target_scale(a_actor, a_scale);
			return APIResult::kOk;
		}
		
		
		return APIResult::kFail;
	}

	APIResult GTSPluginIntfc::ModTargetScale(RE::Actor* a_actor, float a_modAmount) noexcept {
		
		if (a_actor && a_actor->Is3DLoaded()) {
			set_target_scale(a_actor, a_modAmount);
			return APIResult::kOk;
		}
		return APIResult::kFail;
	}

	APIResult GTSPluginIntfc::GetAnimationSlowdown(RE::Actor* a_actor, float& a_multiplier) noexcept {
		
		if (a_actor && a_actor->Is3DLoaded()) {
			a_multiplier = GTS::GetAnimationSlowdown(a_actor);
			return APIResult::kOk;
		}
		return APIResult::kFail;
	}


	APIResult GTSPluginIntfc::GetAnimationSlowdownArgs(std::array<float, 5>& a_args) noexcept {
		a_args = GTS::Config::Advanced.fAnimSpeedSoftCore;
		return APIResult::kOk;
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
