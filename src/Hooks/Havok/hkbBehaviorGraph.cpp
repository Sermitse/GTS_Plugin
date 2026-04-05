#include "Hooks/Havok/hkbBehaviorGraph.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Hooks/Util/HookUtil.hpp"

using namespace GTS;

namespace {

	float Animation_GetSpeedCorrection(Actor* actor) { // Fixes Hug animation de-sync by copying Gts anim speed to Tiny
		auto transient = Transient::GetActorData(actor);
		if (transient) {
			if (transient->HugAnimationSpeed < 1.0f) {
				return transient->HugAnimationSpeed;
			}
		} 
		return AnimationManager::GetAnimSpeed(actor);
	}
}

namespace Hooks {

	struct hkbBehaviorGraphUpdate {

		static constexpr size_t funcIndex = 0x05;

		static void thunk(hkbBehaviorGraph* a_this, const hkbContext& a_context, float a_timestep) {


			float anim_speed = 1.0f;

			{
				GTS_PROFILE_ENTRYPOINT("HavokBehavior::hkbBehaviorGraphUpdate");

				for (auto actor : find_actors()) {
					BSAnimationGraphManagerPtr animGraphManager;
					if (actor->GetAnimationGraphManager(animGraphManager)) {
						for (auto& graph : animGraphManager->graphs) {
							if (graph) {
								if (a_this == graph->behaviorGraph) {
									float multi = Animation_GetSpeedCorrection(actor);
									Perk_ApplyAccelerationPerk(actor, anim_speed);
									anim_speed *= multi;
								}
							}
						}
					}
				}
			}

			func(a_this, a_context, a_timestep * anim_speed);

		}

		FUNCTYPE_VFUNC func;

	};

	void Hook_hkbBehaviorGraph::Install() {
		logger::info("Installing hkbBehaviorGraph VTABLE Hooks...");
		stl::write_vfunc<hkbBehaviorGraphUpdate>(VTABLE_hkbBehaviorGraph[0]);
	}

}
