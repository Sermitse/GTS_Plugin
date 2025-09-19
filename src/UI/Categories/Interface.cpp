#include "UI/Categories/Interface.hpp"
#include "UI/ImGUI/ImUtil.hpp"
#include "UI/ImGUI/ImWindowManager.hpp"
#include "UI/Windows/WindowStatus.hpp"

namespace GTS {

	void CategoryInterface::DrawLeft(){

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

				ImUtil::CheckBox("Lock Config Window Position", &Settings.SettingsWindow.bLock, T0);
				ImGui::BeginDisabled(!Settings.SettingsWindow.bLock);

				{
					ImUtil::SliderF("Window Size", &Settings.SettingsWindow.fWindowSize, 60.0f, 100.0f, T1,"%.0f%%");
					ImUtil::ComboEx<ImWindow::WindowAnchor>("Anchor", Settings.SettingsWindow.sAnchor, T2);
					ImGui::BeginDisabled(Settings.SettingsWindow.sAnchor == "kCenter");

					ImUtil::SliderF2("Anchor Offsets", &Settings.SettingsWindow.f2Offset.at(0), 0.0f, 1280.f, T3, "%.1f");


					ImGui::EndDisabled();
				}

				ImGui::EndDisabled();

				ImGui::Spacing();

				ImUtil::SliderF("Window Alpha", &Settings.SettingsWindow.fAlpha, 0.2f, 1.0f, T5, "%.1fx");
				ImUtil::SliderF("Background Alpha", &Settings.SettingsWindow.fBGAlphaMult, 0.2f, 1.0f, T6, "%.1fx");

				ImGui::Spacing();
			}
		}
	}

	void CategoryInterface::DrawRight(){

		// -----  Misc Settings

	    ImUtil_Unique {

	        PSString T0 = "Choose which type of measurement units to display.";

	        if(ImGui::CollapsingHeader("Misc Settings",ImUtil::HeaderFlagsDefaultOpen)){
	            ImUtil::ComboEx<LDisplayUnit_t>("Measurement Units",Settings.sDisplayUnits, T0);
	            ImGui::Spacing();
	        }
	    }


		// -----  UI Settings

	    ImUtil_Unique {

	        if(ImGui::CollapsingHeader("UI Settings",ImUtil::HeaderFlagsDefaultOpen)){

	            PSString T0 = "Adjust the scale of all elements and fonts.";
	            PSString T1 = "Modify the width of UI controls";
	            PSString T2 = "Set the accent color for the UI.";

	            ImUtil::SliderF("UI Scale", &Settings.fScale, 0.7f, 1.8f, T0,"%.1fx");
	            if (ImGui::IsItemDeactivatedAfterEdit()) {
	                StyleMgr.LoadStyle();
	            }

	            ImUtil::SliderF("Item Width", &Settings.fItemWidth, 0.4f, 0.7f, T1,"%.2fx");

	            ImGui::ColorEdit3("Accent Color", Settings.f3AccentColor.data(), ImGuiColorEditFlags_DisplayHSV);
	            if (ImGui::IsItemDeactivatedAfterEdit() || (ImGui::IsItemActive() && ImGui::GetIO().MouseDown[0])) {
	                StyleMgr.LoadStyle();
	            }
	            if (ImGui::IsItemHovered()){
	                ImGui::SetTooltip(T2);
	            }

	            ImGui::Spacing();
	        }
	    }
	}
}