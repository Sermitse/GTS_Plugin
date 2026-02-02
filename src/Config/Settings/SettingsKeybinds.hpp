#pragma once
#include "Config/Util/TomlRefl.hpp"

//-------------------------------------------------------------------------------------------------------------------
//  ENUMS ----- Assumed to be the reference values 
//  magic_enum will use to convert an enum to a string representation for serialization (Saving The TOML)
//-------------------------------------------------------------------------------------------------------------------

//Don't forget to add the entries to CategoryNames as well if adding any new ones.
enum class LInputCategory_t : uint8_t {
	kDefault = 0,
	kMenu,
	kVore,
	kHugs,
	kThighs,
	kGrab,
	kBreats,
	kGrabPlay,
	kStomp,
	kKickSwipe,
	kMovement,
	kCrush,
	kCleavage,
	kCamera,
	kAbility,
	kMisc,

	kTotal
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
	const bool AdvFeature; //Disables/Hides Keybind if Advanced Settings is Disabled
	const char* const UIName = nullptr;
	const char* const UIDescription = nullptr;

	BaseEventData_t Event;
};

namespace Strings::Keybinds {
	constexpr PSString CategoryNames[] = {
		"Default",
		"Menu / UI",
		"Vore",
		"Hugs",
		"Thighs",
		"Grabs",
		"(Grabs) Breasts",
		"(Grabs) Play",
		"Stomps",
		"Kicks / Swipes",
		"Movement",
		"Crush",
		"(Grabs) Cleavage",
		"Camera",
		"Abilities / Perks",
		"Other"
	};

	static_assert(std::size(CategoryNames) == static_cast<size_t>(LInputCategory_t::kTotal));

}

namespace GTS {

    //----------------------------------------
    // EVENT LIST ----
    //----------------------------------------

    static inline const std::vector<InputEvent_t> DefaultEvents = {
    	
        /* // ----- EXAMPLE
        {
           .UICategory    = LInputCategory_t::kDefault,
		   .AdvFeature    = false,
		   .UIName        = nullptr,
		   .UIDescription = nullptr,
	        {
	            .Event      = "Event",
	            .Keys       = {"A", "B"},
	            .Exclusive  = false,
	            .Duration   = 1.0f,
	            .Trigger    = "Once",
	            .BlockInput = "Automatic",
	            .Disabled   = false,
	        }
        },
        */

		/*Note
		 * If the UI fields are left empty "" or nullptr
		 * the event name is used instead and no description will be displayed on hover.
		 */



        //=======================================================
	    //================ M E N U
	    //=======================================================

	    {
	        .UICategory    = LInputCategory_t::kMenu,
		    .AdvFeature    = false,
		    .UIName        = "Settings Menu",
		    .UIDescription = "Open this window, you can alternatively disable this keybind and use the \"gts menu\" console command to open it again.",
            .Event = {
		        .Event      = "OpenModSettings",
		        .Keys       = {"F1"},
		        .Trigger    = "Once",
		        .BlockInput = "Always"
		    }
	    },
        {
	        .UICategory    = LInputCategory_t::kMenu,
		    .AdvFeature    = false,
		    .UIName        = "Skill Tree",
		    .UIDescription = "Open the skill tree if \"Custom Skills Framework\" is Installed",
			.Event = {
		        .Event      = "OpenSkillTree",
		        .Keys       = {"F4"},
		        .Trigger    = "Once",
		        .BlockInput = "Always"
		    }
        },
		{
	        .UICategory    = LInputCategory_t::kMenu,
		    .AdvFeature    = true,
		    .UIName        = "Debug Menu",
		    .UIDescription = "Open the debug menu.",
			.Event = {
				.Event      = "OpenDebugMenu",
				.Keys       = {"LCONTROL", "LSHIFT", "F12"},
				.Trigger    = "Once",
				.BlockInput = "Always"
			}
		},
        {
	        .UICategory    = LInputCategory_t::kMenu,
		    .AdvFeature    = false,
		    .UIName        = "Quick Stats",
		    .UIDescription = "Show brief size info for the player and current followers.",
			.Event = {
                .Event      = "ShowQuickStats",
                .Keys       = {"F3"},
                .BlockInput = "Never"
            }
        },
        {
	        .UICategory    = LInputCategory_t::kMenu,
		    .AdvFeature    = true,
		    .UIName        = "Size Report",
		    .UIDescription = "Prints NPC/Player debug information in the console.",
			.Event = {
                .Event      = "DebugReport",
                .Keys       = {"RCONTROL"},
                .Duration   = 1.33f,
                .BlockInput = "Never"
            }
        },

        //========================================================
	    //================ V O R E
	    //========================================================

		{
	        .UICategory    = LInputCategory_t::kVore,
		    .AdvFeature    = false,
		    .UIName        = "Start Vore",
		    .UIDescription = "Start the basic vore action.",
			.Event = {
		        .Event      = "Vore",
		        .Keys       = {"LSHIFT", "V"},
		        .Trigger    = "Once"
		    }
        },
        {
	        .UICategory    = LInputCategory_t::kVore,
		    .AdvFeature    = false,
		    .UIName        = "Start Vore (Player)",
		    .UIDescription = "Starts the basic vore action with the player as the target.",
			.Event = {
		        .Event      = "PlayerVore",
		        .Keys       = {"LSHIFT", "V"},
		        .Trigger    = "Continuous",
		        .Duration   = 1.0,
		        .BlockInput = "Never"
		    }

        },

        //========================================================
	    //================ S T O M P S
	    //========================================================

		{
	        .UICategory    = LInputCategory_t::kStomp,
		    .AdvFeature    = false,
		    .UIName        = "Start L. Stomp (Right)",
		    .UIDescription = "Start a light stomp action.",
			.Event = {
		        .Event      = "RightStomp",
		        .Keys       = {"LSHIFT", "E"},
		        .Trigger    = "Release",
		    }
        },
        {
	        .UICategory    = LInputCategory_t::kStomp,
		    .AdvFeature    = false,
		    .UIName        = "Start L. Stomp (Left)",
		    .UIDescription = "Start a light stomp action.",
			.Event = {
		        .Event      = "LeftStomp",
		        .Keys       = {"LSHIFT", "Q"},
		        .Trigger    = "Release",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kStomp,
			.AdvFeature    = false,
			.UIName        = "Start H. Stomp (Right)",
			.UIDescription = "Start a heavy stomp action.",
			.Event = {
		        .Event      = "RightStomp_Strong",
		        .Keys       = {"LSHIFT", "E"},
		        .Trigger    = "Continuous",
		        .Duration   = 0.44f,
		    }
        },
        {
			.UICategory    = LInputCategory_t::kStomp,
			.AdvFeature    = false,
			.UIName        = "Start H. Stomp (Left)",
			.UIDescription = "Start a heavy stomp action.",
			.Event = {
		        .Event      = "LeftStomp_Strong",
		        .Keys       = {"LSHIFT", "Q"},
		        .Trigger    = "Continuous",
		        .Duration   = 0.44f,
		    }
        },
		{
			.UICategory    = LInputCategory_t::kStomp,
			.AdvFeature    = false,
			.UIName        = "Start Trample (Right)",
			.UIDescription = "Start a trample action.",
			.Event = {
		        .Event      = "TrampleRight",
		        .Keys       = {"LSHIFT", "E"},
		        .Trigger    = "Release",
		        .Duration   = 0.20f,
		    }
        },
        {
			.UICategory    = LInputCategory_t::kStomp,
			.AdvFeature    = false,
			.UIName        = "Start Trample (Left)",
			.UIDescription = "Start a trample action.",
			.Event = {
		        .Event      = "TrampleLeft",
		        .Keys       = {"LSHIFT", "Q"},
		        .Trigger    = "Release",
		        .Duration   = 0.20f,
		    }
        },

        //========================================================
	    //================ T H I G H  C R U S H
	    //========================================================


        {
			.UICategory    = LInputCategory_t::kThighs,
			.AdvFeature    = false,
			.UIName        = "Start Thigh-Crush",
			.UIDescription = "Start the thigh crush action.",
			.Event = {
		        .Event      = "ThighCrush",
		        .Keys       = {"LSHIFT", "W", "C"},
		        .Exclusive  = true
		    }
        },
        {
			.UICategory    = LInputCategory_t::kThighs,
			.AdvFeature    = false,
			.UIName        = "Thigh-Crush Attack",
			.UIDescription = "Perform a thigh crush attack / kill action.",
			.Event = {
		        .Event      = "ThighCrushKill",
		        .Keys       = {"LMB"},
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kThighs,
			.AdvFeature    = false,
			.UIName        = "Thigh-Crush Exit",
			.UIDescription = "Exit thigh crush mode.",
			.Event = {
		        .Event      = "ThighCrushSpare",
		        .Keys       = {"W"},
		        .Exclusive  = true,
		        .BlockInput = "Never"
		    }
        },

		//========================================================
		//============ T H I G H  S A N D W I C H
		//========================================================

        {
			.UICategory    = LInputCategory_t::kThighs,
			.AdvFeature    = false,
			.UIName        = "Start Thigh-Sandwich",
			.UIDescription = "Start the thigh sandwich action.",
			.Event = {
		        .Event      = "ThighSandwichEnter",
		        .Keys       = {"LSHIFT", "C"},
		        .Exclusive  = true,
		    }
        },
        {
			.UICategory    = LInputCategory_t::kThighs,
			.AdvFeature    = false,
			.UIName        = "Start Thigh-Sandwich (Player)",
			.UIDescription = "Starts the thigh sandwich action with the player as the target.",
			.Event = {
		        .Event      = "PlayerThighSandwichEnter",
		        .Keys       = {"LSHIFT", "C"},
		        .Exclusive  = true,
		        .Trigger    = "Continuous",
		        .Duration   = 1.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kThighs,
			.AdvFeature    = false,
			.UIName        = "Thigh-Sandwich H. Attack",
			.UIDescription = "Perform a heavy attack while in thigh sandwich mode.",
			.Event = {
		        .Event      = "ThighSandwichAttackHeavy",
		        .Keys       = {"LMB"},
		        .Trigger    = "Continuous",
		        .Duration   = 0.33f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kThighs,
			.AdvFeature    = false,
			.UIName        = "Thigh-Sandwich L. Attack",
			.UIDescription = "Perform a light attack while in thigh sandwich mode.",
			.Event = {
		        .Event      = "ThighSandwichAttack",
		        .Keys       = {"LMB"},
		        .Trigger    = "Release",
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kThighs,
			.AdvFeature    = false,
			.UIName        = "Thigh-Sandwich Exit",
			.UIDescription = "Exit thigh sandwich mode.",
			.Event = {
		        .Event      = "ThighSandwichExit",
		        .Keys       = {"W"},
		        .Exclusive  = true,
		        .BlockInput = "Never"
		    }
        },


		//========================================================
		//============ T H I G H  S A N D W I C H - P A R T  2
		//========================================================

		{
			.UICategory = LInputCategory_t::kThighs,
			.AdvFeature = false,
			.UIName = "Thigh-Sandwich Butt Start",
			.UIDescription = "Enter thigh-sandwich butt mode.",
			.Event = {
				.Event = "SandwichButtStart",
				.Keys = {"LSHIFT", "H"},
				.Exclusive = true,
			}
		},
		{
			.UICategory = LInputCategory_t::kThighs,
			.AdvFeature = false,
			.UIName = "Thigh-Sandwich Butt Stop",
			.UIDescription = "Exit thigh-sandwich butt mode.",
			.Event = {
				.Event = "SandwichButtStop",
				.Keys = {"LSHIFT", "H"},
				.Exclusive = true,
			}
		},
		{
			.UICategory = LInputCategory_t::kThighs,
			.AdvFeature = false,
			.UIName = "Thigh-Sandwich Butt Attack H.",
			.UIDescription = "Perform a heavy butt attack while in thigh-sandwich butt mode.",
			.Event = {
				.Event = "SandwichHeavyAttack",
				.Keys       = {"LMB"},
		        .Exclusive  = true,
		        .Trigger    = "Continuous",
		        .Duration   = 0.33f,
		        .BlockInput = "Never"
			}
		},
		{
			.UICategory = LInputCategory_t::kThighs,
			.AdvFeature = false,
			.UIName = "Thigh-Sandwich Butt Attack L.",
			.UIDescription = "Perform a light butt attack while in thigh-sandwich butt mode.",
			.Event = {
				.Event = "SandwichLightAttack",
				.Keys = {"LMB"},
				.Exclusive = true,
				.Trigger = "Release"
			}
		},
		{
			.UICategory = LInputCategory_t::kThighs,
			.AdvFeature = false,
			.UIName = "Thigh-Sandwich Grow",
			.UIDescription = "Grow in size while in thigh-sandwich butt mode. Requires perk.",
			.Event = {
				.Event = "SandwichGrowth",
				.Keys = {"W"},
				.Exclusive = true
			}
		},
		{
			.UICategory = LInputCategory_t::kThighs,
			.AdvFeature = false,
			.UIName = "Thigh-Sandwich Grind Start",
			.UIDescription = "Grind the one under your butt over time",
			.Event = {
				.Event = "SandwichGrindStart",
				.Keys = {"S"},
				.Exclusive = true,
			}
		},
		{
			.UICategory = LInputCategory_t::kThighs,
			.AdvFeature = false,
			.UIName = "Thigh-Sandwich Grind Stop",
			.UIDescription = "Stop Grindind the one under your butt",
			.Event = {
				.Event = "SandwichGrindStop",
				.Keys = {"S"},
				.Exclusive = true,
			}
		},

		{
			.UICategory = LInputCategory_t::kThighs,
			.AdvFeature = false,
			.UIName = "Thigh-Sandwich Unbirth",
			.UIDescription = "TODO",
			.Event = {
				.Event = "SandwichUB",
				.Keys = {"V"},
				.Exclusive = true,
				.Duration = 0.33f,
			}
		},

        //========================================================
	    //============ K I C K S
	    //========================================================

        {
			.UICategory    = LInputCategory_t::kKickSwipe,
			.AdvFeature    = false,
			.UIName        = "Start L. Kick (Right)",
			.UIDescription = "Perform a light kick.",
			.Event = {
		        .Event      = "LightKickRight",
		        .Keys       = {"LALT", "E"},
		        .Trigger    = "Release",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kKickSwipe,
			.AdvFeature    = false,
			.UIName        = "Start L. Kick (Left)",
			.UIDescription = "Perform a light kick.",
			.Event = {
		        .Event      = "LightKickLeft",
		        .Keys       = {"LALT", "Q"},
		        .Trigger    = "Release",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kKickSwipe,
			.AdvFeature    = false,
			.UIName        = "Start H. Kick (Right)",
			.UIDescription = "Perform a heavy kick.",
			.Event = {
		        .Event      = "HeavyKickRight",
		        .Keys       = {"LALT", "E"},
		        .Trigger    = "Continuous",
		        .Duration   = 0.33f,
		    }
        },
        {
			.UICategory    = LInputCategory_t::kKickSwipe,
			.AdvFeature    = false,
			.UIName        = "Start H. Kick (Left)",
			.UIDescription = "Perform a heavy kick.",
			.Event = {
		        .Event      = "HeavyKickLeft",
		        .Keys       = {"LALT", "Q"},
		        .Trigger    = "Continuous",
		        .Duration   = 0.33f,
		    }
        },
        {
			.UICategory    = LInputCategory_t::kKickSwipe,
			.AdvFeature    = false,
			.UIName        = "Start H. Low Kick (Right)",
			.UIDescription = "Perform an alternative variant of the heavy kick.",
			.Event = {
		        .Event      = "HeavyKickRight_Low",
		        .Keys       = {"LALT", "E"},
		        .Trigger    = "Release",
		        .Duration   = 0.1f,
		    }
        },
        {
			.UICategory    = LInputCategory_t::kKickSwipe,
			.AdvFeature    = false,
			.UIName        = "Start H. Low Kick (Left)",
			.UIDescription = "Perform an alternative variant of the heavy kick.",
			.Event = {
		        .Event      = "HeavyKickLeft_Low",
		        .Keys       = {"LALT", "Q"},
		        .Trigger    = "Release",
		        .Duration   = 0.1f,
		    }
        },

        //========================================================
	    //============ C R A W L I N G
	    //========================================================

        {
			.UICategory    = LInputCategory_t::kMovement,
			.AdvFeature    = false,
			.UIName        = "Toggle Crawl (Player)",
			.UIDescription = "Swap between the regular sneak animations or an alternative crawl ones.",
			.Event = {
		        .Event      = "TogglePlayerCrawl",
		        .Keys       = {"NUMPAD1"},
		    }
        },
        {
			.UICategory    = LInputCategory_t::kMovement,
			.AdvFeature    = false,
			.UIName        = "Toggle Crawl (Followers)",
			.UIDescription = "Swap between the regular sneak animations or an alternative crawl ones.",
			.Event = {
		        .Event      = "ToggleFollowerCrawl",
		        .Keys       = {"NUMPAD3"},
		    }
        },
        {
			.UICategory    = LInputCategory_t::kKickSwipe,
			.AdvFeature    = false,
			.UIName        = "Start Swipe L. (Right)",
			.UIDescription = "Perform a light swipe attack (only works when sneaking / crawling).",
			.Event = {
		        .Event      = "LightSwipeRight",
		        .Keys       = {"LALT", "E"},
		        .Trigger    = "Release",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kKickSwipe,
			.AdvFeature    = false,
			.UIName        = "Start Swipe L. (Left)",
			.UIDescription = "Perform a light swipe attack (only works when sneaking / crawling).",
			.Event = {
		        .Event      = "LightSwipeLeft",
		        .Keys       = {"LALT", "Q"},
		        .Trigger    = "Release",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kKickSwipe,
			.AdvFeature    = false,
			.UIName        = "Start Swipe H. (Right)",
			.UIDescription = "Perform a heavy swipe attack (only works when sneaking / crawling).",
			.Event = {
		        .Event      = "HeavySwipeRight",
		        .Keys       = {"LALT", "E"},
		        .Trigger    = "Continuous",
		        .Duration   = 0.33f,
		    }
        },
        {
			.UICategory    = LInputCategory_t::kKickSwipe,
			.AdvFeature    = false,
			.UIName        = "Start Swipe H. (Left)",
			.UIDescription = "Perform a light swipe attack (only works when sneaking / crawling).",
			.Event = {
		        .Event      = "HeavySwipeLeft",
		        .Keys       = {"LALT", "Q"},
		        .Trigger    = "Continuous",
		        .Duration   = 0.33f,
		    }
        },

        //========================================================
	    //============ C L E A V A G E
	    //========================================================

        {
			.UICategory    = LInputCategory_t::kCleavage,
			.AdvFeature    = false,
			.UIName        = "Start Cleavage Mode",
			.UIDescription = "Enter cleavage mode (Requires an NPC to be placed there first).",
			.Event = {
		        .Event      = "CleavageEnter",
		        .Keys       = {"LSHIFT", "H"},
		    }
        },
        {
			.UICategory    = LInputCategory_t::kCleavage,
			.AdvFeature    = false,
			.UIName        = "Exit Cleavage Mode",
			.UIDescription = "Exit cleavage mode and return to breast hold mode.",
			.Event = {
		        .Event      = "CleavageExit",
		        .Keys       = {"RMB"},
		        .BlockInput = "Never"
		    }
        },
		{
			.UICategory    = LInputCategory_t::kCleavage,
			.AdvFeature    = false,
			.UIName        = "Cleavage L. Attack",
			.UIDescription = "Perform a light attack.",
			.Event = {
		        .Event      = "CleavageLightAttack",
		        .Keys       = {"LMB"},
		        .Exclusive  = true,
		        .Trigger    = "Release",
		        .BlockInput = "Never"
		    }
		},
        {
			.UICategory    = LInputCategory_t::kCleavage,
			.AdvFeature    = false,
			.UIName        = "Cleavage H. Attack",
			.UIDescription = "Perform a heavy attack.",
			.Event = {
		        .Event      = "CleavageHeavyAttack",
		        .Keys       = {"LMB"},
		        .Exclusive  = true,
		        .Trigger    = "Continuous",
		        .Duration   = 0.33f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kCleavage,
			.AdvFeature    = false,
			.UIName        = "Cleavage Suffocate",
			.UIDescription = "Perform a cleavage suffocation attack / action.",
			.Event = {
		        .Event      = "CleavageSuffocate",
		        .Keys       = {"W"},
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kCleavage,
			.AdvFeature    = false,
			.UIName        = "Cleavage Absorb",
			.UIDescription = "Absorb (and kill) the held NPC.",
			.Event = {
		        .Event      = "CleavageAbsorb",
		        .Keys       = {"S"},
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kCleavage,
			.AdvFeature    = false,
			.UIName        = "Cleavage Vore",
			.UIDescription = "Vore/Eat the held NPC.",
			.Event = {
		        .Event      = "CleavageVore",
		        .Keys       = {"V"},
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kCleavage,
			.AdvFeature    = false,
			.UIName        = "Cleavage Strangle",
			.UIDescription = "Perform a cleavage strangle attack / action.",
			.Event = {
		        .Event      = "CleavageDOT",
		        .Keys       = {"E"},
		    }
        },

        //========================================================
		//============ H U G S
		//========================================================

        {
			.UICategory    = LInputCategory_t::kHugs,
			.AdvFeature    = false,
			.UIName        = "Start Hug",
			.UIDescription = "Start a hug action and enter hug mode.",
			.Event = {
		        .Event      = "HugAttempt",
		        .Keys       = {"LSHIFT", "H"},
		        .Trigger    = "Release",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kHugs,
			.AdvFeature    = false,
			.UIName        = "Start Hug (Player)",
			.UIDescription = "Start a hug action and enter hug mode with the player as the target.",
			.Event = {
		        .Event      = "HugPlayer",
		        .Keys       = {"LSHIFT", "H"},
		        .Trigger    = "Continuous",
		        .Duration   = 1.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kHugs,
			.AdvFeature    = false,
			.UIName        = "Hug Shrink",
			.UIDescription = "Perform a hug shrink attack / action.",
			.Event = {
		        .Event      = "HugShrink",
		        .Keys       = {"LMB"},
		        .Exclusive  = true,
		        .Trigger    = "Release"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kHugs,
			.AdvFeature    = false,
			.UIName        = "Hug Heal",
			.UIDescription = "Perform a hug heal action.",
			.Event = {
		        .Event      = "HugHeal",
		        .Keys       = {"LMB"},
		        .Exclusive  = true,
		        .Trigger    = "Continuous",
		        .Duration   = 0.33f,
		    }
        },
        {
			.UICategory    = LInputCategory_t::kHugs,
			.AdvFeature    = false,
			.UIName        = "Hug Crush",
			.UIDescription = "Perfom a hug crush action (will kill the held target).",
			.Event = {
		        .Event      = "HugCrush",
		        .Keys       = {"S"},
		    }
        },
        {
			.UICategory    = LInputCategory_t::kHugs,
			.AdvFeature    = false,
			.UIName        = "Hug Exit / Release",
			.UIDescription = "Release the held NPC / target and exit hug mode.",
			.Event = {
		        .Event      = "HugRelease",
		        .Keys       = {"RMB"},
		        .Exclusive  = true,
		        .BlockInput = "Never"
			}
        },

        //========================================================
	    //============ B U T T / B R E A S T   C R U S H
	    //========================================================

        {
			.UICategory    = LInputCategory_t::kCrush,
			.AdvFeature    = false,
			.UIName        = "Start Quick Crush",
			.UIDescription = "Start an untargeted butt / breast crush action.",
			.Event = {
		        .Event      = "QuickButtCrushStart",
		        .Keys       = {"LSHIFT", "B"},
		        .Trigger    = "Continuous",
		        .Duration   = 1.0f,
		    }
        },
        {
			.UICategory    = LInputCategory_t::kCrush,
			.AdvFeature    = false,
			.UIName        = "Start Targeted Crush",
			.UIDescription = "Start a targeted butt / breast crush action and enter crush mode.",
			.Event = {
		        .Event      = "ButtCrushStart",
		        .Keys       = {"LSHIFT", "B"},
		        .Trigger    = "Release",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kCrush,
			.AdvFeature    = false,
			.UIName        = "Start Targeted Crush (Player)",
			.UIDescription = "Start a targeted butt / breast crush action with the player as the target and enter crush mode.",
			.Event = {
		        .Event      = "ButtCrushStart_Player",
		        .Keys       = {"LSHIFT", "B"},
		        .Trigger    = "Continuous",
		        .Duration   = 1.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kCrush,
			.AdvFeature    = false,
			.UIName        = "Crush Grow",
			.UIDescription = "Perform a grow action while in crush mode.",
			.Event = {
		        .Event      = "ButtCrushGrow",
		        .Keys       = {"W"},
		        .Trigger    = "Continuous",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kCrush,
			.AdvFeature    = false,
			.UIName        = "Crush Attack",
			.UIDescription = "Peform a crush attack, this will also exit crush mode.",
			.Event = {
		        .Event      = "ButtCrushAttack",
		        .Keys       = {"LMB"},
		        .Duration   = 0.0f,
		    }
        },

        //========================================================
        //======== P R O N E   B E H A V I O R
        //========================================================

        {
			.UICategory    = LInputCategory_t::kMovement,
			.AdvFeature    = false,
			.UIName        = "Enter Prone",
			.UIDescription = "Enter Prone mode (only possible if already sneaking / crawling.",
			.Event = {
		        .Event      = "SBO_ToggleProne",
		        .Keys       = {"X"},
		        .Trigger    = "Continuous",
		        .Duration   = 0.66f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kMovement,
			.AdvFeature    = false,
			.UIName        = "Prone Dive (Standing)",
			.UIDescription = "Enter prone mode by diving to the ground while not sneaking / crawling.",
			.Event = {
		        .Event      = "SBO_ToggleDive_Standing",
		        .Keys       = {"W", "S"},
		        .Trigger    = "Continuous",
		        .Duration   = 0.50f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kMovement,
			.AdvFeature    = false,
			.UIName        = "Prone Dive (Sneaking)",
			.UIDescription = "Enter prone mode by diving to the ground while sneaking / crawling.",
			.Event = {
		        .Event      = "SBO_ToggleDive_Sneak",
		        .Keys       = {"W", "S"},
		        .Trigger    = "Continuous",
		        .Duration   = 0.50f,
		        .BlockInput = "Never"
		    }
        },

        //========================================================
	    //========================= G R A B
	    //======================================================== 

        {
			.UICategory    = LInputCategory_t::kGrab,
			.AdvFeature    = false,
			.UIName        = "Start Grab",
			.UIDescription = "Start a grab action and enter grab mode.",
			.Event = {
		        .Event      = "GrabOther",
		        .Keys       = {"F"},
		        .Duration   = 0.25f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kGrab,
			.AdvFeature    = false,
			.UIName        = "Start Grab (Player)",
			.UIDescription = "Start a grab action and enter grab mode with the player as the target.",
			.Event = {
		        .Event      = "GrabPlayer",
		        .Keys       = {"F"},
		        .Trigger    = "Continuous",
		        .Duration   = 1.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kGrab,
			.AdvFeature    = false,
			.UIName        = "Grab Attack",
			.UIDescription = "Perform a grab crush attack on a held character.",
			.Event = {
				.Event      = "GrabAttack",
		        .Keys       = {"E"},
		        .Trigger    = "Continuous",
		        .Duration   = 0.50f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kGrab,
			.AdvFeature    = false,
			.UIName        = "Grab Vore",
			.UIDescription = "Vore the held character.",
			.Event = {
		        .Event      = "GrabVore",
		        .Keys       = {"V"},
		        .Trigger    = "Continuous",
		        .Duration   = 0.50f,
		    }
        },
        {
			.UICategory    = LInputCategory_t::kGrab,
			.AdvFeature    = false,
			.UIName        = "Grab Throw",
			.UIDescription = "Throw the held character and exit grab mode.",
			.Event = {
		        .Event      = "GrabThrow",
		        .Keys       = {"X"},
		        .Trigger    = "Continuous",
		        .BlockInput = "Never"
		    }
        },
        {
		    .UICategory    = LInputCategory_t::kGrab,
			.AdvFeature    = false,
			.UIName        = "Grab Exit / Release",
			.UIDescription = "Drop the held character and exit grab mode.",
			.Event = {
		        .Event      = "GrabRelease",
		        .Keys       = {"RMB"},
		    }
        },

		//========================================================
		//========================= G R A B  B R E A S T S
		//======================================================== 


        {
			.UICategory    = LInputCategory_t::kBreats,
			.AdvFeature    = false,
			.UIName        = "Breasts Place",
			.UIDescription = "Place the held character between the holder's breasts.\n(This leaves grab mode, so you can grab another NPC while having another between breasts)",
			.Event = {
		        .Event      = "BreastsPut",
		        .Keys       = {"LSHIFT", "B"},
		        .Duration   = 0.50f,
		    }
        },
        {
			.UICategory    = LInputCategory_t::kBreats,
			.AdvFeature    = false,
			.UIName        = "Breasts Remove",
			.UIDescription = "Remove the placed character from the holder's breasts.",
			.Event = {
		        .Event      = "BreastsRemove",
		        .Keys       = {"LSHIFT", "B"},
		        .Duration   = 0.50f,
		    }
        },


        //========================================================
	    //========================= G R A B  P L A Y
	    //======================================================== 

        {
			.UICategory    = LInputCategory_t::kGrabPlay,
			.AdvFeature    = false,
			.UIName        = "Start GrabPlay",
			.UIDescription = "Enter grab play mode with the held character.",
			.Event = {
		        .Event      = "GrabPlay_Start",
		        .Keys       = {"LSHIFT", "H"},
		        .Trigger    = "Once",
		        .Duration   = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kGrabPlay,
			.AdvFeature    = false,
			.UIName        = "Exit GrabPlay",
			.UIDescription = "Exit grab play mode.",
			.Event = {
		        .Event      = "GrabPlay_Exit",
		        .Keys       = {"LSHIFT","H"},
		        .Trigger    = "Once",
		        .Duration   = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kGrabPlay,
			.AdvFeature    = false,
			.UIName        = "GrabPlay Crush",
			.UIDescription = "Perform a crush attack.",
			.Event = {
		        .Event      = "GrabPlay_CrushHeavy",
		        .Keys       = {"LMB"},
		        .Trigger    = "Continuous",
		        .Duration   = 0.5f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kGrabPlay,
			.AdvFeature    = false,
			.UIName        = "GrabPlay Vore",
			.UIDescription = "Vore the held character.",
			.Event = {
		        .Event      = "GrabPlay_Vore",
		        .Keys       = {"V"},
		        .Trigger    = "Once",
		        .Duration   = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kGrabPlay,
			.AdvFeature    = false,
			.UIName        = "GrabPlay Kiss",
			.UIDescription = "Kiss the held character, healing them.",
			.Event = {
		        .Event      = "GrabPlay_Kiss",
		        .Keys       = {"E"},
		        .Trigger    = "Once",
		        .Duration   = 0.0f,
		        .BlockInput = "Never"
		    }
        },
		{
			.UICategory    = LInputCategory_t::kGrabPlay,
			.AdvFeature    = false,
			.UIName        = "GrabPlay Vore During Kiss",
			.UIDescription = "Vore the held character whilst performing the kiss action.",
			.Event = {
				.Event      = "GrabPlay_KissVore",
				.Keys       = {"V"},
				.Trigger    = "Once",
				.Duration   = 0.0f,
				.BlockInput = "Never"
			}
		},
        {
			.UICategory    = LInputCategory_t::kGrabPlay,
			.AdvFeature    = false,
			.UIName        = "GrabPlay Poke",
			.UIDescription = "Poke the held character.",
			.Event = {
		        .Event      = "GrabPlay_Poke",
		        .Keys       = {"LMB"},
		        .Trigger    = "Once",
		        .Duration   = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kGrabPlay,
			.AdvFeature    = false,
			.UIName        = "GrabPlay Flick",
			.UIDescription = "Flick the held charcater away, this exits grab mode.",
			.Event = {
		        .Event      = "GrabPlay_Flick",
		        .Keys       = {"RMB"},
		        .Trigger    = "Once",
		        .Duration   = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kGrabPlay,
			.AdvFeature    = false,
			.UIName        = "GrabPlay Sandwich",
			.UIDescription = "Perform a sandwich action.",
			.Event = {
		        .Event      = "GrabPlay_Sandwich",
		        .Keys       = {"S"},
		        .Trigger    = "Once",
		        .Duration   = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kGrabPlay,
			.AdvFeature    = false,
			.UIName        = "GrabPlay Grind (Enter)",
			.UIDescription = "Enter the grabplay grind mode, this slowly damages the held character.",
			.Event = {
		        .Event      = "GrabPlay_GrindStart",
		        .Keys       = {"W"},
		        .Trigger    = "Once",
		        .Duration   = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kGrabPlay,
			.AdvFeature    = false,
			.UIName        = "GrabPlay Grind (Exit)",
			.UIDescription = "Exit grab play grind mode.",
			.Event = {
		        .Event      = "GrabPlay_GrindStop",
		        .Keys       = {"W"},
		        .Trigger    = "Once",
		        .Duration   = 0.0f,
		        .BlockInput = "Never"
		    }
        },

        //========================================================
	    //========================= C A M E R A
	    //========================================================

        {
			.UICategory    = LInputCategory_t::kCamera,
			.AdvFeature    = false,
			.UIName        = "Camera Reset (Horizontal)",
			.UIDescription = "Reset the temporary horizontal camera offsets.",
			.Event = {
		        .Event      = "HorizontalCameraReset",
		        .Keys       = {"RIGHT", "LEFT"},
		        .Trigger    = "Continuous",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kCamera,
			.AdvFeature    = false,
			.UIName        = "Camera Reset (Vertical)",
			.UIDescription = "Reset the temporary vertical camera offsets.",
			.Event = {
		        .Event      = "VerticalCameraReset",
		        .Keys       = {"UP", "DOWN"},
		        .Trigger    = "Continuous",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kCamera,
			.AdvFeature    = false,
			.UIName        = "Shift Camera (Right)",
			.UIDescription = "Temporarily shift the camera to the right.",
			.Event = {
		        .Event      = "CameraRight",
		        .Keys       = {"LALT", "RIGHT"},
		        .Trigger    = "Continuous",
		    }
        },
		{
			.UICategory    = LInputCategory_t::kCamera,
			.AdvFeature    = false,
			.UIName        = "Shift Camera (Left)",
			.UIDescription = "Temporarily shift the camera to the left.",
			.Event = {
				.Event      = "CameraLeft",
				.Keys       = {"LALT", "LEFT"},
				.Trigger    = "Continuous",
			}
		},
        {
			.UICategory    = LInputCategory_t::kCamera,
			.AdvFeature    = false,
			.UIName        = "Shift Camera (Up)",
			.UIDescription = "Temporarily shift the camera upwards.",
			.Event = {
		        .Event      = "CameraUp",
		        .Keys       = {"LALT", "UP"},
		        .Trigger    = "Continuous",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kCamera,
			.AdvFeature    = false,
			.UIName        = "Shift Camera (Down)",
			.UIDescription = "Temporarily shift the camera downwards.",
			.Event = {
		        .Event      = "CameraDown",
		        .Keys       = {"LALT", "DOWN"},
		        .Trigger    = "Continuous",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kCamera,
			.AdvFeature    = false,
			.UIName        = "Cycle Camera Mode",
			.UIDescription = "Switch between alternative camera modes.",
			.Event = {
		        .Event      = "SwitchCameraMode",
		        .Keys       = {"F2"}
		    }
        },

        //========================================================
		//========================= A N I M  S P E E D
		//========================================================

        {
			.UICategory    = LInputCategory_t::kMisc,
			.AdvFeature    = false,
			.UIName        = "Attack Speedup",
			.UIDescription = "Increase the attack speed of some animations.",
			.Event = {
		        .Event      = "AnimSpeedUp",
		        .Keys       = {"LMB"},
		        .Trigger    = "Continuous",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kMisc,
			.AdvFeature    = false,
			.UIName        = "Attack Slowdown",
			.UIDescription = "Decrease the attack speed of some animations.",
			.Event = {
		        .Event      = "AnimSpeedDown",
		        .Keys       = {"RMB"},
		        .Trigger    = "Continuous",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kMisc,
			.AdvFeature    = false,
			.UIName        = "Attack Max Speed",
			.UIDescription = "Sets the attack speed to max for supported actions.",
			.Event = {
		        .Event      = "AnimMaxSpeed",
		        .Keys       = {"LMB", "RMB"},
		        .Trigger    = "Continuous",
		    }
        },

        //========================================================
	    //========================= A B I L I T I E S
	    //========================================================

        {
			.UICategory    = LInputCategory_t::kAbility,
			.AdvFeature    = false,
			.UIName        = "Shrink Outburst",
			.UIDescription = "Do a shrink outburst attack, shrinking everyone near you.",
			.Event = {
		        .Event      = "ShrinkOutburst",
		        .Keys       = {"LSHIFT", "F"},
		    }
        },
        {
			.UICategory    = LInputCategory_t::kAbility,
			.AdvFeature    = false,
			.UIName        = "Size Reseve (Use)",
			.UIDescription = "Apply the stored size reserve (the longer this is held down the more size is converted).",
			.Event = {
		        .Event      = "SizeReserve",
		        .Keys       = {"E"},
		        .Trigger    = "Continuous",
		        .Duration   = 1.33f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kAbility,
			.AdvFeature    = false,
			.UIName        = "Size Reserve (Display)",
			.UIDescription = "Show a notification containing the amount of stored size reserve.",
			.Event = {
		        .Event      = "DisplaySizeReserve",
		        .Keys       = {"F"},
		        .Trigger    = "Continuous",
		        .Duration   = 1.33f,
		        .BlockInput = "Never"
		    }
        },
        {
			.UICategory    = LInputCategory_t::kAbility,
			.AdvFeature    = false,
			.UIName        = "Grow (Rapid)",
			.UIDescription = "A one-off animated growth spurt.",
			.Event = {
		        .Event      = "RapidGrowth",
		        .Keys       = {"LSHIFT", "1"},
		    }
        },
        {
			.UICategory    = LInputCategory_t::kAbility,
			.AdvFeature    = false,
			.UIName        = "Shrink (Rapid)",
			.UIDescription = "A one-off animated shrink spurt.",
			.Event = {
		        .Event      = "RapidShrink",
		        .Keys       = {"LSHIFT", "2"},
		    }
        },
        {
			.UICategory    = LInputCategory_t::kAbility,
			.AdvFeature    = false,
			.UIName        = "Grow (Manual)",
			.UIDescription = "Slow continuous growth.",
			.Event = {
		        .Event      = "ManualGrow",
		        .Keys       = {"UP", "LEFT"},
		        .Trigger    = "Continuous",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kAbility,
			.AdvFeature    = false,
			.UIName        = "Shrink (Manual)",
			.UIDescription = "Slow continuous shrinking.",
			.Event = {
		        .Event      = "ManualShrink",
		        .Keys       = {"DOWN", "LEFT"},
		        .Trigger    = "Continuous",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kAbility,
			.AdvFeature    = false,
			.UIName        = "Grow Followers (Manual)",
			.UIDescription = "Slow continuous growth.",
			.Event = {
		        .Event      = "ManualGrowOther",
		        .Keys       = {"LSHIFT", "UP", "LEFT"},
		        .Trigger    = "Continuous",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kAbility,
			.AdvFeature    = false,
			.UIName        = "Shrink Folowers (Manual)",
			.UIDescription = "Slow continuous shrinking.",
			.Event = {
		        .Event      = "ManualShrinkOther",
		        .Keys       = {"LSHIFT", "DOWN", "LEFT"},
		        .Trigger    = "Continuous",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kAbility,
			.AdvFeature    = false,
			.UIName        = "Grow (Incremental)",
			.UIDescription = "An incremental non animated growth spurt.",
			.Event = {
		        .Event      = "ManualGrowOverTime",
		        .Keys       = {"NUMPAD8"},
		        .Trigger    = "Once",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kAbility,
			.AdvFeature    = false,
			.UIName        = "Shrink (Incremental)",
			.UIDescription = "An incremental non animated shrink spurt.",
			.Event = {
		        .Event      = "ManualShrinkOverTime",
		        .Keys       = {"NUMPAD4"},
		        .Trigger    = "Once",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kAbility,
			.AdvFeature    = false,
			.UIName        = "Grow Folowers (Incremental)",
			.UIDescription = "An incremental non animated growth spurt.",
			.Event = {
		        .Event      = "ManualGrowOtherOverTime",
		        .Keys       = {"NUMPAD5"},
		        .Trigger    = "Once",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kAbility,
			.AdvFeature    = false,
			.UIName        = "Shrink Folowers (Incremental)",
			.UIDescription = "An incremental non animated shrink spurt.",
			.Event = {
		        .Event      = "ManualShrinkOtherOverTime",
		        .Keys       = {"NUMPAD2"},
		        .Trigger    = "Once",
		    }
        },
        {
			.UICategory    = LInputCategory_t::kAbility,
			.AdvFeature    = false,
			.UIName        = "Protect Small Ones",
			.UIDescription = "Activate the \"Protect Small Ones\" perk granting size damage immunity to others around you.",
			.Event = {
		        .Event      = "ProtectSmallOnes",
		        .Keys       = {"C"},
		        .Duration   = 1.0f,
		        .BlockInput = "Never"
		    }
        }
		
    };
}