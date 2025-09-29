#include "Config/SettingsModHandler.hpp"
#include "UI/ImGui/ImStyleManager.hpp"
#include "spdlog/spdlog.h"

#include "UI/UIManager.hpp"

namespace GTS {

	void HandleCameraTrackingReset() {

		if (!Config::General.bTrackBonesDuringAnim) {
			auto actors = find_actors();
			for (auto actor : actors) {
				if (actor) {
					ResetCameraTracking(actor);
				}
			}
		}
	}

	void HandleHHReset() {

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

	void HandleSettingsReset() {

		Config::ResetToDefaults();
		UIManager::WindowManager->ApplyStyle();


		spdlog::set_level(spdlog::level::from_str(Config::Advanced.sLogLevel));

		// ----- If You need to do something when settings get reset add it here.

		HandleHHReset();
		HandleCameraTrackingReset();

		Notify("Mod settings have been reset");
		logger::info("Mod Settings Reset");

	}

	void HandleSettingsRefresh() {
		UIManager::WindowManager->ApplyStyle();
		spdlog::set_level(spdlog::level::from_str(Config::Advanced.sLogLevel));
		HandleHHReset();
		HandleCameraTrackingReset();
		logger::trace("Settings Refreshed");
	}
}
