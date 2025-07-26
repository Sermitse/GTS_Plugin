#include "Hooks/Actor/Race.hpp"
#include "Hooks/Util/HookUtil.hpp"

namespace Hooks {

    struct SwitchRace {

        static void thunk(Actor* actor, TESRace* a_race, bool a_player) {

            GTS_PROFILE_ENTRYPOINT("ActorRace::SwitchRace");

            if (actor) {
                if (actor->formID == 0x14) { // Updates natural scale of Player when changing races
                    RefreshInitialScales(actor);
                }
            }

            return func(actor, a_race, a_player);
        }

		FUNCTYPE_DETOUR func;

    };



    void Hook_Race::Install() {

        logger::info("Installing Race Hooks...");
        stl::write_detour<SwitchRace>(REL::RelocationID(36901, 37925, NULL));

    }
}