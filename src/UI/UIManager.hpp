#pragma once

#include "ImGui/ImGraphics.hpp"
#include "ImGui/ImInput.hpp"
#include "ImGui/ImWindow.hpp"
#include "ImGui/ImWindowManager.hpp"

namespace GTS {

	class UIManager : public CInitSingleton<UIManager> {

		private:
		constexpr static std::string_view m_ImGuiINI = R"(Data\SKSE\Plugins\GTSPlugin\GTSPluginImGui.ini)";
		std::atomic_bool m_initialized = false;
		std::atomic_bool m_frameReady = false;
		std::atomic_bool m_gamePaused = false;
		float m_unpausedGameTime = 1.0f;
		inline static std::atomic_flag g_alreadyPresenting = ATOMIC_FLAG_INIT;

		public:
		static inline ImGraphics* Graphics = nullptr;
		static inline ImInput* Input = nullptr;
		static inline ImWindowManager* WindowManager = nullptr;

		static void ShowInfos();
		static void CloseSettings();
		void Init();
		void Update();
		void Present(ImWindow::DrawLevel a_level);

		[[nodiscard]] static bool Ready();
		static void ProcessAndFilterEvents(RE::InputEvent** a_events);

	};
}
