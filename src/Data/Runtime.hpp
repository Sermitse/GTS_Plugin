#pragma once
#include "Data/Runtime/Util/RuntimeData.hpp"

namespace GTS {

	class Runtime : public EventListener, public CInitSingleton<Runtime> {

		public:
		static inline RuntimeData::SoundDescriptors SNDR;
		static inline RuntimeData::MagicEffects MGEF;
		static inline RuntimeData::Spells SPEL;
		static inline RuntimeData::Perks PERK;
		static inline RuntimeData::Explosions EXPL;
		static inline RuntimeData::Globals GLOB;
		static inline RuntimeData::Quests QUST;
		static inline RuntimeData::Factions FACT;
		static inline RuntimeData::ImpactDataSets IDTS;
		static inline RuntimeData::Races RACE;
		static inline RuntimeData::Keywords KYWD;
		static inline RuntimeData::Containers CONT;
		static inline RuntimeData::LeveledItems LVLI;


		virtual std::string DebugName() override;
		virtual void DataReady() override;

		static BSISoundDescriptor* GetSound(const std::string_view& a_tag);
		static BSISoundDescriptor* GetSound(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry);
		static EffectSetting* GetMagicEffect(const std::string_view& a_tag);
		static EffectSetting* GetMagicEffect(const RuntimeData::RuntimeEntry<RE::EffectSetting>& a_entry);

		static void PlaySound(const std::string_view& a_tag, Actor* a_actor, const float& a_volume, const float& a_frequency = 1.0f);
		static void PlaySound(const std::string_view& a_tag, TESObjectREFR* a_ref, const float& a_volume, const float& a_frequency = 1.0f);
		static void PlaySound(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry, Actor* a_actor, const float& a_volume, const float& a_frequency = 1.0f);
		static void PlaySound(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry, TESObjectREFR* a_ref, const float& a_volume, const float& a_frequency = 1.0f);

		static void PlaySoundAtNode_FallOff(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry, Actor* a_actor, const float& a_volume, const std::string_view& a_node, float a_falloff, float a_frequency = 1.0f);
		static void PlaySoundAtNode_FallOff(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry, const float& a_volume, NiAVObject* a_node, float a_falloff, float a_frequency = 1.0f);
		static void PlaySoundAtNode_FallOff(const std::string_view& a_tag, Actor* a_actor, const float& a_volume, const std::string_view& a_node, float a_falloff, float a_frequency = 1.0f);
		static void PlaySoundAtNode_FallOff(const std::string_view& a_tag, const float& a_volume, NiAVObject* a_node, float a_falloff, float a_frequency = 1.0f);

		static void PlaySoundAtNode(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry, Actor* a_actor, const float& a_volume, const std::string_view& a_node, float a_frequency = 1.0f);
		static void PlaySoundAtNode(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry, const float& a_volume, NiAVObject* a_node, float a_frequency = 1.0f);
		static void PlaySoundAtNode(const std::string_view& a_tag, Actor* a_actor, const float& a_volume, const std::string_view& a_node, float a_frequency = 1.0f);
		static void PlaySoundAtNode(const std::string_view& a_tag, const float& a_volume, NiAVObject* a_node, float a_frequency = 1.0f);

		static void PlaySound_Impl_Actor(BSISoundDescriptor* a_soundDescriptor, Actor* a_actor, const float& a_volume, const float& a_frequency);
		static void PlaySound_Impl(BSISoundDescriptor* a_soundDescriptor, TESObjectREFR* a_ref, const float& a_volume, const float& a_frequency);
		static void PlaySoundAtNode_FallOff_Impl(BSISoundDescriptor* a_soundDescriptor, const float& a_volume, NiAVObject* a_node, float a_falloff, float a_frequency);
		static void PlaySoundAtNode_Impl(BSISoundDescriptor* a_soundDescriptor, const float& a_volume, NiAVObject* a_node, float a_frequency);



		// Spell Effects


		static bool HasMagicEffect(Actor* actor, const std::string_view& tag);
		static bool HasMagicEffectOr(Actor* actor, const std::string_view& tag, const bool& default_value);

		// Spells
		static SpellItem* GetSpell(const std::string_view& tag);
		static SpellItem* GetSpell(const RuntimeData::RuntimeEntry<RE::SpellItem>& a_entry);
		static void AddSpell(Actor* actor, const std::string_view& tag);
		static void RemoveSpell(Actor* actor, const std::string_view& tag);
		static bool HasSpell(Actor* actor, const std::string_view& tag);
		static bool HasSpellOr(Actor* actor, const std::string_view& tag, const bool& default_value);
		static void CastSpell(Actor* caster, Actor* target, const std::string_view& tag);

		// Perks
		static BGSPerk* GetPerk(const std::string_view& tag);
		static BGSPerk* GetPerk(const RuntimeData::RuntimeEntry<RE::BGSPerk>& a_entry);
		static void AddPerk(Actor* actor, const std::string_view& tag);
		static void RemovePerk(Actor* actor, const std::string_view& tag);
		static bool HasPerk(Actor* actor, const std::string_view& tag);
		static bool HasPerkOr(Actor* actor, const std::string_view& tag, const bool& default_value);

		// Explosion
		static BGSExplosion* GetExplosion(const std::string_view& tag);
		static BGSExplosion* GetExplosion(const RuntimeData::RuntimeEntry<RE::BGSExplosion>& a_entry);
		static void CreateExplosion(Actor* actor, const float& scale, const std::string_view& tag);
		static void CreateExplosionAtNode(Actor* actor, const std::string_view& node, const float& scale, const std::string_view& tag);
		static void CreateExplosionAtPos(Actor* actor, NiPoint3 pos, const float& scale, const std::string_view& tag);

		// Globals
		static TESGlobal* GetGlobal(const std::string_view& tag);
		static TESGlobal* GetGlobal(const RuntimeData::RuntimeEntry<RE::TESGlobal>& a_entry);
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
		static TESQuest* GetQuest(const RuntimeData::RuntimeEntry<RE::TESQuest>& a_entry);
		static std::uint16_t GetStage(const std::string_view& tag);
		static std::uint16_t GetStageOr(const std::string_view& tag, const std::uint16_t& default_value);

		// Factions
		static TESFaction* GetFaction(const std::string_view& tag);
		static TESFaction* GetFaction(const RuntimeData::RuntimeEntry<RE::TESFaction>& a_entry);
		static bool InFaction(Actor* actor, const std::string_view& tag);
		static bool InFactionOr(Actor* actor, const std::string_view& tag, const bool& default_value);

		// Impacts
		static BGSImpactDataSet* GetImpactEffect(const std::string_view& tag);
		static BGSImpactDataSet* GetImpactEffect(const RuntimeData::RuntimeEntry<RE::BGSImpactDataSet>& a_entry);
		static void PlayImpactEffect(Actor* actor, const std::string_view& tag, const std::string_view& node, NiPoint3 pick_direction, const float& length, const bool& applyRotation, const bool& useLocalRotation);

		// Races
		static TESRace* GetRace(const std::string_view& tag);
		static TESRace* GetRace(const RuntimeData::RuntimeEntry<RE::TESRace>& a_entry);
		static bool IsRace(Actor* actor, const std::string_view& tag);

		// Keywords
		static BGSKeyword* GetKeyword(const std::string_view& tag);
		static BGSKeyword* GetKeyword(const RuntimeData::RuntimeEntry<RE::BGSKeyword>& a_entry);
		static bool HasKeyword(Actor* actor, const std::string_view& tag);

		// Leveled Items
		static TESLevItem* GetLeveledItem(const std::string_view& tag);
		static TESLevItem* GetLeveledItem(const RuntimeData::RuntimeEntry<RE::TESLevItem>& a_entry);

		// Containers
		static TESObjectCONT* GetContainer(const std::string_view& tag);
		static TESObjectCONT* GetContainer(const RuntimeData::RuntimeEntry<RE::TESObjectCONT>& a_entry);
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

		//Dependency Checks
		[[nodiscard]] __forceinline static inline bool IsSexlabInstalled() {
			return SexlabInstalled;
		}

		[[nodiscard]] __forceinline static inline bool IsSurvivalModeInstalled() {
			return SurvivalModeInstalled;
		}

		[[nodiscard]] __forceinline static inline bool IsDevourmentInstalled() {
			return DevourmentInstalled;
		}

		[[nodiscard]] __forceinline static inline bool IsAltConversationCamInstalled() {
			return AltConversationCamInstalled;
		}

		private:
		static inline bool SexlabInstalled = false;
		static inline bool DevourmentInstalled = false;
		static inline bool SurvivalModeInstalled = false;
		static inline bool AltConversationCamInstalled = false;
	};
}
