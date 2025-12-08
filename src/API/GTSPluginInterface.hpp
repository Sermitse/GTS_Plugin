#pragma once
#include "API/External//GTSPluginAPI.hpp"

namespace GTS {

	using APIResult = GTSPluginAPI::APIResult;
	using Interface = GTSPluginAPI::GTSPluginAPIV1;

	class GTSPluginIntfc : public Interface {
		private:
		GTSPluginIntfc() noexcept = default;
		~GTSPluginIntfc() noexcept = default;

		public:
		static GTSPluginIntfc* GetSingleton() noexcept {
			static GTSPluginIntfc singleton;
			return std::addressof(singleton);
		}

		APIResult GetVisualScale(RE::ActorHandle a_actorHandle, float& a_out) noexcept override;
		APIResult GetMaxScale(RE::ActorHandle a_actorHandle, float& a_out) noexcept override;
		APIResult GetNaturalScale(RE::ActorHandle a_actorHandle, float& a_out) noexcept override;

		APIResult GetTargetScale(RE::ActorHandle a_actorHandle, float& a_out) noexcept override;
		APIResult SetTargetScale(RE::ActorHandle a_actorHandle, float a_scale) noexcept override;
		APIResult ModTargetScale(RE::ActorHandle a_actorHandle, float a_modAmount) noexcept override;

	};

}