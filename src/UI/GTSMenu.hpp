#pragma once

#include "UI/Core/ImInput.hpp"
#include "UI/Core/ImWindowManager.hpp"

namespace GTS {

	class GTSMenu final : IMenu, public EventListener, public CInitSingleton<GTSMenu> {

		public:
		static constexpr const char* MENU_PATH = "GTSPlugin/OverlayMenu";
		static constexpr const char* MENU_NAME = "GTSMenu";

		static inline ImInput* Input = nullptr;
		static inline ImWindowManager* WindowManager = nullptr;
		static inline ImGuiIO* ImIO = nullptr;
		static inline std::atomic_bool m_DrawOnPresent = false;
		static inline std::atomic_uint32_t m_localPauseCount{ 0 };

		private:
		static inline std::vector<std::string> m_hideSources = {};
		constexpr static std::string_view m_ImGuiINI = R"(Data\SKSE\Plugins\GTSPlugin\GTSPluginImGui.ini)";

		std::atomic_bool m_imGuiInitialized = false;
		std::atomic_bool m_scaleformInitialized = false;
		std::atomic_bool m_frameReady = false;
		std::atomic_bool m_isScaleformVisible = false;
		std::atomic_bool m_cursorEnabled = false;


		static inline float m_originalGameTime = 1.0f;
		inline static std::atomic_flag g_alreadyPresenting = ATOMIC_FLAG_INIT;

		public:

		//Init
		void InitScaleform();
		void InitImGui();
		static bool Ready();

		//Scaleform
		static RE::stl::owner<RE::IMenu*> Creator();

		void SetVisibility(bool a_state);

		//InputHanding
		void ProcessAndFilterEvents(RE::InputEvent** a_events) const;
		void SetInputFlags(bool a_enable);

		static void BlurBackground(bool a_enable);
		static void PauseGame(bool a_enable);
		static void AlterTimeScale(bool a_enable);
		static bool CloseInputConsumers();

		void Present();

		private:
		void Show(const std::string& source);
		void Hide(const std::string& source);

		// Inherited via EventListener
		std::string DebugName() override;
		void DataReady() override;
		void MenuChange(const MenuOpenCloseEvent* a_event) override;

		// Inherited via IMenu
		void AdvanceMovie(float a_interval, std::uint32_t a_currentTime) override;
		void PostDisplay() override;
	};
}