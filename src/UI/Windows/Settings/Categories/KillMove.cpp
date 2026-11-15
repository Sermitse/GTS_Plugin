#include "UI/Windows/Settings/Categories/KillMove.hpp"
#include "UI/Controls/CollapsingTabHeader.hpp"

#include "UI/Core/ImUtil.hpp"

#include "UI/Controls/CheckBox.hpp"
#include "UI/Controls/Slider.hpp"
#include "UI/Controls/ToolTip.hpp"
#include "UI/Controls/ComboBox.hpp"
#include "UI/Controls/ConditionalHeader.hpp"

#include "Managers/MaxSizeManager.hpp"
#include "Config/Config.hpp"

#include "UI/Controls/Text.hpp"

using namespace GTS;

namespace {
    void DrawAbsorb_Big() {
        ImUtil_Unique {
            PSString T0 = "Determines how far the camera will zoom away from GTS by default";
            PSString T1 = "Limits closes the distance increase with GTS based on how small the target is";
            PSString T2 = "Determines how high the camera will be";
            PSString T3 = "Sets the maximum amount the camera can shift down based on enemy size";
            PSString T4 = "Determines how high the camera will be for enemies that don't have the required bones, such as creatures";
            ImGui::BeginDisabled(!Config::KillMove.bEnableKillMoves || Config::KillMove.bThirdPersonBreastKillMove);
            ImGuiEx::SliderF("Away from GTS", &Config::KillMove.fBreastAbsorb_ForwardFromGTS_AtLarge, -100.0f, 100.0f, T0,"%.1f");
            ImGuiEx::SliderF("Closer to GTS Limit", &Config::KillMove.fBreastAbsorb_ForwardFromGTS_AtLarge_Min, -100.0f, 100.0f, T1,"%.1f");
            ImGuiEx::SliderF("Camera Focus Height", &Config::KillMove.fBreastAbsorb_FocusHeightOffset_AtLarge, -100.0f, 100.0f, T2,"%.1f");
            ImGuiEx::SliderF("Camera Focus Height Limit", &Config::KillMove.fBreastAbsorb_FocusHeightOffset_AtLarge_Min, -100.0f, 100.0f, T3,"%.1f");
            ImGuiEx::SliderF("Camera Focus: Creatures", &Config::KillMove.fBreastAbsorb_FocusHeightOffset_NoBone, -100.0f, 100.0f, T4,"%.1f");
            ImGui::EndDisabled();
            ImGui::Spacing();
        }
    }
    void DrawAbsorb_Small() {
        ImUtil_Unique {
            PSString T0 = "Determines how far the camera will zoom away from GTS by default";
            PSString T1 = "Limits closes the distance increase with GTS based on how small the target is";
            PSString T2 = "Determines how high the camera will be";
            PSString T3 = "Sets the maximum amount the camera can shift down based on enemy size";
            PSString T4 = "Determines how high the camera will be for enemies that don't have the required bones, such as creatures";
            ImGui::BeginDisabled(!Config::KillMove.bEnableKillMoves || Config::KillMove.bThirdPersonBreastKillMove);
            ImGuiEx::SliderF("Away from GTS", &Config::KillMove.fBreastAbsorb_ForwardFromGTS_AtSmall, -100.0f, 100.0f, T0,"%.1f");
            ImGuiEx::SliderF("Closer to GTS Limit", &Config::KillMove.fBreastAbsorb_ForwardFromGTS_AtSmall_Min, -100.0f, 100.0f, T1,"%.1f");
            ImGuiEx::SliderF("Camera Focus Height", &Config::KillMove.fBreastAbsorb_FocusHeightOffset_AtSmall, -100.0f, 100.0f, T2,"%.1f");
            ImGuiEx::SliderF("Camera Focus Height Limit", &Config::KillMove.fBreastAbsorb_FocusHeightOffset_AtSmall_Min, -100.0f, 100.0f, T3,"%.1f");
            ImGui::EndDisabled();
            ImGui::Spacing();
        }
    }

    void DrawSuffocate_Big() {
        ImUtil_Unique {
            PSString T0 = "Determines how far the camera will zoom away from GTS by default";
            PSString T1 = "Limits closes the distance increase with GTS based on how small the target is";
            PSString T2 = "Determines how far the camera moves away after GTS pulls a tiny out of her breasts with her hand";
            PSString T3 = "Determines how high the camera will be after GTS pull a tiny out of her breasts with her hand";
            PSString T4 = "Determines the height limit of camera after GTS pull a tiny out of her breasts with her hand";
            PSString T5 = "Determines how high the camera will be";
            PSString T6 = "Determines how high the camera will be for enemies that don't have the required bones, such as creatures";
            ImGui::BeginDisabled(!Config::KillMove.bEnableKillMoves || Config::KillMove.bThirdPersonBreastKillMove);
            ImGuiEx::SliderF("Away from GTS", &Config::KillMove.fBreastSuffocate_ForwardFromGTS_AtLarge, -100.0f, 100.0f, T0,"%.1f");
            ImGuiEx::SliderF("Pulled Out: Camera Distance", &Config::KillMove.fBreastSuffocate_PulledOutForwardOffset_AtLarge, -100.0f, 100.0f, T2,"%.1f");
            ImGuiEx::SliderF("Pulled Out: Height", &Config::KillMove.fBreastSuffocate_PulledOutUpOffset, -100.0f, 100.0f, T3,"%.1f");
            ImGuiEx::SliderF("Camera Focus Height", &Config::KillMove.fBreastSuffocate_FocusHeightOffset_AtLarge, -100.0f, 100.0f, T5,"%.1f");
            ImGuiEx::SliderF("Camera Focus: Creatures", &Config::KillMove.fBreastSuffocate_FocusHeightOffset_NoBone, -100.0f, 100.0f, T6,"%.1f");
            ImGui::EndDisabled();
            ImGui::Spacing();
        }
    }
    void DrawSuffocate_Small() {
        ImUtil_Unique {
            PSString T0 = "Determines how far the camera will zoom away from GTS by default";
            PSString T2 = "Limits closes the distance increase with GTS based on how small the target is";
            PSString T3 = "Determines how far the camera moves away after GTS pulls a tiny out of her breasts with her hand";
            ImGui::BeginDisabled(!Config::KillMove.bEnableKillMoves || Config::KillMove.bThirdPersonBreastKillMove);
            ImGuiEx::SliderF("Away from GTS", &Config::KillMove.fBreastSuffocate_ForwardFromGTS_AtSmall, -100.0f, 100.0f, T0,"%.1f");
            ImGuiEx::SliderF("Camera Focus Height", &Config::KillMove.fBreastSuffocate_FocusHeightOffset_AtSmall, -100.0f, 100.0f, T2,"%.1f");
            ImGuiEx::SliderF("Pulled Out: Camera Distance", &Config::KillMove.fBreastSuffocate_PulledOutForwardOffset_AtSmall, -100.0f, 100.0f, T3,"%.1f");
            ImGui::EndDisabled();
            ImGui::Spacing();
        }
    }
}

namespace GTS {
    CategoryKillMove::CategoryKillMove() {
        m_name = "Killmoves";
    }

    void CategoryKillMove::DrawLeft() {
        ImUtil_Unique 
		{
            PSString T0 = "Enables or disables size-related Kill-Moves entirely\n"
                            "- Only the Player can trigger Kill-Moves\n"
                            "- Killmoves work only in third person";

            PSString T1 = "Slows down the game to 0.025x speed during Breast Kill-Moves\n"
                        "This gives you enough time to manually adjust parameters for the best visual results\n"
                        "Only works when the GTS UI is not pausing the game\n"
                        "- Don't forget to turn it off once configured";
            PSString THelp = "Size Kill-Moves are similar to vanilla Kill-Moves, but are designed for size-related actions\n"
                "They do not alter or interact with vanilla Kill-Moves in any way\n"
                "They are triggered based on predicted damage, size difference, and the enemy's position determined by Auto-Aim\n"
                "The prediction may be slightly inaccurate because we cannot reliably determine whether an attack will actually hit or miss the enemy\n"
                "As a result, false triggers are possible";
            PSString THelp1 = "Configuration Mode slows down game time to 0.025x during Breast Kill-Moves\n"
                "It is needed because Breast Offsets are unique to each Player model\n"
                "With some models, the camera position may be too far from or too close to the character\n"
                "Which can cause clipping or poor positioning, requiring manual adjustments\n"
                "- To use this mode, you need to disable game pausing in the GTS UI\n"
                "- Navigate to the following section in GTS UI: \n"
                "- Interface -> UI Settings -> disable 'Pause Game'";

            if (ImGui::CollapsingHeader("Size Kill-Moves", ImUtil::HeaderFlagsDefaultOpen)) {
                ImGuiEx::HelpText("What is Size Kill-Move", THelp);
                ImGuiEx::HelpText("What is configuration Mode", THelp1);
                ImGuiEx::CheckBox("Enable Size-Based Kill-Moves [3rd person only]", &Config::KillMove.bEnableKillMoves, T0);
                ImGui::BeginDisabled(Config::KillMove.bThirdPersonBreastKillMove);
                ImGuiEx::CheckBox("Configuration Mode", &Config::KillMove.bConfigureMode, T1);
                ImGui::EndDisabled();
                ImGui::BeginDisabled(!Config::KillMove.bEnableKillMoves);
                ImGui::Spacing();
            }
            ImGui::EndDisabled();
        }

        ImUtil_Unique {
            PSString T0 = "Enables or disables third person POV for breast Absorb/Suffocate kill-moves";
            if (ImGui::CollapsingHeader("Breast Absorb/Suffocate POV", ImUtil::HeaderFlagsDefaultOpen)) {
                ImGui::BeginDisabled(!Config::KillMove.bEnableKillMoves);
                ImGuiEx::CheckBox("Third Person POV", &Config::KillMove. bThirdPersonBreastKillMove, T0);
                ImGui::Spacing();
            }
            ImGui::EndDisabled();
        }
        ImUtil_Unique {
            PSString T0 = "Chance of triggering a size-related killmove when the enemy is below the overkill size threshold\n"
                        "For example, when the enemy can be crushed into mush";
            PSString T1 = "Chance of triggering a size-related killmove when the enemy simply dies from a size-related attack";
            ImGui::BeginDisabled(!Config::KillMove.bEnableKillMoves);
            if (ImGui::CollapsingHeader("Kill-Move Chance", ImUtil::HeaderFlagsDefaultOpen)) {
                ImGuiEx::SliderF("On Overkilling", &Config::KillMove.fKillMoveChance_Crush, 1.0f, 100.0f, T0, "%.1f%%");
                ImGuiEx::SliderF("On Killing", &Config::KillMove.fKillMoveChance_Death, 1.0f, 100.0f, T1, "%.1f%%");
                
                ImGui::Spacing();
            }
            ImGui::EndDisabled();
        }
    }
    void CategoryKillMove::DrawRight() {
        if (ImGui::CollapsingHeader("Kill-Move Camera Offsets", ImUtil::HeaderFlagsDefaultOpen)) {
            static ImGuiEx::CollapsingTabHeader ActionHeader_Absorb (
                "Breast Absorb Offsets",
                {
                    "Large Size (>8x)",
                    "Small Size (<8x)",
                }
            );
            if (ImGuiEx::BeginCollapsingTabHeader(ActionHeader_Absorb)) {
                // Content based on active tab
                switch (ActionHeader_Absorb.GetActiveTab()) {
                    case 0: DrawAbsorb_Big();           break;
                    case 1: DrawAbsorb_Small();         break;
                    default:                            break;
                }
            }
            ImGuiEx::EndCollapsingTabHeader(ActionHeader_Absorb);
            //-------------------------------------------------------------------------
            //
            //-------------------------------------------------------------------------
            static ImGuiEx::CollapsingTabHeader ActionHeader_Suffocate (
                "Breast Suffocate Offsets",
                {
                    "Large Size (>8x)",
                    "Small Size (<8x)",
                }
            );
            if (ImGuiEx::BeginCollapsingTabHeader(ActionHeader_Suffocate)) {
                // Content based on active tab
                switch (ActionHeader_Suffocate.GetActiveTab()) {
                    case 0: DrawSuffocate_Big();            break;
                    case 1: DrawSuffocate_Small();          break;
                    default:                                break;
                }
            }
            ImGuiEx::EndCollapsingTabHeader(ActionHeader_Suffocate);
            ImGui::Spacing();
        }
        
    }
}