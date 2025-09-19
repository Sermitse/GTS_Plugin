#include "UI/Categories/AI.hpp"

#include "UI/ImGui/ImFontManager.hpp"
#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/ImUtil.hpp"


namespace  {

	auto& Settings = GTS::Config::AI;

    void DrawAIAction_Vore() {

        static const char* const T0 = "Enable starting a vore action.";
        static const char* const T1 = "Set the chance for a vore action to be started.";

        ImUtil::CheckBox("Enable Action", &Settings.Vore.bEnableAction, T0);
        ImUtil::SliderF("Start Chance", &Settings.Vore.fProbability, 1.0f, 100.0f, T1, "%.0f%%", !Settings.Vore.bEnableAction);
    }

    void DrawAIAction_Stomps() {

        static const char* const T0 = "Enable foot stomps when standing or hand stomps when sneaking / crawling.";
        static const char* const T1 = "Set the chance for a stomp action to be started.";
        static const char* const T2 = "Set the chance to pefrom a grind animation when doing under stomps";

        ImUtil::CheckBox("Enable Action", &Settings.Stomp.bEnableAction, T0);
        ImUtil::SliderF("Start Chance", &Settings.Stomp.fProbability, 1.0f, 100.0f, T1, "%.0f%%",!Settings.Stomp.bEnableAction);
        ImUtil::SliderF("Grind On Understomp Chance", &Settings.Stomp.fUnderstompGrindProbability, 0.0f, 100.0f, T2, "%.0f%%", !Settings.Stomp.bEnableAction);
            
    }

    void DrawAIAction_KickSwipe() {

        static const char* const T0 = "Enable kicks when standing or hand swipes when sneaking / crawling.";
        static const char* const T1 = "Set the chance for a kick or swipe action to be started.";

        ImUtil::CheckBox("Enable Action", &Settings.KickSwipe.bEnableAction, T0);
        ImUtil::SliderF("Start Chance", &Settings.KickSwipe.fProbability, 1.0f, 100.0f, T1,"%.0f%%",!Settings.KickSwipe.bEnableAction);
            
    }

    void DrawAIAction_ThighSandwich() {

        static const char* const T0 = "Enable thigh sandwich actions.";
        static const char* const T1 = "Set the chance for a thigh sandwich action to be started.";

        static const char* const T2 = "When a thigh sandwich action has started,\n"
			                          "modify the time interval for attack attempts.\n"
			                          "If an attack happens, it depends on the probabilities set below.";

        static const char* const T3 = "Modify the chance to start a heavy attack.";
        static const char* const T4 = "Modify the chance to start a light attack.";

        
		ImUtil::CheckBox("Enable Action", &Settings.ThighSandwich.bEnableAction, T0);
    
        ImGui::BeginDisabled(!Settings.ThighSandwich.bEnableAction);
        {
            ImUtil::SliderF("Start Chance", &Settings.ThighSandwich.fProbability, 1.0f, 100.0f, T1, "%.0f%%");

            ImGui::Spacing();

            ImUtil::SliderF("Action Interval", &Settings.ThighSandwich.fInterval, 1.0f, 5.0f, T2, "Every %.1f Second(s)");
            ImUtil::SliderF("Heavy Attack Chance", &Settings.ThighSandwich.fProbabilityHeavy, 0.0f, 100.0f, T3, "%.0f%%");
            ImUtil::SliderF("Light Attack Chance", &Settings.ThighSandwich.fProbabilityLight, 0.0f, 100.0f, T4, "%.0f%%");
        }
        ImGui::EndDisabled();

    }

    void DrawAIAction_ThighCrush() {

        static const char* const T0 = "Enable thigh crush actions.";
        static const char* const T1 = "Set the chance for a thigh crush action to be started.";

        static const char* const T2 = "When a thigh crush action has started\n"
                         "modify the time interval for attack attempts.\n"
                         "If an attack happens, it depends on the probabilities set below.";

        static const char* const T3 = "Modify the chance to perform an attack.";

        ImUtil::CheckBox("Enable Action", &Settings.ThighCrush.bEnableAction, T0);

        ImGui::BeginDisabled(!Settings.ThighCrush.bEnableAction);
        {
            ImUtil::SliderF("Start Chance", &Settings.ThighCrush.fProbability, 1.0f, 100.0f, T1, "%.0f%%");

            ImGui::Spacing();

            ImUtil::SliderF("Action Interval", &Settings.ThighCrush.fInterval, 1.0f, 10.0f, T2, "Every %.1f Second(s)");
            ImUtil::SliderF("Attack Chance", &Settings.ThighCrush.fProbabilityHeavy, 0.0f, 100.0f, T3, "%.0f%%");

        }
        ImGui::EndDisabled();

    }

    void DrawAIAction_Hugs() {

        static const std::string HugsInfo = fmt::format(
            fmt::runtime(
				 "Hugs have following conditions to occur:\n"
	             "- Size Difference must be greater than x{:.2f}\n"
	             "- Size Difference must be less than x2.5\n\n"
	             "AI Hugs can happen only if these two conditions are met.\n"
	             "Maximal Size Difference can be increased by Hug perks."
            ),
            GTS::Action_Hug
        );

        static const char* const T0 = "Enable hug actions.";
        static const char* const T1 = "Set the chance for a hug action to be started.";
        static const char* const T2 = "Allow followers to perform the hug-crush action on other followers.";
        static const char* const T3 = "Allow followers to perform the hug-crush action on friendly (not in combat) NPCs.";
        static const char* const T5 = "Set the interval at which an attempt is made to do any of the following hug actions when hugging someone.";
        static const char* const T6 = "Set the chance to perform a hug heal action.";
        static const char* const T7 = "Set the chance to perform a hug crush action.";
        static const char* const T8 = "Set the chance to perform a hug shrink action.";
        static const char* const T9 = "Set the chance to perform a hug shrink action on a follower/teammate/player";
        static const char* const T10 = "Should the hugged actor be let go if they can't be shrunk any further.\nApplies only to Followers/Player. Others will be always let go.";
        static const char* const T11 = HugsInfo.c_str();


        ImGui::TextColored(ImUtil::ColorSubscript, "Why aren't hugs starting (?)");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip(T11);
        }

        ImUtil::CheckBox("Enable Action", &Settings.Hugs.bEnableAction, T0);
        
        ImGui::BeginDisabled(!Settings.Hugs.bEnableAction);
        {
            ImUtil::SliderF("Start Chance", &Settings.Hugs.fProbability, 1.0f, 100.0f, T1, "%.0f%%");

            ImGui::Spacing();

            ImUtil::CheckBox("Allow Crushing (Followers & Player)", &Settings.Hugs.bKillFollowersOrPlayer, T2);
            ImUtil::CheckBox("Allow Crushing (Friendly NPCs)", &Settings.Hugs.bKillFriendlies, T3);
            ImUtil::CheckBox("Stop When Too Small (Followers & Player)", &Settings.Hugs.bStopIfCantShrink, T10);

            ImGui::Spacing();

            ImUtil::SliderF("Action Interval", &Settings.Hugs.fInterval, 1.0f, 10.0f, T5, "Every %.1f Second(s)");
            ImUtil::SliderF("Chance To Heal", &Settings.Hugs.fHealProb, 0.0f, 100.0f, T6, "%.0f%%");
            ImUtil::SliderF("Chance To Crush", &Settings.Hugs.fKillProb, 0.0f, 100.0f, T7, "%.0f%%");
            ImUtil::SliderF("Chance To Shrink", &Settings.Hugs.fShrinkProb, 0.0f, 100.0f, T8, "%.0f%%");
            ImUtil::SliderF("Chance To Shrink Friendly", &Settings.Hugs.fFriendlyShrinkProb, 0.0f, 100.0f, T9, "%.0f%%");
        }
        ImGui::EndDisabled();

    }

    void DrawAIAction_ButtCrush() {

        static const char* const T0 = "Enable butt crush actions.";
        static const char* const T1 = "Set the chance for a butt crush action to be started.";

        static const char* const T2 = "If the AI decides to start a butt crush action, choose between a fast one or a targeted one.\n"
		                              "Modify the chances here:\n"
		                              "0%% -> Always perform a fast one.\n"
		                              "100%% -> Always perform a targeted one.";

        static const char* const T3 = "Set the interval at which an attempt is made to do any of the butt crush actions when doing a targeted one.";
        static const char* const T4 = "Increase/decrease the chance to grow. You should probably keep this value high.";
        static const char* const T5 = "Increase/decrease the chance to perform the butt crush.\n"
		                              "Note: The chance to perform the crush is internally increased based on growth.\n"
		                              "If you want the follower to grow often and only crush after a while, keep this value low.";

        ImUtil::CheckBox("Enable Action", &Settings.ButtCrush.bEnableAction, T0);

        
        ImGui::BeginDisabled(!Settings.ButtCrush.bEnableAction);
        {
            ImUtil::SliderF("Start Chance", &Settings.ButtCrush.fProbability, 1.0f, 100.0f, T1, "%.0f%%");
            ImUtil::SliderF("Fast / Targeted Crush Chance", &Settings.ButtCrush.fButtCrushTypeProb, 0.0f, 100.0f, T2, "%.0f%%");

            ImGui::Spacing();

            ImUtil::SliderF("Targeted Action Interval", &Settings.ButtCrush.fInterval, 1.0f, 10.0f, T3, "Every %.1f Second(s)");
            ImUtil::SliderF("Chance To Grow", &Settings.ButtCrush.fGrowProb, 0.0f, 100.0f, T4, "%.0f%%");
            ImUtil::SliderF("Chance To Crush", &Settings.ButtCrush.fCrushProb, 0.0f, 100.0f, T5, "%.0f%%");
        }
        ImGui::EndDisabled();

    }

    void DrawAIAction_Grab() {

        static const char* const T0 = "Enable grab actions.";
        static const char* const T1 = "Set the chance for the actor to grab someone if possible.";
        static const char* const T2 = "Set the interval at which an attempt is made to do a grab action when the actor is holding someone.";
        static const char* const T3 = "Set the chance for the actor to decide to throw the held NPC.";
        static const char* const T4 = "Set the chance for the actor to vore the held NPC.";
        static const char* const T5 = "Set the chance for the actor to crush the held NPC.";
        static const char* const T6 = "Set the chance for the actor to place the held NPC between their breasts.";

        static const char* const T7 = "If an NPC is between the actor's cleavage, set the chance for an absorb action to start.";
        static const char* const T8 = "If an NPC is between the actor's' cleavage, set the chance for a vore action to start.";
        static const char* const T9 = "If an NPC is between the actor's' cleavage, set the chance for a crush attack action to start.";
        static const char* const T10 = "If an NPC is between the actor's' cleavage, set the chance for a suffocation action to start.";

        static const char* const T11 = "Set the chance for the actor to be released when grabbed.";
        static const char* const T12 = "Set the chance cleavage actions to stop.";

        ImUtil::CheckBox("Enable Action", &Settings.Grab.bEnableAction, T0);

        ImGui::BeginDisabled(!Settings.Grab.bEnableAction);
        {
            ImUtil::SliderF("Start Chance", &Settings.Grab.fProbability, 1.0f, 100.0f, T1, "%.0f%%");

            ImGui::Spacing();

            ImUtil::SliderF("Action Interval", &Settings.Grab.fInterval, 1.0f, 10.0f, T2, "Every %.1f Second(s)");

            ImGui::Spacing();
            ImGui::Text("Grab Actions");
            ImUtil::SliderF("Chance To Throw", &Settings.Grab.fThrowProb, 0.0f, 100.0f, T3, "%.0f%%");
            ImUtil::SliderF("Chance To Vore", &Settings.Grab.fVoreProb, 0.0f, 100.0f, T4, "%.0f%%");
            ImUtil::SliderF("Chance To Crush", &Settings.Grab.fCrushProb, 0.0f, 100.0f, T5, "%.0f%%");
            ImUtil::SliderF("Chance To Release", &Settings.Grab.fReleaseProb, 0.0f, 100.0f, T11, "%.0f%%");


            ImGui::Spacing();

            ImGui::Text("Cleavage Actions");
            ImUtil::SliderF("Place in Cleavage Chance", &Settings.Grab.fCleavageProb, 0.0f, 100.0f, T6, "%.0f%%");
            ImUtil::SliderF("Cleavage Absorb Chance", &Settings.Grab.fCleavageAbsorbProb, 0.0f, 100.0f, T7, "%.0f%%");
            ImUtil::SliderF("Cleavage Vore Chance", &Settings.Grab.fCleavageVoreProb, 0.0f, 100.0f, T8, "%.0f%%");
            ImUtil::SliderF("Cleavage Crush Chance", &Settings.Grab.fCleavageAttackProb, 0.0f, 100.0f, T9, "%.0f%%");
            ImUtil::SliderF("Cleavage Suffocate Chance", &Settings.Grab.fCleavageSuffocateProb, 0.0f, 100.0f, T10, "%.0f%%");
            ImUtil::SliderF("Cleavage Stop Chance", &Settings.Grab.fCleavageStopProb, 0.0f, 100.0f, T12, "%.0f%%");
        }
        ImGui::EndDisabled();
    }

} 

namespace GTS {

    void CategoryAI::DrawLeft() {

        ImUtil_Unique {

            static const char* const T0 = "Globaly Enable/Disable the follower action AI.";


            static const char* const T1 = "Set the time interval followers should attempt to start a new GTS action.\n"
                             "This does not guarantee a follower will do something every x seconds.\n"
                             "It just changes the time interval an attempt is made to start something.\n"
                             "If anything is done depends on the probabilities configured in each action on the right pane.";

            static const char* const T2 = "Allow the AI to target the player.";
            static const char* const T3 = "Allow the AI to target other followers.";
            static const char* const T4 = "Allow AI to perform actions only if NPC is hostile to performer.";
            static const char* const T5 = "Only allow the action AI to be active when the follower is in combat.";
            static const char* const T6 = "Prevent Followers from using regular attacks based on their size, when they're large.\n"
            "- The chance to not attack increases with size.\n"
            "- Works only if enemies are close to Follower";
            static const char* const T7 = "If on, always prevents Attacks of followers even if there's no enemies nearby";

           

            static const char* const THelp = "The GTS AI checks at a configurable time interval what GTS actions it can start.\n"
                                "Whether the AI takes an action depends on said actions' probability settings.\n"
                                "If its probability percentage is low, the action is less likely to be started.\n"
                                "When all action chances are low, the AI is more likely to simply \"do nothing\".\n"
                                "Some actions (like hugs or grabs) have their own actions (sub-actions) that work on the same idea but with their own settings\n"
                                "whom you can modify on the right pane of this settings page.";

            if (ImGui::CollapsingHeader("AI Settings",ImUtil::HeaderFlagsDefaultOpen)) {


                ImGui::TextColored(ImUtil::ColorSubscript,"How does this work (?)");
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip(THelp);
                }

                ImUtil::CheckBox("Enable AI",&Settings.bEnableActionAI, T0);

                {
                    ImGui::BeginDisabled(!Settings.bEnableActionAI);

                    ImUtil::SliderF("Try To Start A New Action",&Settings.fMasterTimer, 2.0f, 15.0f, T1 ,"Every %.1f Seconds");
                    ImGui::Spacing();

                    ImUtil::CheckBox("Target Player", &Settings.bAllowPlayer, T2);
                    ImGui::SameLine();
                    ImUtil::CheckBox("Target Other Followers", &Settings.bAllowFollowers, T3);
                    ImGui::SameLine();
                    ImUtil::CheckBox("Target Only Hostiles", &Settings.bHostileOnly, T4);

                    ImUtil::CheckBox("Enable AI Only During Combat",&Settings.bCombatOnly, T5);
                    ImUtil::CheckBox("Disable Regular Attacks When Large", &Settings.bDisableAttacks, T6);
                    ImUtil::CheckBox("Always Disable Regular Attacks When Large", &Settings.bAlwaysDisableAttacks, T7, !Settings.bDisableAttacks);

                    ImGui::EndDisabled();
                }

                ImGui::Spacing();
            }
        }

        ImUtil_Unique {
            
            static const char* const T0 = "Toggle whether Followers should have movement speed further reduced past x1.25 size.\n"
                            "- This toggle makes Follower Sprinting less likely based on size, so Followers will try to menacingly walk instead of sprinting\n"
                            "- Alters SpeedMult Actor Value, can reduce it up to 15\n\n"
                            "- This toggle requires Dynamic Animation Speed toggle in order to work";
            static const char* const T2 = "Toggle whether Followers should have decreased rotation speed past x1.25 size.\n\n"
                            "- This toggle requires Dynamic Animation Speed toggle in order to work";
            static const char* const T3 = "Toggle whether Followers should record movement speed of their legs/hands\n"
                             "- If on - followers won't push others away if just standing still\n"
                             "- May affect fps in a negative way since it records data each frame for each follower";
            static const char* const T4 = "Toggle whether other NPCs should panic when near a GTS.";

            if (ImGui::CollapsingHeader("Misc Settings",ImUtil::HeaderFlagsDefaultOpen)) {
                ImUtil::CheckBox("Decrease Movement Speed AV", &Settings.bSlowMovementDown, T0);
                ImUtil::CheckBox("Decrease Combat Rotation Speed", &Settings.bSlowRotationDown, T2);
                ImUtil::CheckBox("Record Node Movement Speed Data", &Settings.bRecordBoneSpeedData, T3);
                ImUtil::CheckBox("Actors Panic", &Settings.bPanic, T4);
                ImGui::Spacing();
            }
		}

    }

    void CategoryAI::DrawRight() {

        ImGui::BeginDisabled(!Settings.bEnableActionAI);

        ImUtil_Unique {

            ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kLargeText);
            ImGui::Text("AI Action Options");
            ImFontManager::PopActiveFont();

            static std::string currentItem = "Vore";

            static const std::vector<std::pair<std::string, std::function<void()>>> Items = {

                {"Vore", DrawAIAction_Vore },
                {"Stomps", DrawAIAction_Stomps},
                {"Kicks / Swipes", DrawAIAction_KickSwipe},
                {"Thigh Sandwich", DrawAIAction_ThighSandwich},
                {"Thigh Crush", DrawAIAction_ThighCrush},
                {"Hugs", DrawAIAction_Hugs},
                {"Butt Crush", DrawAIAction_ButtCrush},
                {"Grabs", DrawAIAction_Grab},

            };

            
            if (ImGui::BeginTabBar("OptionsTabBar")) {
                for (const auto& [name, func] : Items) {
                    if (ImGui::BeginTabItem(name.c_str())) {
                        func();
                        ImGui::EndTabItem();
                    }
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::EndDisabled();
    }
}