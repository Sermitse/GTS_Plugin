#include "UI/Windows/Settings/Categories/Morphs.hpp"
#include "API/Racemenu.hpp"
#include "Config/Config.hpp"

#include "UI/GTSMenu.hpp"
#include "UI/Controls/Button.hpp"
#include "UI/Controls/CheckBox.hpp"
#include "UI/Controls/CollapsingTabHeader.hpp"
#include "UI/Controls/Slider.hpp"
#include "UI/Controls/Text.hpp"

#include "UI/Core/ImUtil.hpp"
#include "UI/Windows/Settings/SettingsWindow.hpp"

namespace {

    PSString THelp = "- !Altering Morphs can be very FPS expensive!\n"
                     "Here you can configure morph targets that will be applied to actors when certain actions are performed.\n\n"
                     "Morphs are split up in to morph categories (like breasts, belly, etc).\n"
                     "Each category can have multiple racemenu/bodyslide morphs defined (Up to 16) that are combined together.\n"
                     "You can find the list of morph names by loading the body mod you use in Outfit Studio.";
                     

	void DrawMorphList(GameplayMorphSettings_t& settings) {

		constexpr int32_t kMax = 16;
		constexpr int32_t kLast = kMax - 1;

		ImGuiEx::HelpText("What is this", THelp);

		auto mark_modified = [] {
			if (auto* win = dynamic_cast<GTS::SettingsWindow*>(GTS::GTSMenu::WindowManager->wSettings)) {
				win->m_MorphDataWasModified = true;
			}
		};

		if (ImGuiEx::SliderF(
			"Multiplier",
			&settings.fMultiplier,
			0.1f,
			5.0f,
			"Multiply the final combined morph shape by this value.",
			"%.2fx")) {
			mark_modified();
		}

		ImGui::SeparatorText("Morph List");

		static int32_t activeEditIndex = -1;

		auto shift_left_from = [&](int32_t idx) {
			for (int32_t j = idx; j < kLast; ++j) {
				settings.MorphNames[j] = std::move(settings.MorphNames[j + 1]);
				settings.MorphScales[j] = settings.MorphScales[j + 1];
			}
			settings.MorphNames[kLast].clear();
			settings.MorphScales[kLast] = 0.0f;
		};

		auto last_used_index = [&]() -> int32_t {
			for (int32_t i = kLast; i >= 0; --i) {
				if (!settings.MorphNames[i].empty()) {
					return i;
				}
			}
			return -1;
		};

		// Always show one empty row after the last non-empty entry (if capacity allows).
		const int32_t lastUsed = last_used_index();
		const int32_t renderUpTo = std::min(lastUsed + 1, kLast);

		const float itemWidth = ImGui::GetWindowWidth() * GTS::Config::UI.fItemWidth * 0.5f;

		for (int32_t i = 0; i <= renderUpTo; ++i) {
			ImGui::PushID(i);

			bool changed = false;

			ImGui::PushItemWidth(itemWidth);

			const bool nameChanged = ImGui::InputText(
				"##name",
				&settings.MorphNames[i],
				ImGuiInputTextFlags_AutoSelectAll
			);

			const bool isActive = ImGui::IsItemActive();

			if (isActive) {
				activeEditIndex = i;
			}
			else if (activeEditIndex == i) {
				// Field just lost focus: if it is empty and not the last slot, remove it by shifting.
				if (settings.MorphNames[i].empty() && i < kLast) {
					shift_left_from(i);
					changed = true;
				}
				activeEditIndex = -1;
			}

			ImGui::SameLine();

			const bool scaleChanged = ImGui::InputFloat(
				"##Scale",
				&settings.MorphScales[i],
				0.01f,
				0.1f,
				"%.2fx",
				ImGuiInputTextFlags_AutoSelectAll
			);

			ImGui::PopItemWidth();

			changed = changed || nameChanged || scaleChanged;

			// Delete button only for non-empty rows
			if (!settings.MorphNames[i].empty()) {
				ImGui::SameLine();
				if (ImGuiEx::ImageButton("##delete", ImageList::Export_Delete, 18, "Delete this morph entry")) {
					shift_left_from(i);
					changed = true;

					// If we deleted the row being edited, clear edit tracking.
					if (activeEditIndex == i) {
						activeEditIndex = -1;
					}
				}
			}

			if (changed) {
				mark_modified();
			}

			ImGui::PopID();
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
                MorphSettings.SetExtraInfo("Couldn't get Racemenu API");
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
            if (ImGui::CollapsingHeader("Performance", ImUtil::HeaderFlagsDefaultOpen)) {

                PSString T0 = "Minimum morph change threshold before applying updates.\n"
							  "Higher values reduce performance cost by skipping small changes, but may make transitions less smooth.\n"
							  "Set to 0 for maximum smoothness at higher performance cost.";

                ImGuiEx::SliderF("Update Delta", &Config::Gameplay.ActionSettings.fMorphEPS, 0.0f, 0.3f, T0, "%.3fx");
                ImGui::Spacing();
            }
        }

        ImUtil_Unique
        {

            if (ImGui::CollapsingHeader("Breast Morph Settings", ImUtil::HeaderFlagsDefaultOpen)) {

                PSString T0 = "Enable breast growth after completing certain actions (like cleavage absorbtion).\n"
                                "- !Altering Morphs can be very FPS expensive!";
                PSString T1 = "Toggle whether the morph should decrease over time.\n"
                                "- !Altering Morphs can be very FPS expensive!";
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

                PSString T0 = "Enable belly growth with vore actions.\n"
                            "- !Altering Morphs can be very FPS expensive!";
                PSString T1 = "Set the amount of growth per eaten tiny.";
                ImGuiEx::CheckBox("Grow Belly On Vore", &Config::Gameplay.ActionSettings.bEnableBellyMorph, T0);
                ImGuiEx::SliderF("Increase Per Vore", &Config::Gameplay.ActionSettings.fBellyAbsorbIncrementBy, 0.1f, 1.0f, T1, "%.2fx");
               
                ImGui::Spacing();
            }
        }

		ImGui::EndDisabled();
    }
}