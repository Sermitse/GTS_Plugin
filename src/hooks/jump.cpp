#include "hooks/jump.hpp"
#include "hooks/callhook.hpp"
#include "scale/scale.hpp"
#include "data/plugin.hpp"
#include "managers/Attributes.hpp"
#include "utils/actorUtils.hpp"
#include "scale/modscale.hpp"

using namespace RE;
using namespace SKSE;

namespace Hooks {

	void Hook_Jumping::Hook(Trampoline& trampoline) {


		static FunctionHook<float(bhkCharacterController* a_this)> GetFallDistance(
			REL::RelocationID(76430, 78269),
			[](auto* a_this){
			float result = GetFallDistance(a_this);
			auto actor = GetCharContActor(a_this);
			if (actor) {
				if (actor->formID == 0x14) {// Apply to Player only
					float scale = get_giantess_scale(actor);
					if (scale > 1e-4) {
						result /= scale;
					}
				}
			}

			return result;
			}
		);

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
							const float CRITICALHEIGHT = 9.70;
							const float ACTORHEIGHT = 1.82*70.0;
							const float FACTOR = 0.20;
							float scale = get_giantess_scale(actor);
							float newCriticalHeight = ACTORHEIGHT*scale*FACTOR;

							float jump_factor = pow(CRITICALHEIGHT/newCriticalHeight,0.5);
							
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
						float Might = 1.0 + Potion_GetMightBonus(actor);
						float modifier = (get_giantess_scale(actor) / game_getactorscale(actor)) * Might; // Compensate it, since SetScale() already boosts jump height by default
						float scale = std::clamp(modifier, 0.8f, 99999.0f);
						log::info("Jump Result: {}", result);
						result *= scale;
						log::info("Jump Multiplier: {}", scale);

						if (!actor->IsInMidair()) {
							SpawnParticle(actor, 6.00, "GTS/Effects/TinyCalamity.nif", NiMatrix3(), actor->GetPosition(), scale * 3.0, 7, nullptr);
							GRumble::Once("MassiveJump", actor, 14.0 * Might, 0.20);
							StaggerActor_Around(actor, 48 * Might, true);
						}
					}
					//log::info("Value: {}", result);
				}
				return result;
			}
		);
	}

	

	/*float Hook_Jumping::GetScaleJumpHook(TESObjectREFR* a_this) {
		float result = _GetScaleJumpHook(a_this);
		Actor* actor = skyrim_cast<Actor*>(a_this);
		if (actor) {
			float scale = get_visual_scale(actor);
			if (scale > 1e-4) {
				log::info("Jump Hook: {} for {}", scale, actor->GetDisplayFullName());
				result *= scale;
			}
		}
		return result;
	}*/
}
