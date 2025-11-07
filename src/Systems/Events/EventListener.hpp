#pragma once

#include "Systems/Events/EventData.hpp"

namespace GTS {

	class EventListener {
		public:
		EventListener() = default;
		virtual ~EventListener() = default;
		EventListener(EventListener const&) = delete;
		EventListener& operator=(EventListener const&) = delete;

		virtual std::string DebugName() = 0;

		virtual void Update();
		virtual void PapyrusUpdate();
		virtual void HavokUpdate();
		virtual void CameraUpdate();
		virtual void Reset();
		virtual void Enabled();
		virtual void Disabled();
		virtual void Start();
		virtual void DataReady();
		virtual void ResetActor(RE::Actor* actor);
		virtual void ActorEquip(RE::Actor* actor);
		virtual void DragonSoulAbsorption();
		virtual void ActorLoaded(RE::Actor* actor);
		virtual void HitEvent(const RE::TESHitEvent* evt);
		virtual void UnderFootEvent(const UnderFoot& evt);
		virtual void OnImpact(const Impact& impact);
		virtual void OnHighheelEquip(const HighheelEquip& evt);
		virtual void OnAddPerk(const AddPerkEvent& evt);
		virtual void OnRemovePerk(const RemovePerkEvent& evt);
		virtual void MenuChange(const RE::MenuOpenCloseEvent* menu_event);
		virtual void ActorAnimEvent(RE::Actor* actor, const std::string_view& tag, const std::string_view& payload);
		virtual void FurnitureEvent(RE::Actor* user, TESObjectREFR* object, bool enter);
		virtual void OnGameSave();
		virtual void OnGameLoaded();
		virtual void OnGameRevert();
		virtual void OnConfigReset();
		virtual void OnConfigRefresh();
		virtual void OnPluginPostLoad();
	};
}