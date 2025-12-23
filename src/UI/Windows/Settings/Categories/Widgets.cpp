#include "UI/Windows/Settings/Categories/Widgets.hpp"

#include "Config/Config.hpp"

#include "UI/GTSMenu.hpp"

#include "UI/Controls/Button.hpp"
#include "UI/Controls/CheckBox.hpp"
#include "UI/Controls/ComboBox.hpp"
#include "UI/Controls/Misc.hpp"
#include "UI/Controls/ProgressBar.hpp"
#include "UI/Controls/Slider.hpp"
#include "UI/Core/ImColorUtils.hpp"

#include "UI/Core/ImUtil.hpp"
#include "UI/Windows/Other/KillFeedWindow.hpp"

#include "UI/Windows/Widgets/SizeBarWindow.hpp"
#include "UI/Windows/Widgets/StatusBarWindow.hpp"
#include "UI/Windows/Widgets/USBarWindow.hpp"

namespace {

	// Common Bar Tooltip strings
	PSString TCVis = "Toggle widget visibility.";
	PSString TCAnchor = "Set the anchor point.";
	PSString TCPosOffset = "Offset the widget position relative to the selected anchor point.";
	PSString TCHeight = "Adjust the height of the progress bar.";
	PSString TCWidth = "Adjust the length of the progress bar.";
	PSString TCAlpha = "Adjust the overall transparency.";
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
	PSString TIcoAlwaysVis = "Toggle whether icons should remain visible when empty.";
	PSString TIcoSize = "Set the size of the icons.";
	PSString TRelativeFontScale = "Set the font size relative to the icon size.";
	PSString TCopyAccent = "Set the accent color as the gradient color";


	PSString KFFlagNoKiller = "Show/Hide the killers name.";
	PSString KFFlagNoKillType = "Show/Hide the death type.";
	PSString KFBGAlpha = "Set background alpha.";
	PSString KFVisDur = "Set the visibility duration for a kill entry.";
	PSString KFWidth = "Set the width of a kill entry.";
	PSString KFMaxVis = "Set the max number of kill entries visible at the same time.\n"
					    "Overflown entries are queued and will be shown once the currently visible ones expire.";

	PSString KFResetFontColor = "Reset the font color";
	PSString KFEnableVanillaKills = "Show vanilla game kills in the killfeed (eg. kills made during normal combat).";
	PSString KFEnableWorldKills = "Show vanilla game kills that have no attacker (eg. from falling damage, scripted deaths, etc).";


	void DrawKillFeedWindowBase(GTS::ImWindow* a_KillFeed) {

		if (!a_KillFeed) return;
		auto Win = dynamic_cast<GTS::KillFeedWindow*>(a_KillFeed);
		if (!Win) return;

		auto& BaseSettings = Win->GetBaseSettings();

		ImGui::SeparatorText("Common Settings");

		ImGuiEx::CheckBox("Enable", &BaseSettings.bVisible, TCVis);

		ImGui::BeginDisabled(!BaseSettings.bVisible);
		{
			// Anchor & Position
			ImGuiEx::ComboEx<GTS::ImWindow::WindowAnchor>("Anchor", BaseSettings.sAnchor, TCAnchor);
			ImGuiEx::SliderF("Position Up/Down", &BaseSettings.f2Position.at(1), 0.0f, 720.f, TCPosOffset, "%.1f");

			if (BaseSettings.sAnchor != "kCenter") {
				ImGuiEx::SliderF("Position Left/Right", &BaseSettings.f2Position.at(0), 0.0f, 1280.f, TCPosOffset, "%.1f");
			}

			ImGuiEx::SliderF("Alpha", &BaseSettings.fAlpha, 0.1f, 1.0f, TCAlpha, "%.2fx");
		}
		ImGui::EndDisabled();
	}

	void DrawKillfeedOptions(GTS::ImWindow* a_KillFeed) {

		if (!a_KillFeed) return;
		auto Win = dynamic_cast<GTS::KillFeedWindow*>(a_KillFeed);
		if (!Win) return;

		auto& BaseSettings = Win->GetBaseSettings();
		auto& ExtraSettings = Win->GetExtraSettings<WindowSettingsKillFeed_t>();

		ImGui::SeparatorText("Extended Settings");

		ImGui::BeginDisabled(!BaseSettings.bVisible);
		{
			
			ImGuiEx::SliderF("Visibility Duration", &BaseSettings.fFadeAfter, 0.1f, 10.f, KFVisDur, "%.2f second(s)");
			ImGuiEx::SliderF("Entry Width", &ExtraSettings.fWidth, 75.0f, 600.f, KFWidth, "%.0f");
			ImGuiEx::SliderU8("Max Visible Entries", &ExtraSettings.iMaxVisibleEntries, 2, 20, KFMaxVis, "%.d Entries");

			float buttonWidth = 18 + ImGui::GetStyle().ItemSpacing.x + ImGui::GetStyle().FramePadding.x * 2;

			{
				static GTS::ImGraphics::ImageTransform T = {
					.recolorEnabled = true,
				};

				T.targetColor = ImUtil::Colors::fRGBToImVec4(GTS::Config::UI.f3AccentColor);
				
				if (ImGuiEx::ImageButtonTransform("##ResetA", ImageList::Generic_Square, T, 18, TCopyAccent)) {
					ExtraSettings.f3BGColor = GTS::Config::UI.f3AccentColor;
				}
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::CalcItemWidth() - buttonWidth);
				ImGui::ColorEdit3("Background Color", ExtraSettings.f3BGColor.data(), ImGuiColorEditFlags_DisplayHSV);
				ImGuiEx::SliderF("Backround Alpha", &BaseSettings.fBGAlphaMult, 0.0f, 1.0f, KFBGAlpha, "%.2fx");
			}

			{
				
				if (ImGuiEx::ImageButton("##ResetAtt", ImageList::Generic_Reset, 18, KFResetFontColor)) {
					ExtraSettings.f3AttackerColor = {1.0f, 1.0f, 1.0f };
				}

				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::CalcItemWidth() - buttonWidth);
				ImGui::ColorEdit3("Attacker Text Color", ExtraSettings.f3AttackerColor.data(), ImGuiColorEditFlags_DisplayHSV);
			}

			{

				if (ImGuiEx::ImageButton("##ResetVi", ImageList::Generic_Reset, 18, KFResetFontColor)) {
					ExtraSettings.f3VictimColor = { 1.0f, 1.0f, 1.0f };
				}

				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::CalcItemWidth() - buttonWidth);
				ImGui::ColorEdit3("Victim Text Color", ExtraSettings.f3VictimColor.data(), ImGuiColorEditFlags_DisplayHSV);
			}

			{

				if (ImGuiEx::ImageButton("##ResetDT", ImageList::Generic_Reset, 18, KFResetFontColor)) {
					ExtraSettings.f3DeathTypeColor = { .6f, .6f, .6f };
				}

				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::CalcItemWidth() - buttonWidth);
				ImGui::ColorEdit3("Death Type Text Color", ExtraSettings.f3DeathTypeColor.data(), ImGuiColorEditFlags_DisplayHSV);
			}

			{
				bool sNoKillType = !(ExtraSettings.iFlags & ImGuiEx::KillFeedEntryFlag_NoKillType);
				bool sNoKiller = !(ExtraSettings.iFlags & ImGuiEx::KillFeedEntryFlag_NoKiller);

				if (ImGuiEx::CheckBox("Show Kill Type", &sNoKillType, KFFlagNoKillType)) {
					ImUtil::ToggleFlag(ExtraSettings.iFlags, ImGuiEx::KillFeedEntryFlag_NoKillType, !sNoKillType);
				}

				ImGui::SameLine();

				if (ImGuiEx::CheckBox("Show Attacker", &sNoKiller, KFFlagNoKiller)) {
					ImUtil::ToggleFlag(ExtraSettings.iFlags, ImGuiEx::KillFeedEntryFlag_NoKiller, !sNoKiller);
				}
			}

			ImGuiEx::CheckBox("Enable Vanilla Deaths", &ExtraSettings.bShowGameKills, KFEnableVanillaKills);
			ImGui::SameLine();
			ImGuiEx::CheckBox("List World Deaths", &ExtraSettings.bShowWorldKills, KFEnableWorldKills, !ExtraSettings.bShowGameKills);

		}
		ImGui::EndDisabled();

	}

	void DrawCommonWidgetOptions(auto& BaseSettings) {

		ImGui::SeparatorText("Common Settings");

		ImGuiEx::CheckBox("Enable", &BaseSettings.bVisible, TCVis);

		ImGui::BeginDisabled(!BaseSettings.bVisible);
		{
			// Anchor & Position
			ImGuiEx::ComboEx<GTS::ImWindow::WindowAnchor>("Anchor", BaseSettings.sAnchor, TCAnchor);
			ImGuiEx::SliderF("Position Up/Down", &BaseSettings.f2Position.at(1), 0.0f, 720.f, TCPosOffset, "%.1f");

			if (BaseSettings.sAnchor != "kCenter") {
				ImGuiEx::SliderF("Position Left/Right", &BaseSettings.f2Position.at(0), 0.0f, 1280.f, TCPosOffset, "%.1f");
			}

			ImGui::Spacing();

			// Fade Settings
			ImGuiEx::CheckBox("Inactivity Fade", &BaseSettings.bEnableFade, TCIFadeEn);
			ImGuiEx::SliderF("Fade After", &BaseSettings.fFadeAfter, 0.5f, 10.0f, TCIFadeTime, "After %.1f Seconds", !BaseSettings.bEnableFade);
			ImGuiEx::SliderF("Reappear Delta", &BaseSettings.fFadeDelta, 0.0, 0.5f, TCIFadeDelta, "After a %.2fx Difference", !BaseSettings.bEnableFade);
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

		float buttonWidth = 18 + ImGui::GetStyle().ItemSpacing.x + ImGui::GetStyle().FramePadding.x * 2;

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
			DrawCommonWidgetOptions(window->GetBaseSettings());
		}
	}

	void DrawStatusBarOptions(GTS::ImWindow* a_targetBar) {

		if (!a_targetBar) return;
		auto Win = dynamic_cast<GTS::StatusBarWindow*>(a_targetBar);
		if (!Win) return;

		auto& BaseSettings = Win->GetBaseSettings();
		auto& ExtraSettings = Win->GetExtraSettings<WindowSettingsStatusBar_t>();

		ImGui::SeparatorText("Extended Settings");

		ImGui::BeginDisabled(!BaseSettings.bVisible);
		{

			ImGuiEx::SliderU16("Icon Size", &ExtraSettings.iIconSize, 8, 128, TIcoSize, "%d px");
			ImGuiEx::SliderF("Relative Font Scale", &ExtraSettings.fRelativeFontScale, 0.5f, 1.5f, TRelativeFontScale, "%.1fx");

			bool sDamReduction   = !(ExtraSettings.iFlagsVis & ImGuiEx::StatusbarFlag_HideDamageReduction);
			bool sLifeAbsorbtion = !(ExtraSettings.iFlagsVis & ImGuiEx::StatusbarFlag_HideLifeAbsorbtion);
			bool sEnachantment   = !(ExtraSettings.iFlagsVis & ImGuiEx::StatusbarFlag_HideEnchantment);
			bool sVoreStacks     = !(ExtraSettings.iFlagsVis & ImGuiEx::StatusbarFlag_HideVoreStacks);
			bool sSizeReserve    = !(ExtraSettings.iFlagsVis & ImGuiEx::StatusbarFlag_HideSizeReserve);
			bool sOnTheEdge      = !(ExtraSettings.iFlagsVis & ImGuiEx::StatusbarFlag_HideOnTheEdge);
			bool sVoreAbsorbing  = !(ExtraSettings.iFlagsVis & ImGuiEx::StatusbarFlag_HideVoreBeingAbsorbed);

			bool ASDamReduction   = (ExtraSettings.iFlagsAS & ImGuiEx::StatusbarASFlag_ASDamageReduction);
			bool ASLifeAbsorbtion = (ExtraSettings.iFlagsAS & ImGuiEx::StatusbarASFlag_ASLifeAbsorbtion);
			bool ASEnachantment   = (ExtraSettings.iFlagsAS & ImGuiEx::StatusbarASFlag_ASEnchantment);
			bool ASVoreStacks     = (ExtraSettings.iFlagsAS & ImGuiEx::StatusbarASFlag_ASVoreStacks);
			bool ASSizeReserve    = (ExtraSettings.iFlagsAS & ImGuiEx::StatusbarASFlag_ASSizeReserve);
			bool ASOnTheEdge      = (ExtraSettings.iFlagsAS & ImGuiEx::StatusbarASFlag_ASOnTheEdge);
			bool ASVoreAbsorbing  = (ExtraSettings.iFlagsAS & ImGuiEx::StatusbarASFlag_ASVoreBeingAbsorbed);

			static GTS::ImGraphics::ImageTransform T = {
				.recolorEnabled = true,
			};

			T.targetColor = ImUtil::Colors::fRGBToImVec4(GTS::Config::UI.f3AccentColor);
			float buttonWidth = 18 + ImGui::GetStyle().ItemSpacing.x + ImGui::GetStyle().FramePadding.x * 2;
			if (ImGuiEx::ImageButtonTransform("##ResetA", ImageList::Generic_Square, T, 18, TCopyAccent)) {
				ExtraSettings.f3BGColor = GTS::Config::UI.f3AccentColor;
			}
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::CalcItemWidth() - buttonWidth);
			ImGui::ColorEdit3("Background Color", ExtraSettings.f3BGColor.data(), ImGuiColorEditFlags_DisplayHSV);
			ImGuiEx::SliderF("Backround Alpha", &BaseSettings.fBGAlphaMult, 0.0f, 1.0f, TFrameAlpha, "%.2fx");
			ImGui::ColorEdit3("Overflow Color", GTS::Config::UI.f3IconOverflowColor.data(), ImGuiColorEditFlags_DisplayHSV);

			ImGui::Spacing();

			ImUtil_Unique
			{

				ImGui::Text("Icon Visibility Toggles");

				if (ImGuiEx::CheckBox("Damage Reduction", &sDamReduction, TIcoVis)) {
					ImUtil::ToggleFlag(ExtraSettings.iFlagsVis, ImGuiEx::StatusbarFlag_HideDamageReduction, !sDamReduction);
				}

				ImGui::SameLine();

				if (ImGuiEx::CheckBox("Life Absorbtion", &sLifeAbsorbtion, TIcoVis)) {
					ImUtil::ToggleFlag(ExtraSettings.iFlagsVis, ImGuiEx::StatusbarFlag_HideLifeAbsorbtion, !sLifeAbsorbtion);
				}

				ImGui::SameLine();

				if (ImGuiEx::CheckBox("GTS Aspect", &sEnachantment, TIcoVis)) {
					ImUtil::ToggleFlag(ExtraSettings.iFlagsVis, ImGuiEx::StatusbarFlag_HideEnchantment, !sEnachantment);
				}

				//------------ Line 2

				if (ImGuiEx::CheckBox("Vore Stacks", &sVoreStacks, TIcoVis)) {
					ImUtil::ToggleFlag(ExtraSettings.iFlagsVis, ImGuiEx::StatusbarFlag_HideVoreStacks, !sVoreStacks);
				}

				ImGui::SameLine();

				if (ImGuiEx::CheckBox("Size Reserve", &sSizeReserve, TIcoVis)) {
					ImUtil::ToggleFlag(ExtraSettings.iFlagsVis, ImGuiEx::StatusbarFlag_HideSizeReserve, !sSizeReserve);
				}

				ImGui::SameLine();

				if (ImGuiEx::CheckBox("On The Edge", &sOnTheEdge, TIcoVis)) {
					ImUtil::ToggleFlag(ExtraSettings.iFlagsVis, ImGuiEx::StatusbarFlag_HideOnTheEdge, !sOnTheEdge);
				}

				//------------ Line 3

				if (ImGuiEx::CheckBox("Vore Absorbing", &sVoreAbsorbing, TIcoVis)) {
					ImUtil::ToggleFlag(ExtraSettings.iFlagsVis, ImGuiEx::StatusbarFlag_HideVoreBeingAbsorbed, !sVoreAbsorbing);
				}
			}

			// -------------- Always Show Toggles

			ImGui::Spacing();

			ImUtil_Unique 
			{

				ImGui::Text("Always Show Icon Toggles");

				if (ImGuiEx::CheckBox("Damage Reduction", &ASDamReduction, TIcoAlwaysVis)) {
					ImUtil::ToggleFlag(ExtraSettings.iFlagsAS, ImGuiEx::StatusbarASFlag_ASDamageReduction, ASDamReduction);
				}

				ImGui::SameLine();

				if (ImGuiEx::CheckBox("Life Absorbtion", &ASLifeAbsorbtion, TIcoAlwaysVis)) {
					ImUtil::ToggleFlag(ExtraSettings.iFlagsAS, ImGuiEx::StatusbarASFlag_ASLifeAbsorbtion, ASLifeAbsorbtion);
				}

				ImGui::SameLine();

				if (ImGuiEx::CheckBox("GTS Aspect", &ASEnachantment, TIcoAlwaysVis)) {
					ImUtil::ToggleFlag(ExtraSettings.iFlagsAS, ImGuiEx::StatusbarASFlag_ASEnchantment, ASEnachantment);
				}

				//------------ Line 2

				if (ImGuiEx::CheckBox("Vore Stacks", &ASVoreStacks, TIcoAlwaysVis)) {
					ImUtil::ToggleFlag(ExtraSettings.iFlagsAS, ImGuiEx::StatusbarASFlag_ASVoreStacks, ASVoreStacks);
				}

				ImGui::SameLine();

				if (ImGuiEx::CheckBox("Size Reserve", &ASSizeReserve, TIcoAlwaysVis)) {
					ImUtil::ToggleFlag(ExtraSettings.iFlagsAS, ImGuiEx::StatusbarASFlag_ASSizeReserve, ASSizeReserve);
				}

				ImGui::SameLine();

				if (ImGuiEx::CheckBox("On The Edge", &ASOnTheEdge, TIcoAlwaysVis)) {
					ImUtil::ToggleFlag(ExtraSettings.iFlagsAS, ImGuiEx::StatusbarASFlag_ASOnTheEdge, ASOnTheEdge);
				}

				//------------ Line 3

				if (ImGuiEx::CheckBox("Vore Absorbing", &ASVoreAbsorbing, TIcoAlwaysVis)) {
					ImUtil::ToggleFlag(ExtraSettings.iFlagsAS, ImGuiEx::StatusbarASFlag_ASVoreBeingAbsorbed, ASVoreAbsorbing);
				}
			}
		}
		ImGui::EndDisabled();
	}

	void DrawUnderstompBarOptions(GTS::ImWindow* a_targetBar) {
		if (!a_targetBar) return;
		auto Win = dynamic_cast<GTS::USBarWindow*>(a_targetBar);
		if (!Win) return;
		auto& BaseSettings = Win->GetBaseSettings();
		auto& ExtraSettings = Win->GetExtraSettings<WindowSettingsUnderstompBar_t>();

		ImGui::SeparatorText("Extended Settings");
		ImGui::BeginDisabled(!BaseSettings.bVisible);
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

		ImGui::BeginDisabled(!BaseSettings.bVisible);
		{
			// Copy Player Style (only for non-player bars)
			if (!a_targetBar->GetWindowName().ends_with("P")) {
				if (ImGuiEx::Button("Copy Player Style", TCopyPlayer)) {
					if (const auto& P = dynamic_cast<GTS::SizeBarWindow*>(GTS::GTSMenu::WindowManager->wSBarP)) {
						const auto& B = P->GetBaseSettings();
						const auto& E = P->GetExtraSettings<WindowSettingsSizeBar_t>();

						BaseSettings.fAlpha = B.fAlpha;
						BaseSettings.bEnableFade = B.bEnableFade;
						BaseSettings.fFadeAfter = B.fFadeAfter;
						BaseSettings.fFadeDelta = B.fFadeDelta;

						ExtraSettings.fBorderLightness = E.fBorderLightness;
						ExtraSettings.fBorderAlpha = E.fBorderAlpha;
						ExtraSettings.fBorderThickness = E.fBorderThickness;
						ExtraSettings.f2Size = E.f2Size;
						ExtraSettings.fRounding = E.fRounding;
						ExtraSettings.f2GradientRange = E.f2GradientRange;
						ExtraSettings.iFlags = E.iFlags;
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

			case 7: DrawUnderstompBarOptions(GTSMenu::WindowManager->wUBar);   break;
			case 8: DrawStatusBarOptions(GTSMenu::WindowManager->wStatusBar);  break;
			case 9: DrawKillfeedOptions(GTSMenu::WindowManager->wKillFeed);    break;

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

			case 7: DrawCommonOptionsFor<USBarWindow, WindowSettingsUnderstompBar_t>(GTSMenu::WindowManager->wUBar);       break;
			case 8: DrawCommonOptionsFor<StatusBarWindow, WindowSettingsStatusBar_t>(GTSMenu::WindowManager->wStatusBar);  break;
			case 9: DrawKillFeedWindowBase(GTSMenu::WindowManager->wKillFeed);  break;

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
		static std::vector<std::string> SizebarList{
			"Size Bar - Player",
			"Size Bar - Follower 1",
			"Size Bar - Follower 2",
			"Size Bar - Follower 3",
			"Size Bar - Follower 4",
			"Size Bar - Follower 5"
		};

		static std::vector<std::string> OtherWidgetList{
			"Size Bars",
			"Understomp Bar",
			"Status Bar",
			"KillFeed"
		};

		SizebarList[0] = fmt::format("Size Bar - {}", PlayerCharacter::GetSingleton()->GetName());
		static std::string CurrentSizebar = SizebarList[0];
		static std::string CurrentOther = OtherWidgetList[0];

		const auto& tmplist = GTSMenu::WindowManager->GetCachedTeamMateList();
		for (uint8_t i = 1; i <= 5; i++) {
			if (i - 1 < tmplist.size() && tmplist[i - 1]) {
				SizebarList[i] = fmt::format("Size Bar - {}", tmplist[i - 1]->GetName());
			}
			else {
				SizebarList[i] = fmt::format("Size Bar - Follower {}", i);
			}
		}

		ImGui::BeginChild("##Options", { -FLT_MIN, 0.0f }, ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_FrameStyle);
		{
			if (ImGui::BeginCombo("Select Widget", CurrentOther.c_str())) {
				for (size_t i = 0; i < OtherWidgetList.size(); ++i) {
					bool selected = (CurrentOther == OtherWidgetList[i]);
					if (ImGui::Selectable(OtherWidgetList[i].c_str(), selected)) {
						CurrentOther = OtherWidgetList[i];
						if (i == 0) { // "Sizebar" selected
							IndexToDraw = 0; // Default to player sizebar
						}
						else {
							IndexToDraw = 6 + i;
						}
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			ImGuiEx::SeperatorV();

			if (CurrentOther == "Size Bars") {

				if (ImGui::BeginCombo("Select Size Bar", CurrentSizebar.c_str())) {
					for (size_t i = 0; i < SizebarList.size(); ++i) {
						bool selected = (CurrentSizebar == SizebarList[i]);
						if (ImGui::Selectable(SizebarList[i].c_str(), selected)) {
							CurrentSizebar = SizebarList[i];
							IndexToDraw = i;
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}
		}
		ImGui::EndChild();
	}
}