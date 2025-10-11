#pragma once
#include "Data/BasicRecord.hpp"
#include "Data/CompressedRecord.hpp"

// Module that holds data that is persistent across saves

namespace GTS {

	//AS OF Version 8 actor data is 100 bytes (0x64) + 4 to store the formid
	//Each actordata cosave entry is thus 104 bytes.

	#pragma pack(push, 1)

	struct ActorData {

		/// --------- V1
		float PAD_00 = 0.0f; 
		float visual_scale = 1.0f;
		float visual_scale_v = 0.0f;
		float target_scale = 1.0f;
		float max_scale = 65535.0f;

		/// --------- V2
		float half_life = 1.0f;

		/// --------- V3
		float anim_speed = 1.0f;

		/// --------- V4
		float PAD_1C = 0.0f;

		/// --------- V5
		float PAD_20 = 0.0f;
		float PAD_24 = 0.0f;
		float PAD_28 = 0.0f;

		/// --------- V6
		float smt_run_speed = 0.0f;
		float NormalDamage = 1.0f;
		float SprintDamage = 1.0f;
		float FallDamage = 1.0f;
		float HHDamage = 1.0f;
		float PAD_40 = 0.0f;
		float PAD_44 = 0.0f;
		float SizeReserve = 0.0f;

		/// --------- V7
		float target_scale_v = 0.0f;

		/// --------- V8
		bool ShowSizebarInUI = false;
		uint8_t MoanSoundDescriptorIndex = 0;
		bool PAD_52 = false;
		bool PAD_53 = false;
		float stolen_attributes = 0.0f;
		float stolen_health = 0.0f;
		float stolen_magick = 0.0f;
		float stolen_stamin = 0.0f;

		/// --------- V9
		//Add New Stuff Here if needed / PAD data is all used up.
		//Must Be Read and written in the same order as found here

		ActorData() = default;
		explicit ActorData(RE::Actor* actor) {}
	};

	#pragma pack(pop)

	static_assert(sizeof(ActorData) == 0x64);

	#pragma pack(push, 1)

	struct KillCountData {
		// Once done, order of data CANNOT be changed
		// But we can expand the data by adding new data at the end of current

		uint32_t iTotalKills = 0; // Total kill count when we don't expand all the info

		uint32_t iShrunkToNothing = 0; // Mostly with spells
		uint32_t iOtherSources = 0;
		//^  Colliding with someone with Tiny Calamity (leads to exploding the tiny) 
		// or inflicting too much damage with weapons when size difference is gigantic (also explodes tiny)

		//Breast data
		uint32_t iBreastAbsorbed = 0;
		uint32_t iBreastCrushed = 0;
		uint32_t iBreastSuffocated = 0;
		// Hug Data
		uint32_t iHugCrushed = 0;
		// Grab Data
		uint32_t iGrabCrushed = 0;
		// Butt Crush Data
		uint32_t iButtCrushed = 0;
		//Thigh Sandwich/Crush
		uint32_t iThighCrushed = 0;
		uint32_t iThighSuffocated = 0; // When dying from DOT damage under thighs
		uint32_t iThighSandwiched = 0; // When dying from Thigh Sandwich
		uint32_t iThighGrinded = 0; // We with Nick plan Thigh Grind anim, so it may be used later

		uint32_t iFingerCrushed = 0;

		uint32_t iErasedFromExistence = 0; // Wrathful Calamity Finisher

		uint32_t iAbsorbed = 0; // Unused for now, may be useful later
		uint32_t iCrushed = 0; // Used in most crush sources
		uint32_t iEaten = 0; // When fully voring someone

		uint32_t iKicked = 0; // Kicked and crushed to death at same time
		uint32_t iGrinded = 0; // Grinded to death

		KillCountData() = default;
		explicit KillCountData(RE::Actor* actor) {}
	};

	#pragma pack(pop)

	static_assert(sizeof(KillCountData) == 0x50);

	class Persistent : public EventListener {

		public:

			virtual void Reset() override;

			virtual void ResetActor(Actor* actor) override;

			virtual std::string DebugName() override {
				return "::Persistent";
			}

			static void OnRevert(SerializationInterface*);

			[[nodiscard]] inline static Persistent& GetSingleton() noexcept {
				static Persistent Instance;
				return Instance;
			}

			//SKSE Callbacks
			static void OnGameSaved(SKSE::SerializationInterface* serde);
			static void OnGameLoaded(SKSE::SerializationInterface* serde);

			void EraseUnloadedPersistentData() const;

			KillCountData* GetKillCountData(Actor& actor);
			KillCountData* GetKillCountData(Actor* actor);

			ActorData* GetActorData(Actor& actor);
			ActorData* GetActorData(Actor* actor);
			ActorData* GetData(TESObjectREFR* refr);
			ActorData* GetData(TESObjectREFR& refr);

			//bool -> 1 byte
			//int -> 4 bytes
			//float -> 4 bytes
			//double -> 8 bytes

			//------ Actor Record Struct
			constexpr static inline uint8_t ActorStructVersion = 8;
			const static inline uint32_t ActorDataRecord = _byteswap_ulong('ACTD');

			//------ Actor Record Struct
			constexpr static inline uint8_t KillCountStructVersion = 1;
			const static inline uint32_t KillCountDataRecord = _byteswap_ulong('AKCD');

			//----- Camera
			static inline BasicRecord<int, 'TCST'> TrackedCameraState = 0;

			//----- Crawl/Sneak State
			static inline BasicRecord<bool, 'ECPL'> EnableCrawlPlayer = false;
			static inline BasicRecord<bool, 'ECFL'> EnableCrawlFollower = false;

			//----- Max Size Related
			static inline BasicRecord<float, 'GBPS'> PlayerExtraPotionSize = 0.0f;
			static inline BasicRecord<float, 'GTSL'> GlobalSizeLimit = 1.0f;
			static inline BasicRecord<float, 'GMBS'> GlobalMassBasedSizeLimit = 0.0f;

			// ---- Quest Progression
			static inline BasicRecord<float, 'QHSR'> HugStealCount = 0.0f;
			static inline BasicRecord<float, 'QSSR'> StolenSize = 0.0f;
			static inline BasicRecord<float, 'QCCR'> CrushCount = 0.0f;
			static inline BasicRecord<float, 'QSTR'> STNCount = 0.0f;
			static inline BasicRecord<float, 'QHCR'> HandCrushed = 0.0f;
			static inline BasicRecord<float, 'QVRR'> VoreCount = 0.0f;
			static inline BasicRecord<float, 'QGCR'> GiantCount = 0.0f;

			// ---- Guide Messages Seen
			static inline BasicRecord<bool, 'MSTC'> MSGSeenTinyCamity = false;
			static inline BasicRecord<bool, 'MSGS'> MSGSeenGrowthSpurt = false;
			static inline BasicRecord<bool, 'MSAG'> MSGSeenAspectOfGTS = false;

			// ---- Unlimited Size slider unlocker
			static inline BasicRecord<bool, 'USSD'> UnlockMaxSizeSliders = false;

			static inline CompressedStringRecord<'CONF'> ModSettings{""};

		private:

			Persistent() = default;
			mutable std::mutex _Lock;

			static inline std::unordered_map<FormID, ActorData> ActorDataMap;
			static inline std::unordered_map<FormID, KillCountData> KillCountDataMap;

			static void ClearData();

			static void LoadPersistent(SerializationInterface* serde);
			static void SavePersistent(SerializationInterface* serde);
			static void LoadKillCountData(SKSE::SerializationInterface* serde, uint32_t RecordType, uint32_t RecordVersion);

			//Actor Data Load
			static void LoadActorData(SKSE::SerializationInterface* serde, const uint32_t RecordType, const uint32_t RecordVersion);
			static void LoadActorRecordFloat(SKSE::SerializationInterface* serde, float* a_Data, uint32_t RecordVersion, uint32_t MinVersion, float DefaultValue);
			static void LoadActorRecordBool(SKSE::SerializationInterface* serde, bool* a_Data, uint32_t RecordVersion, uint32_t MinVersion, bool DefaultValue);
			static void LoadActorRecordU8(SKSE::SerializationInterface* serde, uint8_t* a_Data, uint32_t RecordVersion, uint32_t MinVersion, uint8_t DefaultValue);
			static void DummyRead32(SKSE::SerializationInterface* serde);
			static void DummyRead8(SKSE::SerializationInterface* serde);

			//Actor Data Save
			static void WriteActorData(SKSE::SerializationInterface* serde, uint8_t Version);
			static void WriteKillCountData(SKSE::SerializationInterface* serde, uint8_t Version);
			static void WriteActorRecordFloat(SKSE::SerializationInterface* serde, const float* Data);
			static void WriteActorRecordBool(SKSE::SerializationInterface* serde, const bool* Data);
			static void WriteActorRecordU8(SKSE::SerializationInterface* serde, const uint8_t* Data);
			static void WriteActorRecordFormID(SKSE::SerializationInterface* serde, const FormID* Id);
			static void DummyWrite32(SKSE::SerializationInterface* serde);
			static void DummyWrite8(SKSE::SerializationInterface* serde);
	};
}
