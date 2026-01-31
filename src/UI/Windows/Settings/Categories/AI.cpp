#include "UI/Windows/Settings/Categories/AI.hpp"

#include "UI/Core/ImUtil.hpp"

#include "UI/Controls/CheckBox.hpp"
#include "UI/Controls/CollapsingTabHeader.hpp"
#include "UI/Controls/Slider.hpp"

#include "Config/Config.hpp"

#include "UI/Controls/Text.hpp"

namespace {

    using namespace GTS;

    void DrawAIAction_Vore() {

        PSString T0 = "Enable starting a vore action.";
        PSString T1 = "Set the chance for a vore action to be started.";

        ImGuiEx::CheckBox("Enable Action", &Config::AI.Vore.bEnableAction, T0);
        ImGuiEx::SliderF("Start Chance", &Config::AI.Vore.fProbability, 1.0f, 100.0f, T1, "%.0f%%", !Config::AI.Vore.bEnableAction);
    }

    void DrawAIAction_Stomps() {

        PSString T0 = "Enable foot stomps when standing or hand stomps when sneaking / crawling.";
        PSString T1 = "Set the chance for a stomp action to be started.";
        PSString T2 = "Set the chance to pefrom a grind animation when doing under stomps";

        ImGuiEx::CheckBox("Enable Action", &Config::AI.Stomp.bEnableAction, T0);
        ImGuiEx::SliderF("Start Chance", &Config::AI.Stomp.fProbability, 1.0f, 100.0f, T1, "%.0f%%",!Config::AI.Stomp.bEnableAction);
        ImGuiEx::SliderF("Grind On Understomp Chance", &Config::AI.Stomp.fUnderstompGrindProbability, 0.0f, 100.0f, T2, "%.0f%%", !Config::AI.Stomp.bEnableAction);
            
    }

    void DrawAIAction_KickSwipe() {

        PSString T0 = "Enable kicks when standing or hand swipes when sneaking / crawling.";
        PSString T1 = "Set the chance for a kick or swipe action to be started.";

        ImGuiEx::CheckBox("Enable Action", &Config::AI.KickSwipe.bEnableAction, T0);
        ImGuiEx::SliderF("Start Chance", &Config::AI.KickSwipe.fProbability, 1.0f, 100.0f, T1,"%.0f%%",!Config::AI.KickSwipe.bEnableAction);
            
    }

    void DrawAIAction_ThighSandwich() {

        PSString T0 = "Enable thigh sandwich actions.";
        PSString T1 = "Set the chance for a thigh sandwich action to be started.";

        PSString T2 = "When a thigh sandwich action has started,\n"
			          "modify the time interval for attack attempts.\n"
			          "If an attack happens, it depends on the probabilities set below.";

        PSString T3 = "Modify the chance to start a heavy attack.";
        PSString T4 = "Modify the chance to start a light attack.";

        
        ImGuiEx::CheckBox("Enable Action", &Config::AI.ThighSandwich.bEnableAction, T0);
    
        ImGui::BeginDisabled(!Config::AI.ThighSandwich.bEnableAction);
        {
            ImGuiEx::SliderF("Start Chance", &Config::AI.ThighSandwich.fProbability, 1.0f, 100.0f, T1, "%.0f%%");

            ImGui::Spacing();

            ImGuiEx::SliderF("Action Interval", &Config::AI.ThighSandwich.fInterval, 1.0f, 5.0f, T2, "Every %.1f Second(s)");
            ImGuiEx::SliderF("Heavy Attack Chance", &Config::AI.ThighSandwich.fProbabilityHeavy, 0.0f, 100.0f, T3, "%.0f%%");
            ImGuiEx::SliderF("Light Attack Chance", &Config::AI.ThighSandwich.fProbabilityLight, 0.0f, 100.0f, T4, "%.0f%%");
        }
        ImGui::EndDisabled();

    }

    void DrawAIAction_ThighCrush() {

        PSString T0 = "Enable thigh crush actions.";
        PSString T1 = "Set the chance for a thigh crush action to be started.";

        PSString T2 = "When a thigh crush action has started\n"
                      "modify the time interval for attack attempts.\n"
                      "If an attack happens, it depends on the probabilities set below.";

        PSString T3 = "Modify the chance to perform an attack.";

        ImGuiEx::CheckBox("Enable Action", &Config::AI.ThighCrush.bEnableAction, T0);

        ImGui::BeginDisabled(!Config::AI.ThighCrush.bEnableAction);
        {
            ImGuiEx::SliderF("Start Chance", &Config::AI.ThighCrush.fProbability, 1.0f, 100.0f, T1, "%.0f%%");

            ImGui::Spacing();

            ImGuiEx::SliderF("Action Interval", &Config::AI.ThighCrush.fInterval, 1.0f, 10.0f, T2, "Every %.1f Second(s)");
            ImGuiEx::SliderF("Attack Chance", &Config::AI.ThighCrush.fProbabilityHeavy, 0.0f, 100.0f, T3, "%.0f%%");

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

        PSString T0 = "Enable hug actions.";
        PSString T1 = "Set the chance for a hug action to be started.";
        PSString T2 = "Allow followers to perform the hug-crush action on other followers.";
        PSString T3 = "Allow followers to perform the hug-crush action on friendly (not in combat) NPCs.";
        PSString T5 = "Set the interval at which an attempt is made to do any of the following hug actions when hugging someone.";
        PSString T6 = "Set the chance to perform a hug heal action.";
        PSString T7 = "Set the chance to perform a hug crush action.";
        PSString T8 = "Set the chance to perform a hug shrink action.";
        PSString T9 = "Set the chance to perform a hug shrink action on a follower/teammate/player";
        PSString T10 = "Should the hugged actor be let go if they can't be shrunk any further.\nApplies only to Followers/Player. Others will be always let go.";
        PSString T11 = HugsInfo.c_str();

        ImGuiEx::HelpText("Why aren't hugs starting", T11);
        ImGuiEx::CheckBox("Enable Action", &Config::AI.Hugs.bEnableAction, T0);
        
        ImGui::BeginDisabled(!Config::AI.Hugs.bEnableAction);
        {
            ImGuiEx::SliderF("Start Chance", &Config::AI.Hugs.fProbability, 1.0f, 100.0f, T1, "%.0f%%");

            ImGui::Spacing();

            ImGuiEx::CheckBox("Allow Crushing (Followers & Player)", &Config::AI.Hugs.bKillFollowersOrPlayer, T2);
            ImGuiEx::CheckBox("Allow Crushing (Friendly NPCs)", &Config::AI.Hugs.bKillFriendlies, T3);
            ImGuiEx::CheckBox("Stop When Too Small (Followers & Player)", &Config::AI.Hugs.bStopIfCantShrink, T10);

            ImGui::Spacing();

            ImGuiEx::SliderF("Action Interval", &Config::AI.Hugs.fInterval, 1.0f, 10.0f, T5, "Every %.1f Second(s)");
            ImGuiEx::SliderF("Chance To Heal", &Config::AI.Hugs.fHealProb, 0.0f, 100.0f, T6, "%.0f%%");
            ImGuiEx::SliderF("Chance To Crush", &Config::AI.Hugs.fKillProb, 0.0f, 100.0f, T7, "%.0f%%");
            ImGuiEx::SliderF("Chance To Shrink", &Config::AI.Hugs.fShrinkProb, 0.0f, 100.0f, T8, "%.0f%%");
            ImGuiEx::SliderF("Chance To Shrink Friendly", &Config::AI.Hugs.fFriendlyShrinkProb, 0.0f, 100.0f, T9, "%.0f%%");
        }
        ImGui::EndDisabled();

    }

    void DrawAIAction_ButtCrush() {

        PSString T0 = "Enable butt crush actions.";
        PSString T1 = "Set the chance for a butt crush action to be started.";

        PSString T2 = "If the AI decides to start a butt crush action, choose between a fast one or a targeted one.\n"
		                              "Modify the chances here:\n"
		                              "0%% -> Always perform a fast one.\n"
		                              "100%% -> Always perform a targeted one.";

        PSString T3 = "Set the interval at which an attempt is made to do any of the butt crush actions when doing a targeted one.";
        PSString T4 = "Increase/decrease the chance to grow. You should probably keep this value high.";
        PSString T5 = "Increase/decrease the chance to perform the butt crush.\n"
		                              "Note: The chance to perform the crush is internally increased based on growth.\n"
		                              "If you want the follower to grow often and only crush after a while, keep this value low.";

        ImGuiEx::CheckBox("Enable Action", &Config::AI.ButtCrush.bEnableAction, T0);

        
        ImGui::BeginDisabled(!Config::AI.ButtCrush.bEnableAction);
        {
            ImGuiEx::SliderF("Start Chance", &Config::AI.ButtCrush.fProbability, 1.0f, 100.0f, T1, "%.0f%%");
            ImGuiEx::SliderF("Fast / Targeted Crush Chance", &Config::AI.ButtCrush.fButtCrushTypeProb, 0.0f, 100.0f, T2, "%.0f%%");

            ImGui::Spacing();

            ImGuiEx::SliderF("Targeted Action Interval", &Config::AI.ButtCrush.fInterval, 1.0f, 10.0f, T3, "Every %.1f Second(s)");
            ImGuiEx::SliderF("Chance To Grow", &Config::AI.ButtCrush.fGrowProb, 0.0f, 100.0f, T4, "%.0f%%");
            ImGuiEx::SliderF("Chance To Crush", &Config::AI.ButtCrush.fCrushProb, 0.0f, 100.0f, T5, "%.0f%%");
        }
        ImGui::EndDisabled();

    }

    void DrawAIAction_Grab() {

        PSString T0 = "Enable grab actions.";
        PSString T1 = "Set the chance for the actor to grab someone if possible.";
        PSString T2 = "Set the interval at which an attempt is made to do a grab action when the actor is holding someone.";
        PSString T3 = "Set the chance for the actor to decide to throw the held NPC.";
        PSString T4 = "Set the chance for the actor to vore the held NPC.";
        PSString T5 = "Set the chance for the actor to crush the held NPC.";
        PSString T6 = "Set the chance for the actor to place the held NPC between their breasts.";

        PSString T7 = "If an NPC is between the actor's cleavage, set the chance for an absorb action to start.";
        PSString T8 = "If an NPC is between the actor's' cleavage, set the chance for a vore action to start.";
        PSString T9 = "If an NPC is between the actor's' cleavage, set the chance for a crush attack action to start.";
        PSString T10 = "If an NPC is between the actor's' cleavage, set the chance for a suffocation action to start.";

        PSString T11 = "Set the chance for the actor to be released when grabbed.";
        PSString T12 = "Set the chance cleavage actions to stop.";

		PSString T13 = "Set the chance for the actor to start a grab-play action.";
		PSString T14 = "Set the chance for the actor to perform a heavy crush during grab-play.";
		PSString T15 = "Set the chance for the actor to vore during grab-play.";
		PSString T16 = "Set the chance for the actor to kiss during grab-play.";
		PSString T17 = "Set the chance for the actor to vore while kissing during grab-play.";
		PSString T18 = "Set the chance for the actor to poke during grab-play.";
		PSString T19 = "Set the chance for the actor to flick during grab-play.";
		PSString T20 = "Set the chance for the actor to sandwich during grab-play.";
		PSString T21 = "Set the chance for the actor to start grinding during grab-play.";
		PSString T22 = "Set the chance for the actor to stop grinding during grab-play.";
		PSString T23 = "Set the chance for the actor to exit grab-play.";


        ImGuiEx::CheckBox("Enable Action", &Config::AI.Grab.bEnableAction, T0);

        ImGui::BeginDisabled(!Config::AI.Grab.bEnableAction);
        {
            ImGuiEx::SliderF("Start Chance", &Config::AI.Grab.fProbability, 1.0f, 100.0f, T1, "%.0f%%");
            ImGuiEx::SliderF("Action Interval", &Config::AI.Grab.fInterval, 1.0f, 10.0f, T2, "Every %.1f Second(s)");

            ImGui::Spacing();
            ImGui::Text("Grab Actions");
            ImGuiEx::SliderF("Chance To Throw", &Config::AI.Grab.fThrowProb, 0.0f, 100.0f, T3, "%.0f%%");
            ImGuiEx::SliderF("Chance To Vore", &Config::AI.Grab.fVoreProb, 0.0f, 100.0f, T4, "%.0f%%");
            ImGuiEx::SliderF("Chance To Crush", &Config::AI.Grab.fCrushProb, 0.0f, 100.0f, T5, "%.0f%%");
            ImGuiEx::SliderF("Chance To Release", &Config::AI.Grab.fReleaseProb, 0.0f, 100.0f, T11, "%.0f%%");


            ImGui::Spacing();

            ImGui::Text("Cleavage Actions");
            ImGuiEx::SliderF("Place in Cleavage Chance", &Config::AI.Grab.fCleavageProb, 0.0f, 100.0f, T6, "%.0f%%");
            ImGui::Spacing();
            ImGuiEx::SliderF("Cleavage Absorb Chance", &Config::AI.Grab.fCleavageAbsorbProb, 0.0f, 100.0f, T7, "%.0f%%");
            ImGuiEx::SliderF("Cleavage Vore Chance", &Config::AI.Grab.fCleavageVoreProb, 0.0f, 100.0f, T8, "%.0f%%");
            ImGuiEx::SliderF("Cleavage Crush Chance", &Config::AI.Grab.fCleavageAttackProb, 0.0f, 100.0f, T9, "%.0f%%");
            ImGuiEx::SliderF("Cleavage Suffocate Chance", &Config::AI.Grab.fCleavageSuffocateProb, 0.0f, 100.0f, T10, "%.0f%%");
            ImGuiEx::SliderF("Cleavage Stop Chance", &Config::AI.Grab.fCleavageStopProb, 0.0f, 100.0f, T12, "%.0f%%");

            ImGui::Spacing();

            ImGui::Text("Grab-play Actions");
            ImGuiEx::SliderF("GP Start Chance", &Config::AI.Grab.fGrabPlayStartProb, 0.0f, 100.0f, T13, "%.0f%%");
            ImGui::Spacing();
            ImGuiEx::SliderF("GP Heavy Crush Chance", &Config::AI.Grab.fGrabPlayHeavyCrushProb, 0.0f, 100.0f, T14, "%.0f%%");
            ImGuiEx::SliderF("GP Vore Chance", &Config::AI.Grab.fGrabPlayVoreProb, 0.0f, 100.0f, T15, "%.0f%%");
            ImGuiEx::SliderF("GP Kiss Chance", &Config::AI.Grab.fGrabPlayKissProb, 0.0f, 100.0f, T16, "%.0f%%");
            ImGuiEx::SliderF("GP Kiss-Vore Chance", &Config::AI.Grab.fGrabPlayKissVoreProb, 0.0f, 100.0f, T17, "%.0f%%");
            ImGuiEx::SliderF("GP Poke Chance", &Config::AI.Grab.fGrabPlayPokeProb, 0.0f, 100.0f, T18, "%.0f%%");
            ImGuiEx::SliderF("GP Flick Chance", &Config::AI.Grab.fGrabPlayFlickProb, 0.0f, 100.0f, T19, "%.0f%%");
            ImGuiEx::SliderF("GP Sandwich Chance", &Config::AI.Grab.fGrabPlaySandwichProb, 0.0f, 100.0f, T20, "%.0f%%");

            ImGuiEx::SliderF("GP Grind Start Chance", &Config::AI.Grab.fGrabPlayGrindStartProb, 0.0f, 100.0f, T21, "%.0f%%");
            ImGuiEx::SliderF("GP Grind Stop Chance", &Config::AI.Grab.fGrabPlayGrindStopProb, 0.0f, 100.0f, T22, "%.0f%%");

            ImGuiEx::SliderF("GP Exit Chance", &Config::AI.Grab.fGrabPlayExitProb, 0.0f, 100.0f, T23, "%.0f%%");

        }
        ImGui::EndDisabled();
    }

} 

namespace GTS {

	CategoryAI::CategoryAI() {
		m_name = "AI";
	}

    void CategoryAI::DrawLeft() {

        ImUtil_Unique 
		{

            PSString T0 = "Globaly Enable/Disable the follower action AI.";

            PSString T1 = "Set the time interval followers should attempt to start a new GTS action.\n"
                          "This does not guarantee a follower will do something every x seconds.\n"
                          "It just changes the time interval an attempt is made to start something.\n"
                          "If anything is done depends on the probabilities configured in each action on the right pane.";

            PSString T2 = "Allow the AI to target the player.";
            PSString T3 = "Allow the AI to target other followers.";
            PSString T4 = "Allow AI to perform actions only if NPC is hostile to performer.";
            PSString T5 = "Only allow the action AI to be active when the follower is in combat.";

            PSString T6 = "Prevent Followers from using regular attacks based on their size, when they're large.\n"
                          "- The chance to not attack increases with size.\n"
                          "- Works only if enemies are close to Follower";

            PSString T7 = "If on, always prevents Attacks of followers even if there's no enemies nearby";

            PSString THelp = "The GTS AI checks at a configurable time interval what GTS actions it can start.\n"
                             "Whether the AI takes an action depends on said actions' probability settings.\n"
                             "If its probability percentage is low, the action is less likely to be started.\n"
                             "When all action chances are low, the AI is more likely to simply \"do nothing\".\n"
                             "Some actions (like hugs or grabs) have their own actions (sub-actions) that work on the same idea but with their own settings\n"
                             "whom you can modify on the right pane of this settings page.";

            if (ImGui::CollapsingHeader("AI Settings",ImUtil::HeaderFlagsDefaultOpen)) {

                ImGuiEx::HelpText("How does this work", THelp);
                ImGuiEx::CheckBox("Enable AI",&Config::AI.bEnableActionAI, T0);

                {
                    ImGui::BeginDisabled(!Config::AI.bEnableActionAI);

                    ImGuiEx::SliderF("Try To Start A New Action",&Config::AI.fMasterTimer, 2.0f, 15.0f, T1 ,"Every %.1f Seconds");
                    ImGui::Spacing();

                    ImGuiEx::CheckBox("Target Player", &Config::AI.bAllowPlayer, T2);
                    ImGui::SameLine();
                    ImGuiEx::CheckBox("Target Other Followers", &Config::AI.bAllowFollowers, T3);
                    ImGui::SameLine();
                    ImGuiEx::CheckBox("Target Only Hostiles", &Config::AI.bHostileOnly, T4);

                    ImGuiEx::CheckBox("Enable AI Only During Combat",&Config::AI.bCombatOnly, T5);
                    ImGuiEx::CheckBox("Disable Regular Attacks When Large", &Config::AI.bDisableAttacks, T6);
                    ImGuiEx::CheckBox("Always Disable Regular Attacks When Large", &Config::AI.bAlwaysDisableAttacks, T7, !Config::AI.bDisableAttacks);

                    ImGui::EndDisabled();
                }

                ImGui::Spacing();
            }
        }

        ImUtil_Unique 
		{
            
            PSString T0 = "Toggle whether Followers should have movement speed further reduced past x1.25 size.\n"
                            "- This toggle makes Follower Sprinting less likely based on size, so Followers will try to menacingly walk instead of sprinting\n"
                            "- Alters SpeedMult Actor Value, can reduce it up to 15\n\n"
                            "- This toggle requires Dynamic Animation Speed toggle in order to work";
            PSString T2 = "Toggle whether Followers should have decreased rotation speed past x1.25 size.\n\n"
                            "- This toggle requires Dynamic Animation Speed toggle in order to work";
            PSString T3 = "Toggle whether Followers should record movement speed of their legs/hands\n"
                             "- If on - followers won't push others away if just standing still\n"
                             "- May affect fps in a negative way since it records data each frame for each follower";
            PSString T4 = "Toggle whether other NPCs should panic when near a GTS.";

            if (ImGui::CollapsingHeader("Misc Settings",ImUtil::HeaderFlagsDefaultOpen)) {
                ImGuiEx::CheckBox("Decrease Movement Speed AV", &Config::AI.bSlowMovementDown, T0);
                ImGuiEx::CheckBox("Decrease Combat Rotation Speed", &Config::AI.bSlowRotationDown, T2);
                ImGuiEx::CheckBox("Record Node Movement Speed Data", &Config::AI.bRecordBoneSpeedData, T3);
                ImGuiEx::CheckBox("Actors Panic", &Config::AI.bPanic, T4);
                ImGui::Spacing();
            }
		}

    }

    void CategoryAI::DrawRight() {

		static ImGuiEx::CollapsingTabHeader ActionHeader (
            "AI Action Settings",
			{
				"Vore",
				"Stomps",
                "Kicks / Swipes",
                "Thigh Sandwich",
                "Thigh Crush",
                "Hugs",
                "Butt Crush",
                "Grabs",
			}
        );

        ActionHeader.SetDisabledState(!Config::AI.bEnableActionAI);

        if (ImGuiEx::BeginCollapsingTabHeader(ActionHeader)) {
            // Content based on active tab
            switch (ActionHeader.GetActiveTab()) {
                case 0: DrawAIAction_Vore();          break;
                case 1: DrawAIAction_Stomps();        break;
                case 2: DrawAIAction_KickSwipe();     break;
                case 3: DrawAIAction_ThighSandwich(); break;
                case 4: DrawAIAction_ThighCrush();    break;
                case 5: DrawAIAction_Hugs();          break;
                case 6: DrawAIAction_ButtCrush();     break;
                case 7: DrawAIAction_Grab();          break;
				default:                              break;
            }
        }
        ImGuiEx::EndCollapsingTabHeader(ActionHeader);
    }
}