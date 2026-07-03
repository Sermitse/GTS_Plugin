#include "UI/Windows/Settings/Categories/AutoAim.hpp"

#include "UI/Core/ImUtil.hpp"

#include "UI/Controls/CheckBox.hpp"
#include "UI/Controls/Slider.hpp"
#include "UI/Controls/ToolTip.hpp"
#include "UI/Controls/ComboBox.hpp"
#include "UI/Controls/ConditionalHeader.hpp"

#include "Managers/MaxSizeManager.hpp"
#include "Config/Config.hpp"

#include "UI/Controls/Text.hpp"


namespace GTS {

	CategoryAutoAim::CategoryAutoAim() {
		m_name = "Auto-Aim";
	}

    void CategoryAutoAim::DrawLeft() {

        ImUtil_Unique 
		{

            PSString T0 = "Enables or disables auto-aim for some size-related attacks.\n"
            "If disabled, look-down angle will be used to perform attacks, just like before Auto-Aim was introduced\n"
            "Can be disabled for Player only, NPCs will still use it";

            PSString THelp = "Auto-Aim is designed to make size-based attacks smarter while reducing the number of required keybinds.\n"
                    "It works with most basic attacks, including Stomps and Hand Slams.\n"
                    "-The system first searches for the closest enemy.\n"
                    "-It then determines which hand or foot is best suited for the attack.\n"
                    "-If no enemy is found, a random attack is performed instead\n\n"

                    "Technical limitations:\n"
                    "-Unlike modern game engines, Skyrim does not provide a fully reverse-engineered Foot IK system\n"
                    "-Instead, Auto-Aim blends multiple animations together to approximate the desired attack\n"
                    "-This may occasionally produce imperfect results, but it is still more reliable than using fixed animations\n"
                    "-The blend amount is determined by the angle and distance between the player and the target\n"
                    "-Internally, Auto-Aim drives the GTS_StompBlend, GTS_StompBlend_X and GTS_StompBlend_Y animation variables\n"
                    "-Which then determines performed animation";
            if (ImGui::CollapsingHeader("Automatic Aim", ImUtil::HeaderFlagsDefaultOpen)) {

                ImGuiEx::HelpText("What is auto-aim", THelp);
                ImGuiEx::CheckBox("Enable Auto-Aim", &Config::AutoAim.bEnableAutoAim, T0);
                ImGui::Spacing();
            }
        }

        ImUtil_Unique 
		{

            PSString T0 = "Enable or Disable visualization of auto-aim range and target enemy.";

            if (ImGui::CollapsingHeader("Debugging", ImUtil::HeaderFlagsDefaultOpen)) {

                ImGuiEx::CheckBox("Show Auto-Aim range", &Config::AutoAim.bDebugAutoAim, T0);
                ImGui::Spacing();
            }
        }
        // ---- Multipiers

        ImUtil_Unique 
		{
            PSString T0 = "[Hand Slam] Determines collider size.\n"
            "Default: 15.0";
            PSString T1 = "[Stomp] Determines collider size.\n"
            "Default: 42.5";
            PSString T2 = "[Kick] Determines collider size.\n"
            "Default: 36.0";

            if (ImGui::CollapsingHeader("Automatic Aim: Ranges", ImUtil::HeaderFlagsDefaultOpen)) {
                ImGuiEx::SliderF("Hand Slam Range", &Config::AutoAim.fAutoAim_Range_Hand, 10.0f, 30.0f, T0, "%.2f");
                ImGuiEx::SliderF("Stomp Range", &Config::AutoAim.fAutoAim_Range_Stomp, 30.0f, 60.0f, T1, "%.2f");
                ImGuiEx::SliderF("Kick Range", &Config::AutoAim.fAutoAim_Range_Kick, 20.0f, 60.0f, T2, "%.2f");
                ImGui::Spacing();
            }
        }

        ImUtil_Unique 
		{
            PSString T0 = "[Foot] left/right offset of the initial target search collider.\n"
            "Default: 10.0";
            PSString T1 = "[Hand] left/right offset of the initial target search collider.\n"
            "Default: 14.5";
            PSString T2 = "[Hand] back/forward offset of the initial target search collider.\n"
            "Default: 50.0";
            PSString T3 = "[Hand Sneak] back/forward offset of the initial target search collider.\n"
            "Default: 35.0";
            PSString T4 = "[Standing Kick] back/forward offset of the initial target search collider.\n"
            "Default: 20.0";

            if (ImGui::CollapsingHeader("Automatic Aim: Initial Collider Offsets", ImUtil::HeaderFlagsDefaultOpen)) {
                ImGuiEx::SliderF("[Foot] Side Offset", &Config::AutoAim.fAutoAim_Foot_OffsetDistance, 0.0f, 30.0f, T0, "%.2f");
                ImGuiEx::SliderF("[Hand] Side Offset", &Config::AutoAim.fAutoAim_Hand_OffsetDistance_Side, 25.0f, 60.0f, T1, "%.2f");
                ImGuiEx::SliderF("[Hand] Forward Offset", &Config::AutoAim.fAutoAim_Hand_OffsetDistance_Forward, 40.0f, 70.0f, T2, "%.2f");
                ImGuiEx::SliderF("[Hand Sneak] Forward Offset", &Config::AutoAim.fAutoAim_Hand_OffsetDistance_Forward_Sneak, 20.0f, 50.0f, T3, "%.2f");
                ImGuiEx::SliderF("[Standing Kicks] Forward Offset", &Config::AutoAim.fAutoAim_Kick_OffsetDistance_Forward, 20.0f, 50.0f, T4, "%.2f");
                ImGui::Spacing();
            }
        }
    }

    void CategoryAutoAim::DrawRight() {
        ImUtil_Unique 
		{
            PSString T0 ="Reduces the priority of enemies located behind the Giantess.\n"
            "Higher values make Auto-Aim less likely to target enemies behind you.\n"
            "Default: 3.0";
            PSString T1 = "Enemies behind you are ignored once they exceed this percentage of the search collider's range.\n"
            "Default: 0.25";
            PSString T2 = "Multiplies the animation blend values used by Auto-Aim.\n"
            "Higher values improve targeting near the edges of the search collider.\n"
            "Default: 1.25";
            PSString T3 = "Controls how much the X and Z animation blend values are randomized when no target is found.\n"
            "Default: 0.25";

            if (ImGui::CollapsingHeader("Automatic Aim Behavior Settings", ImUtil::HeaderFlagsDefaultOpen)) {
                ImGuiEx::SliderF("Behind Target Penalty", &Config::AutoAim.fAutoAim_BackPenalty, 0.01f, 30.0f, T0, "%.2f");
                ImGuiEx::SliderF("Ignore Rear Targets After", &Config::AutoAim.fAutoAim_IgnoreBehindAfter, 0.0f, 1.0f, T1, "%.2f");
                ImGuiEx::SliderF("Auto-Aim Blend Multiplier", &Config::AutoAim.fAutoAim_aimMagnitudeMultiplier, 1.0f, 1.5f, T2, "%.2fx");
                ImGuiEx::SliderF("Blend Randomization on Miss", &Config::AutoAim.fAutoAim_noHitValueRandomRange, 0.0f, 1.0f, T3, "%.2fx");
                ImGui::Spacing();
            }
        }
    }
}
