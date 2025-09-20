#include "UI/Categories/Widgets.hpp"

#include "UI/UIManager.hpp"
#include "UI/ImGUI/ImUtil.hpp"
#include "UI/Windows/GTSInfo.hpp"
#include "UI/Windows/WindowStatus.hpp"

namespace GTS {

	static void DrawBaseOptions(WindowConfWidget_t& AwSettings, const char* aTitle) {

		PSString T0 = "Toggle visibility.";
		PSString T1 = "Adjust the opacity of the entire widget window.";

		PSString T2 = "Automatically handle positioning and size.\n"
						 "Disabling this allows you to move and resize the widget manually.\n"
						 "Otherwise, if left enabled you can adjust the position offsets below.";

		PSString T3 = "Choose the alignment point for the widget on screen.";
		PSString T4 = "Adjust horizontal offset from the selected anchor point.";
		PSString T5 = "Adjust vertical offset from the selected anchor point.";
		PSString T6 = "Adjust the opacity of the widget window's backround.";
		PSString T7 = "Toggle whether the widget fades after no value has changed after a while.";
		PSString T8 = "Change how long the widget should be shown for after a value has changed.";
		PSString T9 = "Set the required change (ie Delta) in size for the window to reappear if inactivity fade is enabled.";

		if (ImGui::CollapsingHeader(aTitle, ImUtil::HeaderFlagsDefaultOpen)) {
			ImUtil::CheckBox("Show Player Stats Widget", &AwSettings.bVisible, T0);

			ImGui::SameLine();

			ImUtil::CheckBox("Inactivity Fade", &AwSettings.bEnableFade, T7);
			ImUtil::SliderF("Fade After", &AwSettings.fFadeAfter, 0.5f, 10.0f, T8, "After %.1f Seconds", !AwSettings.bEnableFade);
			ImUtil::SliderF("Reappear Delta", &AwSettings.fFadeDelta, 0.0, 0.5f, T9, "After a %.2fx Difference", !AwSettings.bEnableFade);

			ImGui::Spacing();

			ImUtil::SliderF("Widget Alpha", &AwSettings.fAlpha, 0.1f, 1.0f, T1, "%.1fx");
			ImUtil::SliderF("Widget BG Alpha", &AwSettings.fBGAlphaMult, 0.0f, 1.0f, T6, "%.1fx");

			ImGui::Spacing();

			ImUtil::CheckBox("Lock Widget Position", &AwSettings.bLock, T2);

			ImGui::BeginDisabled(!AwSettings.bLock);
			{
				ImUtil::ComboEx<IImWindow::WindowAnchor>("Anchor", AwSettings.sAnchor, T3);
				ImUtil::SliderF2("Anchor Offsets", &AwSettings.f2Offset.at(0), -100.0f, 1500.0f, T4, "%.1f%", AwSettings.sAnchor == "kCenter");
			}
			ImGui::EndDisabled();

			ImGui::Spacing();

		}
	}

	static void DrawBarOptions(WindowConfWidget_t& AwSettings, const char* aTitle) {

		PSString T0 = "Change the progress bar's width.";
		PSString T1 = "Change the progress bar's height multiplier.";
		PSString T2 = "Round the progress bar's border if it's visible.";
		PSString T3 = "Change the thickness of the border surrounding the progress bar.";
		PSString T4 = "Change how light/dark the border is.";
		PSString T5 = "Change the border's tranparency multiplier.";
		PSString T6 = "Draw the progress bar with a gradient instead of only a flat color.";

		PSString T7 = "The gradient normally applies a lightening/darkening effect on the base color.\n"
					     "By enabling this you can select your own color for the gradient to use.";

		PSString T8 = "Change the darkening factor of the gradient.";
		PSString T9 = "Change the lightening factor of the gradient.";
		PSString T10 = "Flip the direction of the gradient.";

		PSString TReset = "Reset this color to be the same as the accent color in the interface settings.";

		if (ImGui::CollapsingHeader(aTitle, ImUtil::HeaderFlagsDefaultOpen)) {


			ImUtil::SliderF("Width", &AwSettings.fFixedWidth, 120.0f, 600.0f, T0, "%.0f");
			ImUtil::SliderF("Height ", &AwSettings.fSizeBarHeightMult, 0.01f, 2.5f, T1, "%.2fx");

			ImGui::Spacing();

			ImUtil::CheckBox("Border Rounding", &AwSettings.bEnableRounding, T2);

			ImUtil::SliderF("Border Thickness", &AwSettings.fBorderThickness, 0.0f, 5.0f, T3, "%.2fx");
			ImUtil::SliderF("Border Lightness", &AwSettings.fBorderLightness, 0.0f, 1.0f, T4, "%.2fx");
			ImUtil::SliderF("Border Alpha", &AwSettings.fBorderAlpha, 0.0f, 1.0f, T5, "%.2fx");

			ImGui::Spacing();

			ImUtil::CheckBox("Gradient", &AwSettings.bUseGradient, T6);

			ImGui::BeginDisabled(!AwSettings.bUseGradient);

			ImGui::SameLine();

			ImUtil::CheckBox("Multi Color", &AwSettings.bUseCustomGradientColors, T7);

			ImGui::SameLine();

			ImUtil::CheckBox("Flip Direction", &AwSettings.bFlipGradientDirection, T10);

			ImGui::EndDisabled();


			{   // Color A

				ImUtil_Unique{

					if (ImUtil::ImageButton("Reset", "generic_reset", 18, TReset)) {
						AwSettings.f3ColorA = Config::UI.f3AccentColor;
					}

					ImGui::SameLine(0.0f, 6.0f);
					ImGui::ColorEdit3("Base Color", AwSettings.f3ColorA.data(), ImGuiColorEditFlags_DisplayHSV);
				}

			}
			if (AwSettings.bUseGradient) {
				if (AwSettings.bUseCustomGradientColors) {   // Color B

					ImUtil_Unique{

						if (ImUtil::ImageButton("Reset", "generic_reset", 18, TReset)) {
							AwSettings.f3ColorB = Config::UI.f3AccentColor;
						}

						ImGui::SameLine(0.0f, 6.0f);
						ImGui::ColorEdit3("Gradient Shift Color", AwSettings.f3ColorB.data(), ImGuiColorEditFlags_DisplayHSV);
					}
				}
				else {
					ImGui::Text("Single Color Gradient Options");
					ImUtil::SliderF("Darken", &AwSettings.fNormalGradientDarkMult, 0.1f, 1.0f, T8, "%.2fx");
					ImUtil::SliderF("Lighten", &AwSettings.fNormalGradientLightMult, 1.0f, 2.0f, T9, "%.2fx");
				}
			}

			ImGui::Spacing();

		}
	}



	void CategoryWidgets::DrawLeft() {

		// ------- Player Stats Widget

		ImUtil_Unique {
			DrawBaseOptions(Settings.StatusWindow, "Player Widget");
		}

		ImUtil_Unique {
			DrawBarOptions(Settings.StatusWindow, "Player Widget Style");
		}

		ImUtil_Unique {
			if (ImGui::CollapsingHeader("Player Widget Content Toggles", ImUtil::HeaderFlagsDefaultOpen)) {
				ImUtil::Bitfield<GTSInfoFeatures>(&Settings.StatusWindow.iFlags);
			}
		}
	}

	void CategoryWidgets::DrawRight() {

		ImUtil_Unique {
			DrawBaseOptions(Settings.UnderstompWindow, "Understomp Widget");
		}

		ImUtil_Unique {
			DrawBarOptions(Settings.UnderstompWindow, "Understomp Widget Style");
		}

		ImUtil_Unique {
			if (ImGui::CollapsingHeader("Understomp Widget Content Toggles", ImUtil::HeaderFlagsDefaultOpen)) {
				ImUtil::CheckBox("Show Text", reinterpret_cast<bool*>(&Settings.UnderstompWindow.iFlags));
			}
		}
	}
}