#include "UI/ImGui/Windows/Settings/Categories/Interface.hpp"

#include "Config/Config.hpp"

#include "UI/ImGui/Windows/Settings/SettingsWindow.hpp"

#include "UI/ImGui/Controls/CheckBox.hpp"
#include "UI/ImGui/Controls/ComboBox.hpp"
#include "UI/ImGui/Controls/Slider.hpp"

#include "UI/ImGui/Core/ImStyleManager.hpp"
#include "UI/ImGUI/Core/ImWindowManager.hpp"
#include "UI/ImGUI/Core/ImUtil.hpp"

#include "UI/GTSMenu.hpp"


namespace GTS {

	void CategoryInterface::DrawLeft(){

		const auto Window = dynamic_cast<SettingsWindow*>(GTSMenu::WindowManager->wSettings);
		if (!Window) return;
	
		WindowSettingsBase_t* Settings = &Window->GetBaseSettings();
		if (!Settings) return;

		//------------  Config Window

		ImUtil_Unique {

			PSString T0 = "Automatically handle positioning for this window.\n"
						  "Disabling this allows you to move and resize the settings window manually.\n"
						  "Otherwise, if left enabled you can adjust the position and window scale below.";

			PSString T1 = "Adjust the window size as a percentage of the screen.";
			PSString T2 = "Choose where to align the window on screen.";
			PSString T3 = "Adjust the offset relative to the selected anchor point.\n"
						  "Left/Right | Up/Down";

			PSString T5 = "Adjust the opacity of the stats window.";
			PSString T6 = "Adjust the opacity of the stats window's backround.";

			if (ImGui::CollapsingHeader("Config Window", ImUtil::HeaderFlagsDefaultOpen)) {

				ImGuiEx::CheckBox("Lock Config Window Position", &Settings->bLock, T0);
				ImGui::BeginDisabled(!Settings->bLock);

				{
					ImGuiEx::SliderF("Window Size", &Settings->fWindowSizePercent, 60.0f, 100.0f, T1,"%.0f%%");
					ImGuiEx::ComboEx<ImWindow::WindowAnchor>("Anchor", Settings->sAnchor, T2);
					ImGui::BeginDisabled(Settings->sAnchor == "kCenter");

					ImGuiEx::SliderF2("Anchor Offsets", &Settings->f2Position.at(0), 0.0f, 1280.f, T3, "%.1f");


					ImGui::EndDisabled();
				}

				ImGui::EndDisabled();

				ImGui::Spacing();

				ImGuiEx::SliderF("Window Alpha", &Settings->fAlpha, 0.2f, 1.0f, T5, "%.1fx");
				ImGuiEx::SliderF("Background Alpha", &Settings->fBGAlphaMult, 0.2f, 1.0f, T6, "%.1fx");

				ImGui::Spacing();
			}
		}
	}

	void CategoryInterface::DrawRight(){

		// -----  Misc Settings

	    ImUtil_Unique {

	        PSString T0 = "Choose which type of measurement units to display.";

	        if(ImGui::CollapsingHeader("Misc Settings",ImUtil::HeaderFlagsDefaultOpen)){
				ImGuiEx::ComboEx<LDisplayUnit_t>("Measurement Units",Config::UI.sDisplayUnits, T0);
	            ImGui::Spacing();
	        }
	    }


		// -----  UI Settings

	    ImUtil_Unique {

	        if(ImGui::CollapsingHeader("UI Settings",ImUtil::HeaderFlagsDefaultOpen)){

	            PSString T0 = "Adjust the scale of all elements and fonts.";
	            PSString T1 = "Modify the width of UI controls";
	            PSString T2 = "Set the accent color for the UI.";
				PSString T3 = "Toggle whether to fully pause the game when a blocking menu (Like Settings) is open.\n"
							  "It's heavily recomended that you don't disable this.\n"
	        	              "Applies after closing and re-opening the menu.";

				PSString T4 = "Enable skyrim's backround blur effect if a blocking menu (Like Settings) is open.\n"
							  "Applies after closing and re-opening the menu.";

				PSString T5 = "Multiply game speed by this value when the settings menu is open.\n"
				              "Only works if \"Pause game\" is disabled.";

				ImGuiEx::SliderF("UI Scale", &Config::UI.fScale, 0.7f, 1.8f, T0,"%.1fx");
	            if (ImGui::IsItemDeactivatedAfterEdit()) {
					ImStyleManager::ApplyStyle();
	            }

				ImGuiEx::SliderF("Item Width", &Config::UI.fItemWidth, 0.4f, 0.7f, T1,"%.2fx");

	            ImGui::ColorEdit3("Accent Color", Config::UI.f3AccentColor.data(), ImGuiColorEditFlags_DisplayHSV);
	            if (ImGui::IsItemDeactivatedAfterEdit() || (ImGui::IsItemActive() && ImGui::GetIO().MouseDown[0])) {
					ImStyleManager::ApplyStyle();
	            }
	            if (ImGui::IsItemHovered()){
	                ImGui::SetTooltip(T2);
	            }

				ImGui::Spacing();

				ImGuiEx::CheckBox("Pause Game", &Config::UI.bDoPause, T3);
				ImGui::SameLine();
				ImGuiEx::CheckBox("Blur Backround", &Config::UI.bDoBGBlur, T4);
				ImGuiEx::SliderF("GameTime Mult", &Config::UI.fSGTMMult, 0.05f, 1.0f, T5, "%.2fx", Config::UI.bDoPause);

	        }
	    }
	}
}
