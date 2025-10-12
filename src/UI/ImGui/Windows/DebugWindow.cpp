#include "UI/ImGui/Windows/DebugWindow.hpp"

#include "Config/Config.hpp"

#include "Managers/Input/InputManager.hpp"
#include "Managers/Input/ManagedInputEvent.hpp"

#include "UI/GTSMenu.hpp"
#include "UI/ImGui/Controls/CheckBox.hpp"
#include "UI/ImGui/Controls/ComboBox.hpp"

#include "Utils/Logger.hpp"

namespace GTS {

	void DebugWindow::Draw() {

		ImGuiEx::CheckBox("Show Stack", &m_showStackWindow);
		ImGuiEx::CheckBox("Show Demo", &m_showDemoWindow);
		ImGuiEx::CheckBox("Show Metrics", &m_showMetricsWindow);
		ImGuiEx::CheckBox("Show Overlay", &Config::Advanced.bShowOverlay);

		#ifdef GTS_PROFILER_ENABLED
			ImGuiEx::CheckBox("Show Profiler", &Profilers::DrawProfiler);
		#endif

	}

	bool DebugWindow::WantsToDraw() {
		return m_show;
	}

	void DebugWindow::HandleOpenClose(bool a_open) {

		if (!Plugin::Ready() && !m_show) {
			logger::warn("Can't show menu: Not Ingame!");
			return;
		}

		if (a_open) {

			if (const auto msgQueue = UIMessageQueue::GetSingleton()) {
				//The console draws above and since we disable input it becomes unclosable, we need to close it ourselves.
				msgQueue->AddMessage(RE::Console::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
			}

			m_show = true;
		}

		else {
			m_show = false;
		}
	}

	void DebugWindow::OpenSettingsKeybindCallback(const ManagedInputEvent& a_event) {
		if (auto Window = dynamic_cast<DebugWindow*>(GTSMenu::WindowManager->wDebug)) {
			Window->HandleOpenClose(true);
			return;
		}
		logger::error("Can't call handler window, pointer was invalid!");
	}

	void DebugWindow::Init() {

		m_windowType = kWidget;

		//m_flags;
		m_name = "DebugWindow";
		m_title = "Debug";
		m_windowType = kDebug;
		m_anchorPos = WindowAnchor::kTopLeft;
		m_fadeSettings.enabled = false;

		InputManager::RegisterInputEvent("OpenDebugMenu", OpenSettingsKeybindCallback);

	}

	float DebugWindow::GetFullAlpha() {
		return 0.7f;
	}

	float DebugWindow::GetBackgroundAlpha() {
		return 1.0f;
	}

	std::string DebugWindow::GetWindowName() {
		return m_name;
	}

	void DebugWindow::RequestClose() {
		HandleOpenClose(false);
	}

	void DebugWindow::DebugDraw() {

		if (m_showDemoWindow) {
			ImGui::ShowDemoWindow(&m_showDemoWindow);
		}

		if (m_showMetricsWindow) {
			ImGui::ShowMetricsWindow(&m_showMetricsWindow);
		}

		if (m_showStackWindow) {
			ImGui::ShowIDStackToolWindow(&m_showStackWindow);
		}

		#ifdef GTS_PROFILER_ENABLED
			Profilers::DisplayReport();
		#endif

	}

	bool DebugWindow::IsDebugging() {
		return m_showStackWindow       ||
			   m_showMetricsWindow     ||
			   Profilers::DrawProfiler ||
			   m_showDemoWindow;
	}
}
