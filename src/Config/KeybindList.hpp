#pragma once


enum class InputCategory : uint8_t {
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

struct GTSInputEvent {
    std::string Event;                      //Event Name
    std::vector<std::string> Keys;          //List Of Dinput key names Minus "DIK_ preffix"
    bool Exclusive = false;                 //Exclusive Flag
    std::string Trigger = "Once";           //Trigger Type
    float Duration = 0.0;                   //Trigger Duration
    std::string BlockInput = "Automatic";   //Block Game Input
    bool Disabled = false;                  //Completely Ignore Event
};
TOML_SERIALIZABLE(GTSInputEvent);

// Re-ordered: category first, then event (so you can do inline init cleanly)
struct CategorizedInputEvent {
    InputCategory Category;
    GTSInputEvent Event;
};

enum class TriggerType : uint8_t {
    Once,
    Continuous,
    Release,
};

enum class BlockInputTypes : uint8_t {
    Automatic,
    Always,
    Never,
};

inline bool CheckDuplicateEvent(const std::vector<CategorizedInputEvent>& items) {
    for (std::size_t i = 0; i < items.size(); ++i) {
        for (std::size_t j = i + 1; j < items.size(); ++j) {
            if (items[i].Event.Event == items[j].Event.Event)
                return true;
        }
    }
    return false;
}

namespace GTS {

    //----------------------------------------
    // EVENT LIST ----
    //----------------------------------------

    inline const std::vector<CategorizedInputEvent> DefaultEvents = {
    	
        /* // ----- EXAMPLE
        {
            InputCategory::kDefault,
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
	        InputCategory::kMenu,
            {
		        .Event = "OpenModSettings",
		        .Keys = {"F1"},
		        .Trigger = "Once",
		        .BlockInput = "Always"
		    }
	    },
        {
            InputCategory::kMenu,
            {
		        .Event = "OpenSkillTree",
		        .Keys = {"F4"},
		        .Trigger = "Once",
		        .BlockInput = "Always"
		    }
        },
        {
            InputCategory::kMenu,
            {
                .Event = "PartyReport",
                .Keys = {"LCONTROL"},
                .Duration = 1.33f,
                .BlockInput = "Never"
            }
        },
        {
            InputCategory::kMenu,
            {
                .Event = "ShowQuickStats",
                .Keys = {"F3"},
                .BlockInput = "Never"
            }
        },
        {
            InputCategory::kMenu,
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
            InputCategory::kVore,
            {
		        .Event = "Vore",
		        .Keys = {"LSHIFT", "V"},
		        .Trigger = "Once"
		    }
        },
        {
            InputCategory::kVore,
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
            InputCategory::kStomp,
            {
		        .Event = "RightStomp",
		        .Keys = {"LSHIFT", "E"},
		        .Trigger = "Release",
		    }
        },
        {
            InputCategory::kStomp,
            {
		        .Event = "LeftStomp",
		        .Keys = {"LSHIFT", "Q"},
		        .Trigger = "Release",
		    }
        },
        {
            InputCategory::kStomp,
            {
		        .Event = "RightStomp_Strong",
		        .Keys = {"LSHIFT", "E"},
		        .Trigger = "Continuous",
		        .Duration = 0.44f,
		    }
        },
        {
            InputCategory::kStomp,
            {
		        .Event = "LeftStomp_Strong",
		        .Keys = {"LSHIFT", "Q"},
		        .Trigger = "Continuous",
		        .Duration = 0.44f,
		    }
        },
		{
            InputCategory::kStomp,
            {
		        .Event = "TrampleRight",
		        .Keys = {"LSHIFT", "E"},
		        .Trigger = "Release",
		        .Duration = 0.20f,
		    }
        },
        {
            InputCategory::kStomp,
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
            InputCategory::kThighs,
            {
		        .Event = "ThighCrush",
		        .Keys = {"LSHIFT", "W", "C"},
		        .Exclusive = true
		    }
        },
        {
            InputCategory::kThighs,
            {
		        .Event = "ThighCrushKill",
		        .Keys = {"LMB"},
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kThighs,
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
            InputCategory::kThighs,
            {
		        .Event = "ThighSandwichEnter",
		        .Keys = {"LSHIFT", "C"},
		        .Exclusive = true,
		    }
        },
        {
            InputCategory::kThighs,
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
            InputCategory::kThighs,
            {
		        .Event = "ThighSandwichAttackHeavy",
		        .Keys = {"LMB"},
		        .Trigger = "Continuous",
		        .Duration = 0.33f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kThighs,
            {
		        .Event = "ThighSandwichAttack",
		        .Keys = {"LMB"},
		        .Trigger = "Release",
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kThighs,
            {
		        .Event = "ThighSandwichExit",
		        .Keys = {"W"},
		        .Exclusive = true,
		    }
        },
        {
            InputCategory::kThighs,
            {
		        .Event = "SandwichButtStart",
		        .Keys = {"LSHIFT", "H"},
		        .Exclusive = true,
		    }
        },
        {        
            InputCategory::kThighs,
            {
		        .Event = "SandwichLightAttack",
		        .Keys = {"LMB"},
		        .Exclusive = true,
                .Trigger = "Release",
		        .BlockInput = "Never"
		    }
        },
        {        
            InputCategory::kThighs,
            {
		        .Event = "SandwichHeavyAttack",
		        .Keys = {"LMB"},
		        .Exclusive = true,
                .Trigger = "Continuous",
                .Duration = 0.33f,
                .BlockInput = "Never"
		    }
        },
        {        
            InputCategory::kThighs,
            {
		        .Event = "SandwichGrowth",
		        .Keys = {"W"},
		        .Exclusive = true,
		    }
        },
        {        
            InputCategory::kThighs,
            {
		        .Event = "SandwichGrindStart",
		        .Keys = {"LSHIFT", "S"},
		        .Exclusive = true,
		    }
        },
        {        
            InputCategory::kThighs,
            {
		        .Event = "SandwichGrindStop",
		        .Keys = {"LSHIFT", "S"},
		        .Exclusive = true,
		    }
        },
        {
            InputCategory::kThighs,
            {
		        .Event = "SandwichButtStop",
		        .Keys = {"S"},
		        .Exclusive = true,
		    }
        },
        {        
            InputCategory::kThighs,
            {
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
            InputCategory::kKickSwipe,
            {
		        .Event = "LightKickRight",
		        .Keys = {"LALT", "E"},
		        .Trigger = "Release",
		    }
        },
        {
            InputCategory::kKickSwipe,
            {
		        .Event = "LightKickLeft",
		        .Keys = {"LALT", "Q"},
		        .Trigger = "Release",
		    }
        },
        {
            InputCategory::kKickSwipe,
            {
		        .Event = "HeavyKickRight",
		        .Keys = {"LALT", "E"},
		        .Trigger = "Continuous",
		        .Duration = 0.33f,
		    }
        },
        {
            InputCategory::kKickSwipe,
            {
		        .Event = "HeavyKickLeft",
		        .Keys = {"LALT", "Q"},
		        .Trigger = "Continuous",
		        .Duration = 0.33f,
		    }
        },
        {
            InputCategory::kKickSwipe,
            {
		        .Event = "HeavyKickRight_Low",
		        .Keys = {"LALT", "E"},
		        .Trigger = "Release",
		        .Duration = 0.1f,
		    }
        },
        {
            InputCategory::kKickSwipe,
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
            InputCategory::kMovement,
            {
		        .Event = "TogglePlayerCrawl",
		        .Keys = {"NUMPAD1"},
		    }
        },
        {
            InputCategory::kMovement,
            {
		        .Event = "ToggleFollowerCrawl",
		        .Keys = {"NUMPAD3"},
		    }
        },
        {
            InputCategory::kKickSwipe,
            {
		        .Event = "LightSwipeRight",
		        .Keys = {"LALT", "E"},
		        .Trigger = "Release",
		    }
        },
        {
            InputCategory::kKickSwipe,
            {
		        .Event = "LightSwipeLeft",
		        .Keys = {"LALT", "Q"},
		        .Trigger = "Release",
		    }
        },
        {
            InputCategory::kKickSwipe,
            {
		        .Event = "HeavySwipeRight",
		        .Keys = {"LALT", "E"},
		        .Trigger = "Continuous",
		        .Duration = 0.33f,
		    }
        },
        {
            InputCategory::kKickSwipe,
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
            InputCategory::kCleavage,
            {
		        .Event = "CleavageEnter",
		        .Keys = {"LSHIFT", "H"},
		    }
        },
        {
            InputCategory::kCleavage,
            {
		        .Event = "CleavageExit",
		        .Keys = {"RMB"},
		        .BlockInput = "Never"
		    }
        },
		{
            InputCategory::kCleavage,
            {
		        .Event = "CleavageLightAttack",
		        .Keys = {"LMB"},
		        .Exclusive = true,
		        .Trigger = "Release",
		        .BlockInput = "Never"
		    }
		},
        {
            InputCategory::kCleavage,
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
            InputCategory::kCleavage,
            {
		        .Event = "CleavageSuffocate",
		        .Keys = {"W"},
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kCleavage,
            {
		        .Event = "CleavageAbsorb",
		        .Keys = {"S"},
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kCleavage,
            {
		        .Event = "CleavageVore",
		        .Keys = {"V"},
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kCleavage,
            {
		        .Event = "CleavageDOT",
		        .Keys = {"E"},
		    }
        },

        //========================================================
		//============ H U G S
		//========================================================

        {
            InputCategory::kHugs,
            {
		        .Event = "HugAttempt",
		        .Keys = {"LSHIFT", "H"},
		        .Trigger = "Release",
		    }
        },
        {
            InputCategory::kHugs,
            {
		        .Event = "HugPlayer",
		        .Keys = {"LSHIFT", "H"},
		        .Trigger = "Continuous",
		        .Duration = 1.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kHugs,
            {
		        .Event = "HugShrink",
		        .Keys = {"LMB"},
		        .Exclusive = true,
		        .Trigger = "Release"
		    }
        },
        {
            InputCategory::kHugs,
            {
		        .Event = "HugHeal",
		        .Keys = {"LMB"},
		        .Exclusive = true,
		        .Trigger = "Continuous",
		        .Duration = 0.33f,
		    }
        },
        {
            InputCategory::kHugs,
            {
		        .Event = "HugCrush",
		        .Keys = {"S"},
		    }
        },
        {
            InputCategory::kHugs,
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
            InputCategory::kCrush,
            {
		        .Event = "QuickButtCrushStart",
		        .Keys = {"LSHIFT", "B"},
		        .Trigger = "Continuous",
		        .Duration = 1.0f,
		    }
        },
        {
            InputCategory::kCrush,
            {
		        .Event = "ButtCrushStart",
		        .Keys = {"LSHIFT", "B"},
		        .Trigger = "Release",
		    }
        },
        {
            InputCategory::kCrush,
            {
		        .Event = "ButtCrushStart_Player",
		        .Keys = {"LSHIFT", "B"},
		        .Trigger = "Continuous",
		        .Duration = 1.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kCrush,
            {
		        .Event = "ButtCrushGrow",
		        .Keys = {"W"},
		        .Trigger = "Continuous",
		    }
        },
        {
            InputCategory::kCrush,
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
            InputCategory::kMovement,
			{
		        .Event = "SBO_ToggleProne",
		        .Keys = {"X"},
		        .Trigger = "Continuous",
		        .Duration = 0.66f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kMovement,
            {
		        .Event = "SBO_ToggleDive_Standing",
		        .Keys = {"W", "S"},
		        .Trigger = "Continuous",
		        .Duration = 0.50f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kMovement,
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
            InputCategory::kGrab,
            {
		        .Event = "GrabOther",
		        .Keys = {"F"},
		        .Duration = 0.25f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kGrab,
            {
		        .Event = "GrabPlayer",
		        .Keys = {"F"},
		        .Trigger = "Continuous",
		        .Duration = 1.0f,
		        .BlockInput = "Never"
		    }
        },
        {
		    InputCategory::kGrab,
		    {
				.Event = "GrabAttack",
		        .Keys = {"E"},
		        .Trigger = "Continuous",
		        .Duration = 0.50f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kGrab,
            {
		        .Event = "GrabVore",
		        .Keys = {"V"},
		        .Trigger = "Continuous",
		        .Duration = 0.50f,
		    }
        },
        {
            InputCategory::kGrab,
            {
		        .Event = "GrabThrow",
		        .Keys = {"X"},
		        .Trigger = "Continuous",
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kGrab,
            {
		        .Event = "GrabRelease",
		        .Keys = {"RMB"},
		    }
        },
        {
            InputCategory::kGrab,
            {
		        .Event = "BreastsPut",
		        .Keys = {"LSHIFT", "B"},
		        .Duration = 0.50f,
		    }
        },
        {
            InputCategory::kGrab,
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
            InputCategory::kGrabPlay,
            {
		        .Event = "GrabPlay_Start",
		        .Keys = {"LSHIFT", "H"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kGrabPlay,
            {
		        .Event = "GrabPlay_Exit",
		        .Keys = {"LSHIFT","H"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kGrabPlay,
            {
		        .Event = "GrabPlay_CrushHeavy",
		        .Keys = {"LMB"},
		        .Trigger = "Continuous",
		        .Duration = 0.5f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kGrabPlay,
            {
		        .Event = "GrabPlay_Vore",
		        .Keys = {"V"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kGrabPlay,
            {
		        .Event = "GrabPlay_KissVore",
		        .Keys = {"V"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kGrabPlay,
            {
		        .Event = "GrabPlay_Kiss",
		        .Keys = {"E"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kGrabPlay,
            {
		        .Event = "GrabPlay_Poke",
		        .Keys = {"LMB"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kGrabPlay,
            {
		        .Event = "GrabPlay_Flick",
		        .Keys = {"RMB"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kGrabPlay,
            {
		        .Event = "GrabPlay_Sandwich",
		        .Keys = {"S"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kGrabPlay,
            {
		        .Event = "GrabPlay_GrindStart",
		        .Keys = {"W"},
		        .Trigger = "Once",
		        .Duration = 0.0f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kGrabPlay,
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
            InputCategory::kCamera,
            {
		        .Event = "HorizontalCameraReset",
		        .Keys = {"RIGHT", "LEFT"},
		        .Trigger = "Continuous",
		    }
        },
        {
            InputCategory::kCamera,
            {
		        .Event = "VerticalCameraReset",
		        .Keys = {"UP", "DOWN"},
		        .Trigger = "Continuous",
		    }
        },
        {
            InputCategory::kCamera,
            {
		        .Event = "CameraLeft",
		        .Keys = {"LALT", "LEFT"},
		        .Trigger = "Continuous",
		    }
        },
        {
            InputCategory::kCamera,
            {
		        .Event = "CameraRight",
		        .Keys = {"LALT", "RIGHT"},
		        .Trigger = "Continuous",
		    }
        },
        {
            InputCategory::kCamera,
            {
		        .Event = "CameraUp",
		        .Keys = {"LALT", "UP"},
		        .Trigger = "Continuous",
		    }
        },
        {
            InputCategory::kCamera,
            {
		        .Event = "CameraDown",
		        .Keys = {"LALT", "DOWN"},
		        .Trigger = "Continuous",
		    }
        },
        {
            InputCategory::kCamera,
            {
		        .Event = "SwitchCameraMode",
		        .Keys = {"F2"}
		    }
        },

        //========================================================
		//========================= A N I M  S P E E D
		//========================================================

        {
            InputCategory::kMisc,
            {
		        .Event = "AnimSpeedUp",
		        .Keys = {"LMB"},
		        .Trigger = "Continuous",
		    }
        },
        {
            InputCategory::kMisc,
            {
		        .Event = "AnimSpeedDown",
		        .Keys = {"RMB"},
		        .Trigger = "Continuous",
		    }
        },
        {
            InputCategory::kMisc,
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
            InputCategory::kAbility,
            {
		        .Event = "ShrinkOutburst",
		        .Keys = {"LSHIFT", "F"},
		    }
        },
        {
            InputCategory::kAbility,
            {
		        .Event = "SizeReserve",
		        .Keys = {"E"},
		        .Trigger = "Continuous",
		        .Duration = 1.33f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kAbility,
			{
		        .Event = "DisplaySizeReserve",
		        .Keys = {"F"},
		        .Trigger = "Continuous",
		        .Duration = 1.33f,
		        .BlockInput = "Never"
		    }
        },
        {
            InputCategory::kAbility,
            {
		        .Event = "RapidGrowth",
		        .Keys = {"LSHIFT", "1"},
		    }
        },
        {
            InputCategory::kAbility,
            {
		        .Event = "RapidShrink",
		        .Keys = {"LSHIFT", "2"},
		    }
        },
        {
            InputCategory::kAbility,
            {
		        .Event = "ManualGrow",
		        .Keys = {"UP", "LEFT"},
		        .Trigger = "Continuous",
		    }
        },
        {
            InputCategory::kAbility,
            {
		        .Event = "ManualShrink",
		        .Keys = {"DOWN", "LEFT"},
		        .Trigger = "Continuous",
		    }
        },
        {
            InputCategory::kAbility,
            {
		        .Event = "ManualGrowOther",
		        .Keys = {"LSHIFT", "UP", "LEFT"},
		        .Trigger = "Continuous",
		    }
        },
        {
            InputCategory::kAbility,
            {
		        .Event = "ManualShrinkOther",
		        .Keys = {"LSHIFT", "DOWN", "LEFT"},
		        .Trigger = "Continuous",
		    }
        },
        {
            InputCategory::kAbility,
            {
		        .Event = "ManualGrowOverTime",
		        .Keys = {"NUMPAD8"},
		        .Trigger = "Once",
		    }
        },
        {
            InputCategory::kAbility,
            {
		        .Event = "ManualShrinkOverTime",
		        .Keys = {"NUMPAD4"},
		        .Trigger = "Once",
		    }
        },
        {
            InputCategory::kAbility,
            {
		        .Event = "ManualGrowOtherOverTime",
		        .Keys = {"NUMPAD5"},
		        .Trigger = "Once",
		    }
        },
        {
            InputCategory::kAbility,
            {
		        .Event = "ManualShrinkOtherOverTime",
		        .Keys = {"NUMPAD2"},
		        .Trigger = "Once",
		    }
        },
        {
            InputCategory::kAbility,
            {
		        .Event = "ProtectSmallOnes",
		        .Keys = {"C"},
		        .Duration = 1.0f,
		        .BlockInput = "Never"
		    }
        }

        //========================================================
	    //========================= M I S C
	    //========================================================
    };

    inline auto GetAllInputEvents() -> std::vector<GTSInputEvent> {
        std::vector<GTSInputEvent> result;
        result.reserve(DefaultEvents.size());
        for (const auto& item : DefaultEvents) {
            result.push_back(item.Event);
        }
        return result;
    }

    inline auto GetAllInputEventsView() {
        return DefaultEvents | std::views::transform([](const CategorizedInputEvent& item) -> const GTSInputEvent& {
            return item.Event;
        });
    }

    inline auto GetInputEventsByCategory(InputCategory category) {
        return DefaultEvents | std::views::filter([category](const CategorizedInputEvent& item) {
			return item.Category == category;
            }) | std::views::transform([](const CategorizedInputEvent& item) -> const GTSInputEvent& {
            return item.Event;
        });
    }

    
    //Add New Events Here

        const std::vector<GTSInputEvent> DefaultVec = {


            //Example

            //{
            //    .Event = "Event",
            //    .Keys = {"A", "B"},
            //    .Exclusive = false,
            //    .Duration = 1.0f,
            //    .Trigger = "Once",
            //    .BlockInput = "Automatic",
            //    .Disabled = false,
            //},


            //=======================================================
            //================ U I
            //=======================================================

            {
                .Event = "OpenModSettings",
                .Keys = {"F1"},
                .Trigger = "Once",
                .BlockInput = "Always"
            },
            {
                .Event = "OpenSkillTree",
                .Keys = {"F4"},
                .Trigger = "Once",
                .BlockInput = "Always"
            },

            //========================================================
            //================ V O R E
            //========================================================

            {
                .Event = "Vore",
                .Keys = {"LSHIFT", "V"},
                .Trigger = "Once"
            },
            {
                .Event = "PlayerVore",
                .Keys = {"LSHIFT", "V"},
                .Trigger = "Continuous",
                .Duration = 1.0,
                .BlockInput = "Never"
            },

            //========================================================
            //================ S T O M P S
            //========================================================

            {
                .Event = "RightStomp",
                .Keys = {"LSHIFT", "E"},
                .Trigger = "Release",
            },
            {
                .Event = "LeftStomp",
                .Keys = {"LSHIFT", "Q"},
                .Trigger = "Release",
            },
            {
                .Event = "RightStomp_Strong",
                .Keys = {"LSHIFT", "E"},
                .Trigger = "Continuous",
                .Duration = 0.44f,
            },
            {
                .Event = "LeftStomp_Strong",
                .Keys = {"LSHIFT", "Q"},
                .Trigger = "Continuous",
                .Duration = 0.44f,
            },
            {
                .Event = "TrampleRight",
                .Keys = {"LSHIFT", "E"},
                .Trigger = "Release",
                .Duration = 0.20f,
            },
            {
                .Event = "TrampleLeft",
                .Keys = {"LSHIFT", "Q"},
                .Trigger = "Release",
                .Duration = 0.20f,
            },

            //========================================================
            //================ T H I G H  C R U S H
            //========================================================

            {
                .Event = "ThighCrush",
                .Keys = {"LSHIFT", "W", "C"},
                .Exclusive = true
            },
            {
                .Event = "ThighCrushKill",
                .Keys = {"LMB"},
                .BlockInput = "Never"
            },
            {
                .Event = "ThighCrushSpare",
                .Keys = {"W"},
                .Exclusive = true,
                .BlockInput = "Never"
            },

            //========================================================
            //============ T H I G H  S A N D W I C H
            //========================================================

            {
                .Event = "ThighSandwichEnter",
                .Keys = {"LSHIFT", "C"},
                .Exclusive = true,
            },
            {
                .Event = "PlayerThighSandwichEnter",
                .Keys = {"LSHIFT", "C"},
                .Exclusive = true,
                .Trigger = "Continuous",
                .Duration = 1.0f,
                .BlockInput = "Never"
            },
            {
                .Event = "ThighSandwichAttackHeavy",
                .Keys = {"LMB"},
                .Trigger = "Continuous",
                .Duration = 0.33f,
                .BlockInput = "Never"
            },
            {
                .Event = "ThighSandwichAttack",
                .Keys = {"LMB"},
                .Trigger = "Release",
                .BlockInput = "Never"
            },
            {
                .Event = "ThighSandwichExit",
                .Keys = {"W"},
                .Exclusive = true,
                .BlockInput = "Never"
            },

            //========================================================
            //============ K I C K S
            //========================================================

            {
                .Event = "LightKickRight",
                .Keys = {"LALT", "E"},
                .Trigger = "Release",
            },
            {
                .Event = "LightKickLeft",
                .Keys = {"LALT", "Q"},
                .Trigger = "Release",
            },
            {
                .Event = "HeavyKickRight",
                .Keys = {"LALT", "E"},
                .Trigger = "Continuous",
                .Duration = 0.33f,
            },
            {
                .Event = "HeavyKickLeft",
                .Keys = {"LALT", "Q"},
                .Trigger = "Continuous",
                .Duration = 0.33f,
            },
            {
                .Event = "HeavyKickRight_Low",
                .Keys = {"LALT", "E"},
                .Trigger = "Release",
                .Duration = 0.1f,
            },
            {
                .Event = "HeavyKickLeft_Low",
                .Keys = {"LALT", "Q"},
                .Trigger = "Release",
                .Duration = 0.1f,
            },

            //========================================================
            //============ C R A W L I N G
            //========================================================
            {
                .Event = "TogglePlayerCrawl",
                .Keys = {"NUMPAD1"},
            },
            {
                .Event = "ToggleFollowerCrawl",
                .Keys = {"NUMPAD3"},
            },
            {
                .Event = "LightSwipeRight",
                .Keys = {"LALT", "E"},
                .Trigger = "Release",
            },
            {
                .Event = "LightSwipeLeft",
                .Keys = {"LALT", "Q"},
                .Trigger = "Release",
            },
            {
                .Event = "HeavySwipeRight",
                .Keys = {"LALT", "E"},
                .Trigger = "Continuous",
                .Duration = 0.33f,
            },
            {
                .Event = "HeavySwipeLeft",
                .Keys = {"LALT", "Q"},
                .Trigger = "Continuous",
                .Duration = 0.33f,
            },


            //========================================================
            //============ C L E A V A G E
            //========================================================

            {
                .Event = "CleavageEnter",
                .Keys = {"LSHIFT", "H"},
            },
            {
                .Event = "CleavageExit",
                .Keys = {"RMB"},
                .BlockInput = "Never"
            },
            {
                .Event = "CleavageLightAttack",
                .Keys = {"LMB"},
                .Exclusive = true,
                .Trigger = "Release",
                .BlockInput = "Never"
            },
            {
                .Event = "CleavageHeavyAttack",
                .Keys = {"LMB"},
                .Exclusive = true,
                .Trigger = "Continuous",
                .Duration = 0.33f,
                .BlockInput = "Never"
            },
            {
                .Event = "CleavageSuffocate",
                .Keys = {"W"},
                .BlockInput = "Never"
            },
            {
                .Event = "CleavageAbsorb",
                .Keys = {"S"},
                .BlockInput = "Never"
            },
            {
                .Event = "CleavageVore",
                .Keys = {"V"},
                .BlockInput = "Never"
            },
        	{
                .Event = "CleavageDOT",
                .Keys = {"E"},
            },

            //========================================================
            //============ H U G S
            //========================================================

            {
                .Event = "HugAttempt",
                .Keys = {"LSHIFT", "H"},
                .Trigger = "Release",
            },
            {
                .Event = "HugPlayer",
                .Keys = {"LSHIFT", "H"},
                .Trigger = "Continuous",
                .Duration = 1.0f,
                .BlockInput = "Never"
            },
            {
                .Event = "HugShrink",
                .Keys = {"LMB"},
                .Exclusive = true,
                .Trigger = "Release"
            },
            {
                .Event = "HugHeal",
                .Keys = {"LMB"},
                .Exclusive = true,
                .Trigger = "Continuous",
                .Duration = 0.33f,
            },
            {
                .Event = "HugCrush",
                .Keys = {"S"},
            },
            {
                .Event = "HugRelease",
                .Keys = {"RMB"},
                .Exclusive = true,
                .BlockInput = "Never"
            },

            //========================================================
            //============ B U T T / B R E A S T   C R U S H
            //========================================================

            {
                .Event = "QuickButtCrushStart",
                .Keys = {"LSHIFT", "B"},
                .Trigger = "Continuous",
                .Duration = 1.0f,
            },
            {
                .Event = "ButtCrushStart",
                .Keys = {"LSHIFT", "B"},
                .Trigger = "Release",
            },
            {
                .Event = "ButtCrushStart_Player",
                .Keys = {"LSHIFT", "B"},
                .Trigger = "Continuous",
                .Duration = 1.0f,
                .BlockInput = "Never"
            },
            {
                .Event = "ButtCrushGrow",
                .Keys = {"W"},
                .Trigger = "Continuous",
            },
            {
                .Event = "ButtCrushAttack",
                .Keys = {"LMB"},
                .Duration = 0.0f,
            },

            //========================================================
            //======== P R O N E   B E H A V I O R
            //========================================================

            {
                .Event = "SBO_ToggleProne",
                .Keys = {"X"},
                .Trigger = "Continuous",
                .Duration = 0.66f,
                .BlockInput = "Never"
            },
            {
                .Event = "SBO_ToggleDive_Standing",
                .Keys = {"W", "S"},
                .Trigger = "Continuous",
                .Duration = 0.50f,
                .BlockInput = "Never"
            },
            {
                .Event = "SBO_ToggleDive_Sneak",
                .Keys = {"W", "S"},
                .Trigger = "Continuous",
                .Duration = 0.50f,
                .BlockInput = "Never"
            },

            //========================================================
            //========================= G R A B
            //======================================================== 

            {
                .Event = "GrabOther",
                .Keys = {"F"},
                .Duration = 0.25f,
                .BlockInput = "Never"
            },
            {
                .Event = "GrabPlayer",
                .Keys = {"F"},
                .Trigger = "Continuous",
                .Duration = 1.0f,
                .BlockInput = "Never"
            },
            {
                .Event = "GrabAttack",
                .Keys = {"E"},
                .Trigger = "Continuous",
                .Duration = 0.50f,
                .BlockInput = "Never"
            },
            {
                .Event = "GrabVore",
                .Keys = {"V"},
                .Trigger = "Continuous",
                .Duration = 0.50f,
            },
            {
                .Event = "GrabThrow",
                .Keys = {"X"},
                .Trigger = "Continuous",
                .BlockInput = "Never"
            },
            {
                .Event = "GrabRelease",
                .Keys = {"RMB"},
            },
            {
                .Event = "BreastsPut",
                .Keys = {"LSHIFT", "B"},
                .Duration = 0.50f,
            },
            {
                .Event = "BreastsRemove",
                .Keys = {"LSHIFT", "B"},
                .Duration = 0.50f,
            },

            //========================================================
            //========================= G R A B  P L A Y
            //======================================================== 
            {
                .Event = "GrabPlay_Start",
                .Keys = {"LSHIFT", "H"},
                .Trigger = "Once",
                .Duration = 0.0f,
                .BlockInput = "Never"
            },
            {
                .Event = "GrabPlay_Exit",
                .Keys = {"LSHIFT","H"},
                .Trigger = "Once",
                .Duration = 0.0f,
                .BlockInput = "Never"
            },
            {
                .Event = "GrabPlay_CrushHeavy",
                .Keys = {"LMB"},
                .Trigger = "Continuous",
                .Duration = 0.5f,
                .BlockInput = "Never"
            },
            {
                .Event = "GrabPlay_Vore",
                .Keys = {"V"},
                .Trigger = "Once",
                .Duration = 0.0f,
                .BlockInput = "Never"
            },
            {
                .Event = "GrabPlay_KissVore",
                .Keys = {"V"},
                .Trigger = "Once",
                .Duration = 0.0f,
                .BlockInput = "Never"
            },
            {
                .Event = "GrabPlay_Kiss",
                .Keys = {"E"},
                .Trigger = "Once",
                .Duration = 0.0f,
                .BlockInput = "Never"
            },
            {
                .Event = "GrabPlay_Poke",
                .Keys = {"LMB"},
                .Trigger = "Once",
                .Duration = 0.0f,
                .BlockInput = "Never"
            },
            {
                .Event = "GrabPlay_Flick",
                .Keys = {"RMB"},
                .Trigger = "Once",
                .Duration = 0.0f,
                .BlockInput = "Never"
            },
            {
                .Event = "GrabPlay_Sandwich",
                .Keys = {"S"},
                .Trigger = "Once",
                .Duration = 0.0f,
                .BlockInput = "Never"
            },
            {
                .Event = "GrabPlay_GrindStart",
                .Keys = {"W"},
                .Trigger = "Once",
                .Duration = 0.0f,
                .BlockInput = "Never"
            },
            {
                .Event = "GrabPlay_GrindStop",
                .Keys = {"W"},
                .Trigger = "Once",
                .Duration = 0.0f,
                .BlockInput = "Never"
            },


            //========================================================
            //========================= C A M E R A
            //========================================================

			
            {
                .Event = "HorizontalCameraReset",
                .Keys = {"RIGHT", "LEFT"},
                .Trigger = "Continuous",
            },
            {
                .Event = "VerticalCameraReset",
                .Keys = {"UP", "DOWN"},
                .Trigger = "Continuous",
            },
            {
                .Event = "CameraLeft",
                .Keys = {"LALT", "LEFT"},
                .Trigger = "Continuous",
            },
            {
                .Event = "CameraRight",
                .Keys = {"LALT", "RIGHT"},
                .Trigger = "Continuous",
            },
            {
                .Event = "CameraUp",
                .Keys = {"LALT", "UP"},
                .Trigger = "Continuous",
            },
            {
                .Event = "CameraDown",
                .Keys = {"LALT", "DOWN"},
                .Trigger = "Continuous",
            },
            {
                .Event = "SwitchCameraMode",
                .Keys = {"F2"}
            },

            //========================================================
            //========================= A N I M  S P E E D
            //========================================================

            {
                .Event = "AnimSpeedUp",
                .Keys = {"LMB"},
                .Trigger = "Continuous",
            },
            {
                .Event = "AnimSpeedDown",
                .Keys = {"RMB"},
                .Trigger = "Continuous",
            },
            {
                .Event = "AnimMaxSpeed",
                .Keys = {"LMB", "RMB"},
                .Trigger = "Continuous",
            },

            //========================================================
            //========================= P E R K S
            //========================================================

            {
                .Event = "ShrinkOutburst",
                .Keys = {"LSHIFT", "F"},
            },
            {
                .Event = "SizeReserve",
                .Keys = {"E"},
                .Trigger = "Continuous",
                .Duration = 1.33f,
                .BlockInput = "Never"
            },
            {
                .Event = "DisplaySizeReserve",
                .Keys = {"F"},
                .Trigger = "Continuous",
                .Duration = 1.33f,
                .BlockInput = "Never"
            },
            {
                .Event = "RapidGrowth",
                .Keys = {"LSHIFT", "1"},
            },
            {
                .Event = "RapidShrink",
                .Keys = {"LSHIFT", "2"},
            },
            {
                .Event = "ManualGrow",
                .Keys = {"UP", "LEFT"},
                .Trigger = "Continuous",
            },
            {
                .Event = "ManualShrink",
                .Keys = {"DOWN", "LEFT"},
                .Trigger = "Continuous",
            },
            {
                .Event = "ManualGrowOther",
                .Keys = {"LSHIFT", "UP", "LEFT"},
                .Trigger = "Continuous",
            },
            {
                .Event = "ManualShrinkOther",
                .Keys = {"LSHIFT", "DOWN", "LEFT"},
                .Trigger = "Continuous",
            },

            {
                .Event = "ManualGrowOverTime",
                .Keys = {"NUMPAD8"},
                .Trigger = "Once",
            },
            {
                .Event = "ManualShrinkOverTime",
                .Keys = {"NUMPAD4"},
                .Trigger = "Once",
            },
            {
                .Event = "ManualGrowOtherOverTime",
                .Keys = {"NUMPAD5"},
                .Trigger = "Once",
            },
            {
                .Event = "ManualShrinkOtherOverTime",
                .Keys = {"NUMPAD2"},
                .Trigger = "Once",
            },

            //========================================================
            //========================= M I S C
            //========================================================

            {
                .Event = "PartyReport",
                .Keys = {"LCONTROL"},
                .Duration = 1.33f,
                .BlockInput = "Never"
            },
            {
                .Event = "ShowQuickStats",
                .Keys = {"F3"},
                .BlockInput = "Never"
            },
            {
                .Event = "DebugReport",
                .Keys = {"RCONTROL"},
                .Duration = 1.33f,
                .BlockInput = "Never"
            },
            {
                .Event = "ProtectSmallOnes",
                .Keys = {"C"},
                .Duration = 1.0f,
                .BlockInput = "Never"
            }

        };


}