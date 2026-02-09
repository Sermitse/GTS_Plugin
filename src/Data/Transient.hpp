#pragma once
#include "Data/Storage/TransientData.hpp"

namespace GTS {

	class Transient final : public EventListener, public CInitSingleton<Transient> {

		public:
		static void EraseUnloadedData();
		static TransientActorData* GetActorData(Actor* actor);

		private:
		virtual std::string DebugName() override;
		virtual void ActorLoaded(RE::Actor* actor) override;
		virtual void Reset() override;
		virtual void ResetActor(Actor* actor) override;
		virtual void OnGameRevert() override;

		static inline std::mutex _Lock;
		static inline std::unordered_map<FormID, TransientActorData> TempActorDataMap {};
	};
}
