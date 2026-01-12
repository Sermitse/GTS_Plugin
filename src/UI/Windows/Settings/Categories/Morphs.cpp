#include "UI/Windows/Settings/Categories/Morphs.hpp"
#include "API/Racemenu.hpp"
#include "Config/Config.hpp"

#include "UI/GTSMenu.hpp"
#include "UI/Controls/Button.hpp"
#include "UI/Controls/CheckBox.hpp"
#include "UI/Controls/CollapsingTabHeader.hpp"
#include "UI/Controls/Slider.hpp"
#include "UI/Controls/ToolTip.hpp"

#include "UI/Core/ImUtil.hpp"
#include "UI/Windows/Settings/SettingsWindow.hpp"

namespace {

    PSString THelp = "Here you can configure morph targets that will be applied to actors when certain actions are performed.\n\n"
                     "Morphs are split up in to morph categories (like breasts, belly, etc).\n"
                     "Each category can have multiple racemenu/bodyslide morphs defined (Up to 16) that are combined together.\n"
                     "You can find the list of morph names by loading the body mod you use in Outfit Studio.";

    void DrawMorphList(GameplayMorphSettings_t& settings) {

        constexpr int32_t MAXIdx = 16;
		constexpr int32_t LastIdx = MAXIdx - 1;

        ImGui::TextColored(ImUtil::Colors::Subscript, "What is this (?)");
        ImGuiEx::Tooltip(THelp, true);

        if (ImGuiEx::SliderF("Multiplier", &settings.fMultiplier, 0.1f, 5.0f,
            "Multiply the final combined morph shape by this value.", "%.2fx")) {
            if (auto win = dynamic_cast<GTS::SettingsWindow*>(GTS::GTSMenu::WindowManager->wSettings)) {
                win->m_MorphDataWasModified = true;
            }
        }

        ImGui::SeparatorText("Morph List");

        int32_t shownCnt = 0;
        static int32_t activeEditIndex = -1;

        for (int i = 0; i < MAXIdx && shownCnt < MAXIdx; ++i)
        {
            // Skip empty fields UNLESS this field is currently being edited
            if (settings.MorphNames[i].empty() && i < LastIdx && activeEditIndex != i) {
                continue;
            }

            ImGui::PushID(i);

            ImGui::PushItemWidth(ImGui::GetWindowWidth() * GTS::Config::UI.fItemWidth * 0.5f);

            bool changed = false;
            bool nameChanged = ImGui::InputText("##name", &settings.MorphNames[i], ImGuiInputTextFlags_AutoSelectAll);
            bool isActive = ImGui::IsItemActive();

            // Track which field is being edited
            if (isActive) {
                activeEditIndex = i;
            }
            else if (activeEditIndex == i) {
                // Field just lost focus
                if (settings.MorphNames[i].empty() && i < LastIdx) {
                    // Delete the empty entry
                    changed = true;
                    for (int j = i; j < LastIdx; ++j) {
                        settings.MorphNames[j] = std::move(settings.MorphNames[j + 1]);
                        settings.MorphScales[j] = settings.MorphScales[j + 1];
                    }
                    settings.MorphNames[LastIdx].clear();
                    settings.MorphScales[LastIdx] = 0.0f;
                }
                activeEditIndex = -1;
            }

            ImGui::SameLine();

            bool scaleChanged = ImGui::InputFloat("##Scale", &settings.MorphScales[i], 0.01f, 0.1f, "%.2fx", ImGuiInputTextFlags_AutoSelectAll);

            ImGui::PopItemWidth();

            changed = changed || nameChanged || scaleChanged;

            if (!settings.MorphNames[i].empty()) {
                ImGui::SameLine();
                if (ImGuiEx::ImageButton("##delete", ImageList::Export_Delete, 18,
                    "Delete this morph entry")) {
                    changed = true;
                    for (int j = i; j < LastIdx; ++j) {
                        settings.MorphNames[j] = std::move(settings.MorphNames[j + 1]);
                        settings.MorphScales[j] = settings.MorphScales[j + 1];
                    }
                    settings.MorphNames[LastIdx].clear();
                    settings.MorphScales[LastIdx] = 0.0f;
                }
            }
            if (changed) {
                if (auto win = dynamic_cast<GTS::SettingsWindow*>(GTS::GTSMenu::WindowManager->wSettings)) {
                    win->m_MorphDataWasModified = true;
                }
            }
            ImGui::PopID();
            ++shownCnt;
        }
    }
}

namespace GTS {

    CategoryMorphs::CategoryMorphs() {
        m_name = "Morphs";
    }

    void CategoryMorphs::DrawLeft() {

        // ------- Morph Settings

        ImUtil_Unique
        {

			static ImGuiEx::CollapsingTabHeader MorphSettings(
	            "Morph Settings",
	            std::initializer_list<std::string>
	            {
	                "Breasts",
	            	"Belly"
	            }
            );

            if (!Racemenu::Loaded()) {
                MorphSettings.SetDisabledState(true);
                MorphSettings.SetExtraInfo("Racemenu API Error");
            }

            if (ImGuiEx::BeginCollapsingTabHeader(MorphSettings)) {
                // Content based on active tab
                switch (MorphSettings.GetActiveTab()) {
                    case 0: {
                        ImGui::BeginDisabled(!Config::Gameplay.ActionSettings.bEnlargeBreastsOnAbsorption);
                        DrawMorphList(Config::Gameplay.ActionSettings.MorphListBreasts);
                        ImGui::EndDisabled();
                    } break;

					case 1: {
						ImGui::BeginDisabled(!Config::Gameplay.ActionSettings.bEnableBellyMorph);
						DrawMorphList(Config::Gameplay.ActionSettings.MorphListBelly);
						ImGui::EndDisabled();
					} break;

                    default: break;
                }

            }
        	ImGuiEx::EndCollapsingTabHeader(MorphSettings);
        }
    }

    void CategoryMorphs::DrawRight() {

        ImGui::BeginDisabled(!Racemenu::Loaded());

        ImUtil_Unique
        {

            if (ImGui::CollapsingHeader("Breast Morph Settings", ImUtil::HeaderFlagsDefaultOpen)) {

                PSString T0 = "Enable breast growth after completing certain actions (like cleavage absorbtion).";
                PSString T1 = "Toggle whether the morph should decrease over time.";
				PSString T2 = "Set the rate at which breasts should shrink over time.";
				PSString T3 = "Set the amount breasts should increase by after each absorbtion.";

                ImGuiEx::CheckBox("Enlarge Breasts On Absorbtion", &Config::Gameplay.ActionSettings.bEnlargeBreastsOnAbsorption, T0);

				ImGui::BeginDisabled(!Config::Gameplay.ActionSettings.bEnlargeBreastsOnAbsorption);
                {
                    ImGuiEx::CheckBox("Shrink Over Time", &Config::Gameplay.ActionSettings.bShrinkBreastsOverTime, T1);
                    ImGuiEx::SliderF("Shrink Rate", &Config::Gameplay.ActionSettings.fBreastShrinkRate, 0.01f, 0.5f, T2, "%.2fx", !Config::Gameplay.ActionSettings.bShrinkBreastsOverTime);
                    ImGuiEx::SliderF("Increase Per Absorbtion", &Config::Gameplay.ActionSettings.fBreastsAbsorbIncrementBy, 0.1f, 5.0f, T3, "%.2fx");
                }
				ImGui::EndDisabled();

                ImGui::Spacing();
            }
        }

        ImUtil_Unique
        {

            if (ImGui::CollapsingHeader("Belly Morph Settings", ImUtil::HeaderFlagsDefaultOpen)) {

                PSString T0 = "Enable belly growth with vore actions.";
                PSString T1 = "Set the amount of growth per eaten tiny.";
                ImGuiEx::CheckBox("Grow Belly On Vore", &Config::Gameplay.ActionSettings.bEnableBellyMorph, T0);
                ImGuiEx::SliderF("Increase Per Vore", &Config::Gameplay.ActionSettings.fBellyAbsorbIncrementBy, 0.1f, 1.0f, T1, "%.2fx");
               
                ImGui::Spacing();
            }
        }

		ImGui::EndDisabled();
    }
}