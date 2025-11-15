#include "UI/GTSMenu.hpp"
#include "UI/Windows/Widgets/StatusBarWindow.hpp"

#include "UI/Core/ImFontManager.hpp"
#include "UI/Lib/imgui.h"
#include "UI/Lib/imgui_internal.h"

#include "UI/Windows/Settings/SettingsWindow.hpp"

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
			.iFlags = 0
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
		return GetBaseSettings().fBGAlphaMult;
	}


	//Todo Fix
	//Instead of autosize

	void StatusBarWindow::Draw() {

		auto& BaseSettings = GetBaseSettings();
		auto& ExtraSettings = GetExtraSettings<WindowSettingsStatusBar_t>();

		//const ImVec2 Size = {
		//	((ExtraSettings.iIconSize * 6) + (ImGui::GetStyle().FramePadding.x * 2.0f) + (ImGui::GetStyle().ItemSpacing.x * 5)),
		//	((ExtraSettings.iIconSize) + (ImGui::GetStyle().FramePadding.y * 2.0f))
		//};

		ImGui::SetWindowSize({});

		m_fadeSettings.enabled = BaseSettings.bEnableFade;
		m_fadeSettings.visibilityDuration = BaseSettings.fFadeAfter;
		bool Configuring = *m_isConfiguring && *m_settingsVisible;

		const ImVec2 Offset{ BaseSettings.f2Position.at(0), BaseSettings.f2Position.at(1) };
		ImGui::SetWindowPos(GetAnchorPos(StringToEnum<WindowAnchor>(BaseSettings.sAnchor), Offset, true));

		RE::Actor* Target = PlayerCharacter::GetSingleton();

		bool stateChanged = false;
		m_buffs->m_iconSize = ExtraSettings.iIconSize;
		int drawnIcons = m_buffs->Draw(Target, ExtraSettings.iFlags, &stateChanged, Configuring);

		if (stateChanged) {
			this->ResetFadeState();
		}

		{
			if (drawnIcons == 0 && this->m_fadeSettings.enabled) {
				this->m_fadeSettings.isFading = false;
				this->m_fadeSettings.fadeAlpha = 0.0f;
				this->m_fadeSettings.visibilityTimer = 0.0f;

			}

			//const ImVec2 Size = {
			//	((ExtraSettings.iIconSize * 2) + (ImGui::GetStyle().FramePadding.x * 2.0f)),
			//	((ExtraSettings.iIconSize) + (ImGui::GetStyle().FramePadding.y * 2.0f))
			//};

			//if (drawnIcons == 0 && !this->m_fadeSettings.enabled) {
			//	ImFontManager::Push(ImFontManager::kWidgetBody, ExtraSettings.iIconSize / 64.0f);
			//	if (BaseSettings.fBGAlphaMult > 0.01f) {
			//		ImGui::SetWindowSize(Size);
			//		const char* txt = "No Buffs Active";
			//		const ImVec2 textSize = ImGui::CalcTextSize(txt);
			//		ImGui::SetCursorPos({
			//			Size.x * .5f - (textSize.x * .5f),
			//			Size.y * .5f - (textSize.y * .5f)
			//		});
			//		ImGui::Text(txt);
			//	}
			//	ImFontManager::Pop();
			//}

			//Workaround
			//Dummy adds nonsensical padding otherwise.
			ImGuiContext& g = *GImGui;
			ImGuiWindow* window = g.CurrentWindow;
			if (window) {
				window->DC.IsSetPos = false;
			}

		}
	}
}
