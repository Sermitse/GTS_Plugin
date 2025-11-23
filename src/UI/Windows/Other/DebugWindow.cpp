#include "UI/Windows/Other/DebugWindow.hpp"
#include "UI/Controls/Button.hpp"
#include "UI/Controls/CheckBox.hpp"
#include "UI/GTSMenu.hpp"

#include "Config/Config.hpp"

#include "Managers/Input/InputManager.hpp"
#include "Managers/Input/ManagedInputEvent.hpp"

#include "UI/Core/ImUtil.hpp"


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

		if (ImGui::CollapsingHeader("Graphics Transform Test")) {

			// Create a transformation with continuously changing values
			static ImGraphics::ImageTransform transform;
			static ImGraphics::ImageTransform transform2;
			static ImGraphics::ImageTransform transform3;
			static ImGraphics::ImageTransform transform4;

			static float time = 0.0f;

			// Increment time
			time += ImGui::GetIO().DeltaTime;

			// Oscillating recolor through RGB spectrum
			transform.recolorEnabled = true;
			float hue = std::fmod(time * 0.3f, 1.0f); // Cycle through hue every ~3.3 seconds
			float r = std::abs(std::sin(hue * std::numbers::pi * 2.0f));
			float g = std::abs(std::sin((hue + 0.333f) * std::numbers::pi * 2.0f));
			float b = std::abs(std::sin((hue + 0.666f) * std::numbers::pi * 2.0f));
			transform.targetColor = { r, g, b, 1.0f };

			// Continuous rotation (full rotation every 4 seconds)
			transform2.affine.rotation = time * (std::numbers::pi / 2.0f);

			// Pulsing scale (between 0.8x and 1.8x)
			float scaleValue = 1.3f + 0.5f * std::sin(time * 2.0f);
			transform.affine.scale = { scaleValue, scaleValue };

			// Oscillating translation
			transform3.affine.translation.x = 10.0f * std::sin(time * 1.5f);
			transform3.affine.translation.y = 10.0f * std::cos(time * 1.5f);

			// Flip every 2 seconds
			transform3.affine.flipHorizontal = static_cast<int>(time / 2.0f) % 2 == 0;
			transform3.affine.flipVertical = static_cast<int>(time / 3.0f) % 2 == 0;

			// Cycling cutoff direction every 5 seconds
			int dirIndex = static_cast<int>(time / 5.0f) % 4;
			switch (dirIndex) {
				case 0: transform4.cutoffDir = ImGraphics::CutoffDirection::LeftToRight; break;
				case 1: transform4.cutoffDir = ImGraphics::CutoffDirection::RightToLeft; break;
				case 2: transform4.cutoffDir = ImGraphics::CutoffDirection::TopToBottom; break;
				case 3: transform4.cutoffDir = ImGraphics::CutoffDirection::BottomToTop; break;
				default: break;
			}

			// Oscillating cutoff percentage (0% to 100% and back)
			transform4.cutoffPercent = 0.5f + 0.5f * std::sin(time);

			ImGui::Text("Time: %.2f", time);
			ImGui::Text("Color: R:%.2f G:%.2f B:%.2f", r, g, b);
			ImGui::Text("Rotation: %.2f rad (%.1f deg)", transform2.affine.rotation, transform2.affine.rotation * 180.0f / std::numbers::pi);
			ImGui::Text("Scale: %.2f", scaleValue);
			ImGui::Text("Cutoff: %s %.1f%%", dirIndex == 0 ? "L->R" : dirIndex == 1 ? "R->L" : dirIndex == 2 ? "T->B" : "B->T", transform4.cutoffPercent * 100.0f);

			ImGraphics::RenderTransformed(ImageList::PlaceHolder, transform,  { 64, 64 });
			ImGui::SameLine();
			ImGraphics::RenderTransformed(ImageList::PlaceHolder, transform2, { 64, 64 }); 
			ImGui::SameLine();
			ImGraphics::RenderTransformed(ImageList::PlaceHolder, transform3, { 64, 64 });
			ImGui::SameLine();
			ImGraphics::RenderTransformed(ImageList::PlaceHolder, transform4, { 64, 64 });

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

		if (!State::Ready() && !m_show) {
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
