#include "UI/Windows/Settings/Categories/AutoAim.hpp"
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

namespace {
    void DrawCloseStompSettings() {
        PSString THelp = "Close stomp is a stomp type that is triggered when enemy is close to legs\n"
        "Can hit enemies that are trying to hide under foot";
        PSString T0 = "[Radius] Determines collider size.";
        PSString T01 = "[Radius] Determines collider size.";
        PSString T1 = "[Offset] left/right offset of the initial target search collider.";
        PSString T2 = "[Offset] forward/back offset of the initial target search collider.";
        ImGuiEx::HelpText("What is close stomp", THelp);
        ImGuiEx::SliderF("Stomp Radius", &GTS::Config::AutoAim.fAimAssist_Range_Stomp, 30.0f, 60.0f, T0, "%.2f");
        ImGuiEx::SliderF("Side Offset", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Foot, 0.0f, 30.0f, T1, "%.2f");
        ImGuiEx::SliderF("[Strong] Stomp Radius", &GTS::Config::AutoAim.fAimAssist_Range_Stomp_Strong, 30.0f, 60.0f, T0, "%.2f");
        ImGuiEx::SliderF("[Sneak] Stomp Radius", &GTS::Config::AutoAim.fAimAssist_Range_Stomp_Sneak, 30.0f, 60.0f, T0, "%.2f");
        ImGuiEx::SliderF("[Sneak] [R] Stomp Forward Offet", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Stomp_Sneak_Forward_R, -60.0f, 60.0f, T2, "%.2f");
        ImGuiEx::SliderF("[Sneak] [L] Stomp Forward Offet", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Stomp_Sneak_Forward_L, -60.0f, 60.0f, T2, "%.2f");
        ImGuiEx::SliderF("[Sneak] Side Offset", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Stomp_Sneak_Side, 0.0f, 30.0f, T1, "%.2f");
        ImGui::EndDisabled();
        ImGui::Spacing();
    }

    void DrawFarStompSettings() {
        PSString THelp = "Far stomp is a stomp variation that is triggered when enemy is outside of normal stomp range";
        PSString T0 = "[Radius] Determines collider size.";
        PSString T1 = "[Radius] Determines collider size of Strong version of Far Stomp";
        PSString T2 = "[Offset] back/forward offset of the initial target search collider.";
        ImGuiEx::HelpText("What is far stomp", THelp);
        ImGuiEx::SliderF("Stomp Radius:", &GTS::Config::AutoAim.fAimAssist_Range_FarStomp, 10.0f, 90.0f, T0, "%.2f");
        ImGuiEx::SliderF("[Strong] Stomp Radius:", &GTS::Config::AutoAim.fAimAssist_Range_FarStomp_Strong, 10.0f, 90.0f, T1, "%.2f");
        ImGuiEx::SliderF("Forward Offset:", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Foot_FarStomp, 0.0f, 75.0f, T2, "%.2f");
        ImGui::EndDisabled();
        ImGui::Spacing();
    }

    void DrawHandSlamSettings_Sneak() {
        PSString THelp = "Hand slam is a light stomp variation, but for the sneak state\n"
        "Giantess slams the ground with her hand";
        PSString T0 = "[Radius] Determines collider size.";
        PSString T1 = "[Offset] left/right offset of the initial target search collider.";
        PSString T2 = "[Offset] back/forward offset of the initial target search collider.";
        ImGuiEx::HelpText("What is sneak hand slam", THelp);
        ImGuiEx::SliderF("Hand Slam Radius", &GTS::Config::AutoAim.fAimAssist_Range_Hand_Sneak_Slam, 10.0f, 90.0f, T0, "%.2f");
        ImGuiEx::SliderF("[Strong] Hand Slam Radius", &GTS::Config::AutoAim.fAimAssist_Range_Hand_Sneak_Slam_Strong, 10.0f, 90.0f, T0, "%.2f");
        ImGuiEx::SliderF("Side Offset", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Hand_Sneak_Side, 5.0f, 60.0f, T1, "%.2f");
        ImGuiEx::SliderF("Forward Offset", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Hand_Sneak_Forward, 0.0f, 70.0f, T2, "%.2f");
        ImGui::EndDisabled();
        ImGui::Spacing();
    }

    void DrawHandSlamSettings_Crawl() {
        PSString THelp = "Hand slam is a light stomp variation, but for the crawl state\n"
        "Giantess slams the ground with her hand";
        PSString T0 = "[Radius] Determines collider size.";
        "Default: 32.0";
        PSString T1 = "[Offset] left/right offset of the initial target search collider.";
        PSString T2 = "[Offset] back/forward offset of the initial target search collider.";
        ImGuiEx::HelpText("What is crawl hand slam", THelp);
        ImGuiEx::SliderF("[Close] Hand Slam Radius", &GTS::Config::AutoAim.fAimAssist_Range_Hand_Crawl_Close, 10.0f, 90.0f, T0, "%.2f");
        ImGuiEx::SliderF("[Close] Side Offset", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Hand_Crawl_Side, 5.0f, 60.0f, T1, "%.2f");
        ImGuiEx::SliderF("[Close] Forward Offset", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Hand_Crawl_Forward, 0.0f, 70.0f, T2, "%.2f");
        //--------------------
        ImGuiEx::SliderF("[Far] Hand Slam Radius", &GTS::Config::AutoAim.fAimAssist_Range_Hand_Crawl_Far, 10.0f, 100.0f, T0, "%.2f");
        ImGuiEx::SliderF("[Far] Side Offset", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Hand_Crawl_Side_Far, 5.0f, 60.0f, T1, "%.2f");
        ImGuiEx::SliderF("[Far] Forward Offset", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Hand_Crawl_Forward_Far, 0.0f, 70.0f, T2, "%.2f");
        //---------------------
        ImGuiEx::SliderF("[Strong] Hand Slam Radius", &GTS::Config::AutoAim.fAimAssist_Range_Hand_Crawl_Strong, 10.0f, 100.0f, T0, "%.2f");
        ImGuiEx::SliderF("[Strong] Side Offset", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Hand_Crawl_Side_Strong, 5.0f, 60.0f, T1, "%.2f");
        ImGuiEx::SliderF("[Strong] Forward Offset", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Hand_Crawl_Forward_Strong, 0.0f, 70.0f, T2, "%.2f");
        ImGui::EndDisabled();
        ImGui::Spacing();
    }

    void DrawButtSlamSettings() {
        PSString THelp = "Butt Slam is a variation of strong stomp, it can be used in Sneak state\n"
        "It is performed if enemy is under giantess legs or butt";
        PSString T0 = "[Radius] Determines collider size.";
        PSString T1 = "[Offset] left/right offset of the initial target search collider.";
        PSString T2 = "[Offset] back/forward offset of the initial target search collider.";
        ImGuiEx::HelpText("What is a butt slam", THelp);
        ImGuiEx::SliderF("Radius", &GTS::Config::AutoAim.fAimAssist_Range_ButtSlam, 20.0f, 75.0f, T0, "%.2f");
        ImGuiEx::SliderF("Side Offset", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Butt_Side, 5.0f, 25.0f, T1, "%.2f");
        ImGuiEx::SliderF("Forward Offset", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Butt_Forward, 0.0f, 30.0f, T2, "%.2f");
        ImGui::EndDisabled();
        ImGui::Spacing();
    }

    void DrawBreastSlamSettings() {
        PSString THelp = "Breast Slam is a variation of strong stomp, it can be used in Crawling state\n"
        "It is performed if enemy is under giantess body and cannot be reached with hands";
        PSString T0 = "[Radius] Determines collider size.";
        PSString T1 = "[Offset] left/right offset of the initial target search collider.";
        PSString T2 = "[Offset] back/forward offset of the initial target search collider.";
        ImGuiEx::HelpText("What is a breast slam", THelp);
        ImGuiEx::SliderF("Radius", &GTS::Config::AutoAim.fAimAssist_Range_BreastSlam, 20.0f, 90.0f, T0, "%.2f");
        ImGuiEx::SliderF("Side Offset", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Breasts_Side, 0.0f, 25.0f, T1, "%.2f");
        ImGuiEx::SliderF("Forward Offset", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Breasts_Forward, -100.0f, 100.0f, T2, "%.2f");
        ImGuiEx::SliderF("Width", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Breasts_Width, 0.0f, 75.0f, T1, "%.2f");
        ImGuiEx::SliderF("Length", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Breasts_Length, 0.0f, 100.0f, T2, "%.2f");
        ImGuiEx::SliderF("Blend Offset", &GTS::Config::AutoAim.fAimAssist_Value_Breasts_BlendOffset, -10.0f, 10.0f, T2, "%.2f");
        ImGui::EndDisabled();
        ImGui::Spacing();
    }

    void DrawKickSettings() {
        PSString THelp = "Kick is a type of size-action that can hit a lot of enemies at once\n"
        "[Standing] Type of kick performed when character isn't sneaking of crawling\n"
        "[Hand Swipe] Type of kick performed when Sneaking or Crawling";
        PSString T0 = "[Radius] Determines collider size.";
        PSString T01 = "[Radius] Determines collider size for Sneak/Crawl state.";
        PSString T1 = "[Offset] back/forward offset of the initial target search collider.";
        PSString T2 = "[Hand Swipe Offset] back/forward offset of the initial target search collider.";
        ImGuiEx::HelpText("What is a kick", THelp);
        ImGuiEx::SliderF("Kick Radius", &GTS::Config::AutoAim.fAimAssist_Range_Kick, 20.0f, 60.0f, T0, "%.2f");
        ImGuiEx::SliderF("[Sneaking] Kick Radius", &GTS::Config::AutoAim.fAimAssist_Range_Kick_Sneak, 20.0f, 60.0f, T01, "%.2f");
        ImGuiEx::SliderF("[Standing] Forward Offset", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Kick_Forward, 10.0f, 70.0f, T1, "%.2f");
        ImGuiEx::SliderF("[Sneaking] Forward Offset", &GTS::Config::AutoAim.fAimAssist_OffsetDistance_Hand_Sneak_Forward_Sneak, 20.0f, 70.0f, T2, "%.2f");
        ImGui::EndDisabled();
        ImGui::Spacing();
    }
}


namespace GTS {

	CategoryAutoAim::CategoryAutoAim() {
		m_name = "Auto-Aim";
	}

    void CategoryAutoAim::DrawLeft() {

        ImUtil_Unique 
		{

            PSString T0 = "Enables or disables auto-aim for some size-related attacks.\n"
            "- If disabled, look-down angle will be used to perform attacks, just like before Auto-Aim was introduced\n"
            "- Can be disabled for Player only, NPCs will still use it\n"
            "- Disables UNDERSTOMP widget.";
            PSString T1 = "Prevents far stomps from happening if no targets were found";

            PSString THelp = "Auto-Aim is designed to make size-based attacks smarter while reducing the number of required keybinds\n"
                    "It works with most basic size-related attacks, such as Stomps and Hand Slams\n"
                    "-The system first searches for the closest enemy\n"
                    "-It then determines which hand or foot is best suited for the attack\n"
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
                ImGui::BeginDisabled(!Config::AutoAim.bEnableAutoAim);
                ImGuiEx::CheckBox("Prevent Far Stomps on Miss", &Config::AutoAim.bPreventFarStomps, T1);
                ImGui::EndDisabled();
                ImGui::Spacing();
            }
        }

        ImUtil_Unique 
		{
            PSString THelp = 
            "- Dark Green: Origin point\n"
            "- Pink: Breast/Butt\n"
            "- Red: Close Stomp\n"
            "- Green: Far Stomp\n"
            "- Light Blue: Kicks\n"
            "- Green: Hit enemy";
            PSString T0 = "Enable or Disable visualization of auto-aim range and targeted enemy, if any.";
            ImGui::BeginDisabled(!Config::AutoAim.bEnableAutoAim);
            if (ImGui::CollapsingHeader("Debugging", ImUtil::HeaderFlagsDefaultOpen)) {
                ImGuiEx::HelpText("Meaning of debug colors", THelp);
                ImGuiEx::CheckBox("Show Auto-Aim range", &Config::AutoAim.bDebugAutoAim, T0);
                ImGui::Spacing();
            }
            ImGui::EndDisabled();
        }
        if (!Config::Hidden.IKnowWhatImDoing) {
            return;
        }
        // ---- Foot Attacks
        static ImGuiEx::CollapsingTabHeader ActionHeader_Foot (
            "Attack Settings",
			{
				"[Stomp] Close",
				"[Stomp] Far",
                "[Sneak] Slam",
				"[Crawl] Slam",
                "[Butt] Slam",
                "[Breast] Slam",
                "[Kicks]",
			}
        );
        if (ImGuiEx::BeginCollapsingTabHeader(ActionHeader_Foot)) {
            ImGui::BeginDisabled(!Config::AutoAim.bEnableAutoAim);
            // Content based on active tab
            switch (ActionHeader_Foot.GetActiveTab()) {
                case 0: DrawCloseStompSettings();           break;
                case 1: DrawFarStompSettings();             break;
                case 2: DrawHandSlamSettings_Sneak();       break;
                case 3: DrawHandSlamSettings_Crawl();       break;
                case 4: DrawButtSlamSettings();             break;
                case 5: DrawBreastSlamSettings();           break;
                case 6: DrawKickSettings();                 break;
				default:                              	    break;
            }
        }
        ImGuiEx::EndCollapsingTabHeader(ActionHeader_Foot);
        
    }

    //Behavior settings
    void CategoryAutoAim::DrawRight() {
        ImUtil_Unique 
		{
            PSString T0 = "Use Rhomb Shape for colliders instead of Sphere\n"
            "It may produce more accurate blend results, but may reduce auto-aim trigger distance";
            ImGui::BeginDisabled(!Config::AutoAim.bEnableAutoAim);
            if (ImGui::CollapsingHeader("Collider Shape", ImUtil::HeaderFlagsDefaultOpen)) {
                ImGuiEx::CheckBox("Use Rhomb Shape", &Config::AutoAim.bUseRhombShape, T0);
                
                ImGui::Spacing();
            }
            ImGui::EndDisabled();
        }
        ImUtil_Unique 
		{
            PSString T0 ="Reduces the priority of enemies located behind the Giantess.\n"
            "Higher values make Auto-Aim less likely to target enemies behind you.";
            PSString T1 = "Reduces the priority of dead enemies.\n"
            "Higher values make Auto-Aim less likely to target dead enemies.";
            PSString T2 = "Enemies behind you are ignored once they exceed this percentage of the search collider's range.";
            PSString T3 = "Multiplies the animation blend values used by Auto-Aim.\n"
            "Higher values improve targeting near the edges of the search collider.";
            PSString T4 = "Controls how much the X and Z animation blend values are randomized when no target is found.";
            ImGui::BeginDisabled(!Config::AutoAim.bEnableAutoAim);
            if (ImGui::CollapsingHeader("Automatic Aim Behavior Settings", ImUtil::HeaderFlagsDefaultOpen)) {
                ImGuiEx::SliderF("Behind Target Penalty", &Config::AutoAim.fAimAssist_BackPenalty, 0.01f, 100.0f, T0, "%.2f");
                ImGuiEx::SliderF("Dead Target Penalty", &Config::AutoAim.fAimAssist_DeadPenalty, 0.01f, 100.0f, T1, "%.2f");
                ImGuiEx::SliderF("Ignore Rear Targets After", &Config::AutoAim.fAimAssist_IgnoreBehindAfter, 0.0f, 1.0f, T2, "%.2f");
                ImGuiEx::SliderF("Auto-Aim Blend Multiplier", &Config::AutoAim.fAimAssist_AimMagnitudeMultiplier, 1.0f, 1.5f, T3, "%.2fx");
                ImGuiEx::SliderF("Blend Randomization on Miss", &Config::AutoAim.fAimAssist_NoHitValueRandomRange, 0.0f, 1.0f, T4, "%.2fx");
                ImGui::Spacing();
            }
            ImGui::EndDisabled();
        }
    }
}
