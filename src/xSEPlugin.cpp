#include "API/TrainWreck.hpp"
#include "API/SmoothCam.hpp"

#include "Config/Config.hpp"
#include "Config/Keybinds.hpp"

#include "Hooks/Hooks.hpp"
#include "Papyrus/Papyrus.hpp"
#include "Debug/DebugDraw.hpp"
#include "Utils/InitUtils.hpp"

#include "Managers/Register.hpp"
#include "Managers/Input/InputManager.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Managers/Console/ConsoleManager.hpp"

#include "Utils/Logger.hpp"

using namespace SKSE;
using namespace RE;
using namespace RE::BSScript;
using namespace GTS;

namespace {

	void InitializeMessaging() {

		if (!GetMessagingInterface()->RegisterListener([](MessagingInterface::Message *message) {
			switch (message->type) {

				// Called after all kPostLoad message handlers have run.
				case MessagingInterface::kPostPostLoad: {
					//Racemenu::Register(); // <- Disabled For Now...
					break;
				}

				// All ESM/ESL/ESP plugins have loaded, main menu is now active.
				case MessagingInterface::kDataLoaded: {

					EventDispatcher::DoDataReady();
					InputManager::GetSingleton().Init();
					ConsoleManager::Init();
					SmoothCam::Register();
					Runtime::CheckSoftDependencies();

					CPrintPluginInfo();
					break;
				}

				// Skyrim game events.
				// Player's selected save game has finished loading.
				case MessagingInterface::kPostLoadGame: {
					Plugin::SetInGame(true);
					Cprint("[GTSPlugin.dll]: [ Succesfully initialized and loaded ]");
					break;
				}

				// Player starts a new game from main menu.
				case MessagingInterface::kNewGame: {
					Plugin::SetInGame(true);
					EventDispatcher::DoReset();
					Cprint("[GTSPlugin.dll]: [ Succesfully initialized and loaded ]");
					break;
				}

				// Player selected a game to load, but it hasn't loaded yet.
				// Data will be the name of the loaded save.
				case MessagingInterface::kPreLoadGame: {
					Plugin::SetInGame(false);
					EventDispatcher::DoReset();
					break;
				}

				default: {};
			}
		})) {
			ReportAndExit("Unable to register message listener.");
		}
	}

	void InitializeSerialization() {
		log::trace("Initializing cosave serialization...");

		auto* serde = GetSerializationInterface();
		serde->SetUniqueID(_byteswap_ulong('GTSP'));

		serde->SetSaveCallback(Persistent::OnGameSaved);
		serde->SetRevertCallback(Persistent::OnRevert);
		serde->SetLoadCallback(Persistent::OnGameLoaded);

		log::info("Cosave serialization initialized.");
	}

	void InitializePapyrus() {

		log::trace("Initializing Papyrus bindings...");

		if (GetPapyrusInterface()->Register(GTS::register_papyrus)) {
			log::info("Papyrus functions bound.");
		}
		else {
			ReportAndExit("Failure to register Papyrus bindings.");
		}
	}
}

SKSEPluginLoad(const LoadInterface * a_skse){

	Config::CopyLegacySettings();

	Init(a_skse);
	logger::Initialize();
	TrainWreck::Install();

#ifndef GTS_DISABLE_PLUGIN

	LogPrintPluginInfo();
	VersionCheck(a_skse);
	InitializeMessaging();
	Hooks::Install();
	InitializePapyrus();
	RegisterManagers();

#endif

	InitializeSerialization();

	logger::info("SKSEPluginLoad OK");

	return true;
}