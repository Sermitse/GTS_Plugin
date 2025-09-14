#pragma once

//-------------------------------------------------------------------------------------------------------------------
//  ENUMS ----- Assumed to be the reference values 
//  magic_enum will use to convert an enum to a string representation for serialization (Saving The TOML)
//-------------------------------------------------------------------------------------------------------------------

enum class LInputCategory_t : uint8_t {
	kDefault = 0,
	kMenu,
	kVore,
	kHugs,
	kThighs,
	kGrab,
	kGrabPlay,
	kStomp,
	kKickSwipe,
	kMovement,
	kCrush,
	kCleavage,
	kCamera,
	kAbility,
	kMisc,
};

enum class LTriggerType_t : uint8_t {
	Once,
	Continuous,
	Release,
};

enum class LBlockInputTypes_t : uint8_t {
	Automatic,
	Always,
	Never,
};

//-------------------------------------------------------------------------------------------------------------------
//  BaseEventData_t Struct ----- Defines the type of data each toml table array entry will contain
//-------------------------------------------------------------------------------------------------------------------

struct BaseEventData_t {
	std::string Event;                      //Event Name
	std::vector<std::string> Keys;          //List Of Dinput key names Minus "DIK_ preffix"
	bool Exclusive = false;                 //Exclusive Flag
	std::string Trigger = "Once";           //Trigger Type
	float Duration = 0.0;                   //Trigger Duration
	std::string BlockInput = "Automatic";   //Block Game Input
	bool Disabled = false;                  //Completely Ignore Event
};
TOML_SERIALIZABLE(BaseEventData_t);

//-------------------------------------------------------------------------------------------------------------------
//  InputEvent_t Struct ----- Wrapper around the base input event.
//  Can hold static data that does not need to be serialized
//-------------------------------------------------------------------------------------------------------------------

struct InputEvent_t {
	LInputCategory_t Category;
	BaseEventData_t Event;
};

namespace GTS {

    //----------------------------------------
    // EVENT LIST ----
    //----------------------------------------

    inline const std::vector<InputEvent_t> DefaultEvents = {
    	
        /* // ----- EXAMPLE
        {
            LInputCategory_t::kDefault,
	        {
	            .Event = "Event",
	            .Keys = {"A", "B"},
	            .Exclusive = false,
	            .Duration = 1.0f,
	            .Trigger = "Once",
	            .BlockInput = "Automatic",
	            .Disabled = false,
	        }
        },
        */

        //=======================================================
	    //================ M E N U
	    //=======================================================

	    {
	        LInputCategory_t::kMenu,
            {
		        .Event = "OpenModSettings",
		        .Keys = {"F1"},
		        .Trigger = "Once",
		        .BlockInput = "Always"
		    }
	    },
        {
            LInputCategory_t::kMenu,
            {
		        .Event = "OpenSkillTree",
		        .Keys = {"F4"},
		        .Trigger = "Once",
		        .BlockInput = "Always"
		    }
        },
        {
            LInputCategory_t::kMenu,
            {
                .Event = "PartyReport",
                .Keys = {"LCONTROL"},
                .Duration = 1.33f,
                .BlockInput = "Never"
            }
        },
        {
            LInputCategory_t::kMenu,
            {
                .Event = "ShowQuickStats",
                .Keys = {"F3"},
                .BlockInput = "Never"
            }
        },
        {
            LInputCategory_t::kMenu,
            {
                .Event = "DebugReport",
                .Keys = {"RCONTROL"},
                .Duration = 1.33f,
                .BlockInput = "Never"
            }
        },

        //========================================================
	    //================ V O R E
	    //========================================================

		{
            LInputCategory_t::kVore,
            {
		        .Event = "Vore",
		        .Keys = {"LSHIFT", "V"},
		        .Trigger = "Once"
		    }
        },
        {
            LInputCategory_t::kVore,
            {
		        .Event = "PlayerVore",
		        .Keys = {"LSHIFT", "V"},
		        .Trigger = "Continuous",
		        .Duration = 1.0,
		        .BlockInput = "Never"
		    }

        },

        //========================================================
	    //================ S T O M P S
	    //========================================================

		{
            LInputCategory_t::kStomp,
            {
		        .Event = "RightStomp",
		        .Keys = {"LSHIFT", "E"},
		        .Trigger = "Release",
		    }
        },
        {
            LInputCategory_t::kStomp,
            {
		        .Event = "LeftStomp",
		        .Keys = {"LSHIFT", "Q"},
		        .Trigger = "Release",
		    }
        },
        {
            LInputCategory_t::kStomp,
            {
		        .Event = "RightStomp_Strong",
		        .Keys = {"LSHIFT", "E"},
		        .Trigger = "Continuous",
		        .Duration = 0.44f,
		    }
        },
        {
            LInputCategory_t::kStomp,
            {
		        .Event = "LeftStomp_Strong",
		        .Keys = {"LSHIFT", "Q"},
		        .Trigger = "Continuous",
		        .Duration = 0.44f,
		    }
        },
		{
            LInputCategory_t::kStomp,
            {
		        .Event = "TrampleRight",
		        .Keys = {"LSHIFT", "E"},
		        .Trigger = "Release",
		        .Duration = 0.20f,
		    }
        },
        {
            LInputCategory_t::kStomp,
            {
		        .Event = "TrampleLeft",
		        .Keys = {"LSHIFT", "Q"},
		        .Trigger = "Release",
		        .Duration = 0.20f,
		    }
        },

        //========================================================
	    //================ T H I G H  C R U S H
	    //========================================================


        {
            LInputCategory_t::kThighs,
            {
		        .Event = "ThighCrush",
		        .Keys = {"LSHIFT", "W", "C"},
		        .Exclusive = true
		    }
        },
        {
            LInputCategory_t::kThighs,
            {
		        .Event = "ThighCrushKill",
		        .Keys = {"LMB"},
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kThighs,
            {
		        .Event = "ThighCrushSpare",
		        .Keys = {"W"},
		        .Exclusive = true,
		        .BlockInput = "Never"
		    }
        },

		//========================================================
		//============ T H I G H  S A N D W I C H
		//========================================================

        {
            LInputCategory_t::kThighs,
            {
		        .Event = "ThighSandwichEnter",
		        .Keys = {"LSHIFT", "C"},
		        .Exclusive = true,
		    }
        },
        {
            LInputCategory_t::kThighs,
            {
		        .Event = "PlayerThighSandwichEnter",
		        .Keys = {"LSHIFT", "C"},
		        .Exclusive = true,
		        .Trigger = "Continuous",
		        .Duration = 1.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kThighs,
            {
		        .Event = "ThighSandwichAttackHeavy",
		        .Keys = {"LMB"},
		        .Trigger = "Continuous",
		        .Duration = 0.33f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kThighs,
            {
		        .Event = "ThighSandwichAttack",
		        .Keys = {"LMB"},
		        .Trigger = "Release",
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kThighs,
            {
		        .Event = "ThighSandwichExit",
		        .Keys = {"W"},
		        .Exclusive = true,
		        .BlockInput = "Never"
		    }
        },

        //========================================================
	    //============ K I C K S
	    //========================================================

        {
            LInputCategory_t::kKickSwipe,
            {
		        .Event = "LightKickRight",
		        .Keys = {"LALT", "E"},
		        .Trigger = "Release",
		    }
        },
        {
            LInputCategory_t::kKickSwipe,
            {
		        .Event = "LightKickLeft",
		        .Keys = {"LALT", "Q"},
		        .Trigger = "Release",
		    }
        },
        {
            LInputCategory_t::kKickSwipe,
            {
		        .Event = "HeavyKickRight",
		        .Keys = {"LALT", "E"},
		        .Trigger = "Continuous",
		        .Duration = 0.33f,
		    }
        },
        {
            LInputCategory_t::kKickSwipe,
            {
		        .Event = "HeavyKickLeft",
		        .Keys = {"LALT", "Q"},
		        .Trigger = "Continuous",
		        .Duration = 0.33f,
		    }
        },
        {
            LInputCategory_t::kKickSwipe,
            {
		        .Event = "HeavyKickRight_Low",
		        .Keys = {"LALT", "E"},
		        .Trigger = "Release",
		        .Duration = 0.1f,
		    }
        },
        {
            LInputCategory_t::kKickSwipe,
            {
		        .Event = "HeavyKickLeft_Low",
		        .Keys = {"LALT", "Q"},
		        .Trigger = "Release",
		        .Duration = 0.1f,
		    }
        },

        //========================================================
	    //============ C R A W L I N G
	    //========================================================

        {
            LInputCategory_t::kMovement,
            {
		        .Event = "TogglePlayerCrawl",
		        .Keys = {"NUMPAD1"},
		    }
        },
        {
            LInputCategory_t::kMovement,
            {
		        .Event = "ToggleFollowerCrawl",
		        .Keys = {"NUMPAD3"},
		    }
        },
        {
            LInputCategory_t::kKickSwipe,
            {
		        .Event = "LightSwipeRight",
		        .Keys = {"LALT", "E"},
		        .Trigger = "Release",
		    }
        },
        {
            LInputCategory_t::kKickSwipe,
            {
		        .Event = "LightSwipeLeft",
		        .Keys = {"LALT", "Q"},
		        .Trigger = "Release",
		    }
        },
        {
            LInputCategory_t::kKickSwipe,
            {
		        .Event = "HeavySwipeRight",
		        .Keys = {"LALT", "E"},
		        .Trigger = "Continuous",
		        .Duration = 0.33f,
		    }
        },
        {
            LInputCategory_t::kKickSwipe,
            {
		        .Event = "HeavySwipeLeft",
		        .Keys = {"LALT", "Q"},
		        .Trigger = "Continuous",
		        .Duration = 0.33f,
		    }
        },

        //========================================================
	    //============ C L E A V A G E
	    //========================================================

        {
            LInputCategory_t::kCleavage,
            {
		        .Event = "CleavageEnter",
		        .Keys = {"LSHIFT", "H"},
		    }
        },
        {
            LInputCategory_t::kCleavage,
            {
		        .Event = "CleavageExit",
		        .Keys = {"RMB"},
		        .BlockInput = "Never"
		    }
        },
		{
            LInputCategory_t::kCleavage,
            {
		        .Event = "CleavageLightAttack",
		        .Keys = {"LMB"},
		        .Exclusive = true,
		        .Trigger = "Release",
		        .BlockInput = "Never"
		    }
		},
        {
            LInputCategory_t::kCleavage,
            {
		        .Event = "CleavageHeavyAttack",
		        .Keys = {"LMB"},
		        .Exclusive = true,
		        .Trigger = "Continuous",
		        .Duration = 0.33f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kCleavage,
            {
		        .Event = "CleavageSuffocate",
		        .Keys = {"W"},
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kCleavage,
            {
		        .Event = "CleavageAbsorb",
		        .Keys = {"S"},
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kCleavage,
            {
		        .Event = "CleavageVore",
		        .Keys = {"V"},
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kCleavage,
            {
		        .Event = "CleavageDOT",
		        .Keys = {"E"},
		    }
        },

        //========================================================
		//============ H U G S
		//========================================================

        {
            LInputCategory_t::kHugs,
            {
		        .Event = "HugAttempt",
		        .Keys = {"LSHIFT", "H"},
		        .Trigger = "Release",
		    }
        },
        {
            LInputCategory_t::kHugs,
            {
		        .Event = "HugPlayer",
		        .Keys = {"LSHIFT", "H"},
		        .Trigger = "Continuous",
		        .Duration = 1.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kHugs,
            {
		        .Event = "HugShrink",
		        .Keys = {"LMB"},
		        .Exclusive = true,
		        .Trigger = "Release"
		    }
        },
        {
            LInputCategory_t::kHugs,
            {
		        .Event = "HugHeal",
		        .Keys = {"LMB"},
		        .Exclusive = true,
		        .Trigger = "Continuous",
		        .Duration = 0.33f,
		    }
        },
        {
            LInputCategory_t::kHugs,
            {
		        .Event = "HugCrush",
		        .Keys = {"S"},
		    }
        },
        {
            LInputCategory_t::kHugs,
            {
		        .Event = "HugRelease",
		        .Keys = {"RMB"},
		        .Exclusive = true,
		        .BlockInput = "Never"
			}
        },

        //========================================================
	    //============ B U T T / B R E A S T   C R U S H
	    //========================================================

        {
            LInputCategory_t::kCrush,
            {
		        .Event = "QuickButtCrushStart",
		        .Keys = {"LSHIFT", "B"},
		        .Trigger = "Continuous",
		        .Duration = 1.0f,
		    }
        },
        {
            LInputCategory_t::kCrush,
            {
		        .Event = "ButtCrushStart",
		        .Keys = {"LSHIFT", "B"},
		        .Trigger = "Release",
		    }
        },
        {
            LInputCategory_t::kCrush,
            {
		        .Event = "ButtCrushStart_Player",
		        .Keys = {"LSHIFT", "B"},
		        .Trigger = "Continuous",
		        .Duration = 1.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kCrush,
            {
		        .Event = "ButtCrushGrow",
		        .Keys = {"W"},
		        .Trigger = "Continuous",
		    }
        },
        {
            LInputCategory_t::kCrush,
            {
		        .Event = "ButtCrushAttack",
		        .Keys = {"LMB"},
		        .Duration = 0.0f,
		    }
        },

        //========================================================
        //======== P R O N E   B E H A V I O R
        //========================================================

        {
            LInputCategory_t::kMovement,
			{
		        .Event = "SBO_ToggleProne",
		        .Keys = {"X"},
		        .Trigger = "Continuous",
		        .Duration = 0.66f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kMovement,
            {
		        .Event = "SBO_ToggleDive_Standing",
		        .Keys = {"W", "S"},
		        .Trigger = "Continuous",
		        .Duration = 0.50f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kMovement,
            {
		        .Event = "SBO_ToggleDive_Sneak",
		        .Keys = {"W", "S"},
		        .Trigger = "Continuous",
		        .Duration = 0.50f,
		        .BlockInput = "Never"
		    }
        },

        //========================================================
	    //========================= G R A B
	    //======================================================== 

        {
            LInputCategory_t::kGrab,
            {
		        .Event = "GrabOther",
		        .Keys = {"F"},
		        .Duration = 0.25f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kGrab,
            {
		        .Event = "GrabPlayer",
		        .Keys = {"F"},
		        .Trigger = "Continuous",
		        .Duration = 1.0f,
		        .BlockInput = "Never"
		    }
        },
        {
		    LInputCategory_t::kGrab,
		    {
				.Event = "GrabAttack",
		        .Keys = {"E"},
		        .Trigger = "Continuous",
		        .Duration = 0.50f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kGrab,
            {
		        .Event = "GrabVore",
		        .Keys = {"V"},
		        .Trigger = "Continuous",
		        .Duration = 0.50f,
		    }
        },
        {
            LInputCategory_t::kGrab,
            {
		        .Event = "GrabThrow",
		        .Keys = {"X"},
		        .Trigger = "Continuous",
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kGrab,
            {
		        .Event = "GrabRelease",
		        .Keys = {"RMB"},
		    }
        },
        {
            LInputCategory_t::kGrab,
            {
		        .Event = "BreastsPut",
		        .Keys = {"LSHIFT", "B"},
		        .Duration = 0.50f,
		    }
        },
        {
            LInputCategory_t::kGrab,
		    {
		        .Event = "BreastsRemove",
		        .Keys = {"LSHIFT", "B"},
		        .Duration = 0.50f,
		    }
        },



        //========================================================
	    //========================= G R A B  P L A Y
	    //======================================================== 

        {
            LInputCategory_t::kGrabPlay,
            {
		        .Event = "GrabPlay_Start",
		        .Keys = {"LSHIFT", "H"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kGrabPlay,
            {
		        .Event = "GrabPlay_Exit",
		        .Keys = {"LSHIFT","H"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kGrabPlay,
            {
		        .Event = "GrabPlay_CrushHeavy",
		        .Keys = {"LMB"},
		        .Trigger = "Continuous",
		        .Duration = 0.5f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kGrabPlay,
            {
		        .Event = "GrabPlay_Vore",
		        .Keys = {"V"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kGrabPlay,
            {
		        .Event = "GrabPlay_KissVore",
		        .Keys = {"V"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kGrabPlay,
            {
		        .Event = "GrabPlay_Kiss",
		        .Keys = {"E"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kGrabPlay,
            {
		        .Event = "GrabPlay_Poke",
		        .Keys = {"LMB"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kGrabPlay,
            {
		        .Event = "GrabPlay_Flick",
		        .Keys = {"RMB"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kGrabPlay,
            {
		        .Event = "GrabPlay_Sandwich",
		        .Keys = {"S"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kGrabPlay,
            {
		        .Event = "GrabPlay_GrindStart",
		        .Keys = {"W"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kGrabPlay,
            {
		        .Event = "GrabPlay_GrindStop",
		        .Keys = {"W"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },

        //========================================================
	    //========================= C A M E R A
	    //========================================================

        {
            LInputCategory_t::kCamera,
            {
		        .Event = "HorizontalCameraReset",
		        .Keys = {"RIGHT", "LEFT"},
		        .Trigger = "Continuous",
		    }
        },
        {
            LInputCategory_t::kCamera,
            {
		        .Event = "VerticalCameraReset",
		        .Keys = {"UP", "DOWN"},
		        .Trigger = "Continuous",
		    }
        },
        {
            LInputCategory_t::kCamera,
            {
		        .Event = "CameraLeft",
		        .Keys = {"LALT", "LEFT"},
		        .Trigger = "Continuous",
		    }
        },
        {
            LInputCategory_t::kCamera,
            {
		        .Event = "CameraRight",
		        .Keys = {"LALT", "RIGHT"},
		        .Trigger = "Continuous",
		    }
        },
        {
            LInputCategory_t::kCamera,
            {
		        .Event = "CameraUp",
		        .Keys = {"LALT", "UP"},
		        .Trigger = "Continuous",
		    }
        },
        {
            LInputCategory_t::kCamera,
            {
		        .Event = "CameraDown",
		        .Keys = {"LALT", "DOWN"},
		        .Trigger = "Continuous",
		    }
        },
        {
            LInputCategory_t::kCamera,
            {
		        .Event = "SwitchCameraMode",
		        .Keys = {"F2"}
		    }
        },

        //========================================================
		//========================= A N I M  S P E E D
		//========================================================

        {
            LInputCategory_t::kMisc,
            {
		        .Event = "AnimSpeedUp",
		        .Keys = {"LMB"},
		        .Trigger = "Continuous",
		    }
        },
        {
            LInputCategory_t::kMisc,
            {
		        .Event = "AnimSpeedDown",
		        .Keys = {"RMB"},
		        .Trigger = "Continuous",
		    }
        },
        {
            LInputCategory_t::kMisc,
            {
		        .Event = "AnimMaxSpeed",
		        .Keys = {"LMB", "RMB"},
		        .Trigger = "Continuous",
		    }
        },

        //========================================================
	    //========================= A B I L I T I E S
	    //========================================================

        {
            LInputCategory_t::kAbility,
            {
		        .Event = "ShrinkOutburst",
		        .Keys = {"LSHIFT", "F"},
		    }
        },
        {
            LInputCategory_t::kAbility,
            {
		        .Event = "SizeReserve",
		        .Keys = {"E"},
		        .Trigger = "Continuous",
		        .Duration = 1.33f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kAbility,
			{
		        .Event = "DisplaySizeReserve",
		        .Keys = {"F"},
		        .Trigger = "Continuous",
		        .Duration = 1.33f,
		        .BlockInput = "Never"
		    }
        },
        {
            LInputCategory_t::kAbility,
            {
		        .Event = "RapidGrowth",
		        .Keys = {"LSHIFT", "1"},
		    }
        },
        {
            LInputCategory_t::kAbility,
            {
		        .Event = "RapidShrink",
		        .Keys = {"LSHIFT", "2"},
		    }
        },
        {
            LInputCategory_t::kAbility,
            {
		        .Event = "ManualGrow",
		        .Keys = {"UP", "LEFT"},
		        .Trigger = "Continuous",
		    }
        },
        {
            LInputCategory_t::kAbility,
            {
		        .Event = "ManualShrink",
		        .Keys = {"DOWN", "LEFT"},
		        .Trigger = "Continuous",
		    }
        },
        {
            LInputCategory_t::kAbility,
            {
		        .Event = "ManualGrowOther",
		        .Keys = {"LSHIFT", "UP", "LEFT"},
		        .Trigger = "Continuous",
		    }
        },
        {
            LInputCategory_t::kAbility,
            {
		        .Event = "ManualShrinkOther",
		        .Keys = {"LSHIFT", "DOWN", "LEFT"},
		        .Trigger = "Continuous",
		    }
        },
        {
            LInputCategory_t::kAbility,
            {
		        .Event = "ManualGrowOverTime",
		        .Keys = {"NUMPAD8"},
		        .Trigger = "Once",
		    }
        },
        {
            LInputCategory_t::kAbility,
            {
		        .Event = "ManualShrinkOverTime",
		        .Keys = {"NUMPAD4"},
		        .Trigger = "Once",
		    }
        },
        {
            LInputCategory_t::kAbility,
            {
		        .Event = "ManualGrowOtherOverTime",
		        .Keys = {"NUMPAD5"},
		        .Trigger = "Once",
		    }
        },
        {
            LInputCategory_t::kAbility,
            {
		        .Event = "ManualShrinkOtherOverTime",
		        .Keys = {"NUMPAD2"},
		        .Trigger = "Once",
		    }
        },
        {
            LInputCategory_t::kAbility,
            {
		        .Event = "ProtectSmallOnes",
		        .Keys = {"C"},
		        .Duration = 1.0f,
		        .BlockInput = "Never"
		    }
        }
    };
}