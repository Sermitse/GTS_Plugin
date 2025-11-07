#include "Data/Storage/Persistent.hpp"

namespace GTS {

	//----------------------
	// Overriden Virtuals
	//----------------------

	std::string Persistent::DebugName() {
		return "::Persistent";
	}

	void Persistent::Reset() {

		ClearData();

		// Ensure we reset them back to inital scales
		// if they are loaded into game memory
		// since skyrim only lazy loads actors
		// that are already in memory it won't reload
		// their nif scales otherwise
		for (const auto& actor : find_actors()) {
			ResetToInitScale(actor);
		}

		logger::info("Persistent: Reset Event");

	}

	void Persistent::ResetActor(Actor* actor) {
		// Fired after a TESReset event
		// This event should be when the game attempts to reset their
		// actor values etc when the cell resets
		auto key = actor->formID;

		auto it = this->ActorMap.value.find(key);
		if (it != this->ActorMap.value.end()) {
			it->second = {};
			ResetToInitScale(actor);
		}
	}

	//----------------------
	// SKSE Callbacks
	//----------------------

	void Persistent::OnGameLoaded(SerializationInterface* serde) {

		logger::debug("Persistent OnGameLoaded Start");

		{
			std::unique_lock lock(_Lock);
			LoadPersistent(serde);
		}
	
		logger::info("Persistent OnGameLoaded OK");

		EventDispatcher::DoSerdePostLoadEvent();

		//TODO Move this out of here when actorutils gets rewritten
		#ifndef GTS_DISABLE_PLUGIN
		FixAnimationsAndCamera(); // Call it from ActorUtils, needed to fix Grab anim on save-reload
		#endif

	}

	void Persistent::OnGameSaved(SerializationInterface* serde) {

		EventDispatcher::DoSerdePreSaveEvent();

		logger::debug("Persistent OnGameSaved Start");

		std::unique_lock lock(_Lock);
		SavePersistent(serde);

		logger::info("Persistent OnGameSaved OK");
	}

	void Persistent::OnRevert(SerializationInterface*) {

		{
			#ifndef GTS_DISABLE_PLUGIN

			std::unique_lock lock(_Lock);
			logger::info("Persistent::OnRevert");
			GetSingleton().Reset();

			#endif
		}

		EventDispatcher::DoSerdeRevert();

	}

	//---------------------------
	// Save/Load Implementations
	//---------------------------

	void Persistent::LoadPersistent(SerializationInterface* serde) {
		std::uint32_t RecordType;
		std::uint32_t RecordSize;
		std::uint32_t RecordVersion;

		logger::info("De-Serializing Persistent...");

		while (serde->GetNextRecordInfo(RecordType, RecordVersion, RecordSize)) {

			//----- ActorData
			ActorMap.Load(serde, RecordType, RecordVersion, RecordSize);
			KillCountMap.Load(serde, RecordType, RecordVersion, RecordSize);

			//----- Camera
			TrackedCameraState.Load(serde, RecordType, RecordVersion, RecordSize);

			//----- Crawk/Sneak State
			EnableCrawlPlayer.Load(serde, RecordType, RecordVersion, RecordSize);
			EnableCrawlFollower.Load(serde, RecordType, RecordVersion, RecordSize);

			//----- Max Size Related
			GlobalMassBasedSizeLimit.Load(serde, RecordType, RecordVersion, RecordSize);
			PlayerExtraPotionSize.Load(serde, RecordType, RecordVersion, RecordSize);
			GlobalSizeLimit.Load(serde, RecordType, RecordVersion, RecordSize);

			// ---- Quest Progression
			HugStealCount.Load(serde, RecordType, RecordVersion, RecordSize);
			StolenSize.Load(serde, RecordType, RecordVersion, RecordSize);
			CrushCount.Load(serde, RecordType, RecordVersion, RecordSize);
			STNCount.Load(serde, RecordType, RecordVersion, RecordSize);
			HandCrushed.Load(serde, RecordType, RecordVersion, RecordSize);
			VoreCount.Load(serde, RecordType, RecordVersion, RecordSize);
			GiantCount.Load(serde, RecordType, RecordVersion, RecordSize);

			// ---- Ability Info
			MSGSeenTinyCamity.Load(serde, RecordType, RecordVersion, RecordSize);
			MSGSeenGrowthSpurt.Load(serde, RecordType, RecordVersion, RecordSize);
			MSGSeenAspectOfGTS.Load(serde, RecordType, RecordVersion, RecordSize);

			// ---- Unlimited Size slider unlocker
			UnlockMaxSizeSliders.Load(serde, RecordType, RecordVersion, RecordSize);

			// ---- Mod Settings
			ModSettings.Load(serde, RecordType, RecordVersion, RecordSize);

		}
	}

	void Persistent::SavePersistent(SerializationInterface* serde) {

		logger::info("Serializing Persistent...");

		//----- Actor Data Struct
		ActorMap.Save(serde);
		KillCountMap.Save(serde);

		//----- Camera
		TrackedCameraState.Save(serde);

		//----- Crawk/Sneak State
		EnableCrawlPlayer.Save(serde);
		EnableCrawlFollower.Save(serde);

		//----- Max Size Related
		GlobalMassBasedSizeLimit.Save(serde);
		PlayerExtraPotionSize.Save(serde);
		GlobalSizeLimit.Save(serde);

		// ---- Quest Progression
		HugStealCount.Save(serde);
		StolenSize.Save(serde);
		CrushCount.Save(serde);
		STNCount.Save(serde);
		HandCrushed.Save(serde);
		VoreCount.Save(serde);
		GiantCount.Save(serde);

		// ---- Ability Info
		MSGSeenTinyCamity.Save(serde);
		MSGSeenGrowthSpurt.Save(serde);
		MSGSeenAspectOfGTS.Save(serde);

		// ---- Unlimited Size slider unlocker
		UnlockMaxSizeSliders.Save(serde);

		// ---- Mod Settings
		ModSettings.Save(serde);
	}

	//---------------------------
	// Getters
	//---------------------------

	PersistentActorData* Persistent::GetActorData(Actor* actor) {
		if (!actor) {
			return nullptr;
		}
		return GetActorData(*actor);
	}

	PersistentActorData* Persistent::GetActorData(Actor& actor) {
		std::unique_lock lock(_Lock);
		auto key = actor.formID;

		// Lambda to add new ActorData if conditions are met
		auto addActorData = [&]() -> PersistentActorData* {
			if (!actor.Is3DLoaded()) {
				return nullptr;
			}
			if (get_scale(&actor) < 0.0f) {
				return nullptr;
			}
			auto [iter, inserted] = ActorMap.value.try_emplace(key);
			return &(iter->second);
		};

		// Attempt to find the actor's data in the map
		auto it = ActorMap.value.find(key);
		if (it != ActorMap.value.end()) {
			return &(it->second);
		}

		// ActorData not found; attempt to add it
		return addActorData();
		
	}

	PersistentKillCountData* Persistent::GetKillCountData(Actor* actor) {
		if (!actor) {
			return nullptr;
		}
		return GetKillCountData(*actor);
	}

	PersistentKillCountData* Persistent::GetKillCountData(Actor& actor) {
		std::unique_lock lock(_Lock);
		auto key = actor.formID;
		auto it = KillCountMap.value.find(key);

		if (it != KillCountMap.value.end()) {
			return &it->second;
		}

		// Key not found, add new entry
		if (!actor.Is3DLoaded()) {
			return nullptr;
		}
		auto [newIt, inserted] = KillCountMap.value.try_emplace(key);
		return &newIt->second;
	}

	//---------------------------
	// Data Management
	//---------------------------

	void Persistent::ClearData() {

		ActorMap.value.clear();
		KillCountMap.value.clear();

		TrackedCameraState = 0;
		EnableCrawlPlayer = false;
		EnableCrawlFollower = false;
		PlayerExtraPotionSize = 0.0f;
		GlobalSizeLimit = 1.0f;
		GlobalMassBasedSizeLimit = 0.0f;
		HugStealCount = 0.0f;
		StolenSize = 0.0f;
		CrushCount = 0.0f;
		STNCount = 0.0f;
		HandCrushed = 0.0f;
		VoreCount = 0.0f;
		GiantCount = 0.0f;
		MSGSeenTinyCamity = false;
		MSGSeenGrowthSpurt = false;
		MSGSeenAspectOfGTS = false;
		UnlockMaxSizeSliders = false;

	}

	void Persistent::EraseUnloadedData() {
		std::unique_lock lock(_Lock);
		// Create a set to hold the whitelisted FormIDs.
		std::unordered_set<FormID> allowedFormIDs;

		// Always keep FormID 0x14 (Player).
		allowedFormIDs.insert(0x14);

		// Get preserve all currently loaded actors
		for (const Actor* ActorToNotDelete : find_actors()) {
			if (ActorToNotDelete) {
				allowedFormIDs.insert(ActorToNotDelete->formID);
			}
		}

		// Iterate through ActorDataMap and remove entries whose key is not in allowedFormIDs.
		for (auto it = ActorMap.value.begin(); it != ActorMap.value.end(); ) {
			if (!allowedFormIDs.contains(it->first)) {
				it = ActorMap.value.erase(it);  // erase returns the next iterator.
			}
			else {
				++it;
			}
		}

		// Iterate through KillCountMap and remove entries whose key is not in allowedFormIDs.
		for (auto it = KillCountMap.value.begin(); it != KillCountMap.value.end(); ) {
			if (!allowedFormIDs.contains(it->first)) {
				it = KillCountMap.value.erase(it);  // erase returns the next iterator.
			}
			else {
				++it;
			}
		}

		logger::critical("All Unloaded actors have beeen purged from persistent.");

	}

}
