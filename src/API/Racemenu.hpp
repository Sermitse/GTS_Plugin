#pragma once

#include "API/Impl/SKEEBodyMorphInterface.hpp"

namespace GTS {

	class Racemenu final : public EventListener, public CInitSingleton<Racemenu> {

		public:
		static void Register();
		static void SetMorph(RE::Actor* a_actor, const char* a_morphName, float a_value, bool a_immediate = false);
		[[nodiscard]] static float GetMorph(RE::Actor* a_actor, const char* a_morphName);
		static void ClearAllMorphs(RE::Actor* a_actor);
		static void ClearMorphs(RE::Actor* a_actor);
		static void ClearMorph(RE::Actor* a_actor, const char* a_morphName);
		static void ApplyMorphs(RE::Actor* a_actor);
		[[nodiscard]] static bool Loaded();

		std::string DebugName() override;
		void OnPluginPostLoad() override;

		private:
		static inline SKEE::IBodyMorphInterface* RaceMenuInterface = nullptr;
		static constexpr std::string MorphKey = "GTSPlugin";

	};


}