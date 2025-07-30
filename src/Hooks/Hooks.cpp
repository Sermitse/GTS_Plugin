#include "Hooks/Hooks.hpp"

#include "Hooks/Actor/Actor.hpp"
#include "Hooks/Actor/ActorValueOwner.hpp"
#include "Hooks/Actor/ActorEquipManager.hpp"
#include "Hooks/Actor/BSAnimationGraph.hpp"
#include "Hooks/Actor/Controls.hpp"
#include "Hooks/Actor/Damage.hpp"
#include "Hooks/Actor/Detection.hpp"
#include "Hooks/Actor/HeadTracking.hpp"
#include "Hooks/Actor/Jump.hpp"
#include "Hooks/Actor/MagicTarget.hpp"
#include "Hooks/Actor/Perk.hpp"
#include "Hooks/Actor/Race.hpp"
#include "Hooks/Actor/Scale.hpp"
#include "Hooks/Actor/Sink.hpp"
#include "Hooks/Animation/HeadtrackingGraph.hpp"
#include "Hooks/Animation/PreventAnimations.hpp"
#include "Hooks/Camera/PlayerCamera.hpp"
#include "Hooks/Camera/TESCamera.hpp"
#include "Hooks/Camera/TESCameraState.hpp"
#include "Hooks/Engine/Input.hpp"
#include "Hooks/Engine/Main.hpp"
#include "Hooks/Engine/Present.hpp"
#include "Hooks/Havok/Havok.hpp"
#include "Hooks/Havok/hkbBehaviorGraph.hpp"
#include "Hooks/Havok/Pushback.hpp"
#include "Hooks/Papyrus/PushAway.hpp"
#include "Hooks/Papyrus/VM.hpp"
#include "Hooks/Projectile/Projectiles.hpp"
#include "Hooks/Sound/BGSImpactManager.hpp"
#include "Hooks/UI/Console.hpp"

namespace Hooks {

	void Install(){

		logger::info("Installing Hooks...");
		auto& SKSETrampoline = SKSE::GetTrampoline();
		SKSETrampoline.create(256);

		//Actor
		Hook_Actor::Install();
		Hook_ActorEquipManager::Install();
		Hook_ActorValueOwner::Install();
		Hook_BSAnimationGraph::Install();
		Hook_Controls::Install();
		Hook_Damage::Install();
		Hook_Detection::Install();
		Hook_HeadTracking::Install();
		Hook_Jump::Install();
		Hook_Perk::Install();
		Hook_Race::Install();
		Hook_Scale::Install();
		Hook_Sink::Install();

		//Animation
		Hook_PreventAnimations::Install();

		//Camera
		Hook_TESCameraState::Install();

		//Engine
		Hook_Input::Install();
		Hook_MainUpdate::Install();
		Hook_Present::Install();

		//Havok
		Hook_Havok::Install();
		Hook_hkbBehaviorGraph::Install();
		Hook_PushBack::Install();

		//Papyrus
		Hook_VM::Install();
		Hook_PushAway::Install();

		//Projectile
		Hook_Projectiles::Install();

		//Sound
		Hook_BGSImpactManager::Install();

		//UI
		Hook_Console::Install();

		//Experiments
		//Hook_Experiments::Install();

		//Unused
		//Hook_PlayerCamera::Install();
		//Hook_TESCamera::Install();
		//Hook_MagicTarget::Install();

		log::info("Finished applying hooks");
		log::info("Default Trampoline Used: {}/{} Bytes", SKSETrampoline.allocated_size(), SKSETrampoline.capacity());

	}
}