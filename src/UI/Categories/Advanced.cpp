#include "UI/Categories/Advanced.hpp"

#include "UI/UIManager.hpp"
#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/ImWindowManager.hpp"
#include "UI/ImGui/ImUtil.hpp"

#include "Utils/Logger.hpp"

namespace GTS {

    void CategoryAdvanced::DrawLeft() {

        ImUtil_Unique{

            const char* T0 = "Show or hide this page.\n"
                             "After Disabling you have to re-add the option to the settings toml again if you want to re-enable it.";

            if (ImGui::CollapsingHeader("Advanced",ImUtil::HeaderFlagsDefaultOpen)) {
                ImUtil::CheckBox("Enable/Disable This Page", &Config::GetHidden().IKnowWhatImDoing, T0);

                ImGui::Spacing();
            }
        }

        ImUtil_Unique{
            	
            const char* T0 = "Show The Profiler";
            const char* T1 = "Enable the debug overlay.";
            const char* T2 = "Set the log severity level. The higher it is the more info is dumped into GTSPlugin.log";

            if (ImGui::CollapsingHeader("Logging / Debugging",ImUtil::HeaderFlagsDefaultOpen)) {

				#ifdef  GTS_PROFILER_ENABLED
                ImUtil::CheckBox("Draw Profiler", &Profilers::DrawProfiler, T0);
				#endif

                ImUtil::CheckBox("Draw Debug Overlay", &Settings.bShowOverlay,T1);

                if (ImUtil::ComboEx<spdlog::level::level_enum>("Log Level", Settings.sLogLevel, T2, false, true)) {
	                logger::SetLevel(Settings.sLogLevel.c_str());
				}

              ImGui::Spacing();

			}

        }

        ImUtil_Unique {

            const char* T0 = "Enable/Disable DamageAV for the player's stamina and magicka.";
            const char* T1 = "GTS actions will have cooldowns if this is enabled.";
            const char* T2 = "Enforce Min/Max Values In UI.";

            if (ImGui::CollapsingHeader("Cheats",ImUtil::HeaderFlagsDefaultOpen)) {
                ImUtil::CheckBox("Enable ActorValue Damage",&Settings.bDamageAV, T0);
                ImUtil::CheckBox("Enable Size-Action Cooldowns",&Settings.bCooldowns, T1);
                ImUtil::CheckBox("Enforce Slider Range", &Settings.bEnforceUIClamps, T2);

                ImGui::Spacing();
            }
        }

        ImUtil_Unique{

	        const char* T0 = "Multiply the resulting GetAnimationSlowdown Value (Acts as a flat multiplier applied after the main formula).";
	        const char* T1 = "Modify The \"SoftCore\" formula used to calculate the amount animations will slow down relative to scale.";
            const char* T2 = "Should AnimSpeed be force set to 1x if IsGTSBusy() == true?";
            const char* T3 = "Define the floor value for the animation speed formula. (Default 0.1f).";

	        if (ImGui::CollapsingHeader("Animation Speed",ImUtil::HeaderFlagsDefaultOpen)) {

                ImUtil::CheckBox("GTS Actions Always 1x Speed", &Settings.bGTSAnimsFullSpeed, T2);
                ImUtil::SliderF("Animspeed Player", &Settings.fAnimSpeedAdjMultPlayer, 0.1f, 3.0f, T0);
                ImUtil::SliderF("Animspeed Teammate", &Settings.fAnimSpeedAdjMultTeammate, 0.1f, 3.0f, T0);
                ImUtil::SliderF("Animspeed Lowest Allowed", &Settings.fAnimspeedLowestBoundAllowed, 0.01f, 1.0f, T3);

                const float PlayerSlowDown = GetAnimationSlowdown(PlayerCharacter::GetSingleton());

                ImGui::Spacing();

                ImGui::Text("Player Slowdown: %.2fx", PlayerSlowDown);

                ImGui::Spacing();

                //https://www.desmos.com/calculator/vyofjrqmrn
                ImGui::Text("Animation Formula");
                ImUtil::SliderF3("Param K, N, S", &Settings.fAnimSpeedFormula.at(0), 0.0f, 10.0f, T1, "%.3f");
                ImUtil::SliderF2("Param O, A", &Settings.fAnimSpeedFormula.at(3), 0.0f, 10.0f, T1, "%.3f");

	            ImGui::Spacing();
	        }
	    }
    }

    void CategoryAdvanced::DrawRight() {

        ImUtil_Unique {

	        const char* T0 = "When performing Breast Absorption, visually enlarge breasts. Not Guaranteed to Work.";
	        const char* T1 = "Count Player as NPC, which makes Player perform random animations";
	        const char* T2 = "Enable the experimental support for devourment using AI manager. Meant to partially replace DV's own PseudoAI";
	        const char* T3 = "Set the probabilty for a DV action to be started.";

	        if (ImGui::CollapsingHeader("Experimental",ImUtil::HeaderFlagsDefaultOpen)) {
	            ImUtil::CheckBox("Enlarge Breasts On Absorbtion", &Settings.bEnlargeBreastsOnAbsorption, T0);
	            ImUtil::CheckBox("Player AI", &Settings.bPlayerAI, T1);

	            ImUtil::CheckBox("DevourmentAI", &Settings.bEnableExperimentalDevourmentAI, T2);
	            ImUtil::SliderF("DevourmentAI Probability", &Settings.fExperimentalDevourmentAIProb, 1.0f, 100.0f, T3,"%.0f%%", !Settings.bEnableExperimentalDevourmentAI);

	            ImGui::Spacing();
	        }
        }

        ImUtil_Unique {

            //ImGui Debug
            if (ImGui::CollapsingHeader("ImGui Debug",ImUtil::HeaderFlagsDefaultOpen)) {
                {
                    const char* T0 = "Show ImGui's Metrics Window";
                    const char* T1 = "Show ImGui's Stack Window";
                    const char* T2 = "Show ImGui's Demo Window";

                    ImUtil::CheckBox("Show Metrics", &ImWindowManager::GetSingleton().ShowMetrics,T0);
                    ImGui::SameLine();
                    ImUtil::CheckBox("Show Stack", &ImWindowManager::GetSingleton().ShowStack,T1);
                    ImUtil::CheckBox("Show Demo Window", &ImWindowManager::GetSingleton().ShowDemoWindow, T2);
                }

                if (ImUtil::Button("Quit", "This will immediatly close the game.", false, 1.0f)) {
                    SKSE::WinAPI::TerminateProcess(SKSE::WinAPI::GetCurrentProcess(), EXIT_SUCCESS);
                }

                ImGui::SameLine();

                if (ImUtil::Button("Trigger Crash", "This will immediatly crash the game.", false, 1.0f)) {
                    using FuncType = void(*)();
                    FuncType func = nullptr;
                    func();
                }

                ImGui::Spacing();
            }
        }

        ImUtil_Unique {
            if (ImGui::CollapsingHeader("Graphics Test")) {
                UIManager::Graphics->DebugDrawTest();
                ImGui::Spacing();
            }
        }

        ImUtil_Unique {

            //Multi-Language Font Test
            if (ImGui::CollapsingHeader("Font Test")) {

                {
                    //Apparently Setting the activescript type is not needed?
					//I'd expect some character overlap due to merging but it appears things "just work".
                    //ImFontManager::SetActiveScript(ImFontManager::ActiveScriptType::EN);
                    ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kText);

                    ImGui::Text(reinterpret_cast <const char*>(u8"This îs à fónt tèst — façade, naïve, jalapeño, groß, déjà vu, fiancée, coöperate, élève"));
                    ImGui::Text(reinterpret_cast<const char*>(u8"Αυτή είναι μια δοκιμή για το συστυμα γραμματοσειράς"));
                    ImGui::Text(reinterpret_cast<const char*>(u8"Это тест загрузчика шрифтов"));

                    ImFontManager::PopActiveFont();
                }

                {
                    //ImFontManager::SetActiveScript(ImFontManager::ActiveScriptType::JP);
                    ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kText);

                    ImGui::Text(reinterpret_cast<const char*>(u8"これはフォントローダーのテストです"));

                    ImFontManager::PopActiveFont();
                }

                {
                    //ImFontManager::SetActiveScript(ImFontManager::ActiveScriptType::KR);
                    ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kText);

                    ImGui::Text(reinterpret_cast<const char*>(u8"이것은 폰트로더 테스트입니다"));

                    ImFontManager::PopActiveFont();
                }

                {
                    //ImFontManager::SetActiveScript(ImFontManager::ActiveScriptType::SC);
                    ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kText);

                    ImGui::Text(reinterpret_cast<const char*>(u8"这是一个字体加载器测试"));

                    ImFontManager::PopActiveFont();
                }

                ImGui::Spacing();
            }
        }

        ImUtil_Unique{

            const char* T0 = "Toggle whether to fully pause the game when the menu is open.\n"
                             "It's heavily recomended that you don't disable this.\n"
                             "Applies after closing and re-opening the menu.";

			const char* T1 = "Multiply game speed by this value when the settings menu is open.\nOnly works if Pause game is disabled.";

            if (ImGui::CollapsingHeader("Pause",ImUtil::HeaderFlagsDefaultOpen)) {

	            ImUtil::CheckBox("Pause Game", &Settings.bPauseGame, T0);
	            ImUtil::SliderF("SGTM Mult", &Settings.fSGTMMult, 0.05f, 1.0f, T1, "%.2fx", Settings.bPauseGame);

                ImGui::Spacing();

            }

        }

        ImUtil_Unique {

            const char* THelp = "Here you can erase the internal actor data of this mod.\n"
        						"Make sure you do this in a cell like qasmoke. Only Unloaded actor data will deleted.\n"
        						"You must save, close and restart the game after doing this else you risk really breaking stuff.";

	        if (ImGui::CollapsingHeader("Data Management",ImUtil::HeaderFlagsDefaultOpen)) {

                if (ImUtil::Button("Erase Persistent", "Clear out all data in persistent", false, 1.0f)) {
                	TES::GetSingleton()->PurgeBufferedCells();
                    logger::critical("Purged cell buffers in preperation of persistent erase.");
                	Persistent::GetSingleton().EraseUnloadedPersistentData();
                }

                ImGui::SameLine();

                if (ImUtil::Button("Erase Transient", "Clear out all data in Transient", false, 1.0f)) {
                    TES::GetSingleton()->PurgeBufferedCells();
                    logger::critical("Purged cell buffers in preperation of transient erase.");
                    Transient::GetSingleton().EraseUnloadedTransientData();
                }

                ImGui::SameLine();

                ImGui::TextColored(ImUtil::ColorError, "Info (!)");
                ImUtil::Tooltip(THelp, true);

                ImGui::Spacing();

	        }
        }
    }
}
