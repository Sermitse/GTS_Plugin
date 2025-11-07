#include "Systems/Events/EventRegistry.hpp"
#include "Systems/Events/GameEvents.hpp"

#include "API/Racemenu.hpp"
#include "API/SmoothCam.hpp"
#include "API/TrainWreck.hpp"

#include "Config/ConfigModHandler.hpp"
#include "Config/Keybinds.hpp"

#include "Managers/AI/AIManager.hpp"
#include "Managers/AI/headtracking.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/BoobCrush.hpp"
#include "Managers/Animation/Controllers/ThighSandwichController.hpp"
#include "Managers/Animation/Controllers/VoreController.hpp"
#include "Managers/Animation/Grab.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Managers/Attributes.hpp"
#include "Managers/Audio/Footstep.hpp"
#include "Managers/Camera.hpp"
#include "Managers/Console/ConsoleManager.hpp"
#include "Managers/Contact.hpp"
#include "Managers/CrushManager.hpp"
#include "Managers/Damage/CollisionDamage.hpp"
#include "Managers/Explosion.hpp"
#include "Managers/FurnitureManager.hpp"
#include "Managers/Gamemode/GameModeManager.hpp"
#include "Managers/GtsManager.hpp"
#include "Managers/GtsSizeManager.hpp"
#include "Managers/Highheel.hpp"
#include "Managers/HitManager.hpp"
#include "Managers/Input/InputManager.hpp"
#include "Managers/OverkillManager.hpp"
#include "Managers/Perks/PerkHandler.hpp"
#include "Managers/RandomGrowth.hpp"
#include "Managers/Rumble.hpp"
#include "Managers/ShrinkToNothingManager.hpp"
#include "Managers/SpectatorManager.hpp"
#include "Managers/Tremor.hpp"

#include "Magic/Magic.hpp"

#include "Utils/DynamicScale.hpp"
#include "Utils/ItemDistributor.hpp"

#include "UI/DebugMenu.hpp"
#include "UI/GTSMenu.hpp"

namespace GTS {

	void RegisterEventListeners() {

		EventDispatcher::AddListener(&Racemenu::GetSingleton());
		EventDispatcher::AddListener(&TrainWreck::GetSingleton());
		EventDispatcher::AddListener(&Runtime::GetSingleton());                 // Stores spells, globals and other important data
		EventDispatcher::AddListener(&Persistent::GetSingleton());
		EventDispatcher::AddListener(&Transient::GetSingleton());
		EventDispatcher::AddListener(&CooldownManager::GetSingleton());
		EventDispatcher::AddListener(&TaskManager::GetSingleton());
		EventDispatcher::AddListener(&SpringHolder::GetSingleton());
		EventDispatcher::AddListener(&Config::GetSingleton());
		EventDispatcher::AddListener(&Keybinds::GetSingleton());
		EventDispatcher::AddListener(&InputManager::GetSingleton());
		EventDispatcher::AddListener(&ConsoleManager::GetSingleton());
		EventDispatcher::AddListener(&SmoothCam::GetSingleton());
		EventDispatcher::AddListener(&GameModeManager::GetSingleton());         // Manages Game Modes
		EventDispatcher::AddListener(&GtsManager::GetSingleton());              // Manages smooth size increase and animation & movement speed
		EventDispatcher::AddListener(&PerkHandler::GetSingleton());             // Manages some perk updates
		EventDispatcher::AddListener(&SizeManager::GetSingleton());             // Manages Max Scale of everyone
		EventDispatcher::AddListener(&HighHeelManager::GetSingleton());         // Applies high heels
		EventDispatcher::AddListener(&CameraManager::GetSingleton());           // Edits the camera
		EventDispatcher::AddListener(&GameEvents::GetSingleton());              // Registers Skyrim Events
		EventDispatcher::AddListener(&CollisionDamage::GetSingleton());         // Handles precise size-related damage
		EventDispatcher::AddListener(&MagicManager::GetSingleton());            // Manages spells and size changes in general
		EventDispatcher::AddListener(&VoreController::GetSingleton());          // Manages vore
		EventDispatcher::AddListener(&CrushManager::GetSingleton());            // Manages crushing
		EventDispatcher::AddListener(&OverkillManager::GetSingleton());         // Manages crushing
		EventDispatcher::AddListener(&ShrinkToNothingManager::GetSingleton());  // Shrink to nothing manager
		EventDispatcher::AddListener(&FootStepManager::GetSingleton());         // Manages footstep sounds
		EventDispatcher::AddListener(&TremorManager::GetSingleton());           // Manages tremors on footsteps
		EventDispatcher::AddListener(&ExplosionManager::GetSingleton());        // Manages clouds/exposions on footstep
		EventDispatcher::AddListener(&Rumbling::GetSingleton());                // Manages rumbling of contoller/camera for multiple frames
		EventDispatcher::AddListener(&AttributeManager::GetSingleton());        // Adjusts most attributes
		EventDispatcher::AddListener(&RandomGrowth::GetSingleton());            // Manages random growth perk
		EventDispatcher::AddListener(&HitManager::GetSingleton());              // Hit Manager for handleing papyrus hit events
		EventDispatcher::AddListener(&AnimationManager::GetSingleton());        // Manages Animation Events
		EventDispatcher::AddListener(&Grab::GetSingleton());                    // Manages grabbing
		EventDispatcher::AddListener(&ThighSandwichController::GetSingleton()); // Manages Thigh Sandwiching
		EventDispatcher::AddListener(&AnimationBoobCrush::GetSingleton());
		EventDispatcher::AddListener(&AIManager::GetSingleton());               //AI controller for GTS-actions
		EventDispatcher::AddListener(&Headtracking::GetSingleton());            // Headtracking fixes
		EventDispatcher::AddListener(&SpectatorManager::GetSingleton());        // Manage Camera Targets
		EventDispatcher::AddListener(&ContactManager::GetSingleton());          // Manages collisions
		EventDispatcher::AddListener(&DynamicScale::GetSingleton());            // Handles room heights
		EventDispatcher::AddListener(&FurnitureManager::GetSingleton());        // Handles furniture stuff
		EventDispatcher::AddListener(&DebugMenu::GetSingleton());
		EventDispatcher::AddListener(&GTSMenu::GetSingleton());
		EventDispatcher::AddListener(&ItemDistributor::GetSingleton());
		EventDispatcher::AddListener(&ConfigModHandler::GetSingleton());

		log::info("Managers Registered");
	}
}
