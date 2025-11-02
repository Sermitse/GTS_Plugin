#include "UI/ImGui/Windows/Settings/Categories/Widgets.hpp"

#include "Config/Config.hpp"

#include "UI/GTSMenu.hpp"

#include "UI/ImGui/Controls/Button.hpp"
#include "UI/ImGui/Controls/CheckBox.hpp"
#include "UI/ImGui/Controls/ComboBox.hpp"
#include "UI/ImGui/Controls/ProgressBar.hpp"
#include "UI/ImGui/Controls/Slider.hpp"
#include "UI/ImGui/Core/ImColorUtils.hpp"

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
	PSString TCopyPlayer = "Copy the styling settings from the player's bar, colors and position are not copied.";

	//Specific to Icon/Buff/Status Bar
	PSString TFrameAlpha = "Set the transparency level for the window containing the statusbar.";
	PSString TIcoVis = "Toggle icon visibility.";
	PSString TIcoSize = "Set the size of the icons.";
	PSString TCopyAccent = "Set the accent color as the gradient color";

	template<typename SettingsType>
	void DrawCommonWidgetOptions(auto& BaseSettings, SettingsType& ExtraSettings) {

		ImGui::SeparatorText("Common Settings");

		ImGuiEx::CheckBox("Enable", &ExtraSettings.bVisible, TCVis);

		ImGui::BeginDisabled(!ExtraSettings.bVisible);
		{
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
		ImGui::EndDisabled();
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

		static GTS::ImGraphics::ImageTransform T = {
			.recolorEnabled = true,
		};
		T.targetColor = ImUtil::Colors::fRGBToImVec4(GTS::Config::UI.f3AccentColor);

		float buttonWidth = 18 + ImGui::GetStyle().ItemSpacing.x;

		if (ImGuiEx::ImageButtonTransform("##ResetA", ImageList::Generic_Square, T, 18, TCopyAccent)) {
			ExtraSettings.f3ColorA = GTS::Config::UI.f3AccentColor;
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::CalcItemWidth() - buttonWidth);
		ImGui::ColorEdit3("Base Color", ExtraSettings.f3ColorA.data(), ImGuiColorEditFlags_DisplayHSV);

		if (useGradient) {
			if (multiColor) {
				if (ImGuiEx::ImageButtonTransform("##ResetB", ImageList::Generic_Square, T, 18, TCopyAccent)) {
					ExtraSettings.f3ColorB = GTS::Config::UI.f3AccentColor;
				}
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::CalcItemWidth() - buttonWidth);
				ImGui::ColorEdit3("Gradient Shift Color", ExtraSettings.f3ColorB.data(), ImGuiColorEditFlags_DisplayHSV);
			}
			else {
				ImGui::Text("Single Color Gradient Options");
				ImGuiEx::SliderF("Gradient Lightness", &ExtraSettings.f2GradientRange.at(1), 1.0f, 3.0f, TCGradientIntens, "%.2fx");
				ImGuiEx::SliderF("Gradient Darkness", &ExtraSettings.f2GradientRange.at(0), 0.01f, 0.99f, TCGradientIntens, "%.2fx");
			}
		}
	}

	//Probably one of the most cursed things i've written for this mod...
	template<typename T, typename S>
	void DrawCommonOptionsFor(void* a_win) {
		if (!a_win) return;

		if (auto window = dynamic_cast<T*>(static_cast<T*>(a_win))) {
			auto& base = window->GetBaseSettings();
			auto& extra = window->template GetExtraSettings<S>();
			DrawCommonWidgetOptions(base, extra);
		}
	}

	void DrawStatusBarOptions(GTS::ImWindow* a_targetBar) {

		if (!a_targetBar) return;
		auto Win = dynamic_cast<GTS::StatusBarWindow*>(a_targetBar);
		if (!Win) return;

		auto& BaseSettings = Win->GetBaseSettings();
		auto& ExtraSettings = Win->GetExtraSettings<WindowSettingsStatusBar_t>();

		ImGui::SeparatorText("Extended Settings");

		ImGui::BeginDisabled(!ExtraSettings.bVisible);
		{

			ImGuiEx::SliderU16("Icon Size", &ExtraSettings.iIconSize, 8, 128, TIcoSize, "%d px");
			ImGuiEx::SliderF("Frame Alpha", &BaseSettings.fBGAlphaMult, 0.0f, 1.0f, TFrameAlpha, "%.2fx");

			bool sDamReduction = !(ExtraSettings.iFlags & ImGuiEx::StatusbarFlag_HideDamageReduction);
			bool sLifeAbsorbtion = !(ExtraSettings.iFlags & ImGuiEx::StatusbarFlag_HideLifeAbsorbtion);
			bool sEnachantment = !(ExtraSettings.iFlags & ImGuiEx::StatusbarFlag_HideEnchantment);
			bool sVoreStacks = !(ExtraSettings.iFlags & ImGuiEx::StatusbarFlag_HideVoreStacks);
			bool sSizeReserve = !(ExtraSettings.iFlags & ImGuiEx::StatusbarFlag_HideSizeReserve);
			bool sOnTheEdge = !(ExtraSettings.iFlags & ImGuiEx::StatusbarFlag_HideOnTheEdge);

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
		ImGui::EndDisabled();
	}

	void DrawUnderstompBarOptions(GTS::ImWindow* a_targetBar) {
		if (!a_targetBar) return;
		auto Win = dynamic_cast<GTS::USBarWindow*>(a_targetBar);
		if (!Win) return;
		auto& ExtraSettings = Win->GetExtraSettings<WindowSettingsUnderstompBar_t>();

		ImGui::SeparatorText("Extended Settings");
		ImGui::BeginDisabled(!ExtraSettings.bVisible);
		{
			DrawCommonBarOptions(ExtraSettings);

			ImGuiEx::CheckBox("Angle as Scale", &ExtraSettings.bShowScale, TShowUStompMult);
			ImGui::SameLine();
			ImGuiEx::CheckBox("Absolute Angle", &ExtraSettings.bShowAbsoluteAngle, TShowAbsAngle);

			DrawColorOptions(ExtraSettings);
		}
		ImGui::EndDisabled();
	}

	void DrawSizeBarOptions(GTS::ImWindow* a_targetBar) {

		if (!a_targetBar) return;
		auto Win = dynamic_cast<GTS::SizeBarWindow*>(a_targetBar);
		if (!Win) return;

		auto& BaseSettings = Win->GetBaseSettings();
		auto& ExtraSettings = Win->GetExtraSettings<WindowSettingsSizeBar_t>();

		ImGui::SeparatorText("Extended Settings");

		ImGui::BeginDisabled(!ExtraSettings.bVisible);
		{
			// Copy Player Style (only for non-player bars)
			if (!a_targetBar->GetWindowName().ends_with("P")) {
				if (ImGuiEx::Button("Copy Player Style", TCopyPlayer)) {
					if (const auto& P = dynamic_cast<GTS::SizeBarWindow*>(GTS::GTSMenu::WindowManager->wSBarP)) {
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
			}

			DrawCommonBarOptions(ExtraSettings);

			ImGuiEx::CheckBox("Show Name", &ExtraSettings.bShowName, TShowSName);
			ImGui::SameLine();
			ImGuiEx::CheckBox("Show Scale", &ExtraSettings.bShowScale, TShowSScale);
			ImGui::SameLine();
			ImGuiEx::CheckBox("Show Size", &ExtraSettings.bShowSize, TShowSSize);

			DrawColorOptions(ExtraSettings);
		}
		ImGui::EndDisabled();

	}

}

namespace GTS {

	CategoryWidgets::CategoryWidgets() {
		m_name = "Widgets";
	}

	void CategoryWidgets::DrawRight() {

		switch (IndexToDraw) {
			case 0: DrawSizeBarOptions(GTSMenu::WindowManager->wSBarP);  break;
			case 1: DrawSizeBarOptions(GTSMenu::WindowManager->wSBarF1); break;
			case 2: DrawSizeBarOptions(GTSMenu::WindowManager->wSBarF2); break;
			case 3: DrawSizeBarOptions(GTSMenu::WindowManager->wSBarF3); break;
			case 4: DrawSizeBarOptions(GTSMenu::WindowManager->wSBarF4); break;
			case 5: DrawSizeBarOptions(GTSMenu::WindowManager->wSBarF5); break;

			case 6: DrawUnderstompBarOptions(GTSMenu::WindowManager->wUBar);   break;
			case 7: DrawStatusBarOptions(GTSMenu::WindowManager->wStatusBar);  break;
			default: break;

		}
	}

	void CategoryWidgets::DrawLeft() {

		switch (IndexToDraw) {

			case 0: DrawCommonOptionsFor<SizeBarWindow, WindowSettingsSizeBar_t>(GTSMenu::WindowManager->wSBarP);  break;
			case 1: DrawCommonOptionsFor<SizeBarWindow, WindowSettingsSizeBar_t>(GTSMenu::WindowManager->wSBarF1); break;
			case 2: DrawCommonOptionsFor<SizeBarWindow, WindowSettingsSizeBar_t>(GTSMenu::WindowManager->wSBarF2); break;
			case 3: DrawCommonOptionsFor<SizeBarWindow, WindowSettingsSizeBar_t>(GTSMenu::WindowManager->wSBarF3); break;
			case 4: DrawCommonOptionsFor<SizeBarWindow, WindowSettingsSizeBar_t>(GTSMenu::WindowManager->wSBarF4); break;
			case 5: DrawCommonOptionsFor<SizeBarWindow, WindowSettingsSizeBar_t>(GTSMenu::WindowManager->wSBarF5); break;

			case 6: DrawCommonOptionsFor<USBarWindow, WindowSettingsUnderstompBar_t>(GTSMenu::WindowManager->wUBar);       break;
			case 7: DrawCommonOptionsFor<StatusBarWindow, WindowSettingsStatusBar_t>(GTSMenu::WindowManager->wStatusBar);  break;
			default: break;

		}
	}

	void CategoryWidgets::Draw() {
		DrawOptions();
		//Draw the split view
		ImCategorySplit::Draw();
	}

	void CategoryWidgets::DrawOptions() {

		ImGui::SetNextWindowBgAlpha(0.1f);

		static std::vector<std::string> ItemList {
			"Sizebar - Player",
			"Sizebar - Follower 1",
			"Sizebar - Follower 2",
			"Sizebar - Follower 3",
			"Sizebar - Follower 4",
			"Sizebar - Follower 5",
			"Understomp Angle",
			"Status/Active Buffs Bar",
		};

		ItemList[0] = fmt::format("Sizebar - {}",PlayerCharacter::GetSingleton()->GetName());
		static std::string CurrentItem = ItemList[0]; //Init to first

		const auto& tmplist = GTSMenu::WindowManager->GetCachedTeamMateList();
		for (uint8_t i = 1; i <= 5; i++) {
			if (i - 1 < tmplist.size() && tmplist[i - 1]) {
				ItemList[i] = fmt::format("Sizebar - {}", tmplist[i - 1]->GetName());
			}
			else {
				ItemList[i] = fmt::format("Sizebar - Follower {}", i);
			}
		}

		ImGui::BeginChild("##Options", { -FLT_MIN, 0.0f }, ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_FrameStyle);
		{

			if (ImGui::BeginCombo("Select Widget", CurrentItem.c_str())) {
				for (size_t i = 0; i < ItemList.size(); ++i) {
					bool is_selected = (CurrentItem == ItemList[i]);
					if (ImGui::Selectable(ItemList[i].c_str(), is_selected)) {
						CurrentItem = ItemList[i];
						IndexToDraw = i;
						ImGui::SetItemDefaultFocus();
						break;
					}
				}
				ImGui::EndCombo();
			}
		}
		ImGui::EndChild();
	}
}