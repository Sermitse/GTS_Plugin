#include "Hooks/Actor/ActorValueOwner.hpp"
#include "Hooks/Util/HookUtil.hpp"
#include "Managers/AttributeManager.hpp"
#include "Managers/AI/AIFunctions.hpp"
#include "Config/Config.hpp"

using namespace GTS;

namespace {

	bool IsHealingEffect(RE::ActorValue a_akValue) {
		bool health = a_akValue == ActorValue::kHealth;
		return health;
	}

	float ReduceHealingEfficiency(Actor* a_actor, ActorValue a_akValue, float original) {
		if (original > 0.0f) { // >0 means gaining positive health/regeneration, also protection against negative regen (being damaged)
			const bool isEnabled_NPC 	= 	!a_actor->IsPlayerRef()	&& Config::Balance.bReducedHealthRegeneration_NPC;
			const bool isEnabled_PC 	= 	a_actor->IsPlayerRef() 	&& Config::Balance.bReducedHealthRegeneration_PC;
		
			const bool isDowned = GetAV(a_actor, ActorValue::kHealth) < 0.0f; // Followers can go down, we don't want to increase recovery time
			const bool isAllowed = (isEnabled_NPC || isEnabled_PC);

			if (isAllowed && !isDowned) {
				const float size = std::max(get_visual_scale(a_actor), 1.0f);
				float reduction = original * (1.0f / size);
				logger::info("Reducing healing effect: {}, value pre: {}, value post: {}, size: {}", a_actor->GetDisplayFullName(), original, reduction, size);
				return reduction;
			}
		} else if (original < 0.0f) {
			logger::info("Original is < 0: {}", original);
		}
		
		return original;
	}
}

namespace Hooks {

	template<int ID>
	struct GetActorValue {

		static constexpr std::size_t funcIndex = 0x01;

		static float thunk(ActorValueOwner* a_owner, ActorValue a_akValue) {

			float value = func(a_owner, a_akValue);

			{
				//Unimportant to track gets barely called plus it
				//gets called By a bunch of game theads and polutes the profiler UI
				//GTS_PROFILE_ENTRYPOINT_UNIQUE("ActorValueOwner::GetActorValue", ID);

				const auto actor = skyrim_cast<Actor*>(a_owner);
				if (actor) {
					if (a_akValue == ActorValue::kCarryWeight) {
						value = AttributeManager::AlterCarryWeightAV(actor, a_akValue, value);
					}
					else if (a_akValue == ActorValue::kSpeedMult && !actor->IsPlayerRef()) {
						value = GetNPCSpeedOverride(actor, value);
					}
				}
			}

			return value;
		}

		FUNCTYPE_VFUNC func;

	};

	template<int ID>
	struct GetBaseActorValue {

		static constexpr std::size_t funcIndex = 0x03;

		static float thunk(ActorValueOwner* a_owner, ActorValue a_akValue) {

			float value = func(a_owner, a_akValue);

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("ActorValueOwner::GetBaseActorValue", ID);

				const auto actor = skyrim_cast<Actor*>(a_owner);
				if (actor && actor->IsPlayerRef()) { // Player Exclusive
					value = AttributeManager::AlterGetBaseAv(actor, a_akValue, value);
				}

			}

			return value;
		}

		FUNCTYPE_VFUNC func;

	};

	template<int ID>
	struct SetBaseActorValue {

		static constexpr std::size_t funcIndex = 0x04;

		
		static void thunk(ActorValueOwner* a_owner, ActorValue a_akValue, float a_value) {

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("ActorValueOwner::SetBaseActorValue", ID);

				const auto actor = skyrim_cast<Actor*>(a_owner);
				if (actor) {
					a_value = AttributeManager::AlterSetBaseAv(actor, a_akValue, a_value);
				}
			}

			func(a_owner, a_akValue, a_value);

		}

		FUNCTYPE_VFUNC func;

	};

	template<int ID>
	struct RestoreActorValue {

		static constexpr std::size_t funcIndex = 0x06;

		static void thunk(ActorValueOwner* a_owner, ACTOR_VALUE_MODIFIER a_modifier, ActorValue a_akValue, float a_value) {

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("ActorValueOwner::SetActorValue", ID);

				const auto actor = skyrim_cast<Actor*>(a_owner);
				if (actor && IsHealingEffect(a_akValue)) {
					a_value = ReduceHealingEfficiency(actor, a_akValue, a_value); // Nerf health regen on huge actors
				}
			}

			func(a_owner, a_modifier, a_akValue, a_value);
		}


		FUNCTYPE_VFUNC func;
	};

	void Hook_ActorValueOwner::Install() {

		logger::info("Installing ActorValueOwner VTABLE MultiHooks...");

		stl::write_vfunc_unique<GetActorValue, 1>(VTABLE_Character[5]);
		stl::write_vfunc_unique<GetActorValue, 2>(VTABLE_PlayerCharacter[5]);

		stl::write_vfunc_unique<GetBaseActorValue, 1>(VTABLE_Character[5]);
		stl::write_vfunc_unique<GetBaseActorValue, 2>(VTABLE_PlayerCharacter[5]);

		stl::write_vfunc_unique<SetBaseActorValue, 1>(VTABLE_Character[5]);
		stl::write_vfunc_unique<SetBaseActorValue, 2>(VTABLE_PlayerCharacter[5]);

		stl::write_vfunc_unique<RestoreActorValue, 1>(VTABLE_Character[5]);
		stl::write_vfunc_unique<RestoreActorValue, 2>(VTABLE_PlayerCharacter[5]);
	}
}
