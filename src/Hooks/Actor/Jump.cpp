#include "Hooks/Actor/Jump.hpp"
#include "Managers/Rumble.hpp"
#include "Managers/Damage/LaunchObject.hpp"
#include "Hooks/Util/HookUtil.hpp"

using namespace GTS;

namespace {

	constexpr float launch_up_radius = 24.0f;
	constexpr float default_gravity = 1.0f;

	float Jump_GetFallDamageReductionMult(Actor* actor) {
		bool HasPerk = Runtime::HasPerkTeam(actor, "GTSPerkCruelFall");
		
		return HasPerk ? 0.1f : 0.0f;
	}

	void Jump_ApplyExtraJumpEffects(Actor* actor, float size, float Might) {
		if (!actor->IsInMidair()) {
			NiPoint3 pos = actor->GetPosition(); 
			pos.z += 4.0f; //shift it up a little

			if (HasSMT(actor)) {
				size += 2.8f;
			}

			float calc_radius = ((54.0f / 3.0f) * size) - 54.0f;
			float stagger_radius = std::clamp(calc_radius, 0.0f, 54.0f); // Should start to appear at the scale of x3.0

			if (stagger_radius > 1.0f) {
				
				float power = stagger_radius / 54;

				std::vector<NiPoint3> position = {
					pos,
				};

				SpawnParticle(actor, 6.00f, "GTS/Effects/TinyCalamity.nif", NiMatrix3(), pos, size * power * 2.0f, 7, nullptr);
				PushObjectsUpwards(actor, position, stagger_radius * size * Might * power, 1.25f * power, true); // Launch cabbages and stuff up
				StaggerActor_Around(actor, stagger_radius * Might, true); // Launch actors up, Radius is scaled inside the function

				//log::info("Jump Power: {}", power);
				//log::info("Jump Radius: {}", stagger_radius);

				Rumbling::Once("MassiveJump", actor, Rumble_Default_MassiveJump * power * Might, 0.035f * power);
			}
		}
	}
}

namespace Hooks {

	struct GetFallDistance {

		static float thunk(bhkCharacterController* a_this) {


			GTS_PROFILE_ENTRYPOINT("ActorJump::GetFallDistance");

			float result = func(a_this);
			auto actor = GetCharContActor(a_this);

			if (actor) {
				const float scale = std::clamp(get_giantess_scale(actor), 1.0f, 99999.0f);
				const float fall_damage_exponent = 0.5f + Jump_GetFallDamageReductionMult(actor);
				if (scale > 1e-4) {
					result /= std::pow(scale, fall_damage_exponent);
				}
			}
			return result;
		}

		FUNCTYPE_DETOUR func;

	};

	struct SetGraphVarialbleFloat {

		static bool thunk(IAnimationGraphManagerHolder* a_graph, const BSFixedString& a_variableName, float a_in) {

			GTS_PROFILE_ENTRYPOINT("ActorJump::SetGraphVarialbleFloat");

			if (a_variableName == "VelocityZ") {

				if (a_in < 0) {
					auto actor = skyrim_cast<Actor*>(a_graph);
					if (actor) {

						constexpr float CRITICALHEIGHT = 9.70f;
						constexpr float ACTORHEIGHT = Characters_AssumedCharSize * 70.0f;
						constexpr float FACTOR = 0.20f;
						float gravity = 1.0f;

						if (auto controller = actor->GetCharController()) { // Fixes Jump Land anim threshold when altering gravity
							gravity = controller->gravity;
						}

						float scale = get_giantess_scale(actor);
						float newCriticalHeight = ACTORHEIGHT * scale * (FACTOR * gravity);
						const float jump_factor = pow(CRITICALHEIGHT / newCriticalHeight, 0.5f);

						a_in *= jump_factor;
					}
				}
			}
			return func(a_graph, a_variableName, a_in);
		}

		FUNCTYPE_DETOUR func;

	};

	struct JumpHeight {

		// SE: find offset : 0x1405d2110 - 0x1405d1f80  
		// So offset is = 0x190 .  36271 = 5D1F80

		static float thunk(Actor* a_actor) {

			GTS_PROFILE_ENTRYPOINT("ActorJump::JumpHeight");

			float result = func(a_actor);
			//log::info("Original jump height: {}", result);
			if (a_actor) {
				if (a_actor->formID == 0x14) {
					float size = get_giantess_scale(a_actor);

					const float might = 1.0f + Potion_GetMightBonus(a_actor);
					const float modifier = size * might; // Compensate it, since SetScale() already boosts jump height by default
					const float scaled = std::clamp(modifier, 1.0f, 99999.0f); // Can't have smaller jump height than x1.0

					Jump_ApplyExtraJumpEffects(a_actor, size, might); // Push items and actors, spawn dust ring and shake the ground

					result *= scaled / game_getactorscale(a_actor);
				}
			}
			return result;
		}

		FUNCTYPE_CALL func;

	};

	void Hook_Jump::Install() {
		logger::info("Installing Jump Hooks...");
		stl::write_detour<GetFallDistance>(REL::RelocationID(76430, 78269, NULL));
		stl::write_detour<SetGraphVarialbleFloat>(REL::RelocationID(32143, 32887, NULL));
		stl::write_call<JumpHeight>(REL::RelocationID(36271, 37257, NULL), REL::VariantOffset(0x190, 0x17F, NULL));
	}

}
