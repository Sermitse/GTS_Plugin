#pragma once 
#include "Config/Config.hpp"

namespace GTS {

	class AIManager : public EventListener, public CInitSingleton<AIManager> {
		public:
		virtual std::string DebugName() override;
		virtual void Update() override;
		bool TryStartAction(Actor* a_Performer) const;

		Timer BeginNewActionTimer = Timer(3.0f);
		SettingsAI_t& AISettings = Config::AI;
		SettingsAdvanced_t& AdvancedSettings = Config::Advanced;
	};
}

