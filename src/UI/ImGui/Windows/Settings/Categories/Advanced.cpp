#include "UI/ImGui/Windows/Settings/Categories/Advanced.hpp"

#include "UI/ImGui/Lib/imgui.h"

#include "UI/ImGui/Core/ImGraphics.hpp"
#include "UI/ImGui/Core/ImUtil.hpp"

#include "UI/ImGui/Controls/Button.hpp"
#include "UI/ImGui/Controls/CheckBox.hpp"
#include "UI/ImGui/Controls/ComboBox.hpp"
#include "UI/ImGui/Controls/Slider.hpp"

#include "Utils/Logger.hpp"

namespace GTS {

	CategoryAdvanced::CategoryAdvanced() {
		m_name = "Advanced";
	}

    bool CategoryAdvanced::IsVisible() const {
        return Config::Hidden.IKnowWhatImDoing;
    }

    void CategoryAdvanced::SetVisible(const bool a_visible) {
        Config::Hidden.IKnowWhatImDoing = a_visible;
    }

    void CategoryAdvanced::DrawLeft() {

        ImUtil_Unique {

            PSString T0 = "Show or hide this page.\n"
        	              "After Disabling you have to re-add the option to the settings toml again if you want to re-enable it.";

            if (ImGui::CollapsingHeader("Advanced", ImUtil::HeaderFlagsDefaultOpen)) {
                ImGuiEx::CheckBox("Enable/Disable This Page", &Config::Hidden.IKnowWhatImDoing, T0);

                ImGui::Spacing();
            }
        }

        ImUtil_Unique {

            PSString T0 = "Set the log severity level. The higher it is the more info is dumped into GTSPlugin.log";

            if (ImGui::CollapsingHeader("Logging", ImUtil::HeaderFlagsDefaultOpen)) {

                if (ImGuiEx::ComboEx<spdlog::level::level_enum>("Log Level", Config::Advanced.sLogLevel, T0, false, true)) {
	                logger::SetLevel(Config::Advanced.sLogLevel.c_str());
				}

				ImGui::Spacing();

			}

        }

        ImUtil_Unique {

            PSString T0 = "Enable/Disable DamageAV for the player's stamina and magicka.";
            PSString T1 = "GTS actions will have cooldowns if this is enabled.";
            PSString T2 = "Enforce Min/Max Values In UI.";

            if (ImGui::CollapsingHeader("Cheats", ImUtil::HeaderFlagsDefaultOpen)) {
                ImGuiEx::CheckBox("Enable ActorValue Damage",&Config::Advanced.bDamageAV, T0);
                ImGuiEx::CheckBox("Enable Size-Action Cooldowns",&Config::Advanced.bCooldowns, T1);
                ImGuiEx::CheckBox("Enforce Slider Range", &Config::Advanced.bEnforceUIClamps, T2);

                ImGui::Spacing();
            }
        }

        ImUtil_Unique {

	        PSString T0 = "Multiply the resulting GetAnimationSlowdown Value (Acts as a flat multiplier applied after the main formula).";
	        PSString T1 = "Modify The \"SoftCore\" formula used to calculate the amount animations will slow down relative to scale.";
            PSString T2 = "Should AnimSpeed be force set to 1x if IsGTSBusy() == true?";
            PSString T3 = "Define the floor value for the animation speed formula. (Default 0.1f).";

	        if (ImGui::CollapsingHeader("Animation Speed", ImUtil::HeaderFlagsDefaultOpen)) {

                ImGuiEx::CheckBox("GTS Actions Always 1x Speed", &Config::Advanced.bGTSAnimsFullSpeed, T2);
                ImGuiEx::SliderF("Animspeed Player", &Config::Advanced.fAnimSpeedAdjMultPlayer, 0.1f, 3.0f, T0);
                ImGuiEx::SliderF("Animspeed Teammate", &Config::Advanced.fAnimSpeedAdjMultTeammate, 0.1f, 3.0f, T0);
                ImGuiEx::SliderF("Animspeed Lowest Allowed", &Config::Advanced.fAnimspeedLowestBoundAllowed, 0.01f, 1.0f, T3);

                const float PlayerSlowDown = GetAnimationSlowdown(PlayerCharacter::GetSingleton());

                ImGui::Spacing();

                ImGui::Text("Player Slowdown: %.2fx", PlayerSlowDown);

                ImGui::Spacing();

                //https://www.desmos.com/calculator/vyofjrqmrn
                ImGui::Text("Animation Formula");
                ImGuiEx::SliderF3("Param K, N, S", &Config::Advanced.fAnimSpeedFormula.at(0), 0.0f, 10.0f, T1, "%.3f");
                ImGuiEx::SliderF2("Param O, A", &Config::Advanced.fAnimSpeedFormula.at(3), 0.0f, 10.0f, T1, "%.3f");

	            ImGui::Spacing();
	        }
	    }
    }

    void CategoryAdvanced::DrawRight() {

        ImUtil_Unique {

	        PSString T0 = "When performing Breast Absorption, visually enlarge breasts. Not Guaranteed to Work.";
	        PSString T1 = "Count Player as NPC, which makes Player perform random animations";
	        PSString T2 = "Enable the experimental support for devourment using AI manager. Meant to partially replace DV's own PseudoAI";
	        PSString T3 = "Set the probabilty for a DV action to be started.";

	        if (ImGui::CollapsingHeader("Experimental",ImUtil::HeaderFlagsDefaultOpen)) {
                ImGuiEx::CheckBox("Enlarge Breasts On Absorbtion", &Config::Advanced.bEnlargeBreastsOnAbsorption, T0, true);
                ImGuiEx::CheckBox("Player AI", &Config::Advanced.bPlayerAI, T1);

                ImGuiEx::CheckBox("DevourmentAI", &Config::Advanced.bEnableExperimentalDevourmentAI, T2, !Runtime::IsDevourmentInstalled());
                ImGuiEx::SliderF("DevourmentAI Probability", &Config::Advanced.fExperimentalDevourmentAIProb, 1.0f, 100.0f, T3,"%.0f%%", !Config::Advanced.bEnableExperimentalDevourmentAI, !Runtime::IsDevourmentInstalled());

	            ImGui::Spacing();
	        }
        }

        ImUtil_Unique {

            //ImGui Debug
            if (ImGui::CollapsingHeader("ImGui Debug",ImUtil::HeaderFlagsDefaultOpen)) {
                {
                    PSString T0 = "Show ImGui's Metrics Window";
                    PSString T1 = "Show ImGui's Stack Window";
                    PSString T2 = "Show ImGui's Demo Window";

                    //ImUtil::CheckBox("Show Metrics", &UIManager::WindowManager->ShowMetrics,T0);
                    ImGui::SameLine();
                    //ImUtil::CheckBox("Show Stack", &UIManager::WindowManager->ShowStack,T1);
                    //ImUtil::CheckBox("Show Demo Window", &UIManager::WindowManager->ShowDemoWindow, T2);
                }

                if (ImGuiEx::Button("Quit", "This will immediatly close the game.", false, 1.0f)) {
                    SKSE::WinAPI::TerminateProcess(SKSE::WinAPI::GetCurrentProcess(), EXIT_SUCCESS);
                }

                ImGui::SameLine();

                if (ImGuiEx::Button("Trigger Crash", "This will immediatly crash the game.", false, 1.0f)) {
                    using FuncType = void(*)();
                    FuncType func = nullptr;
                    func();
                }

                ImGui::Spacing();
            }
        }

        ImUtil_Unique {
            if (ImGui::CollapsingHeader("Graphics Test")) {
                ImGraphics::DebugDrawTest();
                ImGui::Spacing();
            }
        }

        ImUtil_Unique {

            //Multi-Language Font Test
            if (ImGui::CollapsingHeader("Font Test")) {

                {
                    ImGui::Text("This îs à fónt tèst — façade, naïve, jalapeño, groß, déjà vu, fiancée, coöperate, élève");
                    ImGui::Text("Αυτή είναι μια δοκιμή για το συστυμα γραμματοσειράς");
                    ImGui::Text("Это тест загрузчика шрифтов");
                }

                {
                    ImGui::Text("これはフォントローダーのテストです");
                }

                {
                    ImGui::Text("이것은 폰트로더 테스트입니다");
                }

                {
                    ImGui::Text("这是一个字体加载器测试");
                }

                ImGui::Spacing();
            }
        }

        ImUtil_Unique {

            PSString THelp = "Here you can erase the internal actor data of this mod.\n"
        					 "Make sure you do this in a cell like qasmoke. Only Unloaded actor data will deleted.\n"
        					 "You must save, close and restart the game after doing this else you risk really breaking stuff.";

	        if (ImGui::CollapsingHeader("Data Management",ImUtil::HeaderFlagsDefaultOpen)) {

                if (ImGuiEx::Button("Erase Persistent", "Clear out all data in persistent", false, 1.0f)) {
                	TES::GetSingleton()->PurgeBufferedCells();
                    logger::critical("Purged cell buffers in preperation of persistent erase.");
                	Persistent::GetSingleton().EraseUnloadedPersistentData();
                }

                ImGui::SameLine();

                if (ImGuiEx::Button("Erase Transient", "Clear out all data in Transient", false, 1.0f)) {
                    TES::GetSingleton()->PurgeBufferedCells();
                    logger::critical("Purged cell buffers in preperation of transient erase.");
                    Transient::GetSingleton().EraseUnloadedTransientData();
                }

                ImGui::SameLine();

                ImGui::TextColored(ImUtil::Colors::Message, "Info (!)");
                ImGuiEx::Tooltip(THelp, true);

                ImGui::Spacing();

	        }
        }
    }
}
