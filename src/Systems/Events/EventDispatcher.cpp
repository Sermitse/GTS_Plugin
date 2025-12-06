#include "Systems/Events/EventDispatcher.hpp"
#include "Systems/Events/EventListener.hpp"
#include "Systems/Events/EventData.hpp"

namespace GTS {

	void EventDispatcher::AddListener(EventListener* a_listener) {
		if (!a_listener) return;
		std::lock_guard lock(m_lock);
		logger::debug("Adding Listener: {}", a_listener->DebugName());
		m_listeners.push_back(ListenerEntry(a_listener));
	}

	void EventDispatcher::RemoveListener(EventListener* a_listener) {
		if (!a_listener) return;
		std::lock_guard lock(m_lock);
		for (auto& entry : m_listeners) {
			EventListener* p = entry.ptr.load(std::memory_order_relaxed);
			if (p == a_listener) {
				logger::debug("Deleting Listener: {}", a_listener->DebugName());
				entry.ptr.store(nullptr, std::memory_order_release);
			}
		}
	}

	void EventDispatcher::Compact() {
		std::lock_guard lock(m_lock);
		tbb::concurrent_vector<ListenerEntry> compacted;
		compacted.reserve(m_listeners.size());

		for (auto& entry : m_listeners) {
			if (EventListener* p = entry.ptr.load(std::memory_order_relaxed)) {
				compacted.push_back(ListenerEntry(p));
			}
		}

		m_listeners.swap(compacted);
	}

	void EventDispatcher::DoUpdate() {

		////Experiment
		//ForEachListenerConcurrent([](EventListener* listener) {
		//	GTS_PROFILE_SCOPE(listener->DebugName());
		//	listener->Update();
		//});

		ForEachListener([](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->Update();
		});
	}

	void EventDispatcher::DoActorUpdate(RE::Actor* actor) {

		////Experiment
		//ForEachListenerConcurrent([](EventListener* listener) {
		//	GTS_PROFILE_SCOPE(listener->DebugName());
		//	listener->Update();
		//});

		ForEachListener([actor](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->ActorUpdate(actor);
		});
	}

	void EventDispatcher::DoPapyrusUpdate() {
		ForEachListener([](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->PapyrusUpdate();
		});
	}

	void EventDispatcher::DoHavokUpdate() {
		ForEachListener([](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->HavokUpdate();
		});
	}

	void EventDispatcher::DoCameraUpdate() {
		ForEachListener([](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->CameraUpdate();
		});
	}

	void EventDispatcher::DoReset() {
		ForEachListener([](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->Reset();
		});
	}

	void EventDispatcher::DoEnabled() {
		ForEachListener([](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->Enabled();
		});
	}

	void EventDispatcher::DoDisabled() {
		ForEachListener([](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->Disabled();
		});
	}

	void EventDispatcher::DoStart() {
		ForEachListener([](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->Start();
		});
	}


	void EventDispatcher::DoDataReady() {
		ForEachListener([](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->DataReady();
		});
	}


	void EventDispatcher::DoResetActor(RE::Actor* actor) {
		ForEachListener([actor](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->ResetActor(actor);
		});
	}


	void EventDispatcher::DoActorEquip(RE::Actor* actor) {
		ForEachListener([actor](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->ActorEquip(actor);
		});
	}


	void EventDispatcher::DoDragonSoulAbsorption() {
		ForEachListener([](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->DragonSoulAbsorption();
		});
	}


	void EventDispatcher::DoActorLoaded(RE::Actor* actor) {
		ForEachListener([actor](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->ActorLoaded(actor);
		});
	}


	void EventDispatcher::DoHitEvent(const RE::TESHitEvent* evt) {
		ForEachListener([evt](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->HitEvent(evt);
		});
	}


	void EventDispatcher::DoUnderFootEvent(const UnderFoot& evt) {
		ForEachListener([evt](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->UnderFootEvent(evt);
		});
	}


	void EventDispatcher::DoOnImpact(const Impact& impact) {
		ForEachListener([impact](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->OnImpact(impact);
		});
	}


	void EventDispatcher::DoHighheelEquip(const HighheelEquip& evt) {
		ForEachListener([evt](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->OnHighheelEquip(evt);
		});
	}


	void EventDispatcher::DoAddPerk(const AddPerkEvent& evt) {
		ForEachListener([evt](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->OnAddPerk(evt);
		});
	}


	void EventDispatcher::DoRemovePerk(const RemovePerkEvent& evt) {
		ForEachListener([evt](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->OnRemovePerk(evt);
		});
	}


	void EventDispatcher::DoMenuChange(const RE::MenuOpenCloseEvent* menu_event) {
		ForEachListener([menu_event](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName()); listener->MenuChange(menu_event);
		});
	}


	void EventDispatcher::DoActorAnimEvent(RE::Actor* actor, const RE::BSFixedString& a_tag, const RE::BSFixedString& a_payload) {
		std::string tag = a_tag.c_str();
		std::string payload = a_payload.c_str();
		ForEachListener([actor, tag, payload](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->ActorAnimEvent(actor, tag, payload);
		});
	}


	void EventDispatcher::DoSerdePreSaveEvent() {
		ForEachListener([](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->OnGameSave();
		});
	}


	void EventDispatcher::DoSerdePostLoadEvent() {
		ForEachListener([](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->OnGameLoaded();
		});
	}

	void EventDispatcher::DoSerdeRevert() {
		ForEachListener([](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->OnGameRevert();
		});
	}


	void EventDispatcher::DoConfigResetEvent() {
		ForEachListener([](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->OnConfigReset();
		});
	}


	void EventDispatcher::DoConfigRefreshEvent() {
		ForEachListener([](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->OnConfigRefresh();
		});
	}

	void EventDispatcher::DoPluginPostLoad() {
		ForEachListener([](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			listener->OnPluginPostLoad();
		});
	}



	void EventDispatcher::DoFurnitureEvent(const TESFurnitureEvent* a_event) {
		Actor* actor = skyrim_cast<Actor*>(a_event->actor.get());
		TESObjectREFR* object = a_event->targetFurniture.get();
		//logger::trace("Furniture Event Seen");
		//logger::trace("Actor: {}", static_cast<bool>(actor != nullptr));
		//logger::trace("Object: {}", static_cast<bool>(object != nullptr));
		if (actor && object) {
			//logger::trace("Both are true");
			ForEachListener([actor, object, a_event](EventListener* listener) {
				GTS_PROFILE_SCOPE(listener->DebugName());
				listener->FurnitureEvent(actor, object, a_event->type == TESFurnitureEvent::FurnitureEventType::kEnter);
			});
		}
	}

	void EventDispatcher::DoDeathEvent(const TESDeathEvent* a_event) {
		ForEachListener([a_event](EventListener* listener) {
			GTS_PROFILE_SCOPE(listener->DebugName());
			Actor* Killer = skyrim_cast<Actor*>(a_event->actorKiller.get());
			Actor* Victim = skyrim_cast<Actor*>(a_event->actorDying.get());
			listener->DeathEvent(Killer, Victim, a_event->dead);

		});
	}
}
