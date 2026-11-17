#include "UI/Windows/Other/SplashWindow.hpp"
#include "UI/Core/ImFontManager.hpp"
#include "Config/Util/KeybindHandler.hpp"
#include "UI/Core/ImUtil.hpp"

#include "Version.hpp"

#include "Config/Config.hpp"

namespace {

	//Helper function to get the current keybind for opening the settings menu
	std::string GetSettingsKeybind() {
		for (const auto& evt : GTS::KeybindHandler::GetAllInputEvents()) {
			if (evt.Event == "OpenModSettings") {
				return std::accumulate(std::next(evt.Keys.begin()), evt.Keys.end(), evt.Keys.front(),[](const std::string& a, const std::string& b) {
					return a + " + " + b;
				});
			}
		}
		return "UNKNOWN";
	}

}

namespace GTS {

	SplashWindow::~SplashWindow() {
		logger::warn("Running SplashWindow dtor");
		EventDispatcher::RemoveListener(this);
	}

	void SplashWindow::Draw() {

		ImGui::SetWindowPos(GetAnchorPos(m_anchorPos, {20.0f, 15.0f}, false));

		{
			ImFontManager::Push(ImFontManager::kWidgetBody, 1.2f);

			ImGui::Text(
				"Size Matters - %s\n"
				"Press %s or type \"gts menu\" in the console to open the mod's settings while in-game.",
				GTSPlugin::ModVersion.string(".").c_str(), 
				GetSettingsKeybind().c_str()
			);

			auto ver = REL::Module::get().version();

			auto isValidGameVer =
				ver == SKSE::RUNTIME_SSE_1_5_97   || //SE
				ver == SKSE::RUNTIME_SSE_1_6_1170 || //Last 1.6 AE
				ver == SKSE::RUNTIME_SSE_1_6_1179 || //Current (17/11/26) GOG
				ver == SKSE::RUNTIME_SSE_1_7_104;    //Current (17/11/26) Steam


			if (!isValidGameVer) {

				std::string msg = fmt::format(
					"Unsupported Game version: {}!\n"
					"THE MOD MAY CRASH OR BEHAVE UNEXPECTENDLY. YOU HAVE BEEN WARNED.",
					ver.string()
				);

				ImGui::TextColored(ImUtil::Colors::Error, msg.c_str());
			}

			ImFontManager::Pop();
		}

	}

	bool SplashWindow::WantsToDraw() {
		return m_isInMainMenu && Config::Persistent.bShowSplashScreen;
	}

	void SplashWindow::Init() {

		EventDispatcher::AddListener(this);

		m_windowType = kWidget;

		m_flags = ImGuiWindowFlags_NoDecoration       |
				  ImGuiWindowFlags_NoSavedSettings    |
				  ImGuiWindowFlags_NoFocusOnAppearing |
				  ImGuiWindowFlags_NoMove             |
				  ImGuiWindowFlags_NoNav;

		m_name = "SplashWindow";
		m_title = "Startup Info";
		m_anchorPos = WindowAnchor::kTopLeft;
		m_fadeSettings.visibilityDuration = 10.0f;
		m_fadeSettings.fadeDuration = 3.0f;
		m_fadeSettings.enabled = true;

	}

	float SplashWindow::GetFullAlpha() {
		return 1.0f;
	}

	float SplashWindow::GetBackgroundAlpha() {
		return 0.7f;
	}

	std::string SplashWindow::GetWindowName() {
		return m_name;
	}

	void SplashWindow::RequestClose() {
		//Do nothing
	}

	std::string SplashWindow::DebugName() {
		return std::string("::") + m_name;
	}

	void SplashWindow::MenuChange(const RE::MenuOpenCloseEvent* a_event) {

		//If main menu is opened
		if (a_event && a_event->menuName == RE::MainMenu::MENU_NAME) {
			ResetFadeState();
			m_isInMainMenu = a_event->opening;
		}
	}
}
