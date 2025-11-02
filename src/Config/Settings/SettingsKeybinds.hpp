#pragma once
#include "Config/Util/TomlRefl.hpp"

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
//  Can hold static data that does not need to be serialized/saved.
//-------------------------------------------------------------------------------------------------------------------

struct InputEvent_t {

	const LInputCategory_t UICategory;
	const bool UIHidden;
	const std::string UIName;
	const std::string UIDescription;

	BaseEventData_t Event;
};

namespace GTS {

    //----------------------------------------
    // EVENT LIST ----
    //----------------------------------------

	//TODO Eats 21kb on the stack :skull:... Find a way to fix this...
    inline static const std::vector<InputEvent_t> DefaultEvents = {
    	
        /* // ----- EXAMPLE
        {
            .UICategory = LInputCategory_t::kDefault,
		   .UIHidden = false,
		   .UIName = "",
		   .UIDescription = "",
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
	        .UICategory = LInputCategory_t::kMenu,
		    .UIHidden = false,
		    .UIName = "",
		    .UIDescription = "",
            .Event = {
		        .Event = "OpenModSettings",
		        .Keys = {"F1"},
		        .Trigger = "Once",
		        .BlockInput = "Always"
		    }
	    },
        {
			.UICategory = LInputCategory_t::kMenu,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "OpenSkillTree",
		        .Keys = {"F4"},
		        .Trigger = "Once",
		        .BlockInput = "Always"
		    }
        },
		{
			.UICategory = LInputCategory_t::kMenu,
			.UIHidden = true,
			.UIName = "",
			.UIDescription = "",
			.Event = {
				.Event = "OpenDebugMenu",
				.Keys = {"LCONTROL", "LSHIFT", "F12"},
				.Trigger = "Once",
				.BlockInput = "Always"
			}
		},
        {
			.UICategory = LInputCategory_t::kMenu,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
                .Event = "PartyReport",
                .Keys = {"LCONTROL"},
                .Duration = 1.33f,
                .BlockInput = "Never"
            }
        },
        {
			.UICategory = LInputCategory_t::kMenu,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
                .Event = "ShowQuickStats",
                .Keys = {"F3"},
                .BlockInput = "Never"
            }
        },
        {
			.UICategory = LInputCategory_t::kMenu,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
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
			.UICategory = LInputCategory_t::kVore,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "Vore",
		        .Keys = {"LSHIFT", "V"},
		        .Trigger = "Once"
		    }
        },
        {
			.UICategory = LInputCategory_t::kVore,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
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
			.UICategory = LInputCategory_t::kStomp,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "RightStomp",
		        .Keys = {"LSHIFT", "E"},
		        .Trigger = "Release",
		    }
        },
        {
			.UICategory = LInputCategory_t::kStomp,
        	.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "LeftStomp",
		        .Keys = {"LSHIFT", "Q"},
		        .Trigger = "Release",
		    }
        },
        {
			.UICategory = LInputCategory_t::kStomp,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "RightStomp_Strong",
		        .Keys = {"LSHIFT", "E"},
		        .Trigger = "Continuous",
		        .Duration = 0.44f,
		    }
        },
        {
			.UICategory = LInputCategory_t::kStomp,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "LeftStomp_Strong",
		        .Keys = {"LSHIFT", "Q"},
		        .Trigger = "Continuous",
		        .Duration = 0.44f,
		    }
        },
		{
			.UICategory = LInputCategory_t::kStomp,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "TrampleRight",
		        .Keys = {"LSHIFT", "E"},
		        .Trigger = "Release",
		        .Duration = 0.20f,
		    }
        },
        {
			.UICategory = LInputCategory_t::kStomp,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
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
			.UICategory = LInputCategory_t::kThighs,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ThighCrush",
		        .Keys = {"LSHIFT", "W", "C"},
		        .Exclusive = true
		    }
        },
        {
			.UICategory = LInputCategory_t::kThighs,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ThighCrushKill",
		        .Keys = {"LMB"},
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kThighs,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
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
			.UICategory = LInputCategory_t::kThighs,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ThighSandwichEnter",
		        .Keys = {"LSHIFT", "C"},
		        .Exclusive = true,
		    }
        },
        {
			.UICategory = LInputCategory_t::kThighs,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "PlayerThighSandwichEnter",
		        .Keys = {"LSHIFT", "C"},
		        .Exclusive = true,
		        .Trigger = "Continuous",
		        .Duration = 1.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kThighs,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ThighSandwichAttackHeavy",
		        .Keys = {"LMB"},
		        .Trigger = "Continuous",
		        .Duration = 0.33f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kThighs,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ThighSandwichAttack",
		        .Keys = {"LMB"},
		        .Trigger = "Release",
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kThighs,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
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
			.UICategory = LInputCategory_t::kKickSwipe,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "LightKickRight",
		        .Keys = {"LALT", "E"},
		        .Trigger = "Release",
		    }
        },
        {
			.UICategory = LInputCategory_t::kKickSwipe,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "LightKickLeft",
		        .Keys = {"LALT", "Q"},
		        .Trigger = "Release",
		    }
        },
        {
			.UICategory = LInputCategory_t::kKickSwipe,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "HeavyKickRight",
		        .Keys = {"LALT", "E"},
		        .Trigger = "Continuous",
		        .Duration = 0.33f,
		    }
        },
        {
			.UICategory = LInputCategory_t::kKickSwipe,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "HeavyKickLeft",
		        .Keys = {"LALT", "Q"},
		        .Trigger = "Continuous",
		        .Duration = 0.33f,
		    }
        },
        {
			.UICategory = LInputCategory_t::kKickSwipe,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "HeavyKickRight_Low",
		        .Keys = {"LALT", "E"},
		        .Trigger = "Release",
		        .Duration = 0.1f,
		    }
        },
        {
			.UICategory = LInputCategory_t::kKickSwipe,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
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
			.UICategory = LInputCategory_t::kMovement,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "TogglePlayerCrawl",
		        .Keys = {"NUMPAD1"},
		    }
        },
        {
			.UICategory = LInputCategory_t::kMovement,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ToggleFollowerCrawl",
		        .Keys = {"NUMPAD3"},
		    }
        },
        {
			.UICategory = LInputCategory_t::kKickSwipe,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "LightSwipeRight",
		        .Keys = {"LALT", "E"},
		        .Trigger = "Release",
		    }
        },
        {
			.UICategory = LInputCategory_t::kKickSwipe,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "LightSwipeLeft",
		        .Keys = {"LALT", "Q"},
		        .Trigger = "Release",
		    }
        },
        {
			.UICategory = LInputCategory_t::kKickSwipe,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "HeavySwipeRight",
		        .Keys = {"LALT", "E"},
		        .Trigger = "Continuous",
		        .Duration = 0.33f,
		    }
        },
        {
			.UICategory = LInputCategory_t::kKickSwipe,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
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
			.UICategory = LInputCategory_t::kCleavage,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "CleavageEnter",
		        .Keys = {"LSHIFT", "H"},
		    }
        },
        {
			.UICategory = LInputCategory_t::kCleavage,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "CleavageExit",
		        .Keys = {"RMB"},
		        .BlockInput = "Never"
		    }
        },
		{
			.UICategory = LInputCategory_t::kCleavage,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "CleavageLightAttack",
		        .Keys = {"LMB"},
		        .Exclusive = true,
		        .Trigger = "Release",
		        .BlockInput = "Never"
		    }
		},
        {
			.UICategory = LInputCategory_t::kCleavage,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "CleavageHeavyAttack",
		        .Keys = {"LMB"},
		        .Exclusive = true,
		        .Trigger = "Continuous",
		        .Duration = 0.33f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kCleavage,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "CleavageSuffocate",
		        .Keys = {"W"},
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kCleavage,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "CleavageAbsorb",
		        .Keys = {"S"},
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kCleavage,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "CleavageVore",
		        .Keys = {"V"},
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kCleavage,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "CleavageDOT",
		        .Keys = {"E"},
		    }
        },

        //========================================================
		//============ H U G S
		//========================================================

        {
			.UICategory = LInputCategory_t::kHugs,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "HugAttempt",
		        .Keys = {"LSHIFT", "H"},
		        .Trigger = "Release",
		    }
        },
        {
			.UICategory = LInputCategory_t::kHugs,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "HugPlayer",
		        .Keys = {"LSHIFT", "H"},
		        .Trigger = "Continuous",
		        .Duration = 1.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kHugs,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "HugShrink",
		        .Keys = {"LMB"},
		        .Exclusive = true,
		        .Trigger = "Release"
		    }
        },
        {
			.UICategory = LInputCategory_t::kHugs,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "HugHeal",
		        .Keys = {"LMB"},
		        .Exclusive = true,
		        .Trigger = "Continuous",
		        .Duration = 0.33f,
		    }
        },
        {
			.UICategory = LInputCategory_t::kHugs,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "HugCrush",
		        .Keys = {"S"},
		    }
        },
        {
			.UICategory = LInputCategory_t::kHugs,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
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
			.UICategory = LInputCategory_t::kCrush,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "QuickButtCrushStart",
		        .Keys = {"LSHIFT", "B"},
		        .Trigger = "Continuous",
		        .Duration = 1.0f,
		    }
        },
        {
			.UICategory = LInputCategory_t::kCrush,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ButtCrushStart",
		        .Keys = {"LSHIFT", "B"},
		        .Trigger = "Release",
		    }
        },
        {
			.UICategory = LInputCategory_t::kCrush,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ButtCrushStart_Player",
		        .Keys = {"LSHIFT", "B"},
		        .Trigger = "Continuous",
		        .Duration = 1.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kCrush,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ButtCrushGrow",
		        .Keys = {"W"},
		        .Trigger = "Continuous",
		    }
        },
        {
			.UICategory = LInputCategory_t::kCrush,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ButtCrushAttack",
		        .Keys = {"LMB"},
		        .Duration = 0.0f,
		    }
        },

        //========================================================
        //======== P R O N E   B E H A V I O R
        //========================================================

        {
			.UICategory = LInputCategory_t::kMovement,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "SBO_ToggleProne",
		        .Keys = {"X"},
		        .Trigger = "Continuous",
		        .Duration = 0.66f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kMovement,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "SBO_ToggleDive_Standing",
		        .Keys = {"W", "S"},
		        .Trigger = "Continuous",
		        .Duration = 0.50f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kMovement,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
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
			.UICategory = LInputCategory_t::kGrab,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "GrabOther",
		        .Keys = {"F"},
		        .Duration = 0.25f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kGrab,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "GrabPlayer",
		        .Keys = {"F"},
		        .Trigger = "Continuous",
		        .Duration = 1.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kGrab,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
				.Event = "GrabAttack",
		        .Keys = {"E"},
		        .Trigger = "Continuous",
		        .Duration = 0.50f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kGrab,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "GrabVore",
		        .Keys = {"V"},
		        .Trigger = "Continuous",
		        .Duration = 0.50f,
		    }
        },
        {
			.UICategory = LInputCategory_t::kGrab,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "GrabThrow",
		        .Keys = {"X"},
		        .Trigger = "Continuous",
		        .BlockInput = "Never"
		    }
        },
        {
		    .UICategory = LInputCategory_t::kGrab,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "GrabRelease",
		        .Keys = {"RMB"},
		    }
        },
        {
			.UICategory = LInputCategory_t::kGrab,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "BreastsPut",
		        .Keys = {"LSHIFT", "B"},
		        .Duration = 0.50f,
		    }
        },
        {
			.UICategory = LInputCategory_t::kGrab,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "BreastsRemove",
		        .Keys = {"LSHIFT", "B"},
		        .Duration = 0.50f,
		    }
        },



        //========================================================
	    //========================= G R A B  P L A Y
	    //======================================================== 

        {
			.UICategory = LInputCategory_t::kGrabPlay,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "GrabPlay_Start",
		        .Keys = {"LSHIFT", "H"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kGrabPlay,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "GrabPlay_Exit",
		        .Keys = {"LSHIFT","H"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kGrabPlay,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "GrabPlay_CrushHeavy",
		        .Keys = {"LMB"},
		        .Trigger = "Continuous",
		        .Duration = 0.5f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kGrabPlay,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "GrabPlay_Vore",
		        .Keys = {"V"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kGrabPlay,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "GrabPlay_KissVore",
		        .Keys = {"V"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kGrabPlay,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "GrabPlay_Kiss",
		        .Keys = {"E"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kGrabPlay,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "GrabPlay_Poke",
		        .Keys = {"LMB"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kGrabPlay,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "GrabPlay_Flick",
		        .Keys = {"RMB"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kGrabPlay,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "GrabPlay_Sandwich",
		        .Keys = {"S"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kGrabPlay,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "GrabPlay_GrindStart",
		        .Keys = {"W"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kGrabPlay,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
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
			.UICategory = LInputCategory_t::kCamera,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "HorizontalCameraReset",
		        .Keys = {"RIGHT", "LEFT"},
		        .Trigger = "Continuous",
		    }
        },
        {
			.UICategory = LInputCategory_t::kCamera,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "VerticalCameraReset",
		        .Keys = {"UP", "DOWN"},
		        .Trigger = "Continuous",
		    }
        },
        {
			.UICategory = LInputCategory_t::kCamera,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "CameraLeft",
		        .Keys = {"LALT", "LEFT"},
		        .Trigger = "Continuous",
		    }
        },
        {
			.UICategory = LInputCategory_t::kCamera,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "CameraRight",
		        .Keys = {"LALT", "RIGHT"},
		        .Trigger = "Continuous",
		    }
        },
        {
			.UICategory = LInputCategory_t::kCamera,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "CameraUp",
		        .Keys = {"LALT", "UP"},
		        .Trigger = "Continuous",
		    }
        },
        {
			.UICategory = LInputCategory_t::kCamera,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "CameraDown",
		        .Keys = {"LALT", "DOWN"},
		        .Trigger = "Continuous",
		    }
        },
        {
			.UICategory = LInputCategory_t::kCamera,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "SwitchCameraMode",
		        .Keys = {"F2"}
		    }
        },

        //========================================================
		//========================= A N I M  S P E E D
		//========================================================

        {
			.UICategory = LInputCategory_t::kMisc,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "AnimSpeedUp",
		        .Keys = {"LMB"},
		        .Trigger = "Continuous",
		    }
        },
        {
			.UICategory = LInputCategory_t::kMisc,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "AnimSpeedDown",
		        .Keys = {"RMB"},
		        .Trigger = "Continuous",
		    }
        },
        {
			.UICategory = LInputCategory_t::kMisc,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "AnimMaxSpeed",
		        .Keys = {"LMB", "RMB"},
		        .Trigger = "Continuous",
		    }
        },

        //========================================================
	    //========================= A B I L I T I E S
	    //========================================================

        {
			.UICategory = LInputCategory_t::kAbility,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ShrinkOutburst",
		        .Keys = {"LSHIFT", "F"},
		    }
        },
        {
			.UICategory = LInputCategory_t::kAbility,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "SizeReserve",
		        .Keys = {"E"},
		        .Trigger = "Continuous",
		        .Duration = 1.33f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kAbility,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "DisplaySizeReserve",
		        .Keys = {"F"},
		        .Trigger = "Continuous",
		        .Duration = 1.33f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory = LInputCategory_t::kAbility,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "RapidGrowth",
		        .Keys = {"LSHIFT", "1"},
		    }
        },
        {
			.UICategory = LInputCategory_t::kAbility,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "RapidShrink",
		        .Keys = {"LSHIFT", "2"},
		    }
        },
        {
			.UICategory = LInputCategory_t::kAbility,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ManualGrow",
		        .Keys = {"UP", "LEFT"},
		        .Trigger = "Continuous",
		    }
        },
        {
			.UICategory = LInputCategory_t::kAbility,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ManualShrink",
		        .Keys = {"DOWN", "LEFT"},
		        .Trigger = "Continuous",
		    }
        },
        {
			.UICategory = LInputCategory_t::kAbility,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ManualGrowOther",
		        .Keys = {"LSHIFT", "UP", "LEFT"},
		        .Trigger = "Continuous",
		    }
        },
        {
			.UICategory = LInputCategory_t::kAbility,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ManualShrinkOther",
		        .Keys = {"LSHIFT", "DOWN", "LEFT"},
		        .Trigger = "Continuous",
		    }
        },
        {
			.UICategory = LInputCategory_t::kAbility,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ManualGrowOverTime",
		        .Keys = {"NUMPAD8"},
		        .Trigger = "Once",
		    }
        },
        {
			.UICategory = LInputCategory_t::kAbility,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ManualShrinkOverTime",
		        .Keys = {"NUMPAD4"},
		        .Trigger = "Once",
		    }
        },
        {
			.UICategory = LInputCategory_t::kAbility,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ManualGrowOtherOverTime",
		        .Keys = {"NUMPAD5"},
		        .Trigger = "Once",
		    }
        },
        {
			.UICategory = LInputCategory_t::kAbility,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ManualShrinkOtherOverTime",
		        .Keys = {"NUMPAD2"},
		        .Trigger = "Once",
		    }
        },
        {
			.UICategory = LInputCategory_t::kAbility,
			.UIHidden = false,
			.UIName = "",
			.UIDescription = "",
			.Event = {
		        .Event = "ProtectSmallOnes",
		        .Keys = {"C"},
		        .Duration = 1.0f,
		        .BlockInput = "Never"
		    }
        }
    };
}