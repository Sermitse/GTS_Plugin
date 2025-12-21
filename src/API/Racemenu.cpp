#include "API/Racemenu.hpp"

namespace GTS {

	void Racemenu::ClearKeyOnAllActors(const char* key) {
		if (!iBodyMorphIntfc) return;
		ClearKeyVisitor visitor(iBodyMorphIntfc, key);
		iBodyMorphIntfc->VisitActors(visitor);
	}

	void Racemenu::Register() {

		logger::info("Getting SKEE Interface Map");
		iInterfaceMap = SKEE::GetInterfaceMap();	// Check if SKEE/RaceMenu is installed

		if (iInterfaceMap) {
			if (SKEE::IBodyMorphInterface* intfc = SKEE::GetBodyMorphInterface(iInterfaceMap)) {
				uint32_t version = intfc->GetVersion();
				logger::info("SKEE BodyMorph Interface v{}", version);
				iBodyMorphIntfc = intfc;
			}
			else {
				logger::error("Couldn't get SKEE BodyMorph Interface");
			}
		}
	}

	void Racemenu::SetMorph(RE::Actor* a_actor, const char* a_morphName, const float a_value, const char* a_morphKey, const bool a_immediate) {
		if (!a_actor || !iBodyMorphIntfc) return;
		if (!a_actor->Is3DLoaded()) return;

		//logger::info("Setting Bodymorph \"{}\" for actor {} to {} ", a_morphName, a_actor->formID, a_value);
		std::string Key = a_morphKey ? MorphKey + a_morphKey : MorphKey;
		iBodyMorphIntfc->SetMorph(a_actor, a_morphName, Key.c_str(), a_value);

		if (a_immediate) {
			ApplyMorphs(a_actor);
		}
	}

	float Racemenu::GetMorph(RE::Actor* a_actor, const char* a_morphName, const char* a_morphKey) {
		if (!a_actor || !iBodyMorphIntfc) return 0.0f;
		std::string Key = a_morphKey ? MorphKey + a_morphKey : MorphKey;
		return iBodyMorphIntfc->GetMorph(a_actor, a_morphName, a_morphKey);
	}

	//Warning this will erase all morphs on a character
	void Racemenu::ClearAllMorphs(RE::Actor* a_actor) {
		if (!a_actor || !iBodyMorphIntfc) return;
		iBodyMorphIntfc->ClearMorphs(a_actor);
		logger::trace("Cleared all racemenu morphs from actor {}", a_actor->formID);
	}

	//Warning this will erase all morphs done by this mod
	void Racemenu::ClearMorphs(RE::Actor* a_actor, const char* a_morphKey) {
		if (!a_actor || !iBodyMorphIntfc) return;
		std::string Key = a_morphKey ? MorphKey + a_morphKey : MorphKey;
		iBodyMorphIntfc->ClearBodyMorphKeys(a_actor, Key.c_str());
		logger::trace("Cleared all {} morphs from actor {}", a_morphKey, a_actor->formID);
	}

	//Remove a morph
	void Racemenu::ClearMorph(RE::Actor* a_actor, const char* a_morphName, const char* a_morphKey) {
		if (!a_actor || !iBodyMorphIntfc) return;
		std::string Key = a_morphKey ? MorphKey + a_morphKey : MorphKey;
		iBodyMorphIntfc->ClearMorph(a_actor, a_morphName, Key.c_str());
		logger::trace("Cleared morph \"{}\" from actor {}", a_morphName, a_actor->formID);
	}

	//Instruct racemenu to update this actor
	void Racemenu::ApplyMorphs(RE::Actor* a_actor) {
		if (!a_actor || !iBodyMorphIntfc) return;
		if (!a_actor->Is3DLoaded()) return;

		iBodyMorphIntfc->ApplyBodyMorphs(a_actor, true);
		iBodyMorphIntfc->UpdateModelWeight(a_actor, false);
		logger::trace("Do bodymorph update on actor {}", a_actor->formID);
	}

	bool Racemenu::Loaded() {
		return iBodyMorphIntfc != nullptr;
	}

	std::string Racemenu::DebugName() {
		return "::RacemenuAPI";
	}

	void Racemenu::OnPluginPostLoad() {
		Register();
	}
}
