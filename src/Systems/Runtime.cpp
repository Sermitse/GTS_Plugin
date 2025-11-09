#include "Systems/Runtime.hpp"

namespace {

	void CheckModLoaded(bool* a_res, const std::string_view& a_name) {
		logger::info("Dependency Checker: Checking for {}", a_name);
		if (a_res) {
			if (const auto DataHandler = RE::TESDataHandler::GetSingleton()) {
				const auto ModFile = DataHandler->LookupModByName(a_name);
				if (ModFile && ModFile->compileIndex != 0xFF) {
					logger::info("SoftDependency Checker: {} was Found.", a_name);
					*a_res = true;
					return;
				}
			}
		}
		*a_res = false;
	}

	void CheckDLLLoaded(bool* a_res, const std::wstring_view& a_name) {
		logger::info("Dependency DLL Checker: Checking for {}", GTS::Utf16ToUtf8(a_name));
		if (a_res) {
			*a_res = (GetModuleHandleW(a_name.data()) != nullptr); // DLL name
			return;
		}
		*a_res = false;
	}

	template<typename ReturnType, typename ListableType>
	ReturnType* GetFormByTagName(ListableType& a_listable, const std::string_view& a_tag) {
		if (auto it = a_listable.List.find(absl::string_view(a_tag.data(), a_tag.size())); it != a_listable.List.end()) {
			if (!it->second->Value) {
				logger::debug("Form lookup found key '{}' but Value is nullptr", a_tag);
				return nullptr;
			}
			return it->second->Value;
		}
		logger::debug("Form lookup failed: key '{}' not found in list", a_tag);
		return nullptr;
	}

}

namespace GTS {

	//--------------------
	// Virtual Overrides
	//--------------------

	std::string Runtime::DebugName() {
		return "::Runtime";
	}

	void Runtime::DataReady() {

		if (auto File = TESDataHandler::GetSingleton()->LookupModByName("GTS.esp")) {
			if (File->compileIndex == 0xFF) {
				ReportAndExit(
				  "GTS.esp was not found in the active load order.\n"
				  "Make sure it exists and is activated.\n"
	              "The game will now close."
				);
			}
		}

		CheckModLoaded(&SexlabInstalled,"SexLab.esm");
		CheckModLoaded(&SurvivalModeInstalled, "ccQDRSSE001-SurvivalMode.esl");
		CheckModLoaded(&DevourmentInstalled, "Devourment.esp");
		CheckDLLLoaded(&AltConversationCamInstalled, L"AlternateConversationCamera.dll");

		//Resolve FormID's -> Game Objects
		SNDR.Resolve();
		MGEF.Resolve();
		SPEL.Resolve();
		PERK.Resolve();
		EXPL.Resolve();
		GLOB.Resolve();
		QUST.Resolve();
		FACT.Resolve();
		IDTS.Resolve();
		RACE.Resolve();
		KYWD.Resolve();
		CONT.Resolve();
		LVLI.Resolve();

	}

	//--------------------
	// GameObject Getters
	//--------------------

	// ---- Sound
	BSISoundDescriptor* Runtime::GetSound(const std::string_view& a_tag) {
		if (const auto& Sound = GetFormByTagName<BGSSoundDescriptorForm>(SNDR, a_tag)) {
			return Sound->soundDescriptor;
		}
		return nullptr;
	}

	BSISoundDescriptor* Runtime::GetSound(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value->soundDescriptor;
		}
		return nullptr;
	}

	// ---- Magic Effect
	EffectSetting* Runtime::GetMagicEffect(const std::string_view& a_tag) {
		return GetFormByTagName<EffectSetting>(MGEF, a_tag);
	}

	EffectSetting* Runtime::GetMagicEffect(const RuntimeData::RuntimeEntry<RE::EffectSetting>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Spell
	SpellItem* Runtime::GetSpell(const std::string_view& tag) {
		return GetFormByTagName<SpellItem>(SPEL, tag);
	}

	SpellItem* Runtime::GetSpell(const RuntimeData::RuntimeEntry<RE::SpellItem>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Perk
	BGSPerk* Runtime::GetPerk(const std::string_view& tag) {
		return GetFormByTagName<BGSPerk>(PERK, tag);
	}

	BGSPerk* Runtime::GetPerk(const RuntimeData::RuntimeEntry<RE::BGSPerk>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Explosion
	BGSExplosion* Runtime::GetExplosion(const std::string_view& tag) {
		return GetFormByTagName<BGSExplosion>(EXPL, tag);
	}

	BGSExplosion* Runtime::GetExplosion(const RuntimeData::RuntimeEntry<RE::BGSExplosion>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Global
	TESGlobal* Runtime::GetGlobal(const std::string_view& tag) {
		return GetFormByTagName<TESGlobal>(GLOB, tag);
	}

	TESGlobal* Runtime::GetGlobal(const RuntimeData::RuntimeEntry<RE::TESGlobal>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Quest
	TESQuest* Runtime::GetQuest(const std::string_view& tag) {
		return GetFormByTagName<TESQuest>(QUST, tag);
	}

	TESQuest* Runtime::GetQuest(const RuntimeData::RuntimeEntry<RE::TESQuest>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Faction
	TESFaction* Runtime::GetFaction(const std::string_view& tag) {
		return GetFormByTagName<TESFaction>(FACT, tag);
	}

	TESFaction* Runtime::GetFaction(const RuntimeData::RuntimeEntry<RE::TESFaction>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Impact Data Set
	BGSImpactDataSet* Runtime::GetImpactEffect(const std::string_view& tag) {
		return GetFormByTagName<BGSImpactDataSet>(IDTS, tag);
	}

	BGSImpactDataSet* Runtime::GetImpactEffect(const RuntimeData::RuntimeEntry<RE::BGSImpactDataSet>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Race
	TESRace* Runtime::GetRace(const std::string_view& tag) {
		return GetFormByTagName<TESRace>(RACE, tag);
	}

	TESRace* Runtime::GetRace(const RuntimeData::RuntimeEntry<RE::TESRace>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Keyword
	BGSKeyword* Runtime::GetKeyword(const std::string_view& tag) {
		return GetFormByTagName<BGSKeyword>(KYWD, tag);
	}

	BGSKeyword* Runtime::GetKeyword(const RuntimeData::RuntimeEntry<RE::BGSKeyword>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Leveled Item
	TESLevItem* Runtime::GetLeveledItem(const std::string_view& tag) {
		return GetFormByTagName<TESLevItem>(LVLI, tag);
	}

	TESLevItem* Runtime::GetLeveledItem(const RuntimeData::RuntimeEntry<RE::TESLevItem>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Container
	TESObjectCONT* Runtime::GetContainer(const std::string_view& tag) {
		return GetFormByTagName<TESObjectCONT>(CONT, tag);
	}

	TESObjectCONT* Runtime::GetContainer(const RuntimeData::RuntimeEntry<RE::TESObjectCONT>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}


	//--------------------
	// Sound Helpers
	//--------------------

	// Overload Helpers
	void Runtime::PlaySound(const std::string_view& a_tag, Actor* a_actor, const float& a_volume, const float& a_frequency) {
		PlaySound_Impl_Actor(GetSound(a_tag), a_actor, a_volume, a_frequency);
	}

	void Runtime::PlaySound(const std::string_view& a_tag, TESObjectREFR* a_ref, const float& a_volume, const float& a_frequency) {
		PlaySound_Impl(GetSound(a_tag), a_ref, a_volume, a_frequency);
	}

	void Runtime::PlaySound(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry, Actor* a_actor, const float& a_volume, const float& a_frequency) {
		PlaySound_Impl_Actor(GetSound(a_entry), a_actor, a_volume, a_frequency);
	}

	void Runtime::PlaySound(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry, TESObjectREFR* a_ref, const float& a_volume, const float& a_frequency) {
		PlaySound_Impl(GetSound(a_entry), a_ref, a_volume, a_frequency);
	}

	void Runtime::PlaySoundAtNode_FallOff(const std::string_view& a_tag, Actor* a_actor, const float& a_volume, const std::string_view& a_node, float a_falloff, float a_frequency) {
		PlaySoundAtNode_FallOff_Impl(GetSound(a_tag), a_volume, find_node(a_actor, a_node), a_falloff, a_frequency);
	}

	void Runtime::PlaySoundAtNode_FallOff(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry, Actor* a_actor, const float& a_volume, const std::string_view& a_node, float a_falloff, float a_frequency) {
		PlaySoundAtNode_FallOff_Impl(GetSound(a_entry), a_volume, find_node(a_actor, a_node), a_falloff, a_frequency);
	}

	void Runtime::PlaySoundAtNode_FallOff(const std::string_view& a_tag, const float& a_volume, NiAVObject* a_node, float a_falloff, float a_frequency) {
		PlaySoundAtNode_FallOff_Impl(GetSound(a_tag), a_volume, a_node, a_falloff, a_frequency);
	}

	void Runtime::PlaySoundAtNode_FallOff(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry, const float& a_volume, NiAVObject* a_node, float a_falloff, float a_frequency) {
		PlaySoundAtNode_FallOff_Impl(GetSound(a_entry), a_volume, a_node, a_falloff, a_frequency);
	}

	void Runtime::PlaySoundAtNode(const std::string_view& a_tag, Actor* a_actor, const float& a_volume, const std::string_view& a_node, float a_frequency) {
		PlaySoundAtNode_Impl(GetSound(a_tag), a_volume, find_node(a_actor, a_node), a_frequency);
	}

	void Runtime::PlaySoundAtNode(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry, const float& a_volume, NiAVObject* a_node, float a_frequency) {
		PlaySoundAtNode_Impl(GetSound(a_entry), a_volume, a_node, a_frequency);
	}

	void Runtime::PlaySoundAtNode(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry, Actor* a_actor, const float& a_volume, const std::string_view& a_node, float a_frequency) {
		PlaySoundAtNode_Impl(GetSound(a_entry), a_volume, find_node(a_actor, a_node), a_frequency);
	}

	void Runtime::PlaySoundAtNode(const std::string_view& a_tag, const float& a_volume, NiAVObject* a_node, float a_frequency) {
		PlaySoundAtNode_Impl(GetSound(a_tag), a_volume, a_node, a_frequency);
	}

	// ----- Concrete Implementations -----

	//Workaround for explicit casting
	void Runtime::PlaySound_Impl_Actor(BSISoundDescriptor* a_soundDescriptor, Actor* a_actor, const float& a_volume, const float& a_frequency) {
		TESObjectREFR* ActorAsRef = skyrim_cast<TESObjectREFR*>(a_actor);
		PlaySound_Impl(a_soundDescriptor, ActorAsRef, a_volume, a_frequency);
	}

	void Runtime::PlaySound_Impl(BSISoundDescriptor* a_soundDescriptor, TESObjectREFR* a_ref, const float& a_volume, const float& a_frequency) {

		if (!a_soundDescriptor) {
			logger::error("Ivallid Sound Descriptor");
			return;
		}

		auto audioManager = BSAudioManager::GetSingleton();
		if (!audioManager) {
			log::error("Audio Manager invalid");
			return;
		}

		BSSoundHandle soundHandle;
		bool success = audioManager->BuildSoundDataFromDescriptor(soundHandle, a_soundDescriptor);
		if (success) {
			ObjectRefHandle ref = a_ref->CreateRefHandle();
			if (TESObjectREFR* refget = ref.get().get()) {

				soundHandle.SetVolume(a_volume);

				SetSoundHandleFrequency(audioManager, soundHandle.soundID, a_frequency);

				if (NiAVObject* current_3d = refget->GetCurrent3D()) {
					NiAVObject* follow = current_3d;
					soundHandle.SetObjectToFollow(follow);
					soundHandle.Play();
				}
			}
			return;
		}

		log::error("Could not build sound");
	}

	void Runtime::PlaySoundAtNode_FallOff_Impl(BSISoundDescriptor* a_soundDescriptor, const float& a_volume, NiAVObject* a_node, float a_falloff, float a_frequency) {

		if (!a_node) {
			logger::warn("Tried to play a sound on a null node");
			return;
		}

		if (!a_soundDescriptor) {
			logger::error("Ivallid Sound Descriptor");
			return;
		}

		auto audioManager = BSAudioManager::GetSingleton();
		if (!audioManager) {
			log::error("Audio Manager invalid");
			return;
		}

		BSSoundHandle soundHandle;
		if (audioManager->BuildSoundDataFromDescriptor(soundHandle, a_soundDescriptor)) {
			float falloff = Sound_GetFallOff(a_node, a_falloff);
			soundHandle.SetVolume(a_volume * falloff);

			SetSoundHandleFrequency(audioManager, soundHandle.soundID, a_frequency);

			soundHandle.SetObjectToFollow(a_node);
			soundHandle.Play();
			return;
		}

		log::error("Could not build sound");
		
	}

	void Runtime::PlaySoundAtNode_Impl(BSISoundDescriptor* a_soundDescriptor, const float& a_volume, NiAVObject* a_node, float a_frequency) {

		if (!a_node) {
			logger::warn("Tried to play a sound on a null node");
			return;
		}

		if (!a_soundDescriptor) {
			logger::error("Ivallid Sound Descriptor");
			return;
		}

		auto audioManager = BSAudioManager::GetSingleton();
		if (!audioManager) {
			log::error("Audio Manager invalid");
			return;
		}

		BSSoundHandle soundHandle;
		if (audioManager->BuildSoundDataFromDescriptor(soundHandle, a_soundDescriptor)) {
			soundHandle.SetVolume(a_volume);

			SetSoundHandleFrequency(audioManager, soundHandle.soundID, a_frequency);

			soundHandle.SetObjectToFollow(a_node);
			soundHandle.Play();

			return;
		}
		log::error("Could not build sound");
	}

	//-----------------------
	// Magic Effect Helpers
	//-----------------------

	bool Runtime::HasMagicEffect(Actor* actor, const std::string_view& tag) {
		return HasMagicEffectOr(actor, tag, false);
	}

	bool Runtime::HasMagicEffectOr(Actor* actor, const std::string_view& tag, const bool& default_value) {
		if (!actor) {
			return false;
		}
		auto data = GetMagicEffect(tag);
		if (data) {
			return actor->AsMagicTarget()->HasMagicEffect(data);
		}

		return default_value;
		
	}

	// Spells


	void Runtime::AddSpell(Actor* actor, const std::string_view& tag) {
		auto data = GetSpell(tag);
		if (data) {
			if (!HasSpell(actor, tag)) {
				actor->AddSpell(data);
			}
		}
	}

	void Runtime::RemoveSpell(Actor* actor, const std::string_view& tag) {
		auto data = GetSpell(tag);
		if (data) {
			if (HasSpell(actor, tag)) {
				actor->RemoveSpell(data);
			}
		}
	}

	bool Runtime::HasSpell(Actor* actor, const std::string_view& tag) {
		return HasSpellOr(actor, tag, false);
	}

	bool Runtime::HasSpellOr(Actor* actor, const std::string_view& tag, const bool& default_value) {
		auto data = GetSpell(tag);
		if (data) {
			return actor->HasSpell(data);
		}

		return default_value;
		
	}

	void Runtime::CastSpell(Actor* caster, Actor* target, const std::string_view& tag) {
		auto data = GetSpell(tag);
		if (data) {
			caster->GetMagicCaster(MagicSystem::CastingSource::kInstant)->CastSpellImmediate(data, false, target, 1.00f, false, 0.0f, caster);
		}
	}

	// Perks


	void Runtime::AddPerk(Actor* actor, const std::string_view& tag) {
		auto data = GetPerk(tag);
		if (data) {
			if (!HasPerk(actor, tag)) {
				actor->AddPerk(data);
			}
		}
	}

	void Runtime::RemovePerk(Actor* actor, const std::string_view& tag) {
		auto data = GetPerk(tag);
		if (data) {
			if (HasPerk(actor, tag)) {
				actor->RemovePerk(data);
			}
		}
	}

	bool Runtime::HasPerk(Actor* actor, const std::string_view& tag) {
		return HasPerkOr(actor, tag, false);
	}

	bool Runtime::HasPerkOr(Actor* actor, const std::string_view& tag, const bool& default_value) {
		auto data = GetPerk(tag);
		if (data) {
			return actor->HasPerk(data);
		}

		return default_value;
		
	}

	// Explosion


	void Runtime::CreateExplosion(Actor* actor, const float& scale, const std::string_view& tag) {
		if (actor) {
			CreateExplosionAtPos(actor, actor->GetPosition(), scale, tag);
		}
	}

	void Runtime::CreateExplosionAtNode(Actor* actor, const std::string_view& node_name, const float& scale, const std::string_view& tag) {
		if (actor) {
			if (actor->Is3DLoaded()) {
				auto model = actor->GetCurrent3D();
				if (model) {
					auto node = model->GetObjectByName(std::string(node_name));
					if (node) {
						CreateExplosionAtPos(actor, node->world.translate, scale, tag);
					}
				}
			}
		}
	}

	void Runtime::CreateExplosionAtPos(Actor* actor, NiPoint3 pos, const float& scale, const std::string_view& tag) {
		auto data = GetExplosion(tag);
		if (data) {
			NiPointer<TESObjectREFR> instance_ptr = actor->PlaceObjectAtMe(data, false);
			if (!instance_ptr) {
				return;
			}
			TESObjectREFR* instance = instance_ptr.get();
			if (!instance) {
				return;
			}

			Explosion* explosion = instance->AsExplosion();
			if (!explosion) {
				return;
			}
			explosion->SetPosition(pos);
			explosion->GetExplosionRuntimeData().radius *= scale;
			explosion->GetExplosionRuntimeData().imodRadius *= scale;
		}
	}

	// Globals


	bool Runtime::GetBool(const std::string_view& tag) {
		return GetBoolOr(tag, false);
	}

	bool Runtime::GetBoolOr(const std::string_view& tag, const bool& default_value) {
		auto data = GetGlobal(tag);
		if (data) {
			return fabs(data->value - 0.0f) > 1e-4;
		}

		return default_value;
		
	}

	void Runtime::SetBool(const std::string_view& tag, const bool& value) {
		auto data = GetGlobal(tag);
		if (data) {
			if (value) {
				data->value = 1.0f;
			}
			else {
				data->value = 0.0f;
			}
		}
	}

	int Runtime::GetInt(const std::string_view& tag) {
		return GetIntOr(tag, false);
	}

	int Runtime::GetIntOr(const std::string_view& tag, const int& default_value) {
		auto data = GetGlobal(tag);
		if (data) {
			return static_cast<int>(data->value);
		}

		return default_value;
		
	}

	void Runtime::SetInt(const std::string_view& tag, const int& value) {
		auto data = GetGlobal(tag);
		if (data) {
			data->value = static_cast<float>(value);
		}
	}

	float Runtime::GetFloat(const std::string_view& tag) {
		return GetFloatOr(tag, false);
	}

	float Runtime::GetFloatOr(const std::string_view& tag, const float& default_value) {
		auto data = GetGlobal(tag);
		if (data) {
			return data->value;
		}

		return default_value;
		
	}

	void Runtime::SetFloat(const std::string_view& tag, const float& value) {
		auto data = GetGlobal(tag);
		if (data) {
			data->value = value;
		}
	}

	// Quests

	std::uint16_t Runtime::GetStage(const std::string_view& tag) {
		return GetStageOr(tag, 0);
	}

	std::uint16_t Runtime::GetStageOr(const std::string_view& tag, const std::uint16_t& default_value) {
		auto data = GetQuest(tag);
		if (data) {
			return data->GetCurrentStageID();
		}

		return default_value;
		
	}



	bool Runtime::InFaction(Actor* actor, const std::string_view& tag) {
		return InFactionOr(actor, tag, false);
	}

	bool Runtime::InFactionOr(Actor* actor, const std::string_view& tag, const bool& default_value) {
		auto data = GetFaction(tag);
		if (data) {
			return actor->IsInFaction(data);
		}

		return default_value;
		
	}



	void Runtime::PlayImpactEffect(Actor* actor, const std::string_view& tag, const std::string_view& node, NiPoint3 pick_direction, const float& length, const bool& applyRotation, const bool& useLocalRotation) {
		auto data = GetImpactEffect(tag);
		if (data) {
			auto impact = BGSImpactManager::GetSingleton();
			impact->PlayImpactEffect(actor, data, node, pick_direction, length, applyRotation, useLocalRotation);
		}
	}

	// Races


	bool Runtime::IsRace(Actor* actor, const std::string_view& tag) {
		auto data = GetRace(tag);
		if (data) {
			return actor->GetRace() == data;
		}

		return false;
		
	}


	bool Runtime::HasKeyword(Actor* actor, const std::string_view& tag) {
		auto data = GetKeyword(tag);
		if (data) {
			return actor->HasKeyword(data);
		}

		return false;
		
	}



	TESObjectREFR* Runtime::PlaceContainer(Actor* actor, const std::string_view& tag) {
		if (actor) {
			return PlaceContainerAtPos(actor, actor->GetPosition(), tag);
		}
		return nullptr;
	}

	TESObjectREFR* Runtime::PlaceContainer(TESObjectREFR* object, const std::string_view& tag) {
		if (object) {
			return PlaceContainerAtPos(object, object->GetPosition(), tag);
		}
		return nullptr;
	}

	TESObjectREFR* Runtime::PlaceContainerAtPos(Actor* actor, NiPoint3 pos, const std::string_view& tag) {
		auto data = GetContainer(tag);
		if (data) {
			NiPointer<TESObjectREFR> instance_ptr = actor->PlaceObjectAtMe(data, false);
			if (!instance_ptr) {
				return nullptr;
			}

			TESObjectREFR* instance = instance_ptr.get();
			if (!instance) {
				return nullptr;
			}

			instance->SetPosition(pos);
			instance->data.angle.x = 0;
			instance->data.angle.y = 0;
			instance->data.angle.z = 0;
			return instance;
		}
		return nullptr;
	}

	TESObjectREFR* Runtime::PlaceContainerAtPos(TESObjectREFR* object, NiPoint3 pos, const std::string_view& tag) {
		auto data = GetContainer(tag);
		if (data) {
			NiPointer<TESObjectREFR> instance_ptr = object->PlaceObjectAtMe(data, false);
			if (!instance_ptr) {
				return nullptr;
			}

			TESObjectREFR* instance = instance_ptr.get();
			if (!instance) {
				return nullptr;
			}

			instance->SetPosition(pos);
			instance->data.angle.x = 0;
			instance->data.angle.y = 0;
			instance->data.angle.z = 0;
			return instance;
		}
		return nullptr;
	}

	// Team Functions
	bool Runtime::HasMagicEffectTeam(Actor* actor, const std::string_view& tag) {
		return HasMagicEffectTeamOr(actor, tag, false);
	}

	bool Runtime::HasMagicEffectTeamOr(Actor* actor, const std::string_view& tag, const bool& default_value) {

		if (HasMagicEffectOr(actor, tag, default_value)) {
			return true;
		}

		if (IsTeammate(actor)) {
			auto player = PlayerCharacter::GetSingleton();
			return HasMagicEffectOr(player, tag, default_value);
		}

		return false;
		
	}

	bool Runtime::HasSpellTeam(Actor* actor, const std::string_view& tag) {
		return HasMagicEffectTeamOr(actor, tag, false);
	}

	bool Runtime::HasSpellTeamOr(Actor* actor, const std::string_view& tag, const bool& default_value) {

		if (HasSpellTeam(actor, tag)) {
			return true;
		}

		if (IsTeammate(actor)) {
			auto player = PlayerCharacter::GetSingleton();
			return HasSpellTeamOr(player, tag, default_value);
		}

		return default_value;
		
	}

	bool Runtime::HasPerkTeam(Actor* actor, const std::string_view& tag) {
		return HasPerkTeamOr(actor, tag, false);
	}

	bool Runtime::HasPerkTeamOr(Actor* actor, const std::string_view& tag, const bool& default_value) {

		if (HasPerk(actor, tag)) {
			return true;
		}

		if (IsTeammate(actor) || CountAsGiantess(actor)) {
			auto player = PlayerCharacter::GetSingleton();
			return HasPerkOr(player, tag, default_value);
		}

		return default_value;
		
	}


}