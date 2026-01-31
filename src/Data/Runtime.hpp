#pragma once
#include "Data/Util/RuntimeData.hpp"

namespace GTS {

	class Runtime : public EventListener, public CInitSingleton<Runtime> {

		public:

		// Runtime Data Containers
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

		//Getters
		static BSISoundDescriptor* GetSound(const std::string_view& a_tag);
		static BSISoundDescriptor* GetSound(const RuntimeData::RuntimeEntry<BGSSoundDescriptorForm>& a_entry);
		static EffectSetting* GetMagicEffect(const std::string_view& a_tag);
		static EffectSetting* GetMagicEffect(const RuntimeData::RuntimeEntry<EffectSetting>& a_entry);
		static SpellItem* GetSpell(const std::string_view& a_tag);
		static SpellItem* GetSpell(const RuntimeData::RuntimeEntry<SpellItem>& a_entry);
		static BGSPerk* GetPerk(const std::string_view& a_tag);
		static BGSPerk* GetPerk(const RuntimeData::RuntimeEntry<BGSPerk>& a_entry);
		static BGSExplosion* GetExplosion(const std::string_view& a_tag);
		static BGSExplosion* GetExplosion(const RuntimeData::RuntimeEntry<BGSExplosion>& a_entry);
		static TESGlobal* GetGlobal(const std::string_view& a_tag);
		static TESGlobal* GetGlobal(const RuntimeData::RuntimeEntry<TESGlobal>& a_entry);
		static TESQuest* GetQuest(const std::string_view& a_tag);
		static TESQuest* GetQuest(const RuntimeData::RuntimeEntry<TESQuest>& a_entry);
		static TESFaction* GetFaction(const std::string_view& a_tag);
		static TESFaction* GetFaction(const RuntimeData::RuntimeEntry<TESFaction>& a_entry);
		static BGSImpactDataSet* GetImpactEffect(const std::string_view& a_tag);
		static BGSImpactDataSet* GetImpactEffect(const RuntimeData::RuntimeEntry<BGSImpactDataSet>& a_entry);
		static TESRace* GetRace(const std::string_view& a_tag);
		static TESRace* GetRace(const RuntimeData::RuntimeEntry<TESRace>& a_entry);
		static BGSKeyword* GetKeyword(const std::string_view& a_tag);
		static BGSKeyword* GetKeyword(const RuntimeData::RuntimeEntry<BGSKeyword>& a_entry);
		static TESLevItem* GetLeveledItem(const std::string_view& a_tag);
		static TESLevItem* GetLeveledItem(const RuntimeData::RuntimeEntry<TESLevItem>& a_entry);
		static TESObjectCONT* GetContainer(const std::string_view& a_tag);
		static TESObjectCONT* GetContainer(const RuntimeData::RuntimeEntry<TESObjectCONT>& a_entry);

		//Helpers - Sound
		static void PlaySound(const std::string_view& a_tag, Actor* a_actor, const float& a_volume, const float& a_frequency = 1.0f);
		static void PlaySound(const std::string_view& a_tag, TESObjectREFR* a_ref, const float& a_volume, const float& a_frequency = 1.0f);
		static void PlaySound(const RuntimeData::RuntimeEntry<BGSSoundDescriptorForm>& a_entry, Actor* a_actor, const float& a_volume, const float& a_frequency = 1.0f);
		static void PlaySound(const RuntimeData::RuntimeEntry<BGSSoundDescriptorForm>& a_entry, TESObjectREFR* a_ref, const float& a_volume, const float& a_frequency = 1.0f);
		static void PlaySoundAtNode(const std::string_view& a_tag, Actor* a_actor, const float& a_volume, const std::string_view& a_node, float a_frequency = 1.0f);
		static void PlaySoundAtNode(const std::string_view& a_tag, const float& a_volume, NiAVObject* a_node, float a_frequency = 1.0f);
		static void PlaySoundAtNode(const RuntimeData::RuntimeEntry<BGSSoundDescriptorForm>& a_entry, Actor* a_actor, const float& a_volume, const std::string_view& a_node, float a_frequency = 1.0f);
		static void PlaySoundAtNode(const RuntimeData::RuntimeEntry<BGSSoundDescriptorForm>& a_entry, const float& a_volume, NiAVObject* a_node, float a_frequency = 1.0f);
		static void PlaySoundAtNode_FallOff(const std::string_view& a_tag, Actor* a_actor, const float& a_volume, const std::string_view& a_node, float a_falloff, float a_frequency = 1.0f);
		static void PlaySoundAtNode_FallOff(const std::string_view& a_tag, const float& a_volume, NiAVObject* a_node, float a_falloff, float a_frequency = 1.0f);
		static void PlaySoundAtNode_FallOff(const RuntimeData::RuntimeEntry<BGSSoundDescriptorForm>& a_entry, Actor* a_actor, const float& a_volume, const std::string_view& a_node, float a_falloff, float a_frequency = 1.0f);
		static void PlaySoundAtNode_FallOff(const RuntimeData::RuntimeEntry<BGSSoundDescriptorForm>& a_entry, const float& a_volume, NiAVObject* a_node, float a_falloff, float a_frequency = 1.0f);

		//Helpers - Magic Effects
		static bool HasMagicEffect(Actor* a_actor, const std::string_view& a_tag);
		static bool HasMagicEffect(Actor* a_actor, const RuntimeData::RuntimeEntry<EffectSetting>& a_entry);
		static bool HasMagicEffectTeam(Actor* a_actor, const std::string_view& a_tag);
		static bool HasMagicEffectTeam(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::EffectSetting>& a_entry);

		//Helpers - Spells
		static void AddSpell(Actor* a_actor, const std::string_view& a_tag);
		static void AddSpell(Actor* a_actor, const RuntimeData::RuntimeEntry<SpellItem>& a_entry);
		static void RemoveSpell(Actor* a_actor, const std::string_view& a_tag);
		static void RemoveSpell(Actor* a_actor, const RuntimeData::RuntimeEntry<SpellItem>& a_entry);
		static bool HasSpell(Actor* a_actor, const std::string_view& a_tag);
		static bool HasSpell(Actor* a_actor, const RuntimeData::RuntimeEntry<SpellItem>& a_entry);
		static bool HasSpellTeam(Actor* a_actor, const std::string_view& a_tag);
		static bool HasSpellTeam(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::SpellItem>& a_entry);
		static void CastSpell(Actor* a_caster, Actor* a_target, const std::string_view& a_tag);
		static void CastSpell(Actor* a_caster, Actor* a_target, const RuntimeData::RuntimeEntry<SpellItem>& a_entry);

		//Helpers - Perks
		static void AddPerk(Actor* a_actor, const std::string_view& a_tag);
		static void AddPerk(Actor* a_actor, const RuntimeData::RuntimeEntry<BGSPerk>& a_entry);
		static void RemovePerk(Actor* a_actor, const std::string_view& a_tag);
		static void RemovePerk(Actor* a_actor, const RuntimeData::RuntimeEntry<BGSPerk>& a_entry);
		static bool HasPerk(Actor* a_actor, const std::string_view& a_tag);
		static bool HasPerk(Actor* a_actor, const RuntimeData::RuntimeEntry<BGSPerk>& a_entry);
		static bool HasPerkTeam(Actor* a_actor, const std::string_view& a_tag);
		static bool HasPerkTeam(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::BGSPerk>& a_entry);

		//Helpers - Explosions
		static void CreateExplosion(Actor* a_actor, const float& a_scale, const std::string_view& a_tag);
		static void CreateExplosion(Actor* a_actor, const float& a_scale, const RuntimeData::RuntimeEntry<RE::BGSExplosion>& a_entry);
		static void CreateExplosionAtNode(Actor* a_actor, const std::string_view& a_nodeName, const float& a_scale, const std::string_view& a_tag);
		static void CreateExplosionAtNode(Actor* a_actor, const std::string_view& a_nodeName, const float& a_scale, const RuntimeData::RuntimeEntry<RE::BGSExplosion>& a_entry);
		static void CreateExplosionAtPos(Actor* a_actor, NiPoint3 a_pos, const float& a_scale, const std::string_view& a_tag);
		static void CreateExplosionAtPos(Actor* a_actor, NiPoint3 a_pos, const float& a_scale, const RuntimeData::RuntimeEntry<RE::BGSExplosion>& a_entry);

		//Helpers - Globals
		static bool GetBool(const std::string_view& a_tag);
		static bool GetBool(const RuntimeData::RuntimeEntry<RE::TESGlobal>& a_entry);
		static void SetBool(const std::string_view& a_tag, const bool& a_value);
		static void SetBool(const RuntimeData::RuntimeEntry<RE::TESGlobal>& a_entry, const bool& a_value);
		static int GetInt(const std::string_view& a_tag);
		static int GetInt(const RuntimeData::RuntimeEntry<RE::TESGlobal>& a_entry);
		static void SetInt(const std::string_view& a_tag, const int& a_value);
		static void SetInt(const RuntimeData::RuntimeEntry<RE::TESGlobal>& a_entry, const int& a_value);
		static float GetFloat(const std::string_view& a_tag);
		static float GetFloat(const RuntimeData::RuntimeEntry<RE::TESGlobal>& a_entry);
		static void SetFloat(const std::string_view& a_tag, const float& a_value);
		static void SetFloat(const RuntimeData::RuntimeEntry<RE::TESGlobal>& a_entry, const float& a_value);

		//Helpers - Quests
		static std::uint16_t GetStage(const std::string_view& a_tag);
		static std::uint16_t GetStage(const RuntimeData::RuntimeEntry<RE::TESQuest>& a_entry);

		//Helpers - Factions
		static bool InFaction(Actor* a_actor, const std::string_view& a_tag);
		static bool InFaction(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::TESFaction>& a_entry);

		//Helpers - Impacts
		static void PlayImpactEffect(Actor* a_actor, const std::string_view& a_tag, const std::string_view& a_node, NiPoint3 a_pickDirection, const float& a_length, const bool& a_applyRotation, const bool& a_useLocalRotation);
		static void PlayImpactEffect(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::BGSImpactDataSet>& a_entry, const std::string_view& a_node, NiPoint3 a_pickDirection, const float& a_length, const bool& a_applyRotation, const bool& a_useLocalRotation);

		//Helpers - Races
		static bool IsRace(Actor* a_actor, const std::string_view& a_tag);
		static bool IsRace(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::TESRace>& a_entry);

		//Helpers - Keywords
		static bool HasKeyword(Actor* a_actor, const std::string_view& a_tag);
		static bool HasKeyword(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::BGSKeyword>& a_entry);

		//Helpers - Containers
		static TESObjectREFR* PlaceContainer(Actor* a_actor, const std::string_view& a_tag);
		static TESObjectREFR* PlaceContainer(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::TESObjectCONT>& a_entry);
		static TESObjectREFR* PlaceContainer(TESObjectREFR* a_object, const std::string_view& a_tag);
		static TESObjectREFR* PlaceContainer(TESObjectREFR* a_object, const RuntimeData::RuntimeEntry<RE::TESObjectCONT>& a_entry);
		static TESObjectREFR* PlaceContainerAtPos(Actor* a_actor, NiPoint3 a_pos, const std::string_view& a_tag);
		static TESObjectREFR* PlaceContainerAtPos(Actor* a_actor, NiPoint3 a_pos, const RuntimeData::RuntimeEntry<RE::TESObjectCONT>& a_entry);
		static TESObjectREFR* PlaceContainerAtPos(TESObjectREFR* a_object, NiPoint3 a_pos, const std::string_view& a_tag);
		static TESObjectREFR* PlaceContainerAtPos(TESObjectREFR* a_object, NiPoint3 a_pos, const RuntimeData::RuntimeEntry<RE::TESObjectCONT>& a_entry);

		//Dependency Checks
		[[nodiscard]] static bool IsSexlabInstalled();
		[[nodiscard]] static bool IsSurvivalModeInstalled();
		[[nodiscard]] static bool IsDevourmentInstalled();
		[[nodiscard]] static bool IsAltConversationCamInstalled();

		private:
		virtual std::string DebugName() override;
		virtual void DataReady() override;

		static inline bool SexlabInstalled = false;
		static inline bool DevourmentInstalled = false;
		static inline bool SurvivalModeInstalled = false;
		static inline bool AltConversationCamInstalled = false;
	};
}
