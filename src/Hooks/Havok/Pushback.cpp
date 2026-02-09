#include "Hooks/Havok/Pushback.hpp"
#include "Hooks/Util/HookUtil.hpp"

using namespace GTS;

namespace {

    float GetPushMult(Actor* giant) {
		float result = 1.0f;
		if (giant->IsPlayerRef() || IsTeammate(giant)) {
			auto tranData = Transient::GetActorData(giant);
			if (tranData) {
				result = tranData->PushForce;
			} else {
				float size = get_giantess_scale(giant);
				if (TinyCalamityActive(giant)) {
					size *= 2.5f;
				}
				result = std::clamp(1.0f / (size*size*size*size), 0.01f, 1.0f);
			}

			if (result <= 0.025f) {
				return 0.0f;
			}
		}

		return result;
	}
}

namespace Hooks {


	struct HavokPush {

		// Reduces amount of CharController being pushed back on being hit based on size
		// SE: DC0930
		// TODO Somwehow improve performance instead of looping through all actors
		static void thunk(bhkCharacterController* a_this, hkVector4& a_from, float a_time) {

			float scale = 1.0f;

			{
				GTS_PROFILE_ENTRYPOINT("HavokPushback::HavokPush");

				Actor* giant = GetCharContActor(a_this);
				
				if (giant) {
					scale = GetPushMult(giant);
				}

			}

			// Size difference is recorded only outside of TGM!
			// In TGM effect isn't applied because of that

			hkVector4 Push = hkVector4(a_from) * scale;

			return func(a_this, Push, a_time);
		}

		FUNCTYPE_DETOUR func;
	};

	struct PushActorAway {

		//TODO Use it to cache size difference between 2 actors
		//Cache size difference and then use it inside hook above
		static void thunk(AIProcess* a_this, Actor* a_target, NiPoint3& a_direction, float a_force) {

			{
				GTS_PROFILE_ENTRYPOINT("HavokPushback::PushActorAway");
			}

			return func(a_this, a_target, a_direction, a_force);
		}

		FUNCTYPE_DETOUR func;
	};

	void Hook_PushBack::Install() {

		logger::info("Installing Havok Push Hooks...");

		stl::write_detour<HavokPush>(REL::RelocationID(76442, 78282, NULL));
		//stl::write_detour<PushActorAway>(REL::RelocationID(38858, 39895, NULL));

    }
}