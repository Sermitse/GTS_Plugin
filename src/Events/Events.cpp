#include "Events/Events.hpp"

namespace GTS {


	//TODO Eventdispatcher doesnt handle dangling pointers
	//as its assumed a event class is a singleton.
	//find a way to make it instance class safe...


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

	//Fires Before Cosave Serialization
	void EventListener::OnGameSave() {}

	//Fires After Cosave Deserialization
	void EventListener::OnGameLoaded() {}

	//Fires If Config settings are reset.
	void EventListener::OnConfigReset() {}

	//Fires If Config settings are reset.
	void EventListener::OnConfigRefresh() {}

	void EventDispatcher::AddListener(EventListener* a_listener) {
		if (a_listener) {
			m_listeners.push_back(a_listener);
		}
	}

	//NEVER USE THIS, WILL EXPLODE GAME.
	void EventDispatcher::RemoveListener(EventListener* a_listener) {
		if (a_listener) {
			std::erase(m_listeners, a_listener);
		}
	}

	void EventDispatcher::DoUpdate() {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->Update();
		}
	}

	void EventDispatcher::DoPapyrusUpdate() {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->PapyrusUpdate();
		}
	}

	void EventDispatcher::DoHavokUpdate() {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->HavokUpdate();
		}
	}

	void EventDispatcher::DoCameraUpdate() {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->CameraUpdate();
		}
	}

	void EventDispatcher::DoReset() {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->Reset();
		}
	}

	void EventDispatcher::DoEnabled() {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->Enabled();
		}
	}
	void EventDispatcher::DoDisabled() {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->Disabled();
		}
	}
	void EventDispatcher::DoStart() {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->Start();
		}
	}

	void EventDispatcher::DoDataReady() {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->DataReady();
		}
	}

	void EventDispatcher::DoResetActor(Actor* actor) {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->ResetActor(actor);
		}
	}

	void EventDispatcher::DoActorEquip(Actor* actor) {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->ActorEquip(actor);
		}
	}

	void EventDispatcher::DoDragonSoulAbsorption() {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->DragonSoulAbsorption();
		}
	}

	void EventDispatcher::DoActorLoaded(Actor* actor) {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->ActorLoaded(actor);
		}
	}

	void EventDispatcher::DoHitEvent(const TESHitEvent* evt) {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->HitEvent(evt);
		}
	}

	void EventDispatcher::DoUnderFootEvent(const UnderFoot& evt) {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->UnderFootEvent(evt);
		}
	}

	void EventDispatcher::DoOnImpact(const Impact& impact) {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->OnImpact(impact);
		}
	}

	void EventDispatcher::DoHighheelEquip(const HighheelEquip& evt) {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->OnHighheelEquip(evt);
		}
	}

	void EventDispatcher::DoAddPerk(const AddPerkEvent& evt)  {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->OnAddPerk(evt);
		}
	}

	void EventDispatcher::DoRemovePerk(const RemovePerkEvent& evt)  {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->OnRemovePerk(evt);
		}
	}

	void EventDispatcher::DoMenuChange(const MenuOpenCloseEvent* menu_event) {
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->MenuChange(menu_event);
		}
	}

	void EventDispatcher::DoActorAnimEvent(Actor* actor, const BSFixedString& a_tag, const BSFixedString& a_payload) {
		std::string tag = a_tag.c_str();
		std::string payload = a_payload.c_str();
		for (auto listener: m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->ActorAnimEvent(actor, tag, payload);
		}
	}


	void EventDispatcher::DoSerdePreSaveEvent() {
		for (auto listener : m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->OnGameSave();
		}
	}

	void EventDispatcher::DoSerdePostLoadEvent() {
		for (auto listener : m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->OnGameLoaded();
		}
	}

	void EventDispatcher::DoConfigResetEvent() {
		for (auto listener : m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->OnConfigReset();
		}
	}

	void EventDispatcher::DoConfigRefreshEvent() {
		for (auto listener : m_listeners) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->OnConfigRefresh();
		}
	}

	void EventDispatcher::DoFurnitureEvent(const TESFurnitureEvent* a_event) {
		Actor* actor = skyrim_cast<Actor*>(a_event->actor.get());
		TESObjectREFR* object = a_event->targetFurniture.get();
		log::info("Furniture Event Seen");
		log::info("Actor: {}", static_cast<bool>(actor != nullptr));
		log::info("Object: {}", static_cast<bool>(object != nullptr));
		if (actor && object) {
			log::info("Both are true");
			for (auto listener: m_listeners) {
				GTS_PROFILE_SCOPE(listener->DebugName());
				listener->FurnitureEvent(actor, object, a_event->type == TESFurnitureEvent::FurnitureEventType::kEnter);
			}
		}
	}

}
