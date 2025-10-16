#pragma once

#include "Systems/Events/EventData.hpp"

namespace GTS {

	class EventListener {
		public:
		EventListener() = default;
		virtual ~EventListener() = default;
		EventListener(EventListener const&) = delete;
		EventListener& operator=(EventListener const&) = delete;

		// Get name used for debug prints
		virtual std::string DebugName() = 0;

		// Called on Live (non paused) gameplay
		virtual void Update();

		// Called on Papyrus OnUpdate
		virtual void PapyrusUpdate();

		// Called on Havok update (when processing hitjobs)
		virtual void HavokUpdate();

		// Called when the camera update event is fired (in the TESCameraState)
		virtual void CameraUpdate();

		// Called on game load started (not yet finished)
		// and when new game is selected
		virtual void Reset();

		// Called when game is enabled (while not paused)
		virtual void Enabled();

		// Called when game is disabled (while not paused)
		virtual void Disabled();

		// Called when a game is started after a load/newgame
		virtual void Start();

		// Called when all forms are loaded (during game load before mainmenu)
		virtual void DataReady();

		// Called when an actor is reset
		virtual void ResetActor(RE::Actor* actor);

		// Called when an actor has an item equipped
		virtual void ActorEquip(RE::Actor* actor);

		// Called when Player absorbs dragon soul
		virtual void DragonSoulAbsorption();

		// Called when an actor has is fully loaded
		virtual void ActorLoaded(RE::Actor* actor);

		// Called when a papyrus hit event is fired
		virtual void HitEvent(const RE::TESHitEvent* evt);

		// Called when an actor is squashed underfoot
		virtual void UnderFootEvent(const UnderFoot& evt);

		// Fired when a foot lands
		virtual void OnImpact(const Impact& impact);

		// Fired when a highheel is (un)equiped or when an actor is loaded with HH
		virtual void OnHighheelEquip(const HighheelEquip& evt);

		// Fired when a perk is added
		virtual void OnAddPerk(const AddPerkEvent& evt);

		// Fired when a perk about to be removed
		virtual void OnRemovePerk(const RemovePerkEvent& evt);

		// Fired when a skyrim menu event occurs
		virtual void MenuChange(const RE::MenuOpenCloseEvent* menu_event);

		// Fired when a actor animation event occurs
		virtual void ActorAnimEvent(RE::Actor* actor, const std::string_view& tag, const std::string_view& payload);

		// Fired when actor uses furniture
		virtual void FurnitureEvent(RE::Actor* user, TESObjectREFR* object, bool enter);

		// Fired during skse cosave save start
		virtual void OnGameSave();

		// Fired during skse cosave load end
		virtual void OnGameLoaded();

		// Fired during config reset
		virtual void OnConfigReset();

		// Fired during config import
		virtual void OnConfigRefresh();

		// Fired at the SKSE kPostLoad Message
		virtual void OnPluginPostLoad();
	};
}