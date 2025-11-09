#pragma once

#include "Data/Storage/Util/BasicRecord.hpp"
#include "Data/Storage/Util/CompressedRecord.hpp"
#include "Data/Storage/Util/MapRecord.hpp"

#include "Data/Storage/PersistentData.hpp"

namespace GTS {

	class Persistent : public EventListener, public CInitSingleton<Persistent> {

		public:
		virtual void Reset() override;
		virtual void ResetActor(Actor* actor) override;
		virtual std::string DebugName() override;

		//SKSE Callbacks
		static void OnRevert(SerializationInterface*);
		static void OnGameSaved(SKSE::SerializationInterface* serde);
		static void OnGameLoaded(SKSE::SerializationInterface* serde);

		static void EraseUnloadedData();

		static PersistentKillCountData* GetKillCountData(Actor& actor);
		static PersistentKillCountData* GetKillCountData(Actor* actor);

		static PersistentActorData* GetActorData(Actor& actor);
		static PersistentActorData* GetActorData(Actor* actor);

		//----- Actor Data
		static inline Serialization::MapRecord<PersistentActorData, 'ACT_'> ActorMap = {};
		static inline Serialization::MapRecord<PersistentKillCountData, 'ACTK'> KillCountMap = {};

		//----- Camera
		static inline Serialization::BasicRecord<int, 'TCST'> TrackedCameraState = 0;

		//----- Crawl/Sneak State
		static inline Serialization::BasicRecord<bool, 'ECPL'> EnableCrawlPlayer = false;
		static inline Serialization::BasicRecord<bool, 'ECFL'> EnableCrawlFollower = false;

		//----- Max Size Related
		static inline Serialization::BasicRecord<float, 'GBPS'> PlayerExtraPotionSize = 0.0f;
		static inline Serialization::BasicRecord<float, 'GTSL'> GlobalSizeLimit = 1.0f;
		static inline Serialization::BasicRecord<float, 'GMBS'> GlobalMassBasedSizeLimit = 0.0f;

		// ---- Quest Progression
		static inline Serialization::BasicRecord<float, 'QHSR'> HugStealCount = 0.0f;
		static inline Serialization::BasicRecord<float, 'QSSR'> StolenSize = 0.0f;
		static inline Serialization::BasicRecord<float, 'QCCR'> CrushCount = 0.0f;
		static inline Serialization::BasicRecord<float, 'QSTR'> STNCount = 0.0f;
		static inline Serialization::BasicRecord<float, 'QHCR'> HandCrushed = 0.0f;
		static inline Serialization::BasicRecord<float, 'QVRR'> VoreCount = 0.0f;
		static inline Serialization::BasicRecord<float, 'QGCR'> GiantCount = 0.0f;

		// ---- Guide Messages Seen
		static inline Serialization::BasicRecord<bool, 'MSTC'> MSGSeenTinyCamity = false;
		static inline Serialization::BasicRecord<bool, 'MSGS'> MSGSeenGrowthSpurt = false;
		static inline Serialization::BasicRecord<bool, 'MSAG'> MSGSeenAspectOfGTS = false;

		// ---- Unlimited Size slider unlocker
		static inline Serialization::BasicRecord<bool, 'USSD'> UnlockMaxSizeSliders = false;

		static inline Serialization::CompressedStringRecord<'CONF'> ModSettings{""};

		private:
		static inline std::mutex _Lock;

		static void ClearData();

		static void LoadPersistent(SerializationInterface* serde);
		static void SavePersistent(SerializationInterface* serde);

	};
}
