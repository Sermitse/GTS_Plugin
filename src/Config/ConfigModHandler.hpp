#pragma once

namespace GTS {

	class ConfigModHandler : public EventListener, public CInitSingleton<ConfigModHandler> {
		public:
		static void DoCameraStateReset();
		static void DoHighHeelStateReset();
		static void HandleRaceMenuDataUpdate();
		std::string DebugName() override;
		void OnGameSave() override;
		void OnGameLoaded() override;
		void OnConfigReset() override;
		void OnConfigRefresh() override;

	};

}