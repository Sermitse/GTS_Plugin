#pragma once
#include "API/External//GTSPluginAPI.hpp"

namespace GTS {

	using APIResult = GTSPluginAPI::APIResult;
	using Interface = GTSPluginAPI::GTSPluginAPIV1;

	class GTSPluginIntfc : public Interface {
		private:
		GTSPluginIntfc() noexcept = default;
		~GTSPluginIntfc() noexcept override = default;

		public:
		static GTSPluginIntfc* GetSingleton() noexcept {
			static GTSPluginIntfc singleton;
			return std::addressof(singleton);
		}

		APIResult GetVisualScale(Actor* a_actor, float& a_out) noexcept override;
		APIResult GetMaxScale(Actor* a_actor, float& a_out) noexcept override;
		APIResult GetNaturalScale(Actor* a_actor, float& a_out) noexcept override;

		APIResult GetTargetScale(Actor* a_actor, float& a_out) noexcept override;
		APIResult SetTargetScale(Actor* a_actor, float a_scale) noexcept override;
		APIResult ModTargetScale(Actor* a_actor, float a_modAmount) noexcept override;

		APIResult GetAnimationSlowdown(RE::Actor* a_actor, float& a_multiplier) noexcept override;
		APIResult GetAnimationSlowdownArgs(std::array<float, 5>& a_args) noexcept override;
	};

}