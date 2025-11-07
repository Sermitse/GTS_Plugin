#include "Systems/Events/GameEvents.hpp"

namespace GTS {

	std::string GameEvents::DebugName() {
		return "::GameEvents";
	}

	void GameEvents::DataReady() {
		auto event_sources = ScriptEventSourceHolder::GetSingleton();
		if (event_sources) {
			event_sources->AddEventSink<TESHitEvent>(this);
			event_sources->AddEventSink<TESObjectLoadedEvent>(this);
			event_sources->AddEventSink<TESEquipEvent>(this);
			event_sources->AddEventSink<TESTrackedStatsEvent>(this);
			event_sources->AddEventSink<TESResetEvent>(this);
			//event_sources->AddEventSink<TESFurnitureEvent>(this); // Uncomment it to enable this event!
			// Also don't forget to uncomment data->UsingFurniture inside PerformRoofRaycastAdjustments
		}


		auto ui = UI::GetSingleton();
		if (ui) {
			ui->AddEventSink<MenuOpenCloseEvent>(this);
			log::info("Successfully registered MenuOpenCloseEventHandler");
		} else {
			log::error("Failed to register MenuOpenCloseEventHandler");
			return;
		}
	}

	BSEventNotifyControl GameEvents::ProcessEvent(const TESHitEvent* evn, BSTEventSource<TESHitEvent>* dispatcher){
		if (evn) EventDispatcher::DoHitEvent(evn);
		return BSEventNotifyControl::kContinue;
	}

	BSEventNotifyControl GameEvents::ProcessEvent(const TESObjectLoadedEvent* evn, BSTEventSource<TESObjectLoadedEvent>* dispatcher) {
		if (evn) {
			if (auto* actor = TESForm::LookupByID<Actor>(evn->formID)){
				EventDispatcher::DoActorLoaded(actor);
			}
		}
		return BSEventNotifyControl::kContinue;
	}

	BSEventNotifyControl GameEvents::ProcessEvent(const TESResetEvent* evn, BSTEventSource<TESResetEvent>* dispatcher) {
		if (evn) {
			if (auto* object = evn->object.get()) {
				if (auto* actor = TESForm::LookupByID<Actor>(object->formID)) {
					EventDispatcher::DoResetActor(actor);
				}
			}
		}
		return BSEventNotifyControl::kContinue;
	}

	BSEventNotifyControl GameEvents::ProcessEvent(const TESEquipEvent* evn, BSTEventSource<TESEquipEvent>* dispatcher) {
		if (evn) {
			if (auto* actor = TESForm::LookupByID<Actor>(evn->actor->formID)) {
				EventDispatcher::DoActorEquip(actor);
			}
		}
		return BSEventNotifyControl::kContinue;
	}

	BSEventNotifyControl GameEvents::ProcessEvent(const TESTrackedStatsEvent* evn, BSTEventSource<TESTrackedStatsEvent>* dispatcher){
		if (evn) {
			if (evn->stat == "Dragon Souls Collected") {
				EventDispatcher::DoDragonSoulAbsorption();
			}
		}
		return BSEventNotifyControl::kContinue;
	}

	BSEventNotifyControl GameEvents::ProcessEvent(const MenuOpenCloseEvent* a_event, BSTEventSource<MenuOpenCloseEvent>* a_eventSource) {
		if (a_event) EventDispatcher::DoMenuChange(a_event);
		return RE::BSEventNotifyControl::kContinue;
	}

	BSEventNotifyControl GameEvents::ProcessEvent(const TESFurnitureEvent* a_event, BSTEventSource<TESFurnitureEvent>* a_eventSource) {
		if (a_event) EventDispatcher::DoFurnitureEvent(a_event);
		return RE::BSEventNotifyControl::kContinue;
	}

}
