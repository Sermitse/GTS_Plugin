#include "UI/Categories/Advanced.hpp"
#include "UI/DearImGui/imgui.h"
#include "UI/ImGui/ImWindowManager.hpp"
#include "UI/ImGui/ImUtil.hpp"

namespace GTS {

    void CategoryAdvanced::DrawLeft() {

        ImUtil_Unique{

            const char* T0 = "Show or hide this page.\n"
                             "After Disabling you have to re-add the option to the settings toml again if you want to re-enable it.";

            if (ImGui::CollapsingHeader("Advanced",ImUtil::HeaderFlagsDefaultOpen)) {
                ImUtil::CheckBox("Enable/Disable This Page", &Config::GetHidden().IKnowWhatImDoing, T0);
            }
        }

        ImUtil_Unique{
            	
            const char* T0 = "Show The Profiler (Only works if #define is enabled)";
            const char* T1 = "Enable the debug overlay.";

            const char* T2 = "Set the log severity level. The higher it is the more info is dumped into GTSPlugin.log";
            const char* T3 = "Set the flush severity level. The higher it is the more info is dumped into GTSPlugin.log when a crash happens";

            if (ImGui::CollapsingHeader("Logging / Debugging",ImUtil::HeaderFlagsDefaultOpen)) {

				#ifdef  GTS_PROFILER_ENABLED
                ImUtil::CheckBox("Draw Profiler", &Profilers::DrawProfiler, T0);
				#endif

                ImUtil::CheckBox("Draw Debug Overlay", &Settings.bShowOverlay,T1);

                if (ImUtil::ComboEx<spdlog::level::level_enum>("Log Level", Settings.sLogLevel, T2, false, true)) {
                	spdlog::set_level(spdlog::level::from_str(Settings.sLogLevel));
				}

              ImGui::Spacing();

			}

        }

        ImUtil_Unique {

            const char* T0 = "Enable/Disable DamageAV for the player's stamina and magicka.";
            const char* T1 = "GTS actions will have cooldowns if this is enabled.";
            const char* T2 = "Multiply the resulting GetAnimationSlowdown Value";

            if (ImGui::CollapsingHeader("Cheats",ImUtil::HeaderFlagsDefaultOpen)) {
                ImUtil::CheckBox("Enable ActorValue Damage",&Settings.bDamageAV, T0);
                ImUtil::CheckBox("Enable Size-Action Cooldowns",&Settings.bCooldowns, T1);
                ImUtil::SliderF("Animspeed Player", &Settings.fAnimSpeedAdjMultPlayer, 0.2f, 1.0f, T2);
                ImUtil::SliderF("Animspeed Teammate", &Settings.fAnimSpeedAdjMultTeammate, 0.2f, 1.0f, T2);

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

                    ImUtil::CheckBox("Show Metrics", &ImWindowManager::GetSingleton().ShowMetrics,T0);
                    ImGui::SameLine();
                    ImUtil::CheckBox("Show Stack", &ImWindowManager::GetSingleton().ShowStack,T1);
                }

                if (ImUtil::Button("Quit", "This will immediatly close the game.", false, 1.0f)) {
                    SKSE::WinAPI::TerminateProcess(SKSE::WinAPI::GetCurrentProcess(), EXIT_FAILURE);
                }
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

                ImUtil::CheckBox("Hide Load Button", &Settings.bHideLoadButton, "Toggle the visibility of the load button");
	        }
        }
    }
}