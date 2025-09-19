#include "Hooks/Havok/hkbBehaviorGraph.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Hooks/Util/HookUtil.hpp"

using namespace GTS;

namespace {

	float Animation_GetSpeedCorrection(Actor* actor) { // Fixes Hug animation de-sync by copying Gts anim speed to Tiny
		auto transient = Transient::GetSingleton().GetData(actor);
		if (transient) {
			if (transient->HugAnimationSpeed < 1.0f) {
				return transient->HugAnimationSpeed;
			}
		} 
		return AnimationManager::GetAnimSpeed(actor);
	}

	void AffectByPerk(Actor* giant, float& anim_speed) {
		auto data = Transient::GetSingleton().GetActorData(giant);
		if (data) {
			float speed = data->PerkBonusSpeed;
			if (speed > 1.0f) {
				bool CanApply = IsStomping(giant) || IsFootGrinding(giant) || IsVoring(giant) || IsTrampling(giant);
				if (CanApply) {
					anim_speed *= speed;
				}
			}
		}
	}
}

namespace Hooks {

	struct hkbBehaviorGraphUpdate {

		static constexpr size_t funcIndex = 0x05;

		static void thunk(hkbBehaviorGraph* a_this, const hkbContext& a_context, float a_timestep) {

			GTS_PROFILE_ENTRYPOINT("HavokBehavior::hkbBehaviorGraphUpdate");

			float anim_speed = 1.0f;

			for (auto actor : find_actors()) {
				BSAnimationGraphManagerPtr animGraphManager;
				if (actor->GetAnimationGraphManager(animGraphManager)) {
					for (auto& graph : animGraphManager->graphs) {
						if (graph) {
							if (a_this == graph->behaviorGraph) {
								float multi = Animation_GetSpeedCorrection(actor);
								AffectByPerk(actor, anim_speed);
								anim_speed *= multi;
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
		log::info("Installing hkbBehaviorGraph VTABLE Hooks...");
		stl::write_vfunc<hkbBehaviorGraphUpdate>(VTABLE_hkbBehaviorGraph[0]);
	}

}
