#include "UI/GTSMenu.hpp"
#include "UI/Windows/Other/KillFeedWindow.hpp"

#include "UI/Controls/KillEntry.hpp"
#include "UI/Lib/imgui.h"

#include "UI/Core/ImFontManager.hpp"
#include "UI/Controls/ProgressBar.hpp"
#include "UI/Core/ImColorUtils.hpp"

#include "UI/Windows/Settings/SettingsWindow.hpp"
namespace GTS {

	void KillFeedWindow::Init() {

		m_windowType = WindowType::kWidget;
		m_fadeSettings.enabled = true;
		m_name = "KillFeed";
		m_title = "KillFeed";

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
			.bLock = true,
			.f2Position = { 0.0f, 60.0f },
			.sAnchor = "kTopRight",
			.fAlpha = 1.0f,
			.fBGAlphaMult = 1.0f,
			.fWindowSizePercent = 0.0f,
			.bVisible = true,
			.bEnableFade = true,
			.fFadeAfter = 2.5f,
			.fFadeDelta = 0.01f,
		});


		this->RegisterExtraSettings(m_extraSettings);
		m_settingsHolder->SetCustomDefaults<WindowSettingsKillFeed_t>({
			
		});

		if (const auto& wSettings = dynamic_cast<SettingsWindow*>(GTSMenu::WindowManager->wSettings)) {
			m_isConfiguring = &wSettings->m_isConfiguringWidgets;
			m_settingsVisible = &wSettings->m_show;
		}

	}

	void KillFeedWindow::RequestClose() {}

	bool KillFeedWindow::WantsToDraw() {

		if (!State::InGame()) {
			return false;
		}

		if (!GetBaseSettings().bVisible) {
			return false;
		}

		//Always draw all if the widget page is open in settings
		if (*m_isConfiguring && *m_settingsVisible) {
			this->ResetFadeState();
			return true;
		}

		return true;
	}

	float KillFeedWindow::GetBackgroundAlpha() {
		return GetBaseSettings().fBGAlphaMult;
	}

	void KillFeedWindow::Draw() {
		ImGui::SetWindowSize({});

		auto& BaseSettings = GetBaseSettings();
		auto& ExtraSettings = GetExtraSettings<WindowSettingsKillFeed_t>();

		m_fadeSettings.enabled = BaseSettings.bEnableFade;
		m_fadeSettings.visibilityDuration = BaseSettings.fFadeAfter;
		bool Configuring = *m_isConfiguring && *m_settingsVisible;

		const ImVec2 Offset{ BaseSettings.f2Position.at(0), BaseSettings.f2Position.at(1) };
		ImGui::SetWindowPos(GetAnchorPos(StringToEnum<WindowAnchor>(BaseSettings.sAnchor), Offset, true));

		static ImGuiEx::KillEntry Test {
			.attacker = "A",
			.victim = "B",
			.weapon_icon = "A",
			.lifetime = 128.0f,
			.max_lifetime = 128.0f,
		};

		static ImGuiEx::KillEntry Test1 {
			.attacker = "C",
			.victim = "",
			.weapon_icon = "B",
			.lifetime = 64.0f,
			.max_lifetime = 64.0f,
		};

		ImGuiEx::DrawKillfeedEntry(Test);
		ImGuiEx::DrawKillfeedEntry(Test1);


		ImFontManager::Push(ImFontManager::ActiveFontType::kWidgetBody);

		ImFontManager::Pop();
	}
}
