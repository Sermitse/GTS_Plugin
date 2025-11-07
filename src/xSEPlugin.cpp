#include "Hooks/Hooks.hpp"
#include "Systems/Events/EventRegistry.hpp"
#include "Papyrus/Papyrus.hpp"
#include "Utils/InitUtils.hpp"
#include "Utils/Logger.hpp"

namespace {

	void InitializeMessaging() {

		if (!SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message *message) {
			switch (message->type) {

				// Called after all kPostLoad message handlers have run.
				case SKSE::MessagingInterface::kPostPostLoad: {
					GTS::EventDispatcher::DoPluginPostLoad();
					break;
				}

				// All ESM/ESL/ESP plugins have loaded, main menu is now active.
				case SKSE::MessagingInterface::kDataLoaded: {
					GTS::EventDispatcher::DoDataReady();
					GTS::CPrintPluginInfo();
					break;
				}

				// Skyrim game events.
				// Player's selected save game has finished loading.
				case SKSE::MessagingInterface::kPostLoadGame: {
					GTS::State::SetInGame(true);
					GTS::Cprint("[GTSPlugin.dll]: [ Succesfully initialized and loaded ]");
					break;
				}

				// Player starts a new game from main menu.
				case SKSE::MessagingInterface::kNewGame: {
					GTS::State::SetInGame(true);
					GTS::EventDispatcher::DoReset();
					GTS::Cprint("[GTSPlugin.dll]: [ Succesfully initialized and loaded ]");
					break;
				}

				// Player selected a game to load, but it hasn't loaded yet.
				// Data will be the name of the loaded save.
				case SKSE::MessagingInterface::kPreLoadGame: {
					GTS::State::SetInGame(false);
					GTS::EventDispatcher::DoReset();
					break;
				}

				default: {};
			}
		})) {
			GTS::ReportAndExit("Unable to register message listener.");
		}
	}

	void InitializeSerialization() {

		auto* serde = SKSE::GetSerializationInterface();
		serde->SetUniqueID(_byteswap_ulong('GTSP'));
		serde->SetSaveCallback(GTS::Persistent::OnGameSaved);
		serde->SetRevertCallback(GTS::Persistent::OnRevert);
		serde->SetLoadCallback(GTS::Persistent::OnGameLoaded);

		logger::info("Cosave serialization initialized.");
	}

	void InitializePapyrus() {

		if (SKSE::GetPapyrusInterface()->Register(GTS::register_papyrus)) {
			logger::info("Papyrus functions bound");
			return;
		}
		GTS::ReportAndExit("Failure to register Papyrus bindings.");
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse){

	Init(a_skse);
	logger::Initialize();

	#ifndef GTS_DISABLE_PLUGIN

	GTS::LogPrintPluginInfo();
	GTS::VersionCheck(a_skse);
	InitializeMessaging();
	InitializePapyrus();
	Hooks::Install();
	GTS::RegisterEventListeners();

	#endif

	InitializeSerialization();

	logger::info("SKSEPluginLoad OK");

	return true;
}