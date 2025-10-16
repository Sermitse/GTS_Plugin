#pragma once

#include "Systems/Events/EventData.hpp"

namespace GTS {

	class EventListener;

	class EventDispatcher {

		public:
		static void AddListener(EventListener* a_listener);
		static void RemoveListener(EventListener* a_listener);
		static void DoUpdate();
		static void DoPapyrusUpdate();
		static void DoHavokUpdate();
		static void DoCameraUpdate();
		static void DoReset();
		static void DoEnabled();
		static void DoDisabled();
		static void DoStart();
		static void DoDataReady();
		static void DoResetActor(RE::Actor* actor);
		static void DoActorEquip(RE::Actor* actor);
		static void DoDragonSoulAbsorption();
		static void DoActorLoaded(RE::Actor* actor);
		static void DoHitEvent(const RE::TESHitEvent* evt);
		static void DoUnderFootEvent(const UnderFoot& evt);
		static void DoOnImpact(const Impact& impact);
		static void DoHighheelEquip(const HighheelEquip& evt);
		static void DoAddPerk(const AddPerkEvent& evt);
		static void DoRemovePerk(const RemovePerkEvent& evt);
		static void DoMenuChange(const RE::MenuOpenCloseEvent* menu_event);
		static void DoActorAnimEvent(RE::Actor* actor, const RE::BSFixedString& a_tag, const RE::BSFixedString& a_payload);
		static void DoSerdePreSaveEvent();
		static void DoSerdePostLoadEvent();
		static void DoConfigResetEvent();
		static void DoConfigRefreshEvent();
		static void DoPluginPostLoad();
		static void DoFurnitureEvent(const TESFurnitureEvent* a_event);
		static void Compact();

		private:

        struct ListenerEntry {
            std::atomic<EventListener*> ptr{ nullptr };

            ListenerEntry() = default;
            explicit ListenerEntry(EventListener* p) : ptr(p) {}

            ListenerEntry(const ListenerEntry& other) : ptr(other.ptr.load(std::memory_order_relaxed)) {}
            ListenerEntry& operator=(const ListenerEntry& other) {
                ptr.store(other.ptr.load(std::memory_order_relaxed), std::memory_order_relaxed);
                return *this;
            }

            ListenerEntry(ListenerEntry&& other) noexcept : ptr(other.ptr.exchange(nullptr, std::memory_order_relaxed)) {}
            ListenerEntry& operator=(ListenerEntry&& other) noexcept {
                ptr.store(other.ptr.exchange(nullptr, std::memory_order_relaxed), std::memory_order_relaxed);
                return *this;
            }
        };

        static inline std::mutex m_lock;
        static inline tbb::concurrent_vector<ListenerEntry> m_listeners;

        template <typename Func>
        static void ForEachListener(Func&& func) {
            for (auto& entry : m_listeners) {
                if (EventListener* listener = entry.ptr.load(std::memory_order_acquire)) {
                    func(listener);
                }
            }
        }

        template <typename Func>
        static void ForEachListenerConcurrent(Func&& func) {
            tbb::parallel_for_each(m_listeners.begin(), m_listeners.end(), [&](auto& entry) {
                if (EventListener* listener = entry.ptr.load(std::memory_order_acquire)) {
                    func(listener);
                }
            });
        }
	};
}