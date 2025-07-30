#include "UI/Categories/AI.hpp"
#include "UI/DearImGui/imgui.h"
#include "UI/ImGui/ImUtil.hpp"
#include "UI/Windows/GTSInfo.hpp"

namespace GTS {

    void CategoryAI::DrawLeft() {

        ImUtil_Unique{

            const char* T0 = "Globaly Enable/Disable the follower action AI.";


            const char* T1 = "Set the time interval followers should attempt to start a new GTS action.\n"
                             "This does not guarantee a follower will do something every x seconds.\n"
                             "It just changes the time interval an attempt is made to start something.\n"
                             "If anything is done depends on the probabilities configured in each action on the right pane.";

            const char* T2 = "Allow the AI to target the player.";
            const char* T3 = "Allow the AI to target other followers.";
            const char* T4 = "Allow AI to perform actions only if NPC is hostile to performer.";
            const char* T5 = "Only allow the action AI to be active when the follower is in combat.";
            const char* T6 = "Prevent Followers from using regular attacks based on their size, when they're large.\n"
            "- The chance to not attack increases with size.\n"
            "- Works only if enemies are close to Follower";
            const char* T7 = "If on, always prevents Attacks of followers even if there's no enemies nearby";

           

            const char* THelp = "The GTS AI checks at a configurable time interval what GTS actions it can start.\n"
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
            
            const char* T0 = "Toggle whether Followers should have movement speed further reduced past x1.25 size.\n"
                            "- This toggle makes Follower Sprinting less likely based on size, so Followers will try to menacingly walk instead of sprinting\n"
                            "- Alters SpeedMult Actor Value, can reduce it up to 15\n\n"
                            "- This toggle requires Dynamic Animation Speed toggle in order to work";
            const char* T2 = "Toggle whether Followers should have decreased rotation speed past x1.25 size.\n\n"
                            "- This toggle requires Dynamic Animation Speed toggle in order to work";
            const char* T3 = "Toggle whether other NPCs should panic when near a GTS.";

            if (ImGui::CollapsingHeader("Misc Settings",ImUtil::HeaderFlagsDefaultOpen)) {
                ImUtil::CheckBox("Decrease Movement Speed AV", &Settings.bSlowMovementDown, T0);
                ImUtil::CheckBox("Decrease Combat Rotation Speed", &Settings.bSlowRotationDown, T2);
                ImUtil::CheckBox("Actors Panic", &Settings.bPanic, T3);
                ImGui::Spacing();
            }
		}

    }

    void CategoryAI::DrawRight() {

        ImGui::BeginDisabled(!Settings.bEnableActionAI);

        //---- VORE

        ImUtil_Unique{

            const char* T0 = "Enable starting a vore action.";
            const char* T1 = "Set the chance for a vore action to be started.";

            if (ImGui::CollapsingHeader("Vore")) {
                ImUtil::CheckBox("Enable Vore", &Settings.Vore.bEnableAction, T0);
                ImUtil::SliderF("Vore Probability", &Settings.Vore.fProbability, 1.0f, 100.0f, T1,"%.0f%%",!Settings.Vore.bEnableAction);
                ImGui::Spacing();
            }
        }

        //---- STOMP

        ImUtil_Unique{

            const char* T0 = "Enable foot stomps when standing or hand stomps when sneaking / crawling.";
            const char* T1 = "Set the chance for a stomp action to be started.";
            const char* T2 = "Set the chance to pefrom a grind animation when doing under stomps";

            if (ImGui::CollapsingHeader("Stomps")) {
                ImUtil::CheckBox("Enable Stomps", &Settings.Stomp.bEnableAction, T0);
                ImUtil::SliderF("Stomp Probability", &Settings.Stomp.fProbability, 1.0f, 100.0f, T1, "%.0f%%",!Settings.Stomp.bEnableAction);
                ImUtil::SliderF("Grind On Understomp Chance", &Settings.Stomp.fUnderstompGrindProbability, 0.0f, 100.0f, T2, "%.0f%%", !Settings.Stomp.bEnableAction);
                ImGui::Spacing();
            }
        }

        //---- KICK

        ImUtil_Unique{

            const char* T0 = "Enable kicks when standing or hand swipes when sneaking / crawling.";
            const char* T1 = "Set the chance for a kick or swipe action to be started.";

            if (ImGui::CollapsingHeader("Kicks / Swipes")) {
                ImUtil::CheckBox("Enable Kicks / Swipes", &Settings.KickSwipe.bEnableAction, T0);
                ImUtil::SliderF("Kick / Swipe Probability", &Settings.KickSwipe.fProbability, 1.0f, 100.0f, T1,"%.0f%%",!Settings.KickSwipe.bEnableAction);
                ImGui::Spacing();
            }
        }

        //---- THIGH SANDWICH

        ImUtil_Unique{

            const char* T0 = "Enable thigh sandwich actions.";
            const char* T1 = "Set the chance for a thigh sandwich action to be started.";

            const char* T2 = "When a thigh sandwich action has started,\n"
                             "modify the time interval for attack attempts.\n"
                             "If an attack happens, it depends on the probabilities set below.";

            const char* T3 = "Modify the chance to start a heavy attack.";
            const char* T4 = "Modify the chance to start a light attack.";

            if (ImGui::CollapsingHeader("Thigh Sandwich")) {
                ImUtil::CheckBox("Enable Thigh Sandwich", &Settings.ThighSandwich.bEnableAction, T0);

                {
                    ImGui::BeginDisabled(!Settings.ThighSandwich.bEnableAction);
                    ImUtil::SliderF("Thigh Sandwich Probability", &Settings.ThighSandwich.fProbability, 1.0f, 100.0f, T1,"%.0f%%");

                    ImGui::Spacing();

                    ImUtil::SliderF("Action Interval",&Settings.ThighSandwich.fInterval, 1.0f, 5.0f, T2, "Every %.1f Second(s)");
                    ImUtil::SliderF("Heavy Attack Chance",&Settings.ThighSandwich.fProbabilityHeavy, 0.0f, 100.0f, T3, "%.0f%%");
                    ImUtil::SliderF("Light Attack Chance",&Settings.ThighSandwich.fProbabilityLight, 0.0f, 100.0f, T4, "%.0f%%");

                    ImGui::EndDisabled();
                }
                ImGui::Spacing();
            }
        }

        //---- THIGH CRUSH

        ImUtil_Unique{

            const char* T0 = "Enable thigh crush actions.";
            const char* T1 = "Set the chance for a thigh crush action to be started.";

            const char* T2 = "When a thigh crush action has started\n"
                             "modify the time interval for attack attempts.\n"
                             "If an attack happens, it depends on the probabilities set below.";

            const char* T3 = "Modify the chance to perform an attack.";

            if (ImGui::CollapsingHeader("Thigh Crush")) {

                ImUtil::CheckBox("Enable Thigh Crush", &Settings.ThighCrush.bEnableAction, T0);

                {
                    ImGui::BeginDisabled(!Settings.ThighCrush.bEnableAction);
                    ImUtil::SliderF("Thigh Crush Probability", &Settings.ThighCrush.fProbability, 1.0f, 100.0f, T1,"%.0f%%");

                    ImGui::Spacing();

                    ImUtil::SliderF("Action Interval",&Settings.ThighCrush.fInterval, 1.0f, 10.0f, T2, "Every %.1f Second(s)");
                    ImUtil::SliderF("Attack Chance",&Settings.ThighCrush.fProbabilityHeavy, 0.0f, 100.0f, T3, "%.0f%%");

                    ImGui::EndDisabled();
                }

                ImGui::Spacing();
            }
        }

        //---- HUGS

        ImUtil_Unique{
            
            std::string HugsInfo = fmt::format(
                fmt::runtime("Hugs have following conditions to occur:\n"
                "- Size Difference must be greater than x{:.2f}\n"
                "- Size Difference must be less than x2.5\n\n"
                "AI Hugs can happen only if these two conditions are met.\n"
                "Maximal Size Difference can be increased by Hug perks."
                ),
                Action_Hug
            );

            const char* T0 = "Enable hug actions.";
            const char* T1 = "Set the chance for a hug action to be started.";
            const char* T2 = "Allow followers to perform the hug-crush action on other followers.";
            const char* T3 = "Allow followers to perform the hug-crush action on friendly (not in combat) NPCs.";
            const char* T5 = "Set the interval at which an attempt is made to do any of the following hug actions when hugging someone.";
            const char* T6 = "Set the chance to perform a hug heal action.";
            const char* T7 = "Set the chance to perform a hug crush action.";
            const char* T8 = "Set the chance to perform a hug shrink action.";
            const char* T9 = "Set the chance to perform a hug shrink action on a follower/teammate/player";
            const char* T10 = "Should the hugged actor be let go if they can't be shrunk any further.\nApplies only to Followers/Player. Others will be always let go.";
            const char* T11 = HugsInfo.c_str();
            
            if (ImGui::CollapsingHeader("Hugs")) {
                ImGui::TextColored(ImUtil::ColorSubscript,"Why hugs aren't starting (?)");
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip(T11);
                }

                ImUtil::CheckBox("Enable Hugs", &Settings.Hugs.bEnableAction, T0);
                {
                    ImGui::BeginDisabled(!Settings.Hugs.bEnableAction);
                    ImUtil::SliderF("Start Hugs Probability", &Settings.Hugs.fProbability, 1.0f, 100.0f, T1,"%.0f%%");

                    ImGui::Spacing();

                    ImUtil::CheckBox("Allow Crushing (Followers & Player)", &Settings.Hugs.bKillFollowersOrPlayer,T2);
                    ImUtil::CheckBox("Allow Crushing (Friendly NPCs)", &Settings.Hugs.bKillFriendlies,T3);
                    ImUtil::CheckBox("Stop When Too Small (Followers & Player)", &Settings.Hugs.bStopIfCantShrink, T10);

                    ImGui::Spacing();

                    ImUtil::SliderF("Action Interval",&Settings.Hugs.fInterval, 1.0f, 10.0f, T5, "Every %.1f Second(s)");
                    ImUtil::SliderF("Chance To Heal",&Settings.Hugs.fHealProb, 0.0f, 100.0f, T6, "%.0f%%");
                    ImUtil::SliderF("Chance To Crush",&Settings.Hugs.fKillProb, 0.0f, 100.0f, T7, "%.0f%%");
                    ImUtil::SliderF("Chance To Shrink",&Settings.Hugs.fShrinkProb, 0.0f, 100.0f, T8, "%.0f%%");
                    ImUtil::SliderF("Chance To Shrink Friendly", &Settings.Hugs.fFriendlyShrinkProb, 0.0f, 100.0f, T9, "%.0f%%");

                    ImGui::EndDisabled();
                }
                ImGui::Spacing();
            }
        }

        //---- BUTT CRUSH

        ImUtil_Unique{

            const char* T0 = "Enable butt crush actions.";
            const char* T1 = "Set the chance for a butt crush action to be started.";

            const char* T2 = "If the AI decides to start a butt crush action, choose between a fast one or a targeted one.\n"
                             "Modify the chances here:\n"
                             "0%% -> Always perform a fast one.\n"
                             "100%% -> Always perform a targeted one.";

            const char* T3 = "Set the interval at which an attempt is made to do any of the butt crush actions when doing a targeted one.";
            const char* T4 = "Increase/decrease the chance to grow. You should probably keep this value high.";
            const char* T5 = "Increase/decrease the chance to perform the butt crush.\n"
                             "Note: The chance to perform the crush is internally increased based on growth.\n"
                             "If you want the follower to grow often and only crush after a while, keep this value low.";

            if (ImGui::CollapsingHeader("Butt Crush")) {

                ImUtil::CheckBox("Enable Butt Crush", &Settings.ButtCrush.bEnableAction, T0);

                {
                    ImGui::BeginDisabled(!Settings.ButtCrush.bEnableAction);
                    ImUtil::SliderF("Start Butt Crush Probability", &Settings.ButtCrush.fProbability, 1.0f, 100.0f, T1 ,"%.0f%%");
                    ImUtil::SliderF("Fast / Targeted Crush Chance",&Settings.ButtCrush.fButtCrushTypeProb, 0.0f, 100.0f, T2, "%.0f%%");


                    ImGui::Spacing();

                    ImUtil::SliderF("Targeted Action Interval",&Settings.ButtCrush.fInterval, 1.0f, 10.0f, T3, "Every %.1f Second(s)");
                    ImUtil::SliderF("Chance To Grow",&Settings.ButtCrush.fGrowProb, 0.0f, 100.0f, T4, "%.0f%%");
                    ImUtil::SliderF("Chance To Crush",&Settings.ButtCrush.fCrushProb, 0.0f, 100.0f, T5, "%.0f%%");

                    ImGui::EndDisabled();
                }

                ImGui::Spacing();
            }
        }

		//---- GRABS

        ImUtil_Unique{

            const char* T0 = "Enable grab actions.";
            const char* T1 = "Set the chance for the actor to grab someone if possible.";
            const char* T2 = "Set the interval at which an attempt is made to do a grab action when the actor is holding someone.";
            const char* T3 = "Set the chance for the actor to decide to throw the held NPC.";
            const char* T4 = "Set the chance for the actor to vore the held NPC.";
            const char* T5 = "Set the chance for the actor to crush the held NPC.";
            const char* T6 = "Set the chance for the actor to place the held NPC between their breasts.";

            const char* T7 = "If an NPC is between the actor's cleavage, set the chance for an absorb action to start.";
            const char* T8 = "If an NPC is between the actor's' cleavage, set the chance for a vore action to start.";
            const char* T9 = "If an NPC is between the actor's' cleavage, set the chance for a crush attack action to start.";
            const char* T10 = "If an NPC is between the actor's' cleavage, set the chance for a suffocation action to start.";

            const char* T11 = "Set the chance for the actor to be released when grabbed.";
            const char* T12 = "Set the chance cleavage actions to stop.";

            if (ImGui::CollapsingHeader("Grabs")) {

                ImUtil::CheckBox("Enable Grabs", &Settings.Grab.bEnableAction, T0);

                {
                    ImGui::BeginDisabled(!Settings.Grab.bEnableAction);
                    ImUtil::SliderF("Start Grabs Probability", &Settings.Grab.fProbability, 1.0f, 100.0f, T1,"%.0f%%");

                    ImGui::Spacing();

                    ImUtil::SliderF("Action Interval",&Settings.Grab.fInterval, 1.0f, 10.0f, T2, "Every %.1f Second(s)");

                	ImGui::Spacing();
                    ImGui::Text("Grab Actions");
                	ImUtil::SliderF("Chance To Throw",&Settings.Grab.fThrowProb, 0.0f, 100.0f, T3, "%.0f%%");
                    ImUtil::SliderF("Chance To Vore",&Settings.Grab.fVoreProb, 0.0f, 100.0f, T4, "%.0f%%");
                    ImUtil::SliderF("Chance To Crush",&Settings.Grab.fCrushProb, 0.0f, 100.0f, T5, "%.0f%%");
                    ImUtil::SliderF("Chance To Release", &Settings.Grab.fReleaseProb, 0.0f, 100.0f, T11, "%.0f%%");
                   

                	ImGui::Spacing();

                    ImGui::Text("Cleavage Actions");
                    ImUtil::SliderF("Place in Cleavage Chance", &Settings.Grab.fCleavageProb, 0.0f, 100.0f, T6, "%.0f%%");
                    ImUtil::SliderF("Cleavage Absorb Chance",&Settings.Grab.fCleavageAbsorbProb, 0.0f, 100.0f, T7, "%.0f%%");
                    ImUtil::SliderF("Cleavage Vore Chance",&Settings.Grab.fCleavageVoreProb, 0.0f, 100.0f, T8, "%.0f%%");
                    ImUtil::SliderF("Cleavage Crush Chance",&Settings.Grab.fCleavageAttackProb, 0.0f, 100.0f, T9, "%.0f%%");
                    ImUtil::SliderF("Cleavage Suffocate Chance",&Settings.Grab.fCleavageSuffocateProb, 0.0f, 100.0f, T10, "%.0f%%");
                    ImUtil::SliderF("Cleavage Stop Chance", &Settings.Grab.fCleavageStopProb, 0.0f, 100.0f, T12, "%.0f%%");

                    ImGui::EndDisabled();
                }

                ImGui::Spacing();
            }
        }
        ImGui::EndDisabled();
    }
}