#include "Data/Runtime.hpp"

#include "Config/Config.hpp"

namespace {

	bool CheckModLoaded(const std::string_view& a_name) {
		logger::info("Dependency Checker: Checking for {}", a_name);

		if (const auto DataHandler = RE::TESDataHandler::GetSingleton()) {
			const auto ModFile = DataHandler->LookupModByName(a_name);
			if (ModFile && ModFile->compileIndex != 0xFF) {
				logger::info("SoftDependency Checker: {} was Found.", a_name);
				return true;
			}
		}
		return false;
	}

	void CheckModLoaded(bool* a_res, const std::string_view& a_name) {
		logger::info("Dependency Checker: Checking for {}", a_name);
		if (a_res) {
			*a_res = CheckModLoaded(a_name);
			return;
		}
		*a_res = false;
	}

	bool CheckDLLLoaded(const std::wstring_view& a_name) {
		logger::info("Dependency DLL Checker: Checking for {}", GTS::Utf16ToUtf8(a_name));
		return GetModuleHandleW(a_name.data()) != nullptr; // DLL name
	}

	void CheckDLLLoaded(bool* a_res, const std::wstring_view& a_name) {
		logger::info("Dependency DLL Checker: Checking for {}", GTS::Utf16ToUtf8(a_name));
		if (a_res) {
			*a_res = CheckDLLLoaded(a_name);
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

	void PlaySoundImpl(RE::BSISoundDescriptor* a_soundDescriptor, RE::TESObjectREFR* a_ref, const float& a_volume, const float& a_frequency) {

		if (!a_soundDescriptor) {
			logger::error("Ivallid Sound Descriptor");
			return;
		}

		auto audioManager = RE::BSAudioManager::GetSingleton();
		if (!audioManager) {
			logger::error("Audio Manager invalid");
			return;
		}

		RE::BSSoundHandle soundHandle;
		if (audioManager->BuildSoundDataFromDescriptor(soundHandle, a_soundDescriptor)) {
			RE::ObjectRefHandle ref = a_ref->CreateRefHandle();
			if (RE::TESObjectREFR* refget = ref.get().get()) {

				soundHandle.SetVolume(a_volume);
				audioManager->SetSoundHandleFrequency(soundHandle.soundID, a_frequency);

				if (RE::NiAVObject* current_3d = refget->GetCurrent3D()) {
					RE::NiAVObject* follow = current_3d;
					soundHandle.SetObjectToFollow(follow);
					soundHandle.Play();
				}
			}
			return;
		}

		logger::error("Could not build sound");
	}

	void PlaySoundImplActor(RE::BSISoundDescriptor* a_soundDescriptor, RE::Actor* a_actor, const float& a_volume, const float& a_frequency) {
		if (a_actor) {
			if (RE::TESObjectREFR* ActorAsRef = skyrim_cast<RE::TESObjectREFR*>(a_actor)) {
				PlaySoundImpl(a_soundDescriptor, ActorAsRef, a_volume, a_frequency);
			}
		}
	}

	void PlaySoundAtNodeFallOffImpl(RE::BSISoundDescriptor* a_soundDescriptor, const float& a_volume, RE::NiAVObject* a_node, float a_falloff, float a_frequency) {

		if (!a_node) {
			logger::warn("Tried to play a sound on a null node");
			return;
		}

		if (!a_soundDescriptor) {
			logger::error("Ivallid Sound Descriptor");
			return;
		}

		auto audioManager = RE::BSAudioManager::GetSingleton();
		if (!audioManager) {
			logger::error("Audio Manager invalid");
			return;
		}

		RE::BSSoundHandle soundHandle;
		if (audioManager->BuildSoundDataFromDescriptor(soundHandle, a_soundDescriptor)) {
			float falloff = GTS::Sound_GetFallOff(a_node, a_falloff);
			soundHandle.SetVolume(a_volume * falloff);

			audioManager->SetSoundHandleFrequency(soundHandle.soundID, a_frequency);

			soundHandle.SetObjectToFollow(a_node);
			soundHandle.Play();
			return;
		}

		logger::error("Could not build sound");

	}

	void PlaySoundAtNodeImpl(RE::BSISoundDescriptor* a_soundDescriptor, const float& a_volume, RE::NiAVObject* a_node, float a_frequency) {

		if (!a_node) {
			logger::warn("Tried to play a sound on a null node");
			return;
		}

		if (!a_soundDescriptor) {
			logger::error("Ivallid Sound Descriptor");
			return;
		}

		auto audioManager = RE::BSAudioManager::GetSingleton();
		if (!audioManager) {
			logger::error("Audio Manager invalid");
			return;
		}

		RE::BSSoundHandle soundHandle;
		if (audioManager->BuildSoundDataFromDescriptor(soundHandle, a_soundDescriptor)) {
			soundHandle.SetVolume(a_volume);

			audioManager->SetSoundHandleFrequency(soundHandle.soundID, a_frequency);

			soundHandle.SetObjectToFollow(a_node);
			soundHandle.Play();

			return;
		}
		logger::error("Could not build sound");
	}

	template <class T>
	bool HasMagicEffectImpl(RE::Actor* a_actor, const T& a_entry) {
		if (!a_actor) return false;

		if (RE::EffectSetting* data = GTS::Runtime::GetMagicEffect(a_entry)) {
			return a_actor->AsMagicTarget()->HasMagicEffect(data);
		}

		return false;
	}

	void CreateExplosionAtPosImpl(RE::Actor* a_actor, RE::NiPoint3 a_pos, const float& a_scale, RE::BGSExplosion* data) {
		if (!data && !a_actor) {
			return;
		}

		RE::NiPointer<RE::TESObjectREFR> instance_ptr = a_actor->PlaceObjectAtMe(data, false);
		if (!instance_ptr) {
			return;
		}

		RE::Explosion* explosion = instance_ptr->AsExplosion();
		if (!explosion) {
			return;
		}

		explosion->SetPosition(a_pos);
		auto& runtime_data = explosion->GetExplosionRuntimeData();
		runtime_data.radius *= a_scale;
		runtime_data.imodRadius *= a_scale;
	}

	RE::TESObjectREFR* PlaceContainerAtPosImpl(RE::TESObjectREFR* a_ref, RE::NiPoint3 a_pos, RE::TESObjectCONT* a_container) {

		if (a_container) {

			RE::NiPointer<RE::TESObjectREFR> instance_ptr = a_ref->PlaceObjectAtMe(a_container, false);
			if (!instance_ptr) {
				return nullptr;
			}

			RE::TESObjectREFR* instance = instance_ptr.get();
			if (!instance) {
				return nullptr;
			}

			instance->SetPosition(a_pos);
			instance->data.angle.x = 0;
			instance->data.angle.y = 0;
			instance->data.angle.z = 0;
			return instance;
		}
		return nullptr;
	}

	RE::TESObjectREFR* PlaceContainerAtPosImplActor(RE::TESObjectREFR* a_actor, RE::NiPoint3 a_pos, RE::TESObjectCONT* a_container) {
		if (a_actor) {
			if (RE::TESObjectREFR* ActorAsRef = skyrim_cast<RE::TESObjectREFR*>(a_actor)) {
				return PlaceContainerAtPosImpl(ActorAsRef, a_pos, a_container);
			}
		}
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

		if (!CheckModLoaded("GTS.esp")) {
			ReportAndExit(
			  "GTS.esp was not found in the active load order.\n"
			  "Make sure it exists and is activated."
			);
		}

		if (CheckDLLLoaded(L"DynamicCollisionAdjustment.dll")) {
			ReportAndExit(
				"Dynamic Collision Adjustment (DCA) has been detected.\n"
				"As of version 3.5.0.0 DCA is no longer compatible with this mod.\n"
				"You must disable or delete \"DynamicCollisionAdjustment.dll\" in order to be able use this mod."
			);
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
	SpellItem* Runtime::GetSpell(const std::string_view& a_tag) {
		return GetFormByTagName<SpellItem>(SPEL, a_tag);
	}

	SpellItem* Runtime::GetSpell(const RuntimeData::RuntimeEntry<RE::SpellItem>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Perk
	BGSPerk* Runtime::GetPerk(const std::string_view& a_tag) {
		return GetFormByTagName<BGSPerk>(PERK, a_tag);
	}

	BGSPerk* Runtime::GetPerk(const RuntimeData::RuntimeEntry<RE::BGSPerk>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Explosion
	BGSExplosion* Runtime::GetExplosion(const std::string_view& a_tag) {
		return GetFormByTagName<BGSExplosion>(EXPL, a_tag);
	}

	BGSExplosion* Runtime::GetExplosion(const RuntimeData::RuntimeEntry<RE::BGSExplosion>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Global
	TESGlobal* Runtime::GetGlobal(const std::string_view& a_tag) {
		return GetFormByTagName<TESGlobal>(GLOB, a_tag);
	}

	TESGlobal* Runtime::GetGlobal(const RuntimeData::RuntimeEntry<RE::TESGlobal>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Quest
	TESQuest* Runtime::GetQuest(const std::string_view& a_tag) {
		return GetFormByTagName<TESQuest>(QUST, a_tag);
	}

	TESQuest* Runtime::GetQuest(const RuntimeData::RuntimeEntry<RE::TESQuest>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Faction
	TESFaction* Runtime::GetFaction(const std::string_view& a_tag) {
		return GetFormByTagName<TESFaction>(FACT, a_tag);
	}

	TESFaction* Runtime::GetFaction(const RuntimeData::RuntimeEntry<RE::TESFaction>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Impact Data Set
	BGSImpactDataSet* Runtime::GetImpactEffect(const std::string_view& a_tag) {
		return GetFormByTagName<BGSImpactDataSet>(IDTS, a_tag);
	}

	BGSImpactDataSet* Runtime::GetImpactEffect(const RuntimeData::RuntimeEntry<RE::BGSImpactDataSet>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Race
	TESRace* Runtime::GetRace(const std::string_view& a_tag) {
		return GetFormByTagName<TESRace>(RACE, a_tag);
	}

	TESRace* Runtime::GetRace(const RuntimeData::RuntimeEntry<RE::TESRace>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Keyword
	BGSKeyword* Runtime::GetKeyword(const std::string_view& a_tag) {
		return GetFormByTagName<BGSKeyword>(KYWD, a_tag);
	}

	BGSKeyword* Runtime::GetKeyword(const RuntimeData::RuntimeEntry<RE::BGSKeyword>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Leveled Item
	TESLevItem* Runtime::GetLeveledItem(const std::string_view& a_tag) {
		return GetFormByTagName<TESLevItem>(LVLI, a_tag);
	}

	TESLevItem* Runtime::GetLeveledItem(const RuntimeData::RuntimeEntry<RE::TESLevItem>& a_entry) {
		if (a_entry.Value) {
			return a_entry.Value;
		}
		return nullptr;
	}

	// ---- Container
	TESObjectCONT* Runtime::GetContainer(const std::string_view& a_tag) {
		return GetFormByTagName<TESObjectCONT>(CONT, a_tag);
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

	void Runtime::PlaySound(const std::string_view& a_tag, Actor* a_actor, const float& a_volume, const float& a_frequency) {
		PlaySoundImplActor(GetSound(a_tag), a_actor, a_volume, a_frequency);
	}

	void Runtime::PlaySound(const std::string_view& a_tag, TESObjectREFR* a_ref, const float& a_volume, const float& a_frequency) {
		PlaySoundImpl(GetSound(a_tag), a_ref, a_volume, a_frequency);
	}

	void Runtime::PlaySound(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry, Actor* a_actor, const float& a_volume, const float& a_frequency) {
		PlaySoundImplActor(GetSound(a_entry), a_actor, a_volume, a_frequency);
	}

	void Runtime::PlaySound(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry, TESObjectREFR* a_ref, const float& a_volume, const float& a_frequency) {
		PlaySoundImpl(GetSound(a_entry), a_ref, a_volume, a_frequency);
	}

	void Runtime::PlaySoundAtNode(const std::string_view& a_tag, Actor* a_actor, const float& a_volume, const std::string_view& a_node, float a_frequency) {
		PlaySoundAtNodeImpl(GetSound(a_tag), a_volume, find_node(a_actor, a_node), a_frequency);
	}

	void Runtime::PlaySoundAtNode(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry, const float& a_volume, NiAVObject* a_node, float a_frequency) {
		PlaySoundAtNodeImpl(GetSound(a_entry), a_volume, a_node, a_frequency);
	}

	void Runtime::PlaySoundAtNode(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry, Actor* a_actor, const float& a_volume, const std::string_view& a_node, float a_frequency) {
		PlaySoundAtNodeImpl(GetSound(a_entry), a_volume, find_node(a_actor, a_node), a_frequency);
	}

	void Runtime::PlaySoundAtNode(const std::string_view& a_tag, const float& a_volume, NiAVObject* a_node, float a_frequency) {
		PlaySoundAtNodeImpl(GetSound(a_tag), a_volume, a_node, a_frequency);
	}
	
	void Runtime::PlaySoundAtNode_FallOff(const std::string_view& a_tag, Actor* a_actor, const float& a_volume, const std::string_view& a_node, float a_falloff, float a_frequency) {
		PlaySoundAtNodeFallOffImpl(GetSound(a_tag), a_volume, find_node(a_actor, a_node), a_falloff, a_frequency);
	}

	void Runtime::PlaySoundAtNode_FallOff(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry, Actor* a_actor, const float& a_volume, const std::string_view& a_node, float a_falloff, float a_frequency) {
		PlaySoundAtNodeFallOffImpl(GetSound(a_entry), a_volume, find_node(a_actor, a_node), a_falloff, a_frequency);
	}

	void Runtime::PlaySoundAtNode_FallOff(const std::string_view& a_tag, const float& a_volume, NiAVObject* a_node, float a_falloff, float a_frequency) {
		PlaySoundAtNodeFallOffImpl(GetSound(a_tag), a_volume, a_node, a_falloff, a_frequency);
	}

	void Runtime::PlaySoundAtNode_FallOff(const RuntimeData::RuntimeEntry<RE::BGSSoundDescriptorForm>& a_entry, const float& a_volume, NiAVObject* a_node, float a_falloff, float a_frequency) {
		PlaySoundAtNodeFallOffImpl(GetSound(a_entry), a_volume, a_node, a_falloff, a_frequency);
	}

	//-----------------------
	// Magic Effect Helpers
	//-----------------------

	bool Runtime::HasMagicEffect(Actor* a_actor, const std::string_view& a_tag) {
		return HasMagicEffectImpl(a_actor, a_tag);
	}

	bool Runtime::HasMagicEffect(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::EffectSetting>& a_entry) {
		return HasMagicEffectImpl(a_actor, a_entry);
	}

	bool Runtime::HasMagicEffectTeam(Actor* a_actor, const std::string_view& a_tag) {
		if (HasMagicEffect(a_actor, a_tag)) {
			return true;
		}

		if (IsTeammate(a_actor)) {
			return HasMagicEffect(PlayerCharacter::GetSingleton(), a_tag);
		}

		return false;
	}

	bool Runtime::HasMagicEffectTeam(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::EffectSetting>& a_entry) {
		if (HasMagicEffect(a_actor, a_entry)) {
			return true;
		}

		if (IsTeammate(a_actor)) {
			return HasMagicEffect(PlayerCharacter::GetSingleton(), a_entry);
		}

		return false;
	}

	//-----------------------
	// Spell Helpers
	//-----------------------

	void Runtime::AddSpell(Actor* a_actor, const std::string_view& a_tag) {
		auto data = GetSpell(a_tag);
		if (data && !HasSpell(a_actor, a_tag)) {
			a_actor->AddSpell(data);
		}
	}

	void Runtime::AddSpell(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::SpellItem>& a_entry) {
		auto data = GetSpell(a_entry);
		if (data && !HasSpell(a_actor, a_entry)) {
			a_actor->AddSpell(data);
		}
	}

	void Runtime::RemoveSpell(Actor* a_actor, const std::string_view& a_tag) {
		auto data = GetSpell(a_tag);
		if (data && HasSpell(a_actor, a_tag)) {
			a_actor->RemoveSpell(data);
		}
	}

	void Runtime::RemoveSpell(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::SpellItem>& a_entry) {
		auto data = GetSpell(a_entry);
		if (data && HasSpell(a_actor, a_entry)) {
			a_actor->RemoveSpell(data);
		}
	}

	bool Runtime::HasSpell(Actor* a_actor, const std::string_view& a_tag) {
		auto data = GetSpell(a_tag);
		return data ? a_actor->HasSpell(data) : false;
	}

	bool Runtime::HasSpell(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::SpellItem>& a_entry) {
		auto data = GetSpell(a_entry);
		return data ? a_actor->HasSpell(data) : false;
	}

	bool Runtime::HasSpellTeam(Actor* a_actor, const std::string_view& a_tag) {
		if (HasSpell(a_actor, a_tag)) {
			return true;
		}
		if (IsTeammate(a_actor)) {
			return HasSpell(PlayerCharacter::GetSingleton(), a_tag);
		}

		return false;
	}

	bool Runtime::HasSpellTeam(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::SpellItem>& a_entry) {
		if (HasSpell(a_actor, a_entry)) {
			return true;
		}
		if (IsTeammate(a_actor)) {
			return HasSpell(PlayerCharacter::GetSingleton(), a_entry);
		}

		return false;
	}

	void Runtime::CastSpell(Actor* a_caster, Actor* a_target, const std::string_view& a_tag) {
		auto data = GetSpell(a_tag);
		if (data) {
			a_caster->GetMagicCaster(MagicSystem::CastingSource::kInstant)->CastSpellImmediate(data, false, a_target, 1.00f, false, 0.0f, a_caster);
		}
	}

	void Runtime::CastSpell(Actor* a_caster, Actor* a_target, const RuntimeData::RuntimeEntry<RE::SpellItem>& a_entry) {
		auto data = GetSpell(a_entry);
		if (data) {
			a_caster->GetMagicCaster(MagicSystem::CastingSource::kInstant)->CastSpellImmediate(data, false, a_target, 1.00f, false, 0.0f, a_caster);
		}
	}

	//-----------------------
	// Perk Helpers
	//-----------------------

	void Runtime::AddPerk(Actor* a_actor, const std::string_view& a_tag) {
		if (auto data = GetPerk(a_tag); data && !HasPerk(a_actor, a_tag)) {
			a_actor->AddPerk(data);
		}
	}

	void Runtime::AddPerk(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::BGSPerk>& a_entry) {
		if (auto data = GetPerk(a_entry); data && !HasPerk(a_actor, a_entry)) {
			a_actor->AddPerk(data);
		}
	}

	void Runtime::RemovePerk(Actor* a_actor, const std::string_view& a_tag) {
		if (auto data = GetPerk(a_tag); data && HasPerk(a_actor, a_tag)) {
			a_actor->RemovePerk(data);
		}
	}

	void Runtime::RemovePerk(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::BGSPerk>& a_entry) {
		if (auto data = GetPerk(a_entry); data && HasPerk(a_actor, a_entry)) {
			a_actor->RemovePerk(data);
		}
	}

	bool Runtime::HasPerk(Actor* a_actor, const std::string_view& a_tag) {
		if (auto data = GetPerk(a_tag)) {
			if (a_actor->HasPerk(data)) {
				return true;
			}

			if (a_actor->GetActorBase()->GetPerkIndex(data).has_value()) {
				return true;
			}

		}
		return false;
	}

	bool Runtime::HasPerk(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::BGSPerk>& a_entry) {
		if (auto data = GetPerk(a_entry)) {

			if (a_actor->HasPerk(data)) {
				return true;
			}
			if (a_actor->GetActorBase()->GetPerkIndex(data).has_value()) {
				return true;
			}
		}
		return false;
	}

	bool Runtime::HasPerkTeam(Actor* a_actor, const std::string_view& a_tag) {
		if (HasPerk(a_actor, a_tag)) {
			return true;
		}

		if (Config::Balance.bSharePerks && (IsTeammate(a_actor) || CountAsGiantess(a_actor))) {
			return HasPerk(PlayerCharacter::GetSingleton(), a_tag);
		}
		return false;
	}

	bool Runtime::HasPerkTeam(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::BGSPerk>& a_entry) {

		if (HasPerk(a_actor, a_entry)) {
			return true;
		}

		if (Config::Balance.bSharePerks && (IsTeammate(a_actor) || CountAsGiantess(a_actor))) {
			return HasPerk(PlayerCharacter::GetSingleton(), a_entry);
		}
		return false;
	}

	//-----------------------
	// Explosion Helpers
	//-----------------------

	void Runtime::CreateExplosion(Actor* a_actor, const float& a_scale, const std::string_view& a_tag) {
		if (a_actor) {
			CreateExplosionAtPos(a_actor, a_actor->GetPosition(), a_scale, a_tag);
		}
	}

	void Runtime::CreateExplosion(Actor* a_actor, const float& a_scale, const RuntimeData::RuntimeEntry<RE::BGSExplosion>& a_entry) {
		if (a_actor) {
			CreateExplosionAtPos(a_actor, a_actor->GetPosition(), a_scale, a_entry);
		}
	}

	void Runtime::CreateExplosionAtNode(Actor* a_actor, const std::string_view& a_nodeName, const float& a_scale, const std::string_view& a_tag) {
		if (a_actor && a_actor->Is3DLoaded()) {
			if (auto model = a_actor->GetCurrent3D()) {
				if (auto node = model->GetObjectByName(std::string(a_nodeName))) {
					CreateExplosionAtPos(a_actor, node->world.translate, a_scale, a_tag);
				}
			}
		}
	}

	void Runtime::CreateExplosionAtNode(Actor* a_actor, const std::string_view& a_nodeName, const float& a_scale, const RuntimeData::RuntimeEntry<RE::BGSExplosion>& a_entry) {
		if (a_actor && a_actor->Is3DLoaded()) {
			if (auto model = a_actor->GetCurrent3D()) {
				if (auto node = model->GetObjectByName(std::string(a_nodeName))) {
					CreateExplosionAtPos(a_actor, node->world.translate, a_scale, a_entry);
				}
			}
		}
	}

	void Runtime::CreateExplosionAtPos(Actor* a_actor, NiPoint3 a_pos, const float& a_scale, const std::string_view& a_tag) {
		CreateExplosionAtPosImpl(a_actor, a_pos, a_scale, GetExplosion(a_tag));
	}

	void Runtime::CreateExplosionAtPos(Actor* a_actor, NiPoint3 a_pos, const float& a_scale, const RuntimeData::RuntimeEntry<RE::BGSExplosion>& a_entry) {
		CreateExplosionAtPosImpl(a_actor, a_pos, a_scale, GetExplosion(a_entry));
	}

	//-----------------------
	// Global Helpers
	//-----------------------

	bool Runtime::GetBool(const std::string_view& a_tag) {
		if (auto data = GetGlobal(a_tag)) {
			return fabs(data->value - 0.0f) > 1e-4;
		}
		return false;
	}

	bool Runtime::GetBool(const RuntimeData::RuntimeEntry<RE::TESGlobal>& a_entry) {
		if (auto data = GetGlobal(a_entry)) {
			return fabs(data->value - 0.0f) > 1e-4;
		}
		return false;
	}

	void Runtime::SetBool(const std::string_view& a_tag, const bool& a_value) {
		if (auto data = GetGlobal(a_tag)) {
			data->value = a_value ? 1.0f : 0.0f;
		}
	}

	void Runtime::SetBool(const RuntimeData::RuntimeEntry<RE::TESGlobal>& a_entry, const bool& a_value) {
		if (auto data = GetGlobal(a_entry)) {
			data->value = a_value ? 1.0f : 0.0f;
		}
	}

	int Runtime::GetInt(const std::string_view& a_tag) {
		if (auto data = GetGlobal(a_tag)) {
			return static_cast<int>(data->value);
		}
		return 0;
	}

	int Runtime::GetInt(const RuntimeData::RuntimeEntry<RE::TESGlobal>& a_entry) {
		if (auto data = GetGlobal(a_entry)) {
			return static_cast<int>(data->value);
		}
		return 0;
	}

	void Runtime::SetInt(const std::string_view& a_tag, const int& a_value) {
		if (auto data = GetGlobal(a_tag)) {
			data->value = static_cast<float>(a_value);
		}
	}

	void Runtime::SetInt(const RuntimeData::RuntimeEntry<RE::TESGlobal>& a_entry, const int& a_value) {
		if (auto data = GetGlobal(a_entry)) {
			data->value = static_cast<float>(a_value);
		}
	}

	float Runtime::GetFloat(const std::string_view& a_tag) {
		if (auto data = GetGlobal(a_tag)) {
			return data->value;
		}
		return 0.0f;
	}

	float Runtime::GetFloat(const RuntimeData::RuntimeEntry<RE::TESGlobal>& a_entry) {
		if (auto data = GetGlobal(a_entry)) {
			return data->value;
		}
		return 0.0f;
	}

	void Runtime::SetFloat(const std::string_view& a_tag, const float& a_value) {
		if (auto data = GetGlobal(a_tag)) {
			data->value = a_value;
		}
	}

	void Runtime::SetFloat(const RuntimeData::RuntimeEntry<RE::TESGlobal>& a_entry, const float& a_value) {
		if (auto data = GetGlobal(a_entry)) {
			data->value = a_value;
		}
	}

	//-----------------------
	// Quest Helpers
	//-----------------------

	std::uint16_t Runtime::GetStage(const std::string_view& a_tag) {
		if (auto data = GetQuest(a_tag)) {
			return data->GetCurrentStageID();
		}
		return 0;
	}

	std::uint16_t Runtime::GetStage(const RuntimeData::RuntimeEntry<RE::TESQuest>& a_entry) {
		if (auto data = GetQuest(a_entry)) {
			return data->GetCurrentStageID();
		}
		return 0;
	}

	//-----------------------
	// Faction Helpers
	//-----------------------

	bool Runtime::InFaction(Actor* a_actor, const std::string_view& a_tag) {
		if (auto data = GetFaction(a_tag); a_actor) {
			return a_actor->IsInFaction(data);
		}
		return false;
	}

	bool Runtime::InFaction(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::TESFaction>& a_entry) {
		if (auto data = GetFaction(a_entry); a_actor) {
			return a_actor->IsInFaction(data);
		}
		return false;
	}

	//-----------------------
	// Impact Helpers
	//-----------------------

	void Runtime::PlayImpactEffect(Actor* a_actor, const std::string_view& a_tag, const std::string_view& a_node, NiPoint3 a_pickDirection, const float& a_length, const bool& a_applyRotation, const bool& a_useLocalRotation) {
		if (auto data = GetImpactEffect(a_tag); a_actor) {
			if (auto impact = BGSImpactManager::GetSingleton()) {
				impact->PlayImpactEffect(a_actor, data, a_node, a_pickDirection, a_length, a_applyRotation, a_useLocalRotation);
			}
		}
	}

	void Runtime::PlayImpactEffect(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::BGSImpactDataSet>& a_entry, const std::string_view& a_node, NiPoint3 a_pickDirection, const float& a_length, const bool& a_applyRotation, const bool& a_useLocalRotation) {
		if (auto data = GetImpactEffect(a_entry); a_actor) {
			if (auto impact = BGSImpactManager::GetSingleton()) {
				impact->PlayImpactEffect(a_actor, data, a_node, a_pickDirection, a_length, a_applyRotation, a_useLocalRotation);
			}
		}
	}

	//-----------------------
	// Race Helpers
	//-----------------------

	bool Runtime::IsRace(Actor* a_actor, const std::string_view& a_tag) {
		if (auto data = GetRace(a_tag); a_actor) {
			return a_actor->GetRace() == data;
		}
		return false;
	}

	bool Runtime::IsRace(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::TESRace>& a_entry) {
		if (auto data = GetRace(a_entry); a_actor) {
			return a_actor->GetRace() == data;
		}
		return false;
	}

	//-----------------------
	// Keyword Helpers
	//-----------------------

	bool Runtime::HasKeyword(Actor* a_actor, const std::string_view& a_tag) {
		if (auto data = GetKeyword(a_tag); a_actor) {
			return a_actor->HasKeyword(data);
		}
		return false;
	}

	bool Runtime::HasKeyword(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::BGSKeyword>& a_entry) {
		if (auto data = GetKeyword(a_entry); a_actor) {
			return a_actor->HasKeyword(data);
		}
		return false;
	}

	//-----------------------
	// Container Helpers
	//-----------------------

	TESObjectREFR* Runtime::PlaceContainer(Actor* a_actor, const std::string_view& a_tag) {
		if (auto data = GetContainer(a_tag); a_actor) {
			return PlaceContainerAtPosImplActor(a_actor, a_actor->GetPosition(), data);
		}
		return nullptr;
	}

	TESObjectREFR* Runtime::PlaceContainer(Actor* a_actor, const RuntimeData::RuntimeEntry<RE::TESObjectCONT>& a_entry) {
		if (auto data = GetContainer(a_entry); a_actor) {
			return PlaceContainerAtPosImplActor(a_actor, a_actor->GetPosition(), data);
		}
		return nullptr;
	}

	TESObjectREFR* Runtime::PlaceContainer(TESObjectREFR* a_object, const std::string_view& a_tag) {
		if (auto data = GetContainer(a_tag); a_object) {
			return PlaceContainerAtPosImpl(a_object, a_object->GetPosition(), data);
		}
		return nullptr;
	}

	TESObjectREFR* Runtime::PlaceContainer(TESObjectREFR* a_object, const RuntimeData::RuntimeEntry<RE::TESObjectCONT>& a_entry) {
		if (auto data = GetContainer(a_entry); a_object) {
			return PlaceContainerAtPosImpl(a_object, a_object->GetPosition(), data);
		}
		return nullptr;
	}

	TESObjectREFR* Runtime::PlaceContainerAtPos(Actor* a_actor, NiPoint3 a_pos, const std::string_view& a_tag) {
		if (auto data = GetContainer(a_tag); a_actor) {
			return PlaceContainerAtPosImplActor(a_actor, a_pos, data);
		}
		return nullptr;
	}

	TESObjectREFR* Runtime::PlaceContainerAtPos(Actor* a_actor, NiPoint3 a_pos, const RuntimeData::RuntimeEntry<RE::TESObjectCONT>& a_entry) {
		if (auto data = GetContainer(a_entry); a_actor) {
			return PlaceContainerAtPosImplActor(a_actor, a_pos, data);
		}
		return nullptr;
	}

	TESObjectREFR* Runtime::PlaceContainerAtPos(TESObjectREFR* a_object, NiPoint3 a_pos, const std::string_view& a_tag) {
		if (auto data = GetContainer(a_tag); a_object) {
			return PlaceContainerAtPosImpl(a_object, a_pos, data);
		}
		return nullptr;
	}

	TESObjectREFR* Runtime::PlaceContainerAtPos(TESObjectREFR* a_object, NiPoint3 a_pos, const RuntimeData::RuntimeEntry<RE::TESObjectCONT>& a_entry) {
		if (auto data = GetContainer(a_entry); a_object) {
			return PlaceContainerAtPosImpl(a_object, a_pos, data);
		}
		return nullptr;
	}

	//-----------------------
	// Dependency Checks
	//-----------------------

	bool Runtime::IsSexlabInstalled() {
		return SexlabInstalled;
	}

	bool Runtime::IsSurvivalModeInstalled() {
		return SurvivalModeInstalled;
	}

	bool Runtime::IsDevourmentInstalled() {
		return DevourmentInstalled;
	}

	bool Runtime::IsAltConversationCamInstalled() {
		return AltConversationCamInstalled;
	}
}
