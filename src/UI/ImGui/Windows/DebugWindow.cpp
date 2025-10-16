#include "UI/ImGui/Windows/DebugWindow.hpp"
#include "UI/ImGui/Controls/Button.hpp"
#include "UI/ImGui/Controls/CheckBox.hpp"
#include "UI/GTSMenu.hpp"

#include "Config/Config.hpp"

#include "Managers/Input/InputManager.hpp"
#include "Managers/Input/ManagedInputEvent.hpp"

#include "UI/ImGui/Core/ImUtil.hpp"


namespace GTS {

	void DebugWindow::Draw() {

		ImGui::PushFont(nullptr, 18);

		if (ImGuiEx::Button("Close")) {
			HandleOpenClose(false);
		}

		if (ImGui::CollapsingHeader("GTS Debug")) {
			ImGuiEx::CheckBox("Show Overlay", &Config::Advanced.bShowOverlay);

			#ifdef GTS_PROFILER_ENABLED
			ImGuiEx::CheckBox("Show Profiler", &Profilers::DrawProfiler);
			#endif
		}

		if (ImGui::CollapsingHeader("ImGui Debug Windows")) {
			ImGuiEx::CheckBox("Show Stack", &m_showStackWindow);
			ImGuiEx::CheckBox("Show Demo", &m_showDemoWindow);
			ImGuiEx::CheckBox("Show Metrics", &m_showMetricsWindow);
		}

		if (ImGui::CollapsingHeader("Graphics Test")) {
			ImGraphics::DebugDrawTest();
			ImGui::Spacing();
		}

		//Multi-Language Font Test
		if (ImGui::CollapsingHeader("Font Test")) {
			ImGui::Text("This îs à fónt tèst — façade, naïve, jalapeño, groß, déjà vu, fiancée, coöperate, élève");
			ImGui::Text("Αυτή είναι μια δοκιμή για το σύστημα γραμματοσειράς");
			ImGui::Text("Это тест загрузчика шрифтов");
			ImGui::Text("これはフォントローダーのテストです");
			ImGui::Text("이것은 폰트로더 테스트입니다");
			ImGui::Text("这是一个字体加载器测试");
			ImGui::Spacing();
		}

		if (ImGui::CollapsingHeader("Misc")) {
			if (ImGuiEx::Button("Quit", "This will immediatly close the game.", false, 1.0f)) {
				SKSE::WinAPI::TerminateProcess(SKSE::WinAPI::GetCurrentProcess(), EXIT_SUCCESS);
			}

			ImGui::SameLine();

			if (ImGuiEx::Button("Trigger Crash", "This will immediatly crash the game.", false, 1.0f)) {
				using FuncType = void(*)();
				FuncType func = nullptr;
				func();
			}
		}

		ImGui::PopFont();
		ImGui::Spacing();

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

			if (!Config::Hidden.IKnowWhatImDoing) {
				return;
			}

			Window->HandleOpenClose(true);
			return;
		}
		logger::error("Can't call handler window, pointer was invalid!");
	}

	void DebugWindow::Init() {

		m_windowType = kWidget;

		//m_flags;
		m_name = "Debug";
		m_title = "Debug";
		m_windowType = kDebug;
		m_anchorPos = WindowAnchor::kTopLeft;
		m_fadeSettings.enabled = false;

		InputManager::RegisterInputEvent("OpenDebugMenu", OpenSettingsKeybindCallback);

	}

	float DebugWindow::GetFullAlpha() {
		return 1.0f;
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

		GTS_PROFILER_DISPLAY_REPORT();

	}

	bool DebugWindow::IsDebugging() {
		return m_showStackWindow       ||
			   m_showMetricsWindow     ||
            #ifdef GTS_PROFILER_ENABLED
			   Profilers::DrawProfiler ||
            #endif
			   m_showDemoWindow;
	}
}
