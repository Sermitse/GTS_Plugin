
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Grab_Throw.hpp"
#include "Managers/OverkillManager.hpp"
#include "Managers/Animation/Grab.hpp"

#include "Managers/Animation/Grab_Play.hpp"

#include "Managers/Rumble.hpp"

#include "Managers/Input/InputManager.hpp"
#include "Utils/Actions/InputConditions.hpp"


using namespace GTS;

namespace Triggers {

	void PassAnimation(std::string anim_gts, std::string anim_tiny) {
		Actor* Player = GetPlayerOrControlled();
		Actor* Tiny = Grab::GetHeldActor(Player);
		if (Tiny) {
			AnimationManager::StartAnim(anim_gts, Player);
			AnimationManager::StartAnim(anim_tiny, Tiny);
		}
	}

	void GrabPlay_StartEvent(const ManagedInputEvent& data) {
		// Don't allow NPC's to start it if they don't have weapon/magic sheathed too
		auto player = PlayerCharacter::GetSingleton();
		auto Camera = PlayerCamera::GetSingleton();
        bool Sheathed = Camera->isWeapSheathed;
		if (!Sheathed) {
			std::string message = std::format("You need to sheathe weapon/magic first");
			shake_camera(player, 0.45f, 0.30f);
			NotifyWithSound(player, message);
			return;
		}
		if (!IsGtsBusy(player) && !IsInCleavageState(player)) {
			PassAnimation("GrabPlay_Enter", "GrabPlay_Enter_T");
		}
	}

	void GrabPlay_ExitEvent(const ManagedInputEvent& data) {
		PassAnimation("GrabPlay_Exit", "GrabPlay_Exit_T");
	}

	void GrabPlay_CrushHeavyEvent(const ManagedInputEvent& data) {
		PassAnimation("GrabPlay_CrushH", "GrabPlay_CrushH_T");
	}

	void GrabPlay_VoreEvent(const ManagedInputEvent& data) {
		PassAnimation("GrabPlay_Vore", "GrabPlay_Vore_T");
	}

	void GrabPlay_KissEvent(const ManagedInputEvent& data) {
		PassAnimation("GrabPlay_Kiss", "GrabPlay_Kiss_T");
	}

	void GrabPlay_KissVoreEvent(const ManagedInputEvent& data) {
		PassAnimation("GrabPlay_KissVore", "GrabPlay_KissVore_T");
	}

	void GrabPlay_PokeEvent(const ManagedInputEvent& data) {
		PassAnimation("GrabPlay_Poke", "GrabPlay_Poke_T");
	}

	void GrabPlay_FlickEvent(const ManagedInputEvent& data) {
		PassAnimation("GrabPlay_Flick", "GrabPlay_Flick_T");
	}

	void GrabPlay_SandwichEvent(const ManagedInputEvent& data) {
		PassAnimation("GrabPlay_Sandwich", "GrabPlay_Sandwich_T");
	}

	void GrabPlay_GrindStartEvent(const ManagedInputEvent& data) {
		PassAnimation("GrabPlay_GrindStart", "GrabPlay_GrindStart_T");
	}

	void GrabPlay_GrindStopEvent(const ManagedInputEvent& data) {
		PassAnimation("GrabPlay_GrindStop", "GrabPlay_GrindStop_T");
	}
}

namespace GTS {

	void Animation_GrabPlay::RegisterTriggers() {
		AnimationManager::RegisterTrigger("GrabPlay_Enter", "GrabPlay", "GTSBEH_Hand_Enter");
		AnimationManager::RegisterTrigger("GrabPlay_Enter_T", "GrabPlay", "GTSBEH_T_Hand_Enter");

		AnimationManager::RegisterTrigger("GrabPlay_Exit", "GrabPlay", "GTSBEH_Hand_Exit");
		AnimationManager::RegisterTrigger("GrabPlay_Exit_T", "GrabPlay", "GTSBEH_T_Hand_Exit");

		AnimationManager::RegisterTrigger("GrabPlay_CrushH", "GrabPlay", "GTSBEH_Hand_Crush_Heavy");
		AnimationManager::RegisterTrigger("GrabPlay_CrushH_T", "GrabPlay", "GTSBEH_T_Hand_Crush_Heavy");

		AnimationManager::RegisterTrigger("GrabPlay_Vore", "GrabPlay", "GTSBEH_Hand_Vore");
		AnimationManager::RegisterTrigger("GrabPlay_Vore_T", "GrabPlay", "GTSBEH_T_Hand_Vore");

		AnimationManager::RegisterTrigger("GrabPlay_KissVore", "GrabPlay", "GTSBEH_Hand_Kiss_Vore");
		AnimationManager::RegisterTrigger("GrabPlay_KissVore_T", "GrabPlay", "GTSBEH_T_Hand_Kiss_Vore");

		AnimationManager::RegisterTrigger("GrabPlay_Kiss", "GrabPlay", "GTSBEH_Hand_Kiss");
		AnimationManager::RegisterTrigger("GrabPlay_Kiss_T", "GrabPlay", "GTSBEH_T_Hand_Kiss");

		AnimationManager::RegisterTrigger("GrabPlay_Poke", "GrabPlay", "GTSBEH_Hand_Poke");
		AnimationManager::RegisterTrigger("GrabPlay_Poke_T", "GrabPlay", "GTSBEH_T_Hand_Poke");

		AnimationManager::RegisterTrigger("GrabPlay_Flick", "GrabPlay", "GTSBEH_Hand_Flick_Heavy");
		AnimationManager::RegisterTrigger("GrabPlay_Flick_T", "GrabPlay", "GTSBEH_T_Hand_Flick_Heavy");

		AnimationManager::RegisterTrigger("GrabPlay_Sandwich", "GrabPlay", "GTSBEH_Hand_Sandwich");
		AnimationManager::RegisterTrigger("GrabPlay_Sandwich_T", "GrabPlay", "GTSBEH_T_Hand_Sandwich");

		AnimationManager::RegisterTrigger("GrabPlay_GrindStart", "GrabPlay", "GTSBEH_Hand_Grind_Start");
		AnimationManager::RegisterTrigger("GrabPlay_GrindStart_T", "GrabPlay", "GTSBEH_T_Hand_Grind_Start");

		AnimationManager::RegisterTrigger("GrabPlay_GrindStop", "GrabPlay", "GTSBEH_Hand_Grind_Stop");
		AnimationManager::RegisterTrigger("GrabPlay_GrindStop_T", "GrabPlay", "GTSBEH_T_Hand_Grind_Stop");

		InputManager::RegisterInputEvent("GrabPlay_Start", Triggers::GrabPlay_StartEvent, GrabPlayStartCondition);
		InputManager::RegisterInputEvent("GrabPlay_Exit", Triggers::GrabPlay_ExitEvent, GrabPlayActionCondition);

		InputManager::RegisterInputEvent("GrabPlay_CrushHeavy", Triggers::GrabPlay_CrushHeavyEvent, GrabPlayActionCondition);

		InputManager::RegisterInputEvent("GrabPlay_Vore", Triggers::GrabPlay_VoreEvent, GrabPlayActionCondition);
		InputManager::RegisterInputEvent("GrabPlay_Kiss", Triggers::GrabPlay_KissEvent, GrabPlayActionCondition);

		InputManager::RegisterInputEvent("GrabPlay_KissVore", Triggers::GrabPlay_KissVoreEvent, GrabPlayActionCondition);
		InputManager::RegisterInputEvent("GrabPlay_Poke", Triggers::GrabPlay_PokeEvent, GrabPlayActionCondition);

		InputManager::RegisterInputEvent("GrabPlay_Flick", Triggers::GrabPlay_FlickEvent, GrabPlayActionCondition);
		InputManager::RegisterInputEvent("GrabPlay_Sandwich", Triggers::GrabPlay_SandwichEvent, GrabPlayActionCondition);

		InputManager::RegisterInputEvent("GrabPlay_GrindStart", Triggers::GrabPlay_GrindStartEvent, GrabPlayActionCondition);
		InputManager::RegisterInputEvent("GrabPlay_GrindStop", Triggers::GrabPlay_GrindStopEvent, GrabPlayActionCondition);
	}
}

/* TRIGGERS
----Events for the GTS hand state
            GTSBEH_Hand_Enter
            GTSBEH_Hand_Exit
            GTSBEH_Hand_Crush_Heavy
            GTSBEH_Hand_Vore
            GTSBEH_Hand_Kiss
            GTSBEH_Hand_Kiss_Vore
            GTSBEH_Hand_Poke
            GTSBEH_Hand_Flick_Heavy
            GTSBEH_Hand_Sandwich
            GTSBEH_Hand_Grind_Start
            GTSBEH_Hand_Grind_Stop
            GTSBEH_Hand_To_Boobs

            
    ---- Trigger for the Tiny hand State
            GTSBEH_T_Hand_Enter
            GTSBEH_T_Hand_Exit
            GTSBEH_T_Hand_Crush_Heavy
            GTSBEH_T_Hand_Vore
            GTSBEH_T_Hand_Kiss
            GTSBEH_T_Hand_Kiss_Vore
            GTSBEH_T_Hand_Poke
            GTSBEH_T_Hand_Flick_Heavy
            GTSBEH_T_Hand_Sandwich
            GTSBEH_T_Hand_Grind_Start
            GTSBEH_T_Hand_Grind_Stop
            GTSBEH_T_Hand_To_Boobs

*/
