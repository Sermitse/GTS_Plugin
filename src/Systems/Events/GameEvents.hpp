#pragma once

// Module that handles various reload events

namespace GTS {

	class GameEvents : public EventListener, public CInitSingleton<GameEvents>,
		public BSTEventSink<TESHitEvent>,
		public BSTEventSink<TESObjectLoadedEvent>,
		public BSTEventSink<TESResetEvent>,
		public BSTEventSink<TESEquipEvent>,
		public BSTEventSink<TESTrackedStatsEvent>,
		public BSTEventSink<MenuOpenCloseEvent>,
		public BSTEventSink<TESFurnitureEvent> {

		public:
		virtual std::string DebugName() override;
		virtual void DataReady() override;

		protected:
		virtual BSEventNotifyControl ProcessEvent(const TESHitEvent * evn, BSTEventSource<TESHitEvent> * dispatcher) override;
		virtual BSEventNotifyControl ProcessEvent(const TESObjectLoadedEvent * evn, BSTEventSource<TESObjectLoadedEvent> * dispatcher) override;
		virtual BSEventNotifyControl ProcessEvent(const TESResetEvent* evn, BSTEventSource<TESResetEvent>* dispatcher) override;
		virtual BSEventNotifyControl ProcessEvent(const TESEquipEvent* evn, BSTEventSource<TESEquipEvent>* dispatcher) override;
		virtual BSEventNotifyControl ProcessEvent(const TESTrackedStatsEvent* evn, BSTEventSource<TESTrackedStatsEvent>* dispatcher) override;
		virtual BSEventNotifyControl ProcessEvent(const MenuOpenCloseEvent* a_event, BSTEventSource<MenuOpenCloseEvent>* a_eventSource) override;
		virtual BSEventNotifyControl ProcessEvent(const TESFurnitureEvent* a_event, BSTEventSource<TESFurnitureEvent>* a_eventSource) override;
	};
}
