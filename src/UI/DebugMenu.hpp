#pragma once

namespace GTS {

	class DebugMenu final : IMenu, public EventListener, public CInitSingleton<DebugMenu> {

		public:

		static constexpr const char* MENU_PATH = "GTSPlugin/OverlayMenu";
		static constexpr const char* MENU_NAME = "GTSDebugMenu";

		private:
		static inline std::vector<std::string> m_hideSources = {};
		std::atomic_bool m_scaleformInitialized = false;

		public:
		//Init
		void InitScaleform();

		//Scaleform
		static void Show(const std::string& source);
		static void Hide(const std::string& source);
		static void ToggleVisibility(bool mode);
		static RE::stl::owner<RE::IMenu*> Creator();
		static bool Ready();


		private:
		// Inherited via EventListener
		std::string DebugName() override;
		void DataReady() override;
		void MenuChange(const MenuOpenCloseEvent* a_event) override;
		void Start() override;
		void Update() override;

		// Inherited via IMenu
		void AdvanceMovie(float a_interval, std::uint32_t a_currentTime) override;
	};
}