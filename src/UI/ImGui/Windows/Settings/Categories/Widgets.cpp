#include "UI/ImGui/Windows/Settings/Categories/Widgets.hpp"

#include "UI/GTSMenu.hpp"

#include "UI/ImGui/Controls/Button.hpp"
#include "UI/ImGui/Controls/CheckBox.hpp"
#include "UI/ImGui/Controls/CollapsingTabHeader.hpp"
#include "UI/ImGui/Controls/ComboBox.hpp"
#include "UI/ImGui/Controls/ProgressBar.hpp"
#include "UI/ImGui/Controls/Slider.hpp"

#include "UI/ImGui/Core/ImUtil.hpp"

#include "UI/ImGui/Windows/SizeBarWindow.hpp"
#include "UI/ImGui/Windows/StatusBarWindow.hpp"
#include "UI/ImGui/Windows/USBarWindow.hpp"

namespace {

	// Common Bar Tooltip strings
	PSString TCVis = "Toggle widget visibility.";
	PSString TCAnchor = "Set the anchor point.";
	PSString TCPosOffset = "Offset the widget position relative to the selected anchor point.";
	PSString TCHeight = "Adjust the height of the progress bar.";
	PSString TCWidth = "Adjust the length of the progress bar.";
	PSString TCAlpha = "Adjust the overall transparency of progress bar.";
	PSString TCIFadeEn = "Enable progress bar hide after a specified amount of inactivity.";
	PSString TCIFadeTime = "Set the time in seconds before the progress bar starts to fade after inactivity.";
	PSString TCIFadeDelta = "Set the minimum required amount of change for the progress bar to reappear again after being hidden.";
	PSString TCThickness = "Adjust the thickness of the progress bar's border.";
	PSString TCBorderCol = "Adjust the grayscale lightness/darkness of the progress bar's border.";
	PSString TCBorderAlpha = "Adjust the transparency of the progress bar's border.";
	PSString TCGradientEn = "Enable color gradients (will progressively darken/lighten the edges of the filled section of the progress bar).";
	PSString TCDualColor = "Use seperate collors for the gradients.";
	PSString TCSwapDir = "Swap the direction of the gradient.";
	PSString TCGradientIntens = "Adjust the lightness/darkness intensity of the single color gradient.";
	PSString TCRoundingEn = "Enable rounded corners for the progress bar.";
	PSString TCRoundingAmt = "Adjust the amount of rounding for the progress bar corners.";

	//Specific to USBar
	PSString TShowUStompMult = "Show the effective understopm angle animation offset multiplier.";
	PSString TShowAbsAngle = "Show the current absolute camera up/down angle in degrees.";

	//Specific to SizeBar
	PSString TShowSName = "Show the name of the Actor being represented by this size bar.";
	PSString TShowSScale = "Show the current size scale of the Actor being represented by this size bar.";
	PSString TShowSSize = "Show the current formated size (in feet/meters) of the Actor being represented by this size bar.";

	//Specific to Icon/Buff/Status Bar
	PSString TFrameAlpha = "Set the transparency level for the window containing the statusbar.";
	PSString TIcoVis = "Toggle icon visibility.";
	PSString TIcoSize = "Set the size of the icons.";

	template<typename SettingsType>
	void DrawCommonWidgetOptions(auto& BaseSettings, SettingsType& ExtraSettings) {
		ImGuiEx::CheckBox("Enable", &ExtraSettings.bVisible, TCVis);

		// Anchor & Position
		ImGuiEx::ComboEx<GTS::ImWindow::WindowAnchor>("Anchor", BaseSettings.sAnchor, TCAnchor);
		ImGuiEx::SliderF("Position Up/Down", &BaseSettings.f2Position.at(1), 0.0f, 720.f, TCPosOffset, "%.1f");

		if (BaseSettings.sAnchor != "kCenter") {
			ImGuiEx::SliderF("Position Left/Right", &BaseSettings.f2Position.at(0), 0.0f, 1280.f, TCPosOffset, "%.1f");
		}

		ImGui::Spacing();

		// Fade Settings
		ImGuiEx::CheckBox("Inactivity Fade", &ExtraSettings.bEnableFade, TCIFadeEn);
		ImGuiEx::SliderF("Fade After", &ExtraSettings.fFadeAfter, 0.5f, 10.0f, TCIFadeTime, "After %.1f Seconds", !ExtraSettings.bEnableFade);
		ImGuiEx::SliderF("Reappear Delta", &ExtraSettings.fFadeDelta, 0.0, 0.5f, TCIFadeDelta, "After a %.2fx Difference", !ExtraSettings.bEnableFade);

		ImGuiEx::SliderF("Alpha", &BaseSettings.fAlpha, 0.1f, 1.0f, TCAlpha, "%.2fx");
	}

	template<typename SettingsType>
	void DrawCommonBarOptions(SettingsType& ExtraSettings) {
		// Toggles
		bool useGradient = ExtraSettings.iFlags & ImGuiEx::ImGuiExProgresbarFlag_Gradient;
		bool multiColor = ExtraSettings.iFlags & ImGuiEx::ImGuiExProgresbarFlag_DualColor;
		bool flipDir = ExtraSettings.iFlags & ImGuiEx::ImGuiExProgresbarFlag_SwapGradientDir;
		bool rounding = ExtraSettings.iFlags & ImGuiEx::ImGuiExProgresbarFlag_Rounding;

		ImGuiEx::SliderF("Height", &ExtraSettings.f2Size.at(1), 0.1f, 3.0f, TCHeight, "%.2fx");
		ImGuiEx::SliderF("Length", &ExtraSettings.f2Size.at(0), 50.0f, 700.0f, TCWidth, "%.0f");

		ImGui::Spacing();

		// Border Settings
		ImGuiEx::SliderF("Border Thickness", &ExtraSettings.fBorderThickness, 0.0f, 5.0f, TCThickness, "%.2fx");
		ImGuiEx::SliderF("Border Lightness", &ExtraSettings.fBorderLightness, 0.0f, 1.0f, TCBorderCol, "%.2fx");
		ImGuiEx::SliderF("Border Alpha", &ExtraSettings.fBorderAlpha, 0.0f, 1.0f, TCBorderAlpha, "%.2fx");

		ImGui::Spacing();

		// Rounding
		if (ImGuiEx::CheckBox("Rounding", &rounding, TCRoundingEn)) {
			ImUtil::ToggleFlag(ExtraSettings.iFlags, ImGuiEx::ImGuiExProgresbarFlag_Rounding, rounding);
		}

		if (rounding) {
			ImGuiEx::SliderF("Rounding Amount", &ExtraSettings.fRounding, 0.1f, 10.0f, TCRoundingAmt, "%.1fx");
		}

		ImGui::Spacing();

		// Gradient Settings
		if (ImGuiEx::CheckBox("Gradient", &useGradient, TCGradientEn)) {
			ImUtil::ToggleFlag(ExtraSettings.iFlags, ImGuiEx::ImGuiExProgresbarFlag_Gradient, useGradient);
		}

		ImGui::BeginDisabled(!useGradient);
		{
			ImGui::SameLine();

			if (ImGuiEx::CheckBox("Dual Color", &multiColor, TCDualColor)) {
				ImUtil::ToggleFlag(ExtraSettings.iFlags, ImGuiEx::ImGuiExProgresbarFlag_DualColor, multiColor);
			}

			ImGui::SameLine();

			if (ImGuiEx::CheckBox("Flip Direction", &flipDir, TCSwapDir)) {
				ImUtil::ToggleFlag(ExtraSettings.iFlags, ImGuiEx::ImGuiExProgresbarFlag_SwapGradientDir, flipDir);
			}
		}
		ImGui::EndDisabled();
	}

	template<typename SettingsType>
	void DrawColorOptions(SettingsType& ExtraSettings) {
		bool useGradient = ExtraSettings.iFlags & ImGuiEx::ImGuiExProgresbarFlag_Gradient;
		bool multiColor = ExtraSettings.iFlags & ImGuiEx::ImGuiExProgresbarFlag_DualColor;

		ImGui::ColorEdit3("Base Color", ExtraSettings.f3ColorA.data(), ImGuiColorEditFlags_DisplayHSV);

		if (useGradient) {
			if (multiColor) {
				ImGui::ColorEdit3("Gradient Shift Color", ExtraSettings.f3ColorB.data(), ImGuiColorEditFlags_DisplayHSV);
			}
			else {
				ImGui::Text("Single Color Gradient Options");
				ImGuiEx::SliderF("Gradient Lightness", &ExtraSettings.f2GradientRange.at(1), 1.0f, 3.0f, TCGradientIntens, "%.2fx");
				ImGuiEx::SliderF("Gradient Darkness", &ExtraSettings.f2GradientRange.at(0), 0.01f, 0.99f, TCGradientIntens, "%.2fx");
			}
		}
	}

}

namespace GTS {

	static void DrawStatusBarOptions(StatusBarWindow* a_targetBar) {
		if (!a_targetBar) return;

		auto& BaseSettings = a_targetBar->GetBaseSettings();
		auto& ExtraSettings = a_targetBar->GetExtraSettings<WindowSettingsStatusBar_t>();

		DrawCommonWidgetOptions(BaseSettings, ExtraSettings);
		ImGuiEx::SliderU16("Icon Size", &ExtraSettings.iIconSize, 8, 128, TIcoSize, "%d px");
		ImGuiEx::SliderF("Frame Alpha", &BaseSettings.fBGAlphaMult, 0.0f, 1.0f, TFrameAlpha , "%.2fx");

		bool sDamReduction   = !(ExtraSettings.iFlags & ImGuiEx::StatusbarFlag_HideDamageReduction);
		bool sLifeAbsorbtion = !(ExtraSettings.iFlags & ImGuiEx::StatusbarFlag_HideLifeAbsorbtion);
		bool sEnachantment   = !(ExtraSettings.iFlags & ImGuiEx::StatusbarFlag_HideEnchantment);
		bool sVoreStacks     = !(ExtraSettings.iFlags & ImGuiEx::StatusbarFlag_HideVoreStacks);
		bool sSizeReserve    = !(ExtraSettings.iFlags & ImGuiEx::StatusbarFlag_HideSizeReserve);
		bool sOnTheEdge      = !(ExtraSettings.iFlags & ImGuiEx::StatusbarFlag_HideOnTheEdge);

		ImGui::Text("Icon Visibility Toggles");

		if (ImGuiEx::CheckBox("Damage Reduction", &sDamReduction, TIcoVis)) {
			ImUtil::ToggleFlag(ExtraSettings.iFlags, ImGuiEx::StatusbarFlag_HideDamageReduction, !sDamReduction);
		}

		ImGui::SameLine();

		if (ImGuiEx::CheckBox("Life Absorbtion", &sLifeAbsorbtion, TIcoVis)) {
			ImUtil::ToggleFlag(ExtraSettings.iFlags, ImGuiEx::StatusbarFlag_HideLifeAbsorbtion, !sLifeAbsorbtion);
		}

		ImGui::SameLine();

		if (ImGuiEx::CheckBox("GTS Aspect", &sEnachantment, TIcoVis)) {
			ImUtil::ToggleFlag(ExtraSettings.iFlags, ImGuiEx::StatusbarFlag_HideEnchantment, !sEnachantment);
		}

		//------------ Line 2

		if (ImGuiEx::CheckBox("Vore Stacks", &sVoreStacks, TIcoVis)) {
			ImUtil::ToggleFlag(ExtraSettings.iFlags, ImGuiEx::StatusbarFlag_HideVoreStacks, !sVoreStacks);
		}

		ImGui::SameLine();

		if (ImGuiEx::CheckBox("Size Reserve", &sSizeReserve, TIcoVis)) {
			ImUtil::ToggleFlag(ExtraSettings.iFlags, ImGuiEx::StatusbarFlag_HideSizeReserve, !sSizeReserve);
		}

		ImGui::SameLine();

		if (ImGuiEx::CheckBox("On The Edge", &sOnTheEdge, TIcoVis)) {
			ImUtil::ToggleFlag(ExtraSettings.iFlags, ImGuiEx::StatusbarFlag_HideOnTheEdge, !sOnTheEdge);
		}

	}

	static void DrawUnderstompBarOptions(USBarWindow* a_targetBar) {
		if (!a_targetBar) return;

		auto& BaseSettings = a_targetBar->GetBaseSettings();
		auto& ExtraSettings = a_targetBar->GetExtraSettings<WindowSettingsUnderstompBar_t>();

		DrawCommonWidgetOptions(BaseSettings, ExtraSettings);
		DrawCommonBarOptions(ExtraSettings);

		ImGuiEx::CheckBox("Angle as Scale", &ExtraSettings.bShowScale, TShowUStompMult);
		ImGui::SameLine();
		ImGuiEx::CheckBox("Absolute Angle", &ExtraSettings.bShowAbsoluteAngle, TShowAbsAngle);

		DrawColorOptions(ExtraSettings);
	}

	static void DrawSizeBarOptions(SizeBarWindow* a_targetBar) {
		if (!a_targetBar) return;

		auto& BaseSettings = a_targetBar->GetBaseSettings();
		auto& ExtraSettings = a_targetBar->GetExtraSettings<WindowSettingsSizeBar_t>();

		// Copy Player Style (only for non-player bars)
		if (!a_targetBar->GetWindowName().ends_with("P")) {
			if (ImGuiEx::Button("Copy Player Style")) {
				if (const auto& P = dynamic_cast<SizeBarWindow*>(GTSMenu::WindowManager->wSBarP)) {
					const auto& B = P->GetBaseSettings();
					const auto& E = P->GetExtraSettings<WindowSettingsSizeBar_t>();

					BaseSettings.fAlpha = B.fAlpha;
					ExtraSettings.fBorderLightness = E.fBorderLightness;
					ExtraSettings.fBorderAlpha = E.fBorderAlpha;
					ExtraSettings.fBorderThickness = E.fBorderThickness;
					ExtraSettings.f2Size = E.f2Size;
					ExtraSettings.fRounding = E.fRounding;
					ExtraSettings.f2GradientRange = E.f2GradientRange;
					ExtraSettings.iFlags = E.iFlags;
					ExtraSettings.bEnableFade = E.bEnableFade;
					ExtraSettings.fFadeAfter = E.fFadeAfter;
					ExtraSettings.fFadeDelta = E.fFadeDelta;
					ExtraSettings.bShowName = E.bShowName;
					ExtraSettings.bShowScale = E.bShowScale;
					ExtraSettings.bShowSize = E.bShowSize;
				}
			}
			ImGui::SameLine();
		}
		DrawCommonWidgetOptions(BaseSettings, ExtraSettings);
		DrawCommonBarOptions(ExtraSettings);

		ImGuiEx::CheckBox("Show Name", &ExtraSettings.bShowName, TShowSName);
		ImGui::SameLine();
		ImGuiEx::CheckBox("Show Scale", &ExtraSettings.bShowScale, TShowSScale);
		ImGui::SameLine();
		ImGuiEx::CheckBox("Show Size", &ExtraSettings.bShowSize, TShowSSize);

		DrawColorOptions(ExtraSettings);
	}

	CategoryWidgets::CategoryWidgets() {
		m_name = "Widgets";
	}

	void CategoryWidgets::DrawWidgetSettings() {
		static ImGuiEx::CollapsingTabHeader SizeBarSettings(
			"Current-Size-Widget Settings",
			{
				"Player",
				"Follower 1",
				"Follower 2",
				"Follower 3",
				"Follower 4",
				"Follower 5"
			}
		);

		SizeBarSettings.RenameTab(0, PlayerCharacter::GetSingleton()->GetName());

		const auto& tmplist = GTSMenu::WindowManager->GetCachedTeamMateList();
		for (uint8_t i = 1; i < 6; i++) {
			if (i - 1 < tmplist.size() && tmplist[i - 1]) {
				SizeBarSettings.RenameTab(i, tmplist[i - 1]->GetName());
			}
			else {
				SizeBarSettings.RenameTab(i, fmt::format("Follower {}", i));
			}
		}

		ImWindow* activeWidget = nullptr;

		if (ImGuiEx::BeginCollapsingTabHeader(SizeBarSettings)) {
			// Content based on active tab
			switch (SizeBarSettings.GetActiveTab()) {
				case 0:  activeWidget = GTSMenu::WindowManager->wSBarP;   break;
				case 1:  activeWidget = GTSMenu::WindowManager->wSBarF1;  break;
				case 2:  activeWidget = GTSMenu::WindowManager->wSBarF2;  break;
				case 3:  activeWidget = GTSMenu::WindowManager->wSBarF3;  break;
				case 4:  activeWidget = GTSMenu::WindowManager->wSBarF4;  break;
				case 5:  activeWidget = GTSMenu::WindowManager->wSBarF5;  break;
			}

			if (activeWidget) {
				DrawSizeBarOptions(dynamic_cast<SizeBarWindow*>(activeWidget));
			}
		}

		ImGuiEx::EndCollapsingTabHeader(SizeBarSettings);
	}

	void CategoryWidgets::DrawLeft() {
		DrawWidgetSettings();
	}

	void CategoryWidgets::DrawRight() {
		static ImGuiEx::CollapsingTabHeader UniqueWidgetSettings(
			"Unique Widget Settings",
			{
				"Understomp Angle",
				"Buff Icon Bar",
			}
		);

		if (ImGuiEx::BeginCollapsingTabHeader(UniqueWidgetSettings)) {
			// Content based on active tab
			switch (UniqueWidgetSettings.GetActiveTab()) {
				case 0: DrawUnderstompBarOptions(dynamic_cast<USBarWindow*>(GTSMenu::WindowManager->wUBar)); break;
				case 1: DrawStatusBarOptions(dynamic_cast<StatusBarWindow*>(GTSMenu::WindowManager->wBBar)); break;
			}
		}

		ImGuiEx::EndCollapsingTabHeader(UniqueWidgetSettings);
	}
}