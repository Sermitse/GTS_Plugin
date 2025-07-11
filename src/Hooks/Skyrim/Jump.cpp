#include "Hooks/Skyrim/Jump.hpp"

#include "Managers/Rumble.hpp"
#include "Managers/Damage/LaunchObject.hpp"

using namespace GTS;

namespace {

	constexpr float launch_up_radius = 24.0f;
	constexpr float default_gravity = 1.0f;

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

	void Hook_Jumping::Hook(Trampoline& trampoline) {


		static FunctionHook<float(bhkCharacterController* a_this)> GetFallDistance (
            REL::RelocationID(76430, 78269),
            [](auto* a_this){
            float result = GetFallDistance(a_this);
            auto actor = GetCharContActor(a_this);
            if (actor) {
                float scale = std::clamp(get_giantess_scale(actor), 1.0f, 99999.0f);
                if (scale > 1e-4) {
                    result /= scale;
                }
            }
			return result;
			});

		// AE 1402bc7c3
		// SE 1402aa40c
		//
		// Is used in the jump anim event handler
		//
		//REL::Relocation<uintptr_t> hook{REL::RelocationID(41811, 42892)};
		//_GetScaleJumpHook = trampoline.write_call<5>(hook.address() + RELOCATION_OFFSET(0x4d, 0x4d), GetScaleJumpHook);

		static FunctionHook<bool(IAnimationGraphManagerHolder* graph, const BSFixedString& a_variableName, const float a_in)> SkyrimSetGraphVarFloat( 
			REL::RelocationID(32143, 32887),
			[](auto* graph, const auto& a_variableName, auto a_in) {
				if (a_variableName == "VelocityZ") {
					if (a_in < 0) {
						auto actor = skyrim_cast<Actor*>(graph);
						if (actor) {
							const float CRITICALHEIGHT = 9.70f;
							const float ACTORHEIGHT = Characters_AssumedCharSize*70.0f;
							const float FACTOR = 0.20f;

							float gravity = 1.0f;

							if (auto controller = actor->GetCharController()) { // Fixes Jump Land anim threshold when altering gravity
								gravity = controller->gravity;
							}

							float scale = get_giantess_scale(actor);
							float newCriticalHeight = ACTORHEIGHT*scale*(FACTOR*gravity);
							float jump_factor = pow(CRITICALHEIGHT/newCriticalHeight,0.5f);

							a_in *= jump_factor;
						}
					}
				} 
				return SkyrimSetGraphVarFloat(graph, a_variableName, a_in);
			}
		);

		static CallHook<float(Actor* actor)> SkyrimJumpHeight(RELOCATION_ID(36271, 37257),  REL::Relocate(0x190, 0x17F),
			// SE: find offset : 0x1405d2110 - 0x1405d1f80  
			// So offset is = 0x190 .  36271 = 5D1F80
			[](auto* actor) {
				float result = SkyrimJumpHeight(actor);
				//log::info("Original jump height: {}", result);
				if (actor) {
					if (actor->formID == 0x14) {
						float size = get_giantess_scale(actor);

						float might = 1.0f + Potion_GetMightBonus(actor);
						float modifier = size * might; // Compensate it, since SetScale() already boosts jump height by default
						float scaled = std::clamp(modifier, 1.0f, 99999.0f); // Can't have smaller jump height than x1.0

						Jump_ApplyExtraJumpEffects(actor, size, might); // Push items and actors, spawn dust ring and shake the ground

						result *= scaled / game_getactorscale(actor);
					}
				}
				return result;
			}
		);
	}
}
