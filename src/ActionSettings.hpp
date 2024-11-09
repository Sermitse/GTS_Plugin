#pragma once

#include "profiler.hpp"

using namespace std;
using namespace RE;
using namespace SKSE;

namespace Gts {
//-----------------------------------------Misc
    const float Collision_Distance_Override = 5.75f;
//-----------------------------------------Size Limits
    const float Minimum_Actor_Scale = 0.04f;
    const float Minimum_Actor_Crush_Scale_Idle = 16.0f; // x16.0 crush threshold for just standing still
    const float Shrink_To_Nothing_After = 3.0f; // Shrink To Nothing immunity in seconds
    const float SHRINK_TO_NOTHING_SCALE = 0.08f;
//-----------------------------------------Default Trigger Threshold Values
    const float Action_Sandwich = 6.0f; // used for sandwich only
    const float Action_AI_ThighCrush = 4.0f; // Used for AI only
	const float Action_Crush = 10.0f;
	const float Action_ButtCrush = 2.0f; // for butt and cleavage crush
	const float Action_Vore = 8.0f;
	const float Action_Grab = 8.0f;
	const float Action_Hug = 0.92f; // for hug grab/drop threshold 
    const float Action_FingerGrind = 6.0f;

//-----------------------------------------Default Damage Values

    const float Damage_Grab_Attack = 4.8f;
    const float Damage_Breast_Squish = 2.2f;
    ////////////////Defaults

    const float Damage_Default_Underfoot = 0.003f; // when we just stand still

    const float Damage_Walk_Defaut = 9.0f; // when we walk around normally
    const float Damage_Jump_Default = 10.6f; // when we jump land

	const float Damage_Stomp = 11.0f;
	const float Damage_Stomp_Strong = 22.0f;

    /////////////////Foot Grind

    const float Damage_Foot_Grind_Impact = 6.8f;
    const float Damage_Foot_Grind_Rotate = 1.4f;
    const float Damage_Foot_Grind_DOT = 0.028f;

    ////////////////Trample

	const float Damage_Trample = 4.0f;
	const float Damage_Trample_Repeat = 5.0f;
	const float Damage_Trample_Finisher = 22.0f;

    ////////////////Butt Crush

	const float Damage_ButtCrush_ButtImpact = 32.0f;
    const float Damage_ButtCrush_HandImpact = 6.0f;
    const float Damage_ButtCrush_LegDrop = 18.0f;
    

    const float Damage_ButtCrush_FootImpact = 6.0f;

    ////////////////Thigh Sandwich
    const float Damage_ThighSandwich_Impact = 0.5f;
    const float Damage_ThighSandwich_FallDownImpact = 12.6f;
    const float Damage_ThighSandwich_DOT = 0.004f;

    ////////////////Thigh Crush
    const float Damage_ThighCrush_Stand_Up = 8.0f;
    const float Damage_ThighCrush_Butt_DOT = 0.003f;
    const float Damage_ThighCrush_Legs_Idle = 0.0012f;
    const float Damage_ThighCrush_CrossLegs_Out = 2.6f; 
    const float Damage_ThighCrush_CrossLegs_In = 3.2f; 
    const float Damage_ThighCrush_CrossLegs_FeetImpact = 2.8f;

    ////////////////breast

    const float Damage_BreastCrush_Body = 18.0f; // for body impact
	const float Damage_BreastCrush_Impact = 32.0f; // when doing impact
    const float Damage_BreastCrush_BodyDOT = 0.0006f; // damage under body
    const float Damage_BreastCrush_BreastDOT = 0.001f; // damage under breasts

    ////////////////Knee

	const float Damage_KneeCrush = 32.0f;

    ////////////////kick

	const float Damage_Kick = 6.0f;
	const float Damage_Kick_Strong = 12.8f;

    ////////////////crawl

    const float Damage_Crawl_Idle = 0.004f;

    const float Damage_Crawl_KneeImpact_Drop = 14.0f;
    const float Damage_Crawl_HandImpact_Drop = 12.0f;

    const float Damage_Crawl_KneeImpact = 6.0f;
    const float Damage_Crawl_HandImpact = 4.2f;

	const float Damage_Crawl_HandSwipe = 5.0f;
	const float Damage_Crawl_HandSwipe_Strong = 10.0f;

    const float Damage_Crawl_HandSlam = 10.0f;
    const float Damage_Crawl_HandSlam_Strong = 18.0f;

    const float Damage_Crawl_Vore_Butt_Impact = 32.0f;

    ////////////////sneaking

	const float Damage_Sneak_HandSwipe = 4.5f;
	const float Damage_Sneak_HandSwipe_Strong = 10.0f;

    const float Damage_Sneak_HandSlam = 4.0f;
    const float Damage_Sneak_HandSlam_Strong = 20.0f;
    const float Damage_Sneak_HandSlam_Strong_Secondary = 2.6f;

    const float Damage_Sneak_FingerGrind_DOT = 0.0032f;
    const float Damage_Sneak_FingerGrind_Impact = 3.2f;
    const float Damage_Sneak_FingerGrind_Finisher = 6.8f;

    ////////////////Vore
    const float Damage_Vore_Standing_Footstep = 8.8f;

    ////////////////Throw

    const float Damage_Throw_Collision = 10.0f; // unused for now, buggy: 
    // actor collides with air and that deals damage, need to fix that first

//-----------------------------------------Default Push Power variables
    // For crawling
    const float Push_Crawl_HandSwipe = 1.35f;           // Used for both Push Actor and Push Object
    const float Push_Crawl_HandSwipe_Strong = 4.2f;     // Used for both Push Actor and Push Object
    // For Sneaking
    const float Push_Sneak_HandSwipe = 1.6f;    
    const float Push_Sneak_HandSwipe_Strong = 4.2f;     // Larger value because of anim speed by default
    // For kicking
    const float Push_Kick_Normal = 1.40f;               // Used for both Push Actor and Push Object
    const float Push_Kick_Strong = 3.80f;               // Used for both Push Actor and Push Object

    // For launching actor(s) when we do initial jump (Not jump land!)
    const float Push_Jump_Launch_Threshold = 8.0f;

    // ----------------------For launching/pushing actors ^-----------------------------------------------

    // Below is For launching objects
    const float Push_Object_Upwards = 0.16f;            // Used for objects only
    const float Push_Object_Forward = 0.0086f;          // Used for objects only
    const float Push_Actor_Upwards = 14.0f;             // Used for Actors only

    

//-----------------------------------------Default effect radius variables

    const float Radius_Default_Idle = 6.6f;

    const float Radius_Walk_Default = 7.0f;
    const float Radius_Jump_Default = 20.0f; 

    const float Radius_Stomp = 7.0f;
    const float Radius_Stomp_Strong = 7.6f;

    /////////Foot Grind
    const float Radius_Foot_Grind_Impact = 7.4f;
    const float Radius_Foot_Grind_DOT = 9.0f;

    /////////Foot Trample
    const float Radius_Trample = 7.0f;
    const float Radius_Trample_Repeat = 7.2f;
    const float Radius_Trample_Finisher = 7.6f;

    /////////Butt Crush

    const float Radius_ButtCrush_Impact = 20.0f;
    const float Radius_ButtCrush_HandImpact = 8.0f;
    const float Radius_ButtCrush_FootImpact = 7.2f;

    /////////Thigh Sandwich
    const float Radius_ThighSandwich_FootFallDown = 8.6f;

    /////////Thigh Crush
    const float Radius_ThighCrush_Butt_DOT = 12.2f;
    const float Radius_ThighCrush_ButtCrush_Drop = 11.8f;
    const float Radius_ThighCrush_Idle = 7.2f;
    
    
    const float Radius_ThighCrush_Spread_In = 9.0f;
    const float Radius_ThighCrush_Spread_Out = 8.5f;

    const float Radius_ThighCrush_ButtImpact = 16.0f;
    const float Radius_ThighCrush_Stand_Up = 6.2f;
    
    ////////Breast Crush

    const float Radius_BreastCrush_BodyImpact = 10.0f;
    const float Radius_BreastCrush_BreastImpact = 16.0f;
    const float Radius_BreastCrush_BodyDOT = 10.0f; 
    const float Radius_BreastCrush_BreastDOT = 14.0f; 

    ///////Proning

    const float Radius_Proning_BodyDOT = 10.0f;

    ////////Crawling
    const float Radius_Crawl_HandSwipe = 20.0f;
    const float Radius_Crawl_KneeImpact = 12.0f;
    const float Radius_Crawl_HandImpact = 12.0f;

    const float Radius_Crawl_KneeImpact_Fall = 18.0f;
    const float Radius_Crawl_HandImpact_Fall = 14.0f;

    const float Radius_Crawl_Slam = 10.0f;
    const float Radius_Crawl_Slam_Strong = 10.0f;

    const float Radius_Crawl_KneeIdle = 7.0f;
    const float Radius_Crawl_HandIdle = 7.0f;

    const float Radius_Crawl_Vore_ButtImpact = 20.0f;

    ///////Sneaking
    const float Radius_Sneak_HandSwipe = 20.0f;
    const float Radius_Sneak_KneeCrush = 16.0f;
    const float Radius_Sneak_HandSlam = 10.0f;
    const float Radius_Sneak_HandSlam_Strong = 10.0f;
    const float Radius_Sneak_HandSlam_Strong_Recover = 8.0f;

    const float Radius_Sneak_FingerGrind_DOT = 4.2f;
    const float Radius_Sneak_FingerGrind_Impact = 4.6f;
    const float Radius_Sneak_FingerGrind_Finisher = 5.4f;

    ///////Vore
    const float Radius_Vore_Standing_Footstep = 7.0f;

    ///////Kicks
    const float Radius_Kick = 19.6f;
    /////////////////////////////////////////////////////


    //-----------------------------------------Camera Rumble power settings
    const float Rumble_Default_FootWalk = 2.10f * 1.0; // Used for vanilla anims such as walking, running, sprinting
    const float Rumble_Default_JumpLand = 1.6f * 1.0; // Multiplies footwalk, used for vanilla anims such as walking, running, sprinting

    const float Rumble_Default_MassiveJump = 2.6f * 1.0; // Used when player jumps and scale is >= x3.0

    ////////////////////////////////////////////////////

    const float Rumble_Stomp_Normal = 2.4f * 1.0;
    const float Rumble_Stomp_Strong = 4.0f * 1.0;
    const float Rumble_Stomp_Land_Normal = 2.75f * 1.0;


    // Tramples
    const float Rumble_Trample_Stage1 = 2.2f * 1.0;
    const float Rumble_Trample_Stage2 = 3.0f * 1.0;
    const float Rumble_Trample_Stage3 = 4.15f * 1.0;

    // Foot Grind
    const float Rumble_FootGrind_DOT = 0.25f;
    const float Rumble_FootGrind_Rotate = 0.75f;
    const float Rumble_FootGrind_Impact = 2.4f * 1.0;

    // Hugs

    const float Rumble_Hugs_HugCrush = 6.0f;
    const float Rumble_Hugs_Release = 4.2f;
    const float Rumble_Hugs_Shrink = 2.0f;
    const float Rumble_Hugs_Catch = 3.0f;
    const float Rumble_Hugs_Heal = 1.6f;

    // Grab
    const float Rumble_Grab_Throw_Footstep = 3.0f;
    const float Rumble_Grab_Hand_Attack = 3.4f;

    // Thigh Sandwich
    const float Rumble_ThighSandwich_ThighImpact_Heavy = 2.8f;
    const float Rumble_ThighSandwich_ThighImpact = 2.0f;
    const float Rumble_ThighSandwich_DropDown = 3.2f;

    /// Thigh Crush
    const float Rumble_ThighCrush_StandUp = 2.0f;
    const float Rumble_ThighCrush_LegSpread_Light_End = 0.16f;
    const float Rumble_ThighCrush_LegCross_Heavy_End = 0.18f;

    const float Rumble_ThighCrush_LegSpread_Light_Loop = 1.20f;
    const float Rumble_ThighCrush_LegSpread_Heavy_Loop = 1.45f;


    // Butt crush
    const float Rumble_ButtCrush_FeetImpact = 2.5f * 1.0;
    const float Rumble_ButtCrush_ButtImpact = 5.8f;  // Butt Crush

    // Knee Crush
    const float Rumble_KneeCrush_FootImpact = 1.75f * 1.0;

    // Breast crush
    const float Rumble_Cleavage_HoverLoop = 0.06f;
    const float Rumble_Cleavage_Impact = 4.9f;      // Breast Crush

    // Crawling
    const float Rumble_Crawl_KneeDrop = 4.9f;       // Knee Crush
    const float Rumble_Crawl_KneeHand_Impact = 2.1f;// A bit higher value since it gets cut off by sneak modifier

    // Finger Grind
    const float Rumble_FingerGrind_Rotate = 1.0f;
    const float Rumble_FingerGrind_Impact = 1.25f;
    const float Rumble_FingerGrind_Finisher = 1.8f;

    // Vore
    const float Rumble_Vore_Stomp_Light = 2.0f;

    // Misc
    const float Rumble_Misc_ShrinkOutburst = 6.25f; // when performing shrink outburst
    const float Rumble_Misc_MightOfDragons = 2.85f; // when growing after gaining dragon soul
    const float Rumble_Misc_TearClothes = 3.5f;
    const float Rumble_Misc_TearAllClothes = 5.5f;

    const float Rumble_Misc_EnableTinyProtection = 3.8f;
    const float Rumble_Misc_FailTinyProtection = 6.2f;

    const float Rumble_Growth_GrowthSpurt = 0.75f;
    const float Rumble_Shrink_GrowthSpurt = 0.75f;

    const float Rumble_Growth_SlowGrowth_Start = 1.75f;
    const float Rumble_Growth_SlowGrowth_Loop = 0.35f;

    const float Rumble_Kill_CrushOther = 7.6f;
    const float Rumble_Kill_ShrinkToNothing = 8.6f;
}