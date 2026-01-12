#include "UI/GTSMenu.hpp"
#include "UI/Windows/Widgets/StatusBarWindow.hpp"
#include "UI/Core/ImColorUtils.hpp"
#include "UI/Windows/Settings/SettingsWindow.hpp"

namespace {

	void DrawWindowFrame(ImVec4 color) {
		const ImGuiStyle& style = ImGui::GetStyle();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImRect r = ImGui::GetCurrentWindow()->Rect();
		ImVec2 p_min{ r.Min.x - 1, r.Min.y - 1 };
		ImVec2 p_max{ r.Max.x - 1, r.Max.y - 1 };
		ImVec2 p_min_out{ r.Min.x, r.Min.y };
		ImVec2 p_max_out{ r.Max.x, r.Max.y };

		ImU32 brd = ImGui::GetColorU32(ImGuiCol_Border);

		draw_list->AddRectFilled(p_min, p_max, ImUtil::Colors::ImVec4ToU32(color), style.FrameRounding);
		draw_list->AddRect(p_min_out, p_max_out, brd, style.FrameRounding);
	}

}

namespace GTS {

	void StatusBarWindow::Init() {

		m_windowType = WindowType::kWidget;
		m_fadeSettings.enabled = true;
		m_name = "StatusBarWindow";
		m_title = "Buffs Bar";

		m_flags =
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoNavInputs |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoBringToFrontOnFocus;


		//Construct Base defaults for this Window
		m_settingsHolder->SetBaseDefaults({
			.bLock = true, //Always True
			.f2Position = { 122.0f, 90.0f },
			.sAnchor = "kBottomLeft",
			.fAlpha = 0.90f,
			.fBGAlphaMult = 0.3f,
			.fWindowSizePercent = 0.0f, //Unused
			.bVisible = true,
			.bEnableFade = true,
			.fFadeAfter = 2.5f,
			.fFadeDelta = 0.01f,
		});

		this->RegisterExtraSettings(m_extraSettings);
		m_settingsHolder->SetCustomDefaults<WindowSettingsStatusBar_t>({
			.iIconSize = 48,
			.iFlagsVis = 0,
			.iFlagsAS = 0,
			.f3BGColor = {0.0f, 0.0f, 0.0f}
		});

		if (const auto& wSettings = dynamic_cast<SettingsWindow*>(GTSMenu::WindowManager->wSettings)) {
			m_isConfiguring = &wSettings->m_isConfiguringWidgets;
			m_settingsVisible = &wSettings->m_show;
		}

		m_buffs = std::make_unique<ImGuiEx::StatusBar>(m_extraSettings.iIconSize, false);

	}

	void StatusBarWindow::RequestClose() {}

	bool StatusBarWindow::WantsToDraw() {

		if (!State::InGame()) {
			return false;
		}

		if (!GetBaseSettings().bVisible) {
			return false;
		}

		//Always draw all if the widget page is open in settings
		if (*m_isConfiguring && *m_settingsVisible) {
			this->ResetFadeState();
		}

		return true;
	}

	float StatusBarWindow::GetBackgroundAlpha() {
		return 0.0f;
	}

	void StatusBarWindow::Draw() {
		auto& BaseSettings = GetBaseSettings();
		auto& ExtraSettings = GetExtraSettings<WindowSettingsStatusBar_t>();

		ImGui::SetWindowSize({});

		m_fadeSettings.enabled = BaseSettings.bEnableFade;
		m_fadeSettings.visibilityDuration = BaseSettings.fFadeAfter;
		bool configuring = *m_isConfiguring && *m_settingsVisible;

		const ImVec2 offset{ BaseSettings.f2Position[0], BaseSettings.f2Position[1] };
		ImGui::SetWindowPos(GetAnchorPos(StringToEnum<WindowAnchor>(BaseSettings.sAnchor), offset, true));

		RE::Actor* target = PlayerCharacter::GetSingleton();

		static int NumDrawn = 0;
		if (NumDrawn > 0) {
			DrawWindowFrame(ImUtil::Colors::AdjustAlpha(
				ImUtil::Colors::fRGBToImVec4(ExtraSettings.f3BGColor),
				BaseSettings.fBGAlphaMult * m_fadeSettings.fadeAlpha)
			);
		}

		bool stateChanged = false;
		m_buffs->m_iconSize = ExtraSettings.iIconSize;
		NumDrawn = m_buffs->Draw(target, ExtraSettings.iFlagsVis, ExtraSettings.iFlagsAS, &stateChanged, configuring);

		if (stateChanged) {
			ResetFadeState();
		}

		// Workaround for ImGui Dummy padding bug
		if (ImGuiWindow* window = GImGui->CurrentWindow) {
			window->DC.IsSetPos = false;
		}
	}
}
