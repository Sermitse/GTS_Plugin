#include "UI/GTSMenu.hpp"
#include "UI/Windows/Widgets/USBarWindow.hpp"

#include "Config/Config.hpp"

#include "Managers/Animation/Stomp_Under.hpp"
#include "Managers/Cameras/CamUtil.hpp"

#include "UI/Core/ImFontManager.hpp"
#include "UI/Controls/ProgressBar.hpp"
#include "UI/Core/ImColorUtils.hpp"

#include "UI/Windows/Settings/SettingsWindow.hpp"


namespace {

	void CalcUnderstomp(float& a_outangleScale, float& a_OutangleAbs) {

		if (GTS::IsFreeCameraEnabled()) {
			return;
		}

		// Range between -1 (looking down) and 1 (looking up)
		const float absPitch = std::abs(GTS::GetCameraRotation().entry[2][1]);

		// Convert normalized pitch to angle in degrees (0 looking forward, 90 fully up or down)
		a_OutangleAbs = absPitch * 90.0f;

		// Remap our starting range
		constexpr float InvLookDownStartAngle = 0.9f;
		const float InvLookdownIntensity = std::clamp(
			GTS::AnimationUnderStomp::Remap(absPitch, 1.0f, InvLookDownStartAngle, 0.0f, 1.0f),
			0.0f, 1.0f
		);

		if (absPitch > InvLookDownStartAngle) {
			a_outangleScale = 1.0f - std::clamp(InvLookdownIntensity * 1.3f, 0.0f, 1.0f);
		}
	}
}

namespace GTS {

	void USBarWindow::Init() {

		m_windowType = WindowType::kWidget;
		m_fadeSettings.enabled = true;
		m_name = "UnderstompAngleBar";
		m_title = "Understomp Angle Bar Widget";

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
			.f2Position = { 0.0f, 0.0f },
			.sAnchor = "kCenter",
			.fAlpha = 1.0f,
			.fBGAlphaMult = 1.0f,       //Unused
			.fWindowSizePercent = 0.0f, //Unused
			.bVisible = true,           //Show player and 1st teammate by default
			.bEnableFade = true,
			.fFadeAfter = 2.5f,
			.fFadeDelta = 0.01f,
		});

		std::array<float, 3> colorA = Config::UI.f3AccentColor;
		std::array<float, 3> colorB = ImUtil::Colors::ShiftHue(ImUtil::Colors::RGBtoHSV(Config::UI.f3AccentColor), 0.08f);

		this->RegisterExtraSettings(m_extraSettings);
		m_settingsHolder->SetCustomDefaults<WindowSettingsUnderstompBar_t>({
			.bShowScale = true,
			.bShowAbsoluteAngle = false,
			.fBorderThickness = 1.45f,
			.fBorderLightness = 0.4f,
			.fRounding = 3.0f,
			.fBorderAlpha = 0.3f,
			.f2GradientRange = {0.7f, 1.3f },
			.f3ColorA = colorA,
			.f3ColorB = colorB,
			.f2Size = { 150.0f, 0.40f },
			.iFlags = ImGuiEx::ImGuiExProgresbarFlag_Gradient |
					  ImGuiEx::ImGuiExProgresbarFlag_Rounding
		});

		if (const auto& wSettings = dynamic_cast<SettingsWindow*>(GTSMenu::WindowManager->wSettings)) {
			m_isConfiguring = &wSettings->m_isConfiguringWidgets;
			m_settingsVisible = &wSettings->m_show;
		}

	}

	void USBarWindow::RequestClose() {}

	bool USBarWindow::WantsToDraw() {

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

	float USBarWindow::GetBackgroundAlpha() {
		return 0.0f;
	}

	void USBarWindow::Draw() {
		ImGui::SetWindowSize({});

		auto& BaseSettings = GetBaseSettings();
		auto& ExtraSettings = GetExtraSettings<WindowSettingsUnderstompBar_t>();

		m_fadeSettings.enabled = BaseSettings.bEnableFade;
		m_fadeSettings.visibilityDuration = BaseSettings.fFadeAfter;
		bool Configuring = *m_isConfiguring && *m_settingsVisible;

		const ImVec2 Offset{ BaseSettings.f2Position.at(0), BaseSettings.f2Position.at(1) };
		ImGui::SetWindowPos(GetAnchorPos(StringToEnum<WindowAnchor>(BaseSettings.sAnchor), Offset, true));

		static float fAngleCurrent = 1.0f;
		static float fAngleAbs = 1.0f;
		RE::Actor* Target = PlayerCharacter::GetSingleton();

		if (!Target || !Target->Get3D(false)) return;
		CalcUnderstomp(fAngleCurrent, fAngleAbs);

		float fScaleProgress = !Configuring ? fAngleCurrent / 1.0f : 1.0f;

		if (std::abs(fAngleCurrent - m_prevAngle) >= BaseSettings.fFadeDelta) {
			m_prevAngle = fAngleCurrent;
			this->ResetFadeState();
		}

		std::string sFmtTxt;

		// Resolve effective progress used for rendering
		const float progress = (fScaleProgress == -1.0f) ? 1.0f : fScaleProgress;

		// Extremes override everything
		if (progress <= 1e-3f && !Configuring) {
			sFmtTxt = "Far";
		}
		else if (progress >= 1.0f - 1e-3f && !Configuring) {
			sFmtTxt = "Under";
		}

		else {
			if (ExtraSettings.bShowScale) {
				sFmtTxt += !Configuring ? fmt::format("{:.2f}x", fAngleCurrent) : "1.00x";
			}

			if (ExtraSettings.bShowAbsoluteAngle) {
				if (!sFmtTxt.empty()) {
					sFmtTxt += " | ";
				}
				sFmtTxt += !Configuring ? fmt::format("{:.1f}°", fAngleAbs) : "90.0°";
			}
		}

		ImFontManager::Push(ImFontManager::ActiveFontType::kWidgetBody);

		ImGuiEx::ProgressBar(
			progress,
			{ ExtraSettings.f2Size[0], 0.0f },
			sFmtTxt.c_str(),
			ExtraSettings.iFlags,
			ExtraSettings.f2Size[1],
			ExtraSettings.fBorderThickness,
			ExtraSettings.fRounding,
			ExtraSettings.f2GradientRange[0],
			ExtraSettings.f2GradientRange[1],
			ImUtil::Colors::fRGBToU32(ExtraSettings.f3ColorA),
			ImUtil::Colors::fRGBToU32(ExtraSettings.f3ColorB),
			ImUtil::Colors::AdjustGrayScaleLightness(
				ExtraSettings.fBorderLightness,
				ExtraSettings.fBorderAlpha)
		);

		ImFontManager::Pop();
	}

}
