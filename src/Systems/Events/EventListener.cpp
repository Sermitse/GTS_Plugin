#include "Systems/Events/EventListener.hpp"
#include "Systems/Events/EventData.hpp"

namespace GTS {

	// Called on Live (non paused) gameplay
	void EventListener::Update() {}

	// Called on Papyrus OnUpdate
	void EventListener::PapyrusUpdate() {}

	// Called on Havok update (when processing hitjobs)
	void EventListener::HavokUpdate() {}

	// Called when the camera update event is fired (in the TESCameraState)
	void EventListener::CameraUpdate() {}

	// Called on game load started (not yet finished)
	// and when new game is selected
	void EventListener::Reset() {}

	// Called when game is enabled (while not paused)
	void EventListener::Enabled() {}

	// Called when game is disabled (while not paused)
	void EventListener::Disabled() {}

	// Called when a game is started after a load/newgame
	void EventListener::Start() {}

	// Called when all forms are loaded (during game load before mainmenu)
	void EventListener::DataReady() {}

	// Called when all forms are loaded (during game load before mainmenu)
	void EventListener::ResetActor(Actor* actor) {}

	// Called when an actor has an item equipped
	void EventListener::ActorEquip(Actor* actor) {}

	// Called when Player absorbs dragon soul
	void EventListener::DragonSoulAbsorption() {}

	// Called when an actor has is fully loaded
	void EventListener::ActorLoaded(Actor* actor) {}

	// Called when a papyrus hit event is fired
	void EventListener::HitEvent(const TESHitEvent* evt) {}

	// Called when an actor is squashed underfoot
	void EventListener::UnderFootEvent(const UnderFoot& evt) {}

	// Fired when a foot lands
	void EventListener::OnImpact(const Impact& impact) {}

	// Fired when a highheel is (un)equiped or when an actor is loaded with HH
	void EventListener::OnHighheelEquip(const HighheelEquip& evt) {}

	// Fired when a perk is added
	void EventListener::OnAddPerk(const AddPerkEvent& evt) {}

	// Fired when a perk about to be removed
	void EventListener::OnRemovePerk(const RemovePerkEvent& evt) {}

	// Fired when a skyrim menu event occurs
	void EventListener::MenuChange(const MenuOpenCloseEvent* menu_event) {}

	// Fired when a actor animation event occurs
	void EventListener::ActorAnimEvent(Actor* actor, const std::string_view& tag, const std::string_view& payload) {}

	// Fired when actor uses furniture
	void EventListener::FurnitureEvent(Actor* user, TESObjectREFR* object, bool enter) {}

	// Fired when actor uses furniture
	void EventListener::DeathEvent(Actor* a_killer, Actor* a_victim, bool a_dead) {}

	//Fires Before Cosave Serialization
	void EventListener::OnGameSave() {}

	//Fires After Cosave Deserialization
	void EventListener::OnGameLoaded() {}

	//Fires After Cosave Deserialization
	void EventListener::OnGameRevert() {}

	//Fires If Config settings are reset.
	void EventListener::OnConfigReset() {}

	//Fires when a config refresh is requested.
	void EventListener::OnConfigRefresh() {}

	//Fires On SKSE PostLoad event
	void EventListener::OnPluginPostLoad() {}

}