#include "API/Racemenu.hpp"

namespace GTS {

	void Racemenu::Register() {

		logger::info("Registering SKEE BodymorphInterface API");

		SKEE::InterfaceExchangeMessage msg;

		const auto* const intfc{ SKSE::GetMessagingInterface() };

		intfc->Dispatch(SKEE::InterfaceExchangeMessage::kExchangeInterface, &msg, sizeof(SKEE::InterfaceExchangeMessage*), "SKEE");

		if (!msg.interfaceMap) {
			logger::error("Couldn't Get SKSE interface map.");
			return;
		}

		RaceMenuInterface = static_cast<SKEE::IBodyMorphInterface*>(msg.interfaceMap->QueryInterface("BodyMorph"));

		if (!RaceMenuInterface) {
			logger::warn("Couldn't get SKEE interface.");
			return;
		}

		logger::info("SKEE BodyMorhInterface Version {}", RaceMenuInterface->GetVersion());
	}

	void Racemenu::SetMorph(RE::Actor* a_actor, const char* a_morphName, const float a_value, const bool a_immediate) {
		if (!a_actor || !RaceMenuInterface) return;
		if (!a_actor->Is3DLoaded()) return;

		//logger::info("Setting Bodymorph \"{}\" for actor {} to {} ", a_morphName, a_actor->formID, a_value);
		RaceMenuInterface->SetMorph(a_actor, a_morphName, MorphKey.c_str(), a_value);

		if (a_immediate)
			ApplyMorphs(a_actor);
	}

	float Racemenu::GetMorph(RE::Actor* a_actor, const char* a_morphName) {
		if (!a_actor || !RaceMenuInterface) return 0.0f;
		return RaceMenuInterface->GetMorph(a_actor, a_morphName, MorphKey.c_str());
	}

	//Warning this will erase all morphs on a character
	void Racemenu::ClearAllMorphs(RE::Actor* a_actor) {
		if (!a_actor || !RaceMenuInterface) return;
		RaceMenuInterface->ClearMorphs(a_actor);
		logger::trace("Cleared all racemenu morphs from actor {}", a_actor->formID);
	}

	//Warning this will erase all morphs done by this mod
	void Racemenu::ClearMorphs(RE::Actor* a_actor) {
		if (!a_actor || !RaceMenuInterface) return;
		RaceMenuInterface->ClearBodyMorphKeys(a_actor, MorphKey.c_str());
		logger::trace("Cleared all {} morphs from actor {}", MorphKey.c_str(), a_actor->formID);
	}

	//Remove a morph
	void Racemenu::ClearMorph(RE::Actor* a_actor, const char* a_morphName) {
		if (!a_actor || !RaceMenuInterface) return;
		RaceMenuInterface->ClearMorph(a_actor, a_morphName, MorphKey.c_str());
		logger::trace("Cleared morph \"{}\" from actor {}", a_morphName, a_actor->formID);
	}

	//Instruct racemenu to update this actor
	void Racemenu::ApplyMorphs(RE::Actor* a_actor) {
		if (!a_actor || !RaceMenuInterface) return;
		if (!a_actor->Is3DLoaded()) return;

		RaceMenuInterface->ApplyBodyMorphs(a_actor, true);
		RaceMenuInterface->UpdateModelWeight(a_actor, false);
		logger::trace("Do bodymorph update on actor {}", a_actor->formID);
	}

	bool Racemenu::Loaded() {
		return RaceMenuInterface != nullptr;
	}

	std::string Racemenu::DebugName() {
		return "::RacemenuAPI";
	}

	void Racemenu::OnPluginPostLoad() {
		//Register(); // <- Disabled For Now...
	}
}
