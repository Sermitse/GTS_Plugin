#pragma once
#include "ImGui/ImGraphics.hpp"
#include "ImGui/ImWindowManager.hpp"
#include "ImGui/ImInput.hpp"

namespace GTS {

	class UIManager : public ImInput {

		private:

		UIManager(const UIManager&) = delete;
		UIManager& operator=(const UIManager&) = delete;

		std::atomic_bool Initialized = false; 
		UIManager() = default;
		~UIManager() = default;

		ImWindowManager& WinMgr = ImWindowManager::GetSingleton();
		ImFontManager& FontMgr = ImFontManager::GetSingleton();
		ImStyleManager& StyleMgr = ImStyleManager::GetSingleton();


		public:
		static inline std::atomic_bool ShouldDrawOverTop = false;
		static inline std::atomic_bool GamePaused = false;
		static inline float UnPausedGameTime = 1.0f;
		static inline ImGraphics* Graphics = nullptr;

		static UIManager& GetSingleton() {
			static UIManager Instance;
			return Instance;
		}

		static void ShowInfos();
		static void CloseSettings();
		void Init();
		void Draw();

		[[nodiscard]] inline bool Ready() const {
			return Initialized.load();
		}

		[[nodiscard]] inline bool InputUpdate(RE::InputEvent** a_event) {

			ProcessInputEvents(a_event);

			if (ImWindowManager::GetSingleton().HasInputConsumers()) {  //the menu is open, eat all keypresses
				return true;
			}

			OnFocusLost();

			return false;

		}

		[[nodiscard]] static inline bool MenuOpen() {
			return ImWindowManager::GetSingleton().HasInputConsumers();
		}

		constexpr static inline std::string_view ImGuiINI = R"(Data\SKSE\Plugins\GTSPlugin\GTSPluginImGui.ini)";

	};
}
