#include "UI/GTSMenu.hpp"
#include "UI/Windows/Widgets/SizeBarWindow.hpp"

#include "Config/Config.hpp"

#include "UI/Core/ImFontManager.hpp"
#include "UI/Controls/ProgressBar.hpp"
#include "UI/Core/ImColorUtils.hpp"

#include "UI/Windows/Settings/SettingsWindow.hpp"
namespace GTS {

	void SizeBarWindow::Init() {

		m_windowType = WindowType::kWidget;
		m_fadeSettings.enabled = true;
		m_name = "Sizebar";
		m_title = "Size Bar Widget";

		DeriveIdentityFromName();

		m_flags =
			ImGuiWindowFlags_NoCollapse             |
			ImGuiWindowFlags_NoTitleBar             |
			ImGuiWindowFlags_NoNav                  |
			ImGuiWindowFlags_NoNavFocus             |
			ImGuiWindowFlags_NoNavInputs            |
			ImGuiWindowFlags_NoScrollbar            |
			ImGuiWindowFlags_NoScrollWithMouse      |
			ImGuiWindowFlags_NoResize               |
			ImGuiWindowFlags_NoMove                 |
			ImGuiWindowFlags_NoSavedSettings        |
			ImGuiWindowFlags_NoFocusOnAppearing     |
			ImGuiWindowFlags_NoBringToFrontOnFocus;


		//Construct Base defaults for this Window
		m_settingsHolder->SetBaseDefaults({
			.bLock = true, //Always True
			.f2Position = { 0.0f, 60.0f + (30.0f * m_identity) },
			.sAnchor = "kCenter",
			.fAlpha = 1.0f,
			.fBGAlphaMult = 1.0f,       //Unused
			.fWindowSizePercent = 0.0f, //Unused
			.bVisible = m_identity < 2, //Show player and 1st teammate by default
			.bEnableFade = true,
			.fFadeAfter = 2.5f,
			.fFadeDelta = 0.01f,
		});

		// Deterministic accent generation & Generate paired accent hues
		std::array<float, 3> baseHSV = ImUtil::Colors::RGBtoHSV(Config::UI.f3AccentColor);
		constexpr float golden_ratio_conjugate = 0.6180339887f;
		float hueShift = std::fmod(static_cast<float>(m_identity) * golden_ratio_conjugate, 1.0f);
		std::array<float, 3> colorA = ImUtil::Colors::ShiftHue(baseHSV, hueShift);
		std::array<float, 3> colorB = ImUtil::Colors::ShiftHue(baseHSV, hueShift + 0.08f);

		this->RegisterExtraSettings(m_extraSettings);
		m_settingsHolder->SetCustomDefaults<WindowSettingsSizeBar_t>({
			.bShowName = m_identity > 1,  //Show name for not default shown teammates by default
			.bShowScale = true,
			.bShowSize = true,
			.fBorderThickness = 1.45f,
			.fBorderLightness = 0.4f,
			.fRounding = 3.0f,
			.fBorderAlpha = 0.3f,
			.f2GradientRange = {0.7f, 1.3f },
			.f3ColorA = colorA, 
			.f3ColorB = colorB,
			.f2Size = { 325.0f, m_identity != 0 ? 0.30f : 0.40f }, //Slightly taller for player
			.iFlags = ImGuiEx::ImGuiExProgresbarFlag_Gradient | 
					  ImGuiEx::ImGuiExProgresbarFlag_Rounding
		});

		if (const auto& wSettings = dynamic_cast<SettingsWindow*>(GTSMenu::WindowManager->wSettings)) {
			m_isConfiguring = &wSettings->m_isConfiguringWidgets;
			m_settingsVisible = &wSettings->m_show;
		}

	}

	void SizeBarWindow::RequestClose() {}

	bool SizeBarWindow::WantsToDraw() {

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

		// Handle teammate index logic
		if (m_identity == 0) {
			// Always draw for player
			return true;
		}

		const uint8_t count = static_cast<uint8_t>(GTSMenu::WindowManager->GetCachedTeamMateList().size());
		if (m_identity > 0 && m_identity <= count && m_identity != 255) {
			// Only draw if teammate exists at that index
			return true;
		}

		return false;
	}

	float SizeBarWindow::GetBackgroundAlpha() {
		return 0.0f;
	}

	void SizeBarWindow::DeriveIdentityFromName() {

		if (m_instanceName.empty()) {
			m_identity = 255;
			return;
		}

		const char last = m_instanceName.back();
		if (last == 'P') m_identity = 0;
		else if (std::isdigit(static_cast<unsigned char>(last)))  m_identity = last - '0';
		else m_identity = 255;
		
	}

	void SizeBarWindow::Draw() {
		ImGui::SetWindowSize({});

		auto& BaseSettings = GetBaseSettings();
		auto& ExtraSettings = GetExtraSettings<WindowSettingsSizeBar_t>();

		m_fadeSettings.enabled = BaseSettings.bEnableFade;
		m_fadeSettings.visibilityDuration = BaseSettings.fFadeAfter;
		bool Configuring = *m_isConfiguring && *m_settingsVisible;

		const ImVec2 Offset{ BaseSettings.f2Position.at(0), BaseSettings.f2Position.at(1) };
		ImGui::SetWindowPos(GetAnchorPos(StringToEnum<WindowAnchor>(BaseSettings.sAnchor), Offset, true));

		RE::Actor* Target = nullptr;
		float fScaleCurrent = 1.0f;
		float fScaleMax = 1.0f;
		std::string sName;

		if (Configuring) {
			sName = "Name";
		}
		else {
			Target = PlayerCharacter::GetSingleton();

			if (m_identity != 0) {
				const auto& List = GTSMenu::WindowManager->GetCachedTeamMateList();
				if (List.size() < m_identity) {
					logger::warn("SizeBarWindow::Draw() - Teammate index {} out of bounds ({} teammates)", m_identity, List.size());
					return;
				}
				Target = List[m_identity - 1];
			}

			if (!Target || !Target->Get3D(false)) return;

			const auto& P = Persistent::GetActorData(Target);
			const auto& T = Transient::GetActorData(Target);
			if (!P || !T) return;

			sName = Target->GetName();
			fScaleCurrent = get_visual_scale(Target);
			fScaleMax = get_max_scale(Target);
		}

		float fScaleProgress = (fScaleMax < 250.0f) ? fScaleCurrent / fScaleMax : -1.0f;

		if (std::abs(fScaleCurrent - m_prevSize) >= BaseSettings.fFadeDelta) {
			m_prevSize = fScaleCurrent;
			this->ResetFadeState();
		}

		std::string sFmtTxt;

		if (ExtraSettings.bShowName) {
			sFmtTxt += sName;
		}

		if (ExtraSettings.bShowSize) {
			if (!sFmtTxt.empty()) sFmtTxt += ": ";
			sFmtTxt += [&]() {
				return Configuring ? GetFormatedHeight(1.0f) : GetFormatedHeight(Target);
			}();
		}

		if (ExtraSettings.bShowScale) {
			if (!sFmtTxt.empty()) sFmtTxt += " ";
			if (ExtraSettings.bShowSize) {
				sFmtTxt += fmt::format("({:.2f}x)", fScaleCurrent);
			}
			else {
				sFmtTxt += fmt::format("{:.2f}x", fScaleCurrent);
			}
		}

		ImFontManager::Push(ImFontManager::ActiveFontType::kWidgetBody);

		ImGuiEx::ProgressBar(
			fScaleProgress == -1.0f ? 1.0f : fScaleProgress,
			{ ExtraSettings.f2Size[0], 0.0f },
			sFmtTxt.c_str(),
			ExtraSettings.iFlags,
			ExtraSettings.f2Size[1], ExtraSettings.fBorderThickness,
			ExtraSettings.fRounding,
			ExtraSettings.f2GradientRange[0], ExtraSettings.f2GradientRange[1],
			ImUtil::Colors::fRGBToU32(ExtraSettings.f3ColorA),
			ImUtil::Colors::fRGBToU32(ExtraSettings.f3ColorB),
			ImUtil::Colors::AdjustGrayScaleLightness(ExtraSettings.fBorderLightness, ExtraSettings.fBorderAlpha)
		);

		ImFontManager::Pop();
	}
}
