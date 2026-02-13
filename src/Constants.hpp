#pragma once

namespace GTS {
    //-----------------------------------------Perks
    constexpr float Perk_SizeManipulation_3 = 0.0330f;
    constexpr float Perk_SizeManipulation_2 = 0.0165f;
    constexpr float Perk_SizeManipulation_1 = 0.1350f;
	//-----------------------------------------Misc
    constexpr float Overkills_Obliteration_Diff_Requirement = 60.0f; // Can obliterate past 60x size difference
    constexpr float Overkills_BonusSizePerKill = 0.0006f; // 100 * 0.0006f = 6% for example, 100 = kills
    constexpr float MassMode_ElixirPowerMultiplier = 0.50f;
	constexpr float Collision_Distance_Override = 5.75f;
	constexpr float Characters_AssumedCharSize = 1.82f; // We assume that default humanoid height is 1.82m, used in some calculations
	//-----------------------------------------Size Limits
	constexpr float Minimum_Actor_Scale = 0.04f;
	constexpr float Minimum_Actor_Crush_Scale_Idle = 16.0f; // x16.0 crush threshold for just standing still
	constexpr float Shrink_To_Nothing_After = 3.0f; // Shrink To Nothing immunity in seconds
	constexpr float SHRINK_TO_NOTHING_SCALE = 0.08f;
	//-----------------------------------------Default Trigger Threshold Values
	constexpr float Action_Sandwich      = 6.0f; // used for sandwich only
	constexpr float Action_AI_ThighCrush = 4.0f; // Used for AI only
	constexpr float Action_Crush         = 10.0f;
	constexpr float Action_ButtCrush     = 2.0f; // for butt and cleavage crush
	constexpr float Action_Vore          = 8.0f;
	constexpr float Action_Grab          = 8.0f;
	constexpr float Action_Hug           = 0.92f; // for hug grab/drop threshold 
	constexpr float Action_FingerGrind   = 6.0f;

	//-----------------------------------------Default Damage Values

    constexpr float Damage_Grab_Play_Light = 2.8f;
    constexpr float Damage_Grab_Play_Heavy = 6.2f;
	constexpr float Damage_Grab_Attack = 4.8f;
	constexpr float Damage_Breast_Squish = 2.2f;
	constexpr float Damage_Breast_Strangle = 0.00980f;
    ////////////////Defaults

    constexpr float Damage_Default_Underfoot = 0.003f; // when we just stand still

    constexpr float Damage_Walk_Defaut = 9.0f; // when we walk around normally
    constexpr float Damage_Jump_Default = 10.6f; // when we jump land

    constexpr float Damage_Stomp = 11.0f;
    constexpr float Damage_Stomp_Strong = 22.0f;

    constexpr float Damage_Stomp_Under_Light = 13.2f; // Slightly more powerful damage
    constexpr float Damage_Stomp_Under_Strong = 20.6f; // Slightly weaker than normal strong stomp
    constexpr float Damage_Stomp_Under_LegLand = 16.2f; // When legs land after butt crush during strong sneak under-stomp 

    constexpr float Damage_Stomp_Under_Breast_Legs = 14.2f; // When legs land after breast impact
    constexpr float Damage_Stomp_Under_Breast_Body = 20.0f; // When body hits the target
    constexpr float Damage_Stomp_Under_Breast_Breasts = 30.0f; // When breasts hit the target 

    /////////////////Foot Grind

    constexpr float Damage_Foot_Grind_Impact = 6.8f;
    constexpr float Damage_Foot_Grind_Rotate = 1.4f;
    constexpr float Damage_Foot_Grind_DOT = 0.028f;

    ////////////////Trample

    constexpr float Damage_Trample = 4.0f;
    constexpr float Damage_Trample_Repeat = 5.0f;
    constexpr float Damage_Trample_Finisher = 22.0f;

    ////////////////Furniture Sit
    constexpr float Damage_ButtCrush_Sit = 12.0f;
    constexpr float Damage_ButtCrush_Idle = 0.0002f;

    ////////////////Butt Crush

    constexpr float Damage_ButtCrush_Under_ButtImpact = 24.4f;

    constexpr float Damage_ButtCrush_ButtImpact = 32.0f;
    constexpr float Damage_ButtCrush_HandImpact = 6.0f;
    constexpr float Damage_ButtCrush_LegDrop = 18.0f;

    constexpr float Damage_ButtCrush_FootImpact = 6.0f;

    ////////////////Thigh Sandwich
    constexpr float Damage_ThighSandwich_Impact_Initial = 0.05f;
    constexpr float Damage_ThighSandwich_Impact_Light = 0.4f;
    constexpr float Damage_ThighSandwich_Impact_Heavy = 1.0f;
    ///////////////Thigh Sandwich: Second Branch
    constexpr float Damage_ThighSandwich_Butt_Light = 1.25f;
    constexpr float Damage_ThighSandwich_Butt_Heavy = 1.75f;
    constexpr float Damage_ThighSandwich_Butt_Grind = 0.008f;
    ///////////////

    constexpr float Damage_ThighSandwich_FallDownImpact = 12.6f; // When falling down with Foot from rune
    constexpr float Damage_ThighSandwich_DOT = 0.003f;

    ////////////////Thigh Crush
    constexpr float Damage_ThighCrush_Stand_Up = 8.0f;
    constexpr float Damage_ThighCrush_Butt_DOT = 0.003f;
    constexpr float Damage_ThighCrush_Legs_Idle = 0.0012f;
    constexpr float Damage_ThighCrush_CrossLegs_Out = 2.6f;
    constexpr float Damage_ThighCrush_CrossLegs_In = 3.2f;
    constexpr float Damage_ThighCrush_CrossLegs_FeetImpact = 2.8f;

    ////////////////breast

    constexpr float Damage_BreastCrush_Body = 18.0f; // for body impact
    constexpr float Damage_BreastCrush_BreastImpact = 32.0f; // when doing breast impact
    constexpr float Damage_BreastCrush_BodyDOT = 0.0006f; // damage under body
    constexpr float Damage_BreastCrush_BreastDOT = 0.001f; // damage under breasts

    ////////////////Knee

    constexpr float Damage_KneeCrush = 32.0f;

    ////////////////kick

    constexpr float Damage_Kick = 6.0f;
    constexpr float Damage_Kick_Strong = 12.8f;

    ////////////////crawl

    constexpr float Damage_Crawl_Idle = 0.004f;

    constexpr float Damage_Crawl_KneeImpact_Drop = 14.0f;
    constexpr float Damage_Crawl_HandImpact_Drop = 12.0f;

    constexpr float Damage_Crawl_KneeImpact = 6.0f;
    constexpr float Damage_Crawl_HandImpact = 4.2f;

    constexpr float Damage_Crawl_HandSwipe = 5.0f;
    constexpr float Damage_Crawl_HandSwipe_Strong = 10.0f;

    constexpr float Damage_Crawl_HandSlam = 10.0f;
    constexpr float Damage_Crawl_HandSlam_Strong = 18.0f;

    constexpr float Damage_Crawl_Vore_Butt_Impact = 32.0f;

    ////////////////sneaking

    constexpr float Damage_Sneak_HandSwipe = 4.5f;
    constexpr float Damage_Sneak_HandSwipe_Strong = 10.0f;

    constexpr float Damage_Sneak_HandSlam = 5.2f;
    constexpr float Damage_Sneak_HandSlam_Sneak = 7.2f;
    constexpr float Damage_Sneak_HandSlam_Strong = 20.0f;
    constexpr float Damage_Sneak_HandSlam_Strong_Secondary = 2.6f;

    constexpr float Damage_Sneak_FingerGrind_DOT = 0.0032f;
    constexpr float Damage_Sneak_FingerGrind_Impact = 3.2f;
    constexpr float Damage_Sneak_FingerGrind_Finisher = 6.8f;

    ////////////////Vore
    constexpr float Damage_Vore_Standing_Footstep = 8.8f;

    ////////////////Throw

    constexpr float Damage_Throw_Collision = 10.0f; // unused for now, buggy: 
    // actor collides with air and that deals damage, need to fix that first

	//-----------------------------------------Default Push Power variables
    // For crawling
    constexpr float Push_Crawl_HandSwipe = 1.35f;           // Used for both Push Actor and Push Object
    constexpr float Push_Crawl_HandSwipe_Strong = 4.2f;     // Used for both Push Actor and Push Object
    // For Sneaking
    constexpr float Push_Sneak_HandSwipe = 1.6f;
    constexpr float Push_Sneak_HandSwipe_Strong = 4.2f;     // Larger value because of anim speed by default
    // For kicking
    constexpr float Push_Kick_Normal = 1.40f;               // Used for both Push Actor and Push Object
    constexpr float Push_Kick_Strong = 3.80f;               // Used for both Push Actor and Push Object

    // For launching actor(s) when we do initial jump (Not jump land!)
    constexpr float Push_Jump_Launch_Threshold = 8.0f;

    // ----------------------For launching/pushing actors ^-----------------------------------------------

    // Below is For launching objects
    constexpr float Push_Object_Upwards = 0.16f;            // Used for objects only
    constexpr float Push_Object_Forward = 0.0086f;          // Used for objects only
    constexpr float Push_Actor_Upwards = 14.0f;             // Used for Actors only

	//-----------------------------------------Default effect radius variables

	constexpr float Radius_Default_Idle = 5.6f;

	constexpr float Radius_Walk_Default = 6.2f;
	constexpr float Radius_Jump_Default = 10.0f;

	constexpr float Radius_Stomp = 6.0f;
	constexpr float Radius_Stomp_Strong = 6.25f;

	constexpr float Radius_UnderStomp_Butt_Impact = 19.0f;

    /////////Foot Grind
    constexpr float Radius_Foot_Grind_Impact = 6.6f;
    constexpr float Radius_Foot_Grind_DOT = 8.0f;

    /////////Foot Trample
    constexpr float Radius_Trample = 6.2f;
    constexpr float Radius_Trample_Repeat = 6.4f;
    constexpr float Radius_Trample_Finisher = 6.6f;

    /////////Furniture Sit
    constexpr float Radius_ButtCrush_Sit = 15.4f;

    /////////Butt Crush

    constexpr float Radius_ButtCrush_Impact = 20.0f;
    constexpr float Radius_ButtCrush_HandImpact = 8.0f;
    constexpr float Radius_ButtCrush_FootImpact = 7.2f;

    /////////Thigh Sandwich
    constexpr float Radius_ThighSandwich_FootFallDown = 8.6f;

    /////////Thigh Crush
    constexpr float Radius_ThighCrush_Butt_DOT = 12.2f;
    constexpr float Radius_ThighCrush_ButtCrush_Drop = 11.8f;
    constexpr float Radius_ThighCrush_Idle = 7.2f;


    constexpr float Radius_ThighCrush_Spread_In = 9.0f;
    constexpr float Radius_ThighCrush_Spread_Out = 8.5f;

    constexpr float Radius_ThighCrush_ButtImpact = 16.0f;
    constexpr float Radius_ThighCrush_Stand_Up = 6.2f;
    
    ////////Breast Crush

    constexpr float Radius_BreastCrush_BodyImpact = 10.0f;
    constexpr float Radius_BreastCrush_BreastImpact = 13.2f;
    constexpr float Radius_BreastCrush_BodyDOT = 10.0f;
    constexpr float Radius_BreastCrush_BreastDOT = 10.0f; 

    ///////Proning

    constexpr float Radius_Proning_BodyDOT = 10.0f;

    ////////Crawling
    constexpr float Radius_Crawl_HandSwipe = 20.0f;
    constexpr float Radius_Crawl_KneeImpact = 12.0f;
    constexpr float Radius_Crawl_HandImpact = 12.0f;

    constexpr float Radius_Crawl_KneeImpact_Fall = 18.0f;
    constexpr float Radius_Crawl_HandImpact_Fall = 14.0f;

    constexpr float Radius_Crawl_Slam = 10.0f;
    constexpr float Radius_Crawl_Slam_Strong = 10.0f;

    constexpr float Radius_Crawl_KneeIdle = 7.0f;
    constexpr float Radius_Crawl_HandIdle = 7.0f;

    constexpr float Radius_Crawl_Vore_ButtImpact = 20.0f;

    ///////Sneaking
    constexpr float Radius_Sneak_HandSwipe = 20.0f;
    constexpr float Radius_Sneak_KneeCrush = 16.0f;
    constexpr float Radius_Sneak_HandSlam = 10.0f;
    constexpr float Radius_Sneak_HandSlam_Strong = 10.0f;
    constexpr float Radius_Sneak_HandSlam_Strong_Recover = 8.0f;

    constexpr float Radius_Sneak_FingerGrind_DOT = 4.2f;
    constexpr float Radius_Sneak_FingerGrind_Impact = 4.6f;
    constexpr float Radius_Sneak_FingerGrind_Finisher = 5.4f;

    ///////Vore
    constexpr float Radius_Vore_Standing_Footstep = 7.0f;

    ///////Kicks
    constexpr float Radius_Kick = 19.6f;
    /////////////////////////////////////////////////////


    //-----------------------------------------Camera Rumble power settings
    constexpr float Rumble_Default_FootWalk = 2.10f * 1.0f; // Used for vanilla anims such as walking, running, sprinting
    constexpr float Rumble_Default_JumpLand = 1.6f * 1.0f; // Multiplies footwalk, used for vanilla anims such as walking, running, sprinting

    constexpr float Rumble_Default_MassiveJump = 2.6f * 1.0f; // Used when player jumps and scale is >= x3.0

    ////////////////////////////////////////////////////

    constexpr float Rumble_Stomp_Normal = 2.25f * 1.0f;
    constexpr float Rumble_Stomp_Strong = 4.0f * 1.0f;
    constexpr float Rumble_Stomp_Land_Normal = 2.75f * 1.0f;

    constexpr float Rumble_Stomp_Under_Light = 2.35f * 1.0f;
    constexpr float Rumble_Stomp_Under_Strong = 3.85f * 1.0f;


    // Tramples
    constexpr float Rumble_Trample_Stage1 = 2.2f * 1.0f;
    constexpr float Rumble_Trample_Stage2 = 3.0f * 1.0f;
    constexpr float Rumble_Trample_Stage3 = 4.15f * 1.0f;

    // Foot Grind
    constexpr float Rumble_FootGrind_DOT = 0.25f;
    constexpr float Rumble_FootGrind_Rotate = 0.75f;
    constexpr float Rumble_FootGrind_Impact = 2.4f * 1.0f;

    // Hugs

    constexpr float Rumble_Hugs_HugCrush = 6.0f;
    constexpr float Rumble_Hugs_Release = 4.2f;
    constexpr float Rumble_Hugs_Shrink = 2.0f;
    constexpr float Rumble_Hugs_Catch = 3.0f;
    constexpr float Rumble_Hugs_Heal = 1.6f;

    // Grab
    constexpr float Rumble_Grab_Throw_Footstep = 3.0f;
    constexpr float Rumble_Grab_Hand_Attack = 3.4f;

    // Thigh Sandwich
    constexpr float Rumble_ThighSandwich_ThighImpact_Heavy = 2.8f;
    constexpr float Rumble_ThighSandwich_ThighImpact = 2.0f;
    constexpr float Rumble_ThighSandwich_DropDown = 3.2f;

    constexpr float Rumble_ThighSandwich_ButtImpact = 2.2f;
    constexpr float Rumble_ThighSandwich_ButtImpact_Heavy = 3.0f;
    constexpr float Rumble_ThighSandwich_ButtImpact_Finisher = 3.8f;

    /// Thigh Crush
    constexpr float Rumble_ThighCrush_StandUp = 2.0f;
    constexpr float Rumble_ThighCrush_LegSpread_Light_End = 0.16f;
    constexpr float Rumble_ThighCrush_LegCross_Heavy_End = 0.18f;

    constexpr float Rumble_ThighCrush_LegSpread_Light_Loop = 1.20f;
    constexpr float Rumble_ThighCrush_LegSpread_Heavy_Loop = 1.45f;

    // Furniture Sitting

    constexpr float Rumble_ButtCrush_Sit = 2.0f;

    // Butt crush
    constexpr float Rumble_ButtCrush_FeetImpact = 2.5f * 1.0f;
    constexpr float Rumble_ButtCrush_ButtImpact = 5.8f;  // Butt Crush

    constexpr float Rumble_ButtCrush_UnderStomp_ButtImpact = 3.4f;  // Butt Crush

    // Knee Crush
    constexpr float Rumble_KneeCrush_FootImpact = 1.75f * 1.0f;

    // Breast crush
    constexpr float Rumble_Cleavage_HoverLoop = 0.06f;
    constexpr float Rumble_Cleavage_Impact = 4.9f;      // Breast Crush

    // Crawling
    constexpr float Rumble_Crawl_KneeDrop = 4.9f;       // Knee Crush
    constexpr float Rumble_Crawl_KneeHand_Impact = 2.1f;// A bit higher value since it gets cut off by sneak modifier

    // Finger Grind
    constexpr float Rumble_FingerGrind_Rotate = 1.0f;
    constexpr float Rumble_FingerGrind_Impact = 1.25f;
    constexpr float Rumble_FingerGrind_Finisher = 1.8f;

    // Vore
    constexpr float Rumble_Vore_Stomp_Light = 2.0f;

    // Misc
    constexpr float Rumble_Misc_ShrinkOutburst = 6.25f; // when performing shrink outburst
    constexpr float Rumble_Misc_MightOfDragons = 2.85f; // when growing after gaining dragon soul
    constexpr float Rumble_Misc_TearClothes = 3.5f;
    constexpr float Rumble_Misc_TearAllClothes = 5.5f;

    constexpr float Rumble_Misc_EnableTinyProtection = 3.8f;
    constexpr float Rumble_Misc_FailTinyProtection = 6.2f;

    constexpr float Rumble_Growth_GrowthSpurt = 0.75f;
    constexpr float Rumble_Shrink_GrowthSpurt = 0.75f;

    constexpr float Rumble_Growth_SlowGrowth_Start = 1.75f;
    constexpr float Rumble_Growth_SlowGrowth_Loop = 0.35f;

    constexpr float Rumble_Kill_CrushOther = 7.6f;
    constexpr float Rumble_Kill_ShrinkToNothing = 8.6f;

    //=========================================================================================
    //Zoom-in parameters
    constexpr float ZoomIn_butt = 0.75f;
    constexpr float ZoomIn_knees = 1.25f;

    constexpr float ZoomIn_Breast02 = 0.75f;
    constexpr float ZoomIn_ThighCrush = 1.0f;
    constexpr float ZoomIn_ThighSandwich = 1.0f;
    constexpr float ZoomIn_RightHand = 0.75f;
    constexpr float ZoomIn_LeftHand = 0.75f;

    constexpr float ZoomIn_GrabLeft = 0.65f;

    constexpr float ZoomIn_RightFoot = 0.825f;
    constexpr float ZoomIn_LeftFoot = 0.825f;

    constexpr float ZoomIn_ButtLegs = 1.25f;
    constexpr float ZoomIn_VoreRight = 1.25f;

    constexpr float ZoomIn_FingerRight = 0.65f;
    constexpr float ZoomIn_FingerLeft = 0.65f;

    constexpr float ZoomIn_ObjectA = 1.0f;
    constexpr float ZoomIn_ObjectB = 1.0f;
    constexpr float ZoomIn_ObjectL = 0.8f;

    //////////////////
    constexpr float ZoomIn_Cam_Spine = 1.0f;
    constexpr float ZoomIn_Cam_Clavicle = 1.0f;
    constexpr float ZoomIn_Cam_Breasts = 1.0f;
    constexpr float ZoomIn_Cam_3BABreasts_00 = 1.0f;
    constexpr float ZoomIn_Cam_3BABreasts_01 = 1.0f;
    constexpr float ZoomIn_Cam_3BABreasts_02 = 1.0f;
    constexpr float ZoomIn_Cam_3BABreasts_03 = 1.0f;
    constexpr float ZoomIn_Cam_3BABreasts_04 = 1.0f;
    constexpr float ZoomIn_Cam_Genitals = 1.0f;
    constexpr float ZoomIn_Cam_Belly = 1.0f;
    constexpr float ZoomIn_Cam_Neck = 1.0f;
    constexpr float ZoomIn_Cam_Butt = 1.0f;

    //////////////////
    
    constexpr float ZoomIn_LookAt_BothFeet = 0.5f;

}