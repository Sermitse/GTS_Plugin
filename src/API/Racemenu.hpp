#pragma once

#include "API/External/SKEEInterface.hpp"

namespace GTS {



	class Racemenu final : public EventListener, public CInitSingleton<Racemenu> {

		public:

		class ClearKeyVisitor final : public SKEE::IBodyMorphInterface::ActorVisitor {
			public:
			ClearKeyVisitor(SKEE::IBodyMorphInterface* iface, const char* key): iface(iface), key(key) {}

			void Visit(RE::TESObjectREFR* actor) override {
				iface->ClearBodyMorphKeys(actor, key);
			}

			private:
			SKEE::IBodyMorphInterface* iface;
			const char* key;
		};

		static void ClearKeyOnAllActors(const char* key);
		static void Register();
		static void SetMorph(Actor* a_actor, const char* a_morphName, float a_value, const char* a_morphKey = nullptr, bool a_immediate = false);
		[[nodiscard]] static float GetMorph(Actor* a_actor, const char* a_morphName, const char* a_morphKey = nullptr);
		static void ClearAllMorphs(RE::Actor* a_actor);
		static void ClearMorphs(Actor* a_actor, const char* a_morphKey = nullptr);
		static void ClearMorph(Actor* a_actor, const char* a_morphName, const char* a_morphKey = nullptr);
		static void ApplyMorphs(RE::Actor* a_actor);
		[[nodiscard]] static bool Loaded();

		std::string DebugName() override;
		void OnPluginPostLoad() override;

		private:
		static inline SKEE::IBodyMorphInterface* iBodyMorphIntfc = nullptr;
		static inline SKEE::IInterfaceMap* iInterfaceMap = nullptr;
		static inline const std::string MorphKey = "GTSPlugin";



	};


}