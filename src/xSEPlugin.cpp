#include "API/Racemenu.hpp"
#include "API/SmoothCam.hpp"
#include "Hooks/Hooks.hpp"
#include "Papyrus/Papyrus.hpp"
#include "UI/DebugAPI.hpp"
#include "Config/Config.hpp"
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

				// Called after all plugins have finished running SKSEPlugin_Load.
				case MessagingInterface::kPostLoad: { 
					break;
				}

				// Called after all kPostLoad message handlers have run.
				case MessagingInterface::kPostPostLoad: {
					//Racemenu::Register(); // <- Disabled For Now...
					break;
				}

				// Called when all game data has been found.
				case MessagingInterface::kInputLoaded: {
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
				case MessagingInterface::kPreLoadGame:{
					Plugin::SetInGame(false);
					EventDispatcher::DoReset();
					break;
				}

				// The player has saved a game.
				case MessagingInterface::kSaveGame:{
					break;
				}

				// The player deleted a saved game from within the load menu.
				// Data will be the save name.
				case MessagingInterface::kDeleteGame:{
					break;
				} 
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

	void InitializeEventSystem() {

		EventDispatcher::AddListener(&DebugOverlayMenu::GetSingleton());
		EventDispatcher::AddListener(&Runtime::GetSingleton()); // Stores spells, globals and other important data
		EventDispatcher::AddListener(&Persistent::GetSingleton());
		EventDispatcher::AddListener(&Transient::GetSingleton());
		EventDispatcher::AddListener(&CooldownManager::GetSingleton());
		EventDispatcher::AddListener(&TaskManager::GetSingleton());
		EventDispatcher::AddListener(&SpringManager::GetSingleton());

		log::info("Added Default Listeners");

		RegisterManagers();
	}

}

SKSEPluginLoad(const LoadInterface * a_skse){

	#ifdef GTSDEBUG
		ReportInfo("GTSplugin Debug Build.\nAttach the debugger and press OK.");
	#endif

	Init(a_skse);

#ifndef GTS_DISABLE_PLUGIN

	logger::Initialize();
	LogPrintPluginInfo();

	if (logger::HasConsole()) {
		logger::SetLevel("Trace");
	}
	else {
		logger::LoadConfig();
	}

	VersionCheck(a_skse);
	InitializeMessaging();
	Hooks::Install();

	InitializePapyrus();
	InitializeEventSystem();

#endif

	InitializeSerialization();


	logger::info("SKSEPluginLoad OK");

	return true;
}