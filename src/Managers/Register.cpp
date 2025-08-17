#include "Managers/Register.hpp"

#include "Managers/MaxSizeManager.hpp"

#include "SpectatorManager.hpp"

#include "AI/AIManager.hpp"

#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/BoobCrush.hpp"
#include "Managers/Animation/Grab.hpp"

#include "Managers/FurnitureManager.hpp"

#include "Managers/Gamemode/GameModeManager.hpp"
#include "Managers/Animation/Controllers/ThighSandwichController.hpp"
#include "Managers/Animation/Controllers/VoreController.hpp"
#include "Managers/ShrinkToNothingManager.hpp"
#include "Managers/Perks/PerkHandler.hpp"
#include "Managers/Damage/CollisionDamage.hpp"
#include "Managers/Audio/Footstep.hpp"

#include "Managers/AI/headtracking.hpp"

#include "Managers/GtsSizeManager.hpp"
#include "Managers/CrushManager.hpp"
#include "Managers/OverkillManager.hpp"
#include "Managers/RandomGrowth.hpp"
#include "Managers/Attributes.hpp"
#include "Managers/GtsManager.hpp"
#include "Managers/HitManager.hpp"
#include "Managers/Explosion.hpp"
#include "Managers/Reloader.hpp"
#include "Managers/Highheel.hpp"
#include "Managers/Contact.hpp"
#include "Managers/Camera.hpp"
#include "Managers/Tremor.hpp"
#include "Managers/Rumble.hpp"


#include "Utils/DynamicScale.hpp"
#include "Magic/Magic.hpp"

namespace GTS {

	void RegisterManagers() {

		EventDispatcher::AddListener(&GameModeManager::GetSingleton()); // Manages Game Modes
		EventDispatcher::AddListener(&GtsManager::GetSingleton()); // Manages smooth size increase and animation & movement speed
		EventDispatcher::AddListener(&PerkHandler::GetSingleton()); // Manages some perk updates
		EventDispatcher::AddListener(&SizeManager::GetSingleton()); // Manages Max Scale of everyone
		EventDispatcher::AddListener(&HighHeelManager::GetSingleton()); // Applies high heels
		EventDispatcher::AddListener(&CameraManager::GetSingleton()); // Edits the camera
		EventDispatcher::AddListener(&ReloadManager::GetSingleton()); // Handles Skyrim Events
		EventDispatcher::AddListener(&CollisionDamage::GetSingleton()); // Handles precise size-related damage
		EventDispatcher::AddListener(&MagicManager::GetSingleton()); // Manages spells and size changes in general
		EventDispatcher::AddListener(&VoreController::GetSingleton()); // Manages vore
		EventDispatcher::AddListener(&CrushManager::GetSingleton()); // Manages crushing
		EventDispatcher::AddListener(&OverkillManager::GetSingleton()); // Manages crushing
		EventDispatcher::AddListener(&ShrinkToNothingManager::GetSingleton()); // Shrink to nothing manager
		EventDispatcher::AddListener(&FootStepManager::GetSingleton()); // Manages footstep sounds
		EventDispatcher::AddListener(&TremorManager::GetSingleton()); // Manages tremors on footsteps
		EventDispatcher::AddListener(&ExplosionManager::GetSingleton()); // Manages clouds/exposions on footstep
		EventDispatcher::AddListener(&Rumbling::GetSingleton()); // Manages rumbling of contoller/camera for multiple frames
		EventDispatcher::AddListener(&AttributeManager::GetSingleton()); // Adjusts most attributes
		EventDispatcher::AddListener(&RandomGrowth::GetSingleton()); // Manages random growth perk
		EventDispatcher::AddListener(&HitManager::GetSingleton()); // Hit Manager for handleing papyrus hit events
		EventDispatcher::AddListener(&AnimationManager::GetSingleton()); // Manages Animation Events
		EventDispatcher::AddListener(&Grab::GetSingleton()); // Manages grabbing
		EventDispatcher::AddListener(&ThighSandwichController::GetSingleton()); // Manages Thigh Sandwiching
		EventDispatcher::AddListener(&AnimationBoobCrush::GetSingleton());
		EventDispatcher::AddListener(&AIManager::GetSingleton()); //AI controller for GTS-actions
		EventDispatcher::AddListener(&Headtracking::GetSingleton()); // Headtracking fixes
		EventDispatcher::AddListener(&SpectatorManager::GetSingleton()); // Manage Camera Targets
		EventDispatcher::AddListener(&ContactManager::GetSingleton()); // Manages collisions
		EventDispatcher::AddListener(&DynamicScale::GetSingleton()); // Handles room heights
		EventDispatcher::AddListener(&FurnitureManager::GetSingleton()); // Handles furniture stuff
		log::info("Managers Registered");
	}
}
