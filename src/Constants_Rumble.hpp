#include "Constants.hpp"
#pragma once

namespace GTS {
    //-----------------------------------------Camera Rumble power settings
    constexpr float Rumble_Walk_FootWalk                         = 0.735f; // Used for vanilla anims such as walking, running, sprinting

    constexpr float Rumble_Jump_JumpLand                         = 1.6f; // Used when player jump lands
    constexpr float Rumble_Jump_MassiveJump                      = 2.6f; // Used when player jumps and scale is >= x3.0

    ////////////////////////////////////////////////////

    constexpr float Rumble_Stomp_Normal                             = 2.25f;
    constexpr float Rumble_Stomp_Strong                             = 4.0f;
    constexpr float Rumble_Stomp_Land_Normal                        = 2.75f;

    constexpr float Rumble_Stomp_Under_Light                        = 2.35f;
    constexpr float Rumble_Stomp_Under_Strong                       = 3.85f;


    // Tramples
    constexpr float Rumble_Trample_Stage1                           = 2.2f;
    constexpr float Rumble_Trample_Stage2                           = 3.0f;
    constexpr float Rumble_Trample_Stage3                           = 4.15f;

    // Foot Grind
    constexpr float Rumble_FootGrind_DOT                            = 0.25f;
    constexpr float Rumble_FootGrind_Rotate                         = 0.75f;
    constexpr float Rumble_FootGrind_Impact                         = 2.4f;

    // Hugs

    constexpr float Rumble_Hugs_HugCrush                            = 6.0f;
    constexpr float Rumble_Hugs_Release                             = 4.2f;
    constexpr float Rumble_Hugs_Shrink                              = 2.0f;
    constexpr float Rumble_Hugs_Catch                               = 3.0f;
    constexpr float Rumble_Hugs_Heal                                = 1.6f;

    // Grab
    constexpr float Rumble_Grab_Throw_Footstep                      = 3.0f;
    constexpr float Rumble_Grab_Hand_Attack                         = 3.4f;

    // Thigh Sandwich
    constexpr float Rumble_ThighSandwich_ThighImpact_Heavy          = 2.8f;
    constexpr float Rumble_ThighSandwich_ThighImpact                = 2.0f;
    constexpr float Rumble_ThighSandwich_DropDown                   = 3.2f;

    constexpr float Rumble_ThighSandwich_ButtImpact                 = 2.2f;
    constexpr float Rumble_ThighSandwich_ButtImpact_Heavy           = 3.0f;
    constexpr float Rumble_ThighSandwich_ButtImpact_Finisher        = 3.8f;

    /// Thigh Crush
    constexpr float Rumble_ThighCrush_StandUp                       = 2.0f;
    constexpr float Rumble_ThighCrush_LegSpread_Light_End           = 0.16f;
    constexpr float Rumble_ThighCrush_LegCross_Heavy_End            = 0.18f;

    constexpr float Rumble_ThighCrush_LegSpread_Light_Loop          = 1.20f;
    constexpr float Rumble_ThighCrush_LegSpread_Heavy_Loop          = 1.45f;

    // Furniture Sitting

    constexpr float Rumble_ButtCrush_Sit                            = 1.0f;

    // Butt crush
    constexpr float Rumble_ButtCrush_FeetImpact                     = 2.5f;
    constexpr float Rumble_ButtCrush_ButtImpact                     = 2.9f;  // Butt Crush
    constexpr float Rumble_ButtCrush_Growth                         = 1.25f;

    constexpr float Rumble_ButtCrush_UnderStomp_ButtImpact          = 3.4f;  // Butt Crush

    // Knee Crush
    constexpr float Rumble_KneeCrush_FootImpact                     = 1.75f;

    // Breast crush
    constexpr float Rumble_Cleavage_HoverLoop                       = 0.06f;
    constexpr float Rumble_Body_HoverLoop                           = 0.045f;
    constexpr float Rumble_Cleavage_Impact                          = 2.45f;      // Breast Crush

    // 
    constexpr float Rumble_Clevage_Strangle_ReachedThreshold        = 4.2f;

    // Crawling
    constexpr float Rumble_Crawl_KneeDrop                           = 2.45f;       // Knee Crush
    constexpr float Rumble_Crawl_KneeHand_Impact                    = 1.05f;// A bit higher value since it gets cut off by sneak modifier

    // Finger Grind
    constexpr float Rumble_FingerGrind_Rotate                       = 1.0f;
    constexpr float Rumble_FingerGrind_Impact                       = 1.25f;
    constexpr float Rumble_FingerGrind_Finisher                     = 1.8f;

    // Vore
    constexpr float Rumble_Vore_Stomp_Light                         = 2.0f;

    // Misc
    constexpr float Rumble_Misc_ShrinkOutburst                      = 6.25f; // when performing shrink outburst
    constexpr float Rumble_Misc_MightOfDragons                      = 2.85f; // when growing after gaining dragon soul
    constexpr float Rumble_Misc_TearClothes                         = 3.5f;
    constexpr float Rumble_Misc_TearAllClothes                      = 5.5f;

    constexpr float Rumble_Misc_EnableTinyProtection                = 3.8f;
    constexpr float Rumble_Misc_FailTinyProtection                  = 6.2f;

    constexpr float Rumble_GrowthSpurt_Shrink                       = 0.75f;
    constexpr float Rumble_GrowthSpurt_Grow                         = 0.75f;

    constexpr float Rumble_Growth_SlowGrowth_Start                  = 3.5f;
    constexpr float Rumble_Growth_SlowGrowth_Loop                   = 0.6f;

    constexpr float Rumble_Kill_CrushOther                          = 7.6f;
    constexpr float Rumble_Kill_ShrinkToNothing                     = 8.6f;
    constexpr float Rumble_Furniture_Sit                            = 0.75f;

    constexpr float Rumble_ShrinkPoison_Start                       = 2.0f;
    constexpr float Rumble_ShrinkPoison_Loop                        = 0.4f;

    constexpr float Rumble_GrowthPotion_Start                       = 2.0f;
    constexpr float Rumble_GrowthPotion_Loop                        = 1.0f;

    constexpr float Rumble_ShrinkPotion_Start                       = 2.0f;
    constexpr float Rumble_ShrinkPotion_Loop                        = 0.4f;

    constexpr float Rumble_RestoreSize_Loop                         = 0.6f;

    constexpr float Rumble_Vore_FinishBuff                          = 1.75f;
}