#include "UI/ImGui/Windows/SplashWindow.hpp"
#include "UI/ImGui/Core/ImFontManager.hpp"
#include "Config/Util/KeybindHandler.hpp"

#include "Version.hpp"

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
		assert(false && "Eventlistener Member Destroyed, Don't do that!!");
		logger::debug("Running SplashWindow dtor");
		EventDispatcher::RemoveListener(this);
	}

	void SplashWindow::Draw() {

		ImGui::SetWindowPos(GetAnchorPos(m_anchorPos, {20.0f, 15.0f}, false));

		{
			ImFontManager::Push(ImFontManager::kWidgetTitle);
			ImGui::Text(fmt::format("Size Matters {} ", GTSPlugin::ModVersion.string(".")).c_str());

			{
				ImFontManager::Push(ImFontManager::kWidgetBody);
				ImGui::Text("Press %s or type \"gts menu\" to open the mod's settings while in-game.", GetSettingsKeybind().c_str());
				
			}

			ImFontManager::Pop(2);
		}

	}

	bool SplashWindow::WantsToDraw() {
		return m_isInMainMenu;
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
		m_title = "Splash Screen";
		m_anchorPos = WindowAnchor::kTopLeft;
		m_fadeSettings.visibilityDuration = 15.0f;
		m_fadeSettings.fadeDuration = 3.0f;
		m_fadeSettings.enabled = true;

	}

	float SplashWindow::GetFullAlpha() {
		return 0.9f;
	}

	float SplashWindow::GetBackgroundAlpha() {
		return 0.8f;
	}

	std::string SplashWindow::GetWindowName() {
		return m_name;
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
