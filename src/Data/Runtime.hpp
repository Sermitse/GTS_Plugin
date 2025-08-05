#pragma once

// Module that holds data that is loaded at runtime
// This includes various forms

namespace GTS {

	struct SoundData {
		BGSSoundDescriptorForm* data;
	};

	struct SpellEffectData {
		EffectSetting* data;
	};

	struct SpellData {
		SpellItem* data;
	};

	struct PerkData {
		BGSPerk* data;
	};

	struct ExplosionData {
		BGSExplosion* data;
	};

	struct GlobalData {
		TESGlobal* data;
	};

	struct QuestData {
		TESQuest* data;
	};

	struct FactionData {
		TESFaction* data;
	};

	struct ImpactData {
		BGSImpactDataSet* data;
	};

	struct RaceData {
		TESRace* data;
	};

	struct KeywordData {
		BGSKeyword* data;
	};

	struct ContainerData {
		TESObjectCONT* data;
	};

	struct LeveledItemsData {
		TESLevItem* data;
	};

	class Runtime : public EventListener {
		public:
			[[nodiscard]] static Runtime& GetSingleton() noexcept;

			virtual std::string DebugName() override;
			virtual void DataReady() override;
			static BSISoundDescriptor* GetSound(const std::string_view& tag);
			static void PlaySound(const std::string_view& a_tag, Actor* a_actor, const float& a_volume, const float& a_frequency = 1.0f);
			static void PlaySound(const std::string_view& a_tag, TESObjectREFR* a_ref, const float& a_volume, const float& a_frequency = 1.0f);
			static void CheckSoftDependencies();

			static void PlaySoundAtNode_FallOff(const std::string_view& a_tag, Actor* a_actor, const float& a_volume, const std::string_view& a_node, float a_falloff, float a_frequency = 1.0f);
			static void PlaySoundAtNode_FallOff(const std::string_view& a_tag, const float& a_volume, NiAVObject* a_node, float a_falloff, float a_frequency = 1.0f);

			static void PlaySoundAtNode(const std::string_view& a_tag, Actor* a_actor, const float& a_volume, const std::string_view& a_node, float a_frequency = 1.0f);
			static void PlaySoundAtNode(const std::string_view& a_tag, const float& a_volume, NiAVObject* a_node, float a_frequency = 1.0f);

			// Spell Effects
			static EffectSetting* GetMagicEffect(const std::string_view& tag);
			static bool HasMagicEffect(Actor* actor, const std::string_view& tag);
			static bool HasMagicEffectOr(Actor* actor, const std::string_view& tag, const bool& default_value);
			// Spells
			static SpellItem* GetSpell(const std::string_view& tag);
			static void AddSpell(Actor* actor, const std::string_view& tag);
			static void RemoveSpell(Actor* actor, const std::string_view& tag);
			static bool HasSpell(Actor* actor, const std::string_view& tag);
			static bool HasSpellOr(Actor* actor, const std::string_view& tag, const bool& default_value);
			static void CastSpell(Actor* caster, Actor* target, const std::string_view& tag);
			// Perks
			static BGSPerk* GetPerk(const std::string_view& tag);
			static void AddPerk(Actor* actor, const std::string_view& tag);
			static void RemovePerk(Actor* actor, const std::string_view& tag);
			static bool HasPerk(Actor* actor, const std::string_view& tag);
			static bool HasPerkOr(Actor* actor, const std::string_view& tag, const bool& default_value);
			// Explosion
			static BGSExplosion* GetExplosion(const std::string_view& tag);
			static void CreateExplosion(Actor* actor, const float& scale, const std::string_view& tag);
			static void CreateExplosionAtNode(Actor* actor, const std::string_view& node, const float& scale, const std::string_view& tag);
			static void CreateExplosionAtPos(Actor* actor, NiPoint3 pos, const float& scale, const std::string_view& tag);
			// Globals
			static TESGlobal* GetGlobal(const std::string_view& tag);
			static bool GetBool(const std::string_view& tag);
			static bool GetBoolOr(const std::string_view& tag, const bool& default_value);
			static void SetBool(const std::string_view& tag, const bool& value);
			static int GetInt(const std::string_view& tag);
			static int GetIntOr(const std::string_view& tag, const int& default_value);
			static void SetInt(const std::string_view& tag, const int& value);
			static float GetFloat(const std::string_view& tag);
			static float GetFloatOr(const std::string_view& tag, const float& default_value);
			static void SetFloat(const std::string_view& tag, const float& value);
			// Quests
			static TESQuest* GetQuest(const std::string_view& tag);
			static std::uint16_t GetStage(const std::string_view& tag);
			static std::uint16_t GetStageOr(const std::string_view& tag, const std::uint16_t& default_value);
			// Factions
			static TESFaction* GetFaction(const std::string_view& tag);
			static bool InFaction(Actor* actor, const std::string_view& tag);
			static bool InFactionOr(Actor* actor, const std::string_view& tag, const bool& default_value);
			// Impacts
			static BGSImpactDataSet* GetImpactEffect(const std::string_view& tag);
			static void PlayImpactEffect(Actor* actor, const std::string_view& tag, const std::string_view& node, NiPoint3 pick_direction, const float& length, const bool& applyRotation, const bool& useLocalRotation);
			// Races
			static TESRace* GetRace(const std::string_view& tag);
			static bool IsRace(Actor* actor, const std::string_view& tag);
			// Keywords
			static BGSKeyword* GetKeyword(const std::string_view& tag);
			static bool HasKeyword(Actor* actor, const std::string_view& tag);

			// Leveled Items
			static TESLevItem* GetLeveledItem(const std::string_view& tag);
			// Containers
			static TESObjectCONT* GetContainer(const std::string_view& tag);
			static TESObjectREFR* PlaceContainer(Actor* actor, const std::string_view& tag);
			static TESObjectREFR* PlaceContainer(TESObjectREFR* object, const std::string_view& tag);
			static TESObjectREFR* PlaceContainerAtPos(Actor* actor, NiPoint3 pos, const std::string_view& tag);
			static TESObjectREFR* PlaceContainerAtPos(TESObjectREFR* object, NiPoint3 pos, const std::string_view& tag);

			// Team Functions
			static bool HasMagicEffectTeam(Actor* actor, const std::string_view& tag);
			static bool HasMagicEffectTeamOr(Actor* actor, const std::string_view& tag, const bool& default_value);
			static bool HasSpellTeam(Actor* actor, const std::string_view& tag);
			static bool HasSpellTeamOr(Actor* actor, const std::string_view& tag, const bool& default_value);
			static bool HasPerkTeam(Actor* actor, const std::string_view& tag);
			static bool HasPerkTeamOr(Actor* actor, const std::string_view& tag, const bool& default_value);

			// Log function
			static bool Logged(const std::string_view& catagory, const std::string_view& key);

			std::unordered_map<std::string, SoundData> sounds;
			std::unordered_map<std::string, SpellEffectData> spellEffects;
			std::unordered_map<std::string, SpellData> spells;
			std::unordered_map<std::string, PerkData> perks;
			std::unordered_map<std::string, ExplosionData> explosions;
			std::unordered_map<std::string, GlobalData> globals;
			std::unordered_map<std::string, QuestData> quests;
			std::unordered_map<std::string, FactionData> factions;
			std::unordered_map<std::string, ImpactData> impacts;
			std::unordered_map<std::string, RaceData> races;
			std::unordered_map<std::string, KeywordData> keywords;
			std::unordered_map<std::string, ContainerData> containers;
			std::unordered_map<std::string, LeveledItemsData> levelitems;

			std::unordered_set<std::string> logged;


			//Dependency Checks
			[[nodiscard]] __forceinline static inline const bool IsSexlabInstalled() {
				return SoftDep_SL_Found;
			}

			[[nodiscard]] __forceinline static inline const bool IsSurvivalModeInstalled() {
				return SoftDep_SurvMode_Found;
			}

			private:
			static inline bool SoftDep_SL_Found = false;
			static inline bool SoftDep_SurvMode_Found = false;
	};
}
