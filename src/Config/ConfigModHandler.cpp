#include "Config/ConfigModHandler.hpp"
#include "Config/Config.hpp"

#include "UI/ImGui/Core/ImStyleManager.hpp"
#include "spdlog/spdlog.h"

namespace GTS {

	//-------------
	// Handlers
	//-------------

	void ConfigModHandler::DoCameraStateReset() {

		if (!Config::General.bTrackBonesDuringAnim) {
			auto actors = find_actors();
			for (auto actor : actors) {
				if (actor) {
					ResetCameraTracking(actor);
				}
			}
		}
	}

	void ConfigModHandler::DoHighHeelStateReset() {

		if (!Config::General.bHighheelsFurniture) {

			auto actors = find_actors();

			for (auto actor : actors) {
				if (!actor) {
					return;
				}

				for (bool person : {false, true}) {
					auto npc_root_node = find_node(actor, "NPC", person);
					if (npc_root_node && actor->GetOccupiedFurniture()) {
						npc_root_node->local.translate.z = 0.0f;
						update_node(npc_root_node);
					}
				}
			}
		}
	}

	std::string ConfigModHandler::DebugName() {
		return "::ConfigModHandler";
	}

	void ConfigModHandler::OnGameSave() {
		logger::trace("ConfigModHandler OnGameSave");
	}

	void ConfigModHandler::OnGameLoaded() {

		ImStyleManager::ApplyStyle();
		spdlog::set_level(spdlog::level::from_str(Config::Advanced.sLogLevel));

		DoCameraStateReset();
		DoHighHeelStateReset();

		logger::trace("ConfigModHandler OnGameLoaded");
	}

	void ConfigModHandler::OnConfigReset() {
		Config::ResetToDefaults();
		ImStyleManager::ApplyStyle();

		spdlog::set_level(spdlog::level::from_str(Config::Advanced.sLogLevel));

		// ----- If You need to do something when settings get reset add it here.

		DoCameraStateReset();
		DoHighHeelStateReset();

		Notify("Mod Settings Have Been Reset");
		logger::info("ConfigModHandler OnConfigReset");
	}

	void ConfigModHandler::OnConfigRefresh() {
		OnGameLoaded();
	}
}
