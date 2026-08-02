#include "Constants.hpp"
#pragma once

namespace GTS {
    struct RumbleParameters {
        float rumble_intensity;
        float rumble_halflife;
        float rumble_duration;
        float duration_multiplier;
        bool ignore_scaling;
        std::string_view node_name = "NPC Root [Root]";
    };
    // Sermit's note:
    // ---
    // Initially, i planned to replace most Rumbling::Once with just passing (Name, Actor, RumbleParameters)
    // But we have too many of these and it requres redoing most of them
    // ---
    // While goal was shortening the code, it would add even more code in some places that needs 2 bones, such as L/R breast / L/R Foot
    // On top of that, some parts of old code multiply 'rumble_intensity', and we'd have to do stuff like 
    //      auto params = Rumble_Params_GrowthPotion_Start
    //      params.rumble_intensity *= bonus
    // In order to achieve same thing, which adds even more code lines, despite goal being shortening it
    // Maybe i'll get back to it someday and that's why im keeping this code, but for now it's abandoned

    //-----------------------------------------Camera Rumble power settings
    constexpr float Rumble_Default_FootWalk                         = 2.10f; // Used for vanilla anims such as walking, running, sprinting
    constexpr float Rumble_Default_JumpLand                         = 1.6f; // Multiplies footwalk, used for vanilla anims such as walking, running, sprinting

    constexpr float Rumble_Default_MassiveJump                      = 2.6f; // Used when player jumps and scale is >= x3.0

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
    constexpr float Rumble_ButtCrush_ButtImpact                     = 5.8f;  // Butt Crush

    constexpr float Rumble_ButtCrush_UnderStomp_ButtImpact          = 3.4f;  // Butt Crush

    // Knee Crush
    constexpr float Rumble_KneeCrush_FootImpact                     = 1.75f;

    // Breast crush
    constexpr float Rumble_Cleavage_HoverLoop                       = 0.06f;
    constexpr float Rumble_Cleavage_Impact                          = 4.9f;      // Breast Crush

    // Crawling
    constexpr float Rumble_Crawl_KneeDrop                           = 4.9f;       // Knee Crush
    constexpr float Rumble_Crawl_KneeHand_Impact                    = 2.1f;// A bit higher value since it gets cut off by sneak modifier

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

    constexpr float Rumble_Growth_GrowthSpurt                       = 0.75f;
    constexpr float Rumble_Shrink_GrowthSpurt                       = 0.75f;

    constexpr float Rumble_Growth_SlowGrowth_Start                  = 3.5f;
    constexpr float Rumble_Growth_SlowGrowth_Loop                   = 0.6f;

    constexpr float Rumble_Kill_CrushOther                          = 7.6f;
    constexpr float Rumble_Kill_ShrinkToNothing                     = 8.6f;
    constexpr float Rumble_Furniture_Sit                            = 1.0f;

    constexpr RumbleParameters Rumble_Params_Default_MassiveJump    = {Rumble_Default_MassiveJump, 0.035f, 1.0f, 0.0f, false};
    constexpr RumbleParameters Rumble_Params_ShrinkPoison_Start     = {2.0f, 0.035f, 1.0f, 0.0f, false};
    constexpr RumbleParameters Rumble_Params_ShrinkPoison_Loop      = {0.4f, 0.05f, 1.0f, 0.0f, false};
    constexpr RumbleParameters Rumble_Params_GrowthPotion_Start     = {2.0f, 0.05f, 1.0f, 0.0f, false};
    constexpr RumbleParameters Rumble_Params_GrowthPotion_Loop      = {1.0f, 0.10f, 1.0f, 0.0f, false, "NPC COM [COM ]"};
    constexpr RumbleParameters Rumble_Params_ShrinkPotion_Start     = {2.0f, 0.05f, 1.0f, 0.0f, false, "NPC COM [COM ]"};
    constexpr RumbleParameters Rumble_Params_ShrinkPotion_Loop      = {0.4f, 0.05f, 1.0f, 0.0f, false, "NPC COM [COM ]"};
    constexpr RumbleParameters Rumble_Params_GrowthSpurt_Shrink     = {Rumble_Shrink_GrowthSpurt, 0.05f, 1.0f, 0.0f};
    constexpr RumbleParameters Rumble_Params_GrowthSpurt_Growth     = {Rumble_Growth_GrowthSpurt, 0.05f, 1.0f, 0.0f, true};
    constexpr RumbleParameters Rumble_Params_RestoreSize_Loop       = {0.6f, 0.05f, 1.0f, 0.0f, false};
    constexpr RumbleParameters Rumble_Params_SlowGrowth_Start       = {Rumble_Growth_SlowGrowth_Start, 0.1f, 1.0f, 0.0f, true, "NPC COM [COM ]"};
    constexpr RumbleParameters Rumble_Params_SlowGrowth_Loop        = {Rumble_Growth_SlowGrowth_Loop, 0.05f, 1.0f, 0.0f, true, "NPC COM [COM ]"};
    constexpr RumbleParameters Rumble_Params_ButtCrush_Sit          = {Rumble_Furniture_Sit, 0.075f, 1.0f, 0.0f, false, "NPC R Butt"};
    constexpr RumbleParameters Rumble_Params_RandomGrowth           = {0.0f, 0.1f, 1.0f, 0.0f, false, "NPC COM [COM ]"}; 
    // ^ Intensity is 0 because it's based on growth power, it's altered by function inside RandomGrowth.cpp
    constexpr RumbleParameters Rumble_Params_TearClothes            = {Rumble_Misc_TearClothes, 0.075f, 1.0f, 0.0f, false,};
    constexpr RumbleParameters Rumble_Params_TearClothes_All        = {Rumble_Misc_TearAllClothes, 0.095f, 1.0f, 0.0f, false,};
}