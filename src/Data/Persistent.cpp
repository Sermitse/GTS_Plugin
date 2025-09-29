
#include "Data/Persistent.hpp"
#include "Config/Config.hpp"
#include "Managers/GtsSizeManager.hpp"

#include "UI/UIManager.hpp"
#include "UI/ImGUI/ImStyleManager.hpp"
#include "Utils/ItemDistributor.hpp"

namespace GTS {

	//-----------------
	// Persistent
	//-----------------

	void Persistent::OnGameLoaded(SerializationInterface* serde) {
		logger::info("Persistent OnGameLoaded Start");
		std::unique_lock lock(GetSingleton()._Lock);

		LoadPersistent(serde);

	#ifndef GTS_DISABLE_PLUGIN

		SizeManager::GetSingleton().Reset();
		DistributeChestItems();
		FixAnimationsAndCamera(); // Call it from ActorUtils, needed to fix Grab anim on save-reload
		LoadModLocalModConfiguration();

	#endif

		logger::info("Persistent OnGameLoaded OK");
	}

	void Persistent::OnGameSaved(SerializationInterface* serde) {
		logger::info("Persistent OnGameSaved Start");
		std::unique_lock lock(GetSingleton()._Lock);
		SavePersistent(serde);
		
		logger::info("Persistent OnGameSaved OK");
	}

	void Persistent::ClearData() {
		std::unique_lock lock(GetSingleton()._Lock);
		ActorDataMap.clear();
		KillCountDataMap.clear();

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

	void Persistent::LoadModLocalModConfiguration() {
		Config::LoadSettingsFromString();
		UIManager::WindowManager->ApplyStyle();
		spdlog::set_level(spdlog::level::from_str(Config::Advanced.sLogLevel));
	}

	void Persistent::LoadPersistent(SerializationInterface* serde) {

		std::uint32_t RecordType;
		std::uint32_t RecordSize;
		std::uint32_t RecordVersion;

		logger::info("De-Serializing Persistent...");

		while (serde->GetNextRecordInfo(RecordType, RecordVersion, RecordSize)) {

			//----- Actor Data Struct
			LoadActorData(serde, RecordType, RecordVersion);
			LoadKillCountData(serde, RecordType, RecordVersion);

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

			// ---- Save Baked Settings
			ModSettings.Load(serde, RecordType, RecordVersion, RecordSize);

		}

	}

	void Persistent::SavePersistent(SerializationInterface* serde) {

		logger::info("Serializing Persistent...");

		//----- Actor Data Struct
		WriteActorData(serde, ActorStructVersion);
		WriteKillCountData(serde, KillCountStructVersion);

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

		// ---- Save Baked Settings
		ModSettings.Save(serde);
	}
	
	//-----------------
	// ActorData Read
	//-----------------

	void Persistent::LoadActorData(SKSE::SerializationInterface* serde, const uint32_t RecordType, const uint32_t RecordVersion) {

		if (RecordType != ActorDataRecord) {
			return;
		}

		if (RecordVersion < 1) {
			return;
		}

		std::size_t RecordCount = 0;
		serde->ReadRecordData(&RecordCount, sizeof(RecordCount));

		for (; RecordCount > 0; --RecordCount) {
			ActorData Data = {};

			//V1
			//FormID Offset 0x00
			RE::FormID ReadFormID;       //FormID Stored in the Cosave;
			serde->ReadRecordData(&ReadFormID, sizeof(ReadFormID));

			//V1
			DummyRead32(serde);                                                             //0x04 - PAD
			LoadActorRecordFloat(serde, &Data.visual_scale, RecordVersion, 1, 1.0f);        //0x08
			LoadActorRecordFloat(serde, &Data.visual_scale_v, RecordVersion, 1, 0.0f);      //0x1C
			LoadActorRecordFloat(serde, &Data.target_scale, RecordVersion, 1, 1.0f);        //0x10
			LoadActorRecordFloat(serde, &Data.max_scale, RecordVersion, 1, 65535.0f);       //0x14

			//V2
			LoadActorRecordFloat(serde, &Data.half_life, RecordVersion, 2, 1.0f);           //0x18

			//V3
			LoadActorRecordFloat(serde, &Data.anim_speed, RecordVersion, 3, 1.0f);          //0x1C

			//V4
			DummyRead32(serde);                                                             //0x20 - PAD

			//V5
			DummyRead32(serde);                                                             //0x24 - PAD
			DummyRead32(serde);                                                             //0x28 - PAD
			DummyRead32(serde);                                                             //0x2C - PAD

			//V6
			LoadActorRecordFloat(serde, &Data.smt_run_speed, RecordVersion, 6, 0.0f);       //0x30
			LoadActorRecordFloat(serde, &Data.NormalDamage, RecordVersion, 6, 1.0f);        //0x34
			LoadActorRecordFloat(serde, &Data.SprintDamage, RecordVersion, 6, 1.0f);        //0x38
			LoadActorRecordFloat(serde, &Data.FallDamage, RecordVersion, 6, 1.0f);          //0x3C
			LoadActorRecordFloat(serde, &Data.HHDamage, RecordVersion, 6, 1.0f);            //0x40
			DummyRead32(serde);                                                             //0x44 - PAD
			DummyRead32(serde);                                                             //0x48 - PAD
			LoadActorRecordFloat(serde, &Data.SizeReserve, RecordVersion, 6, 0.0f);         //0x4C

			//V7
			LoadActorRecordFloat(serde, &Data.target_scale_v, RecordVersion, 7, 0.0f);      //0x50

			//V8
			LoadActorRecordBool(serde, &Data.ShowSizebarInUI, RecordVersion, 8, false);     //0x54
			LoadActorRecordU8(serde, &Data.MoanSoundDescriptorIndex, RecordVersion, 8,0);   //0x55
			DummyRead8(serde);                                                              //0x56 - PAD
			DummyRead8(serde);                                                              //0x57 - PAD
			LoadActorRecordFloat(serde, &Data.stolen_attributes, RecordVersion, 8, 0.0f);   //0x58
			LoadActorRecordFloat(serde, &Data.stolen_health, RecordVersion, 8, 0.0f);       //0x5C
			LoadActorRecordFloat(serde, &Data.stolen_magick, RecordVersion, 8, 0.0f);       //0x60
			LoadActorRecordFloat(serde, &Data.stolen_stamin, RecordVersion, 8, 0.0f);       //0x64

			//Do this last. If we continue early we'll have shifted the read pointer by 4 bytes for the next  read
			//Corrupting any future deserialization
			RE::FormID CorrectedFormID;  //Load order may have changed. This is the New FormID
			if (serde->ResolveFormID(ReadFormID, CorrectedFormID)) {

				log::trace("LoadActorData() Actor Persistent data loaded for FormID {:08X}", ReadFormID);

				if (Actor* ActorForm = TESForm::LookupByID<Actor>(CorrectedFormID)) {
					if (ActorForm) {
						ActorDataMap.insert_or_assign(CorrectedFormID, Data);
					}
				}
				else {
					log::warn("LoadActorData() Actor FormID {:08X} could not be found after loading the save.", CorrectedFormID);
					ActorDataMap.erase(CorrectedFormID);
				}
			}
			else {
				log::warn("LoadActorData() Actor FormID {:08X} could not be resolved. Not adding to ActorDataMap.", ReadFormID);
			}
		}
	}

	void Persistent::LoadActorRecordFloat(SKSE::SerializationInterface* serde, float* a_Data, uint32_t RecordVersion, uint32_t MinVersion, float DefaultValue) {

		if (RecordVersion >= MinVersion) {
			serde->ReadRecordData(a_Data, sizeof(float));
		}
		else {
			*a_Data = DefaultValue;
		}

		if (std::isnan(*a_Data)) {
			*a_Data = DefaultValue;
		}

	}

	void Persistent::LoadActorRecordBool(SKSE::SerializationInterface* serde, bool* a_Data, uint32_t RecordVersion, uint32_t MinVersion, bool DefaultValue) {

		if (RecordVersion >= MinVersion) {
			serde->ReadRecordData(a_Data, sizeof(bool));
		}
		else {
			*a_Data = DefaultValue;
		}
	}

	void Persistent::LoadActorRecordU8(SKSE::SerializationInterface* serde, uint8_t* a_Data, uint32_t RecordVersion, uint32_t MinVersion, uint8_t DefaultValue) {

		if (RecordVersion >= MinVersion) {
			serde->ReadRecordData(a_Data, sizeof(uint8_t));
		}
		else {
			*a_Data = DefaultValue;
		}
	}

	void Persistent::DummyRead32(SKSE::SerializationInterface* serde) {
		uint32_t Dummy;
		serde->ReadRecordData(&Dummy, sizeof(uint32_t));
	}


	void Persistent::DummyRead8(SKSE::SerializationInterface* serde) {
		uint8_t Dummy;
		serde->ReadRecordData(&Dummy, sizeof(uint8_t));
	}

	//-----------------
	// ActorData Write
	//-----------------

	void Persistent::WriteActorData(SKSE::SerializationInterface* serde, const uint8_t Version) {

		const size_t NumOfActorRecords = ActorDataMap.size();
		
		if (!serde->OpenRecord(ActorDataRecord, Version)) {
			log::critical("Unable to open ActorDataRecord in CoSave. Something is really wrong, your save is probably broken!");
			return;
		}

		serde->WriteRecordData(&NumOfActorRecords, sizeof(NumOfActorRecords));

		for (auto const& [ActorFormID, Data] : ActorDataMap) {

			//V1
			WriteActorRecordFormID(serde, &ActorFormID);                    //0x00 - FORMID
			DummyWrite32(serde);                                            //0x04 - PAD
			WriteActorRecordFloat(serde, &Data.visual_scale);               //0x08
			WriteActorRecordFloat(serde, &Data.visual_scale_v);             //0x1C
			WriteActorRecordFloat(serde, &Data.target_scale);               //0x10
			WriteActorRecordFloat(serde, &Data.max_scale);                  //0x14

			//V2
			WriteActorRecordFloat(serde, &Data.half_life);                  //0x18

			//V3
			WriteActorRecordFloat(serde, &Data.anim_speed);                 //0x1C

			//V4
			DummyWrite32(serde);                                            //0x20 - PAD

			//V5
			DummyWrite32(serde);                                            //0x24 - PAD
			DummyWrite32(serde);                                            //0x28 - PAD
			DummyWrite32(serde);                                            //0x2C - PAD

			//V6
			WriteActorRecordFloat(serde, &Data.smt_run_speed);              //0x30
			WriteActorRecordFloat(serde, &Data.NormalDamage);               //0x34
			WriteActorRecordFloat(serde, &Data.SprintDamage);               //0x38
			WriteActorRecordFloat(serde, &Data.FallDamage);                 //0x3C
			WriteActorRecordFloat(serde, &Data.HHDamage);                   //0x40
			DummyWrite32(serde);                                            //0x44 - PAD
			DummyWrite32(serde);                                            //0x48 - PAD
			WriteActorRecordFloat(serde, &Data.SizeReserve);                //0x4C

			//V7
			WriteActorRecordFloat(serde, &Data.target_scale_v);             //0x50

			//V8
			WriteActorRecordBool(serde, &Data.ShowSizebarInUI);             //0x54
			WriteActorRecordU8(serde, &Data.MoanSoundDescriptorIndex);      //0x55
			DummyWrite8(serde);                                             //0x56 - PAD
			DummyWrite8(serde);                                             //0x57 - PAD
			WriteActorRecordFloat(serde, &Data.stolen_attributes);          //0x58
			WriteActorRecordFloat(serde, &Data.stolen_health);              //0x5C
			WriteActorRecordFloat(serde, &Data.stolen_magick);              //0x60
			WriteActorRecordFloat(serde, &Data.stolen_stamin);              //0x64

			log::trace("Persistent data serialized for Actor FormID {:08X}", ActorFormID);
		}
	}

	void Persistent::WriteActorRecordFloat(SKSE::SerializationInterface* serde, const float* Data) {
		serde->WriteRecordData(Data, sizeof(float));
	}

	void Persistent::WriteActorRecordBool(SKSE::SerializationInterface* serde, const bool* Data) {
		serde->WriteRecordData(Data, sizeof(bool));
	}

	void Persistent::WriteActorRecordU8(SKSE::SerializationInterface* serde, const uint8_t* Data) {
		serde->WriteRecordData(Data, sizeof(uint8_t));
	}

	void Persistent::WriteActorRecordFormID(SKSE::SerializationInterface* serde, const FormID* Id) {
		serde->WriteRecordData(Id, sizeof(FormID));
	}

	void Persistent::DummyWrite32(SKSE::SerializationInterface* serde) {
		constexpr uint32_t Dummy = 0;
		serde->WriteRecordData(&Dummy, sizeof(uint32_t));
	}

	void Persistent::DummyWrite8(SKSE::SerializationInterface* serde) {
		constexpr uint8_t Dummy = 0;
		serde->WriteRecordData(&Dummy, sizeof(uint8_t));
	}

	//-----------------
	// ActorData Other
	//-----------------

	ActorData* Persistent::GetActorData(Actor* actor) {
		if (!actor) {
			return nullptr;
		}
		return this->GetActorData(*actor);
	}

	ActorData* Persistent::GetActorData(Actor& actor) {
		std::unique_lock lock(this->_Lock);
		auto key = actor.formID;

		// Lambda to add new ActorData if conditions are met
		auto addActorData = [&]() -> ActorData* {
			if (!actor.Is3DLoaded()) {
				return nullptr;
			}
			if (get_scale(&actor) < 0.0f) {
				return nullptr;
			}
			auto [iter, inserted] = this->ActorDataMap.try_emplace(key, &actor);
			return &(iter->second);
		};

		// Attempt to find the actor's data in the map
		auto it = this->ActorDataMap.find(key);
		if (it != this->ActorDataMap.end()) {
			return &(it->second);
		}

		// ActorData not found; attempt to add it
		return addActorData();
		
	}

	ActorData* Persistent::GetData(TESObjectREFR* refr) {
		if (!refr) {
			return nullptr;
		}
		return this->GetData(*refr);
	}

	ActorData* Persistent::GetData(TESObjectREFR& refr) {
		auto key = refr.formID;
		try {
			if (!this->ActorDataMap.contains(key)) {
				return nullptr;
			}
			return &this->ActorDataMap.at(key);
		}
		catch (const std::out_of_range&) {
			return nullptr;
		}
		
	}

	void Persistent::Reset(){

		ClearData();

		// Ensure we reset them back to inital scales
		// if they are loaded into game memory
		// since skyrim only lazy loads actors
		// that are already in memory it won't reload
		// their nif scales otherwise
		for (auto actor : find_actors()) {
			ResetToInitScale(actor);
		}

		logger::info("Persistent: Reset Event");

	}

	void Persistent::ResetActor(Actor* actor) {
		// Fired after a TESReset event
		// This event should be when the game attempts to reset their
		// actor values etc when the cell resets
		auto data = this->GetData(actor);
		if (data) {
			data->PAD_00 = 0.0f;
			data->visual_scale = 1.0f; 
			data->target_scale = 1.0f; 
			data->max_scale = 65535.0f;
			data->visual_scale_v = 0.0f;
			data->half_life = 1.0f;
			data->anim_speed = 1.0f;
			data->PAD_1C = 0.0f;
			data->PAD_20 = 0.0f;
			data->PAD_24 = 0.0f;
			data->PAD_28 = 0.0f;
			data->smt_run_speed = 0.0f;
			data->NormalDamage = 1.0f;
			data->SprintDamage = 1.0f;
			data->FallDamage = 1.0f;
			data->HHDamage = 1.0f;
			data->PAD_40 = 0.0f;
			data->PAD_44 = 0.0f;
			data->SizeReserve = 0.0f;
			data->ShowSizebarInUI = false;
			data->MoanSoundDescriptorIndex = 0;
			data->PAD_52 = false;
			data->PAD_53 = false;
			data->stolen_attributes = 0.0f;
			data->stolen_health = 0.0f;
			data->stolen_magick = 0.0f;
			data->stolen_stamin = 0.0f;
		}
		ResetToInitScale(actor);
	}

	// The Revert Callback Fires when we exit to the main menu and when a game is loaded.
	void Persistent::OnRevert(SerializationInterface*) {
	#ifndef GTS_DISABLE_PLUGIN
		logger::info("Persistent::OnRevert");
		Persistent::GetSingleton().Reset();
		Transient::GetSingleton().Reset();
	#endif
	
	}

	void Persistent::EraseUnloadedPersistentData() const {
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
		for (auto it = ActorDataMap.begin(); it != ActorDataMap.end(); ) {
			if (!allowedFormIDs.contains(it->first)) {
				it = ActorDataMap.erase(it);  // erase returns the next iterator.
			}
			else {
				++it;
			}
		}

		logger::critical("All Unloaded actors have beeen purged from persistent.");

	}

	//----------------------
	// KillCountData Read
	//----------------------

	void Persistent::LoadKillCountData(SKSE::SerializationInterface* serde, const uint32_t RecordType, const uint32_t RecordVersion) {

		if (RecordType != KillCountDataRecord) {
			return;
		}

		if (RecordVersion != KillCountStructVersion) {
			return;
		}

		size_t RecordCount = 0;
		uint32_t RecordSize = 0;
		serde->ReadRecordData(&RecordCount, sizeof(size_t));
		serde->ReadRecordData(&RecordSize, sizeof(uint32_t));

		//Killcount data must be as big or larger than the value stored in the cosave otherwise we'll write out of struct bounds and corrupt adjacent memory
		if (sizeof(KillCountData) < RecordSize || RecordSize == 0) {
			ReportInfo("KillCountData structure size missmatch, proceeding will corrupt memory. Kill Count Data Will not be loaded from save.");
			return;
		}

		for (; RecordCount > 0; --RecordCount) {

			KillCountData Data = {};
			RE::FormID ReadFormID;

			serde->ReadRecordData(&ReadFormID, sizeof(FormID));        //FormID Offset 0x00 (Size 4)
			serde->ReadRecordData(&Data, RecordSize);                  //Struct Offset 0x04 (Size 76 As of V1)

			//Do this last. If we continue early we'll have shifted the read pointer by 4 bytes for the next  read
			//Corrupting any future deserialization
			RE::FormID CorrectedFormID;  //Load order may have changed. This is the New FormID
			if (serde->ResolveFormID(ReadFormID, CorrectedFormID)) {

				log::trace("LoadKillCountData() Actor KillCountData loaded for FormID {:08X}", ReadFormID);

				if (Actor* ActorForm = TESForm::LookupByID<Actor>(CorrectedFormID)) {
					if (ActorForm) {
						KillCountDataMap.insert_or_assign(CorrectedFormID, Data);
					}
				}
				else {
					log::warn("LoadKillCountData() Actor FormID {:08X} could not be found after loading the save.", CorrectedFormID);
					KillCountDataMap.erase(CorrectedFormID);
				}
			}
			else {
				log::warn("LoadKillCountData() Actor FormID {:08X} could not be resolved. Not adding to KillCountData.", ReadFormID);
			}
		}
	}

	//----------------------
	// KillCountData Write
	//----------------------

	void Persistent::WriteKillCountData(SKSE::SerializationInterface* serde, const uint8_t Version) {

		const size_t NumOfActorRecords = KillCountDataMap.size();
		constexpr uint32_t SizeOfStructDataToWrite = sizeof(KillCountData);

		if (!serde->OpenRecord(KillCountDataRecord, Version)) {
			log::critical("Unable to open KillCountDataRecord in CoSave. Something is really wrong, your save is probably broken!");
			return;
		}

		serde->WriteRecordData(&NumOfActorRecords, sizeof(size_t));         //0x00 - NumOfRecords
		serde->WriteRecordData(&SizeOfStructDataToWrite, sizeof(uint32_t)); //0x08 - Struct Size

		for (auto const& [ActorFormID, Data] : KillCountDataMap) {

			//V1
			WriteActorRecordFormID(serde, &ActorFormID);                    //0x00 - FORMID
			serde->WriteRecordData(&Data, SizeOfStructDataToWrite);         //0x04 - Struct

			log::trace("Persistent KillCountData serialized for Actor FormID {:08X}", ActorFormID);
		}
	}

	//----------------------
	// KillCountData Other
	//----------------------

	KillCountData* Persistent::GetKillCountData(Actor& actor) {
		std::unique_lock lock(this->_Lock);
		auto key = actor.formID;
		auto it = this->KillCountDataMap.find(key);

		if (it != this->KillCountDataMap.end()) {
			return &it->second;
		}

		// Key not found, add new entry
		if (!actor.Is3DLoaded()) {
			return nullptr;
		}
		auto [newIt, inserted] = this->KillCountDataMap.try_emplace(key, &actor);
		return &newIt->second;
	}

	KillCountData* Persistent::GetKillCountData(Actor* actor) {
		if (!actor) {
			return nullptr;
		}
		return this->GetKillCountData(*actor);
	}
}
