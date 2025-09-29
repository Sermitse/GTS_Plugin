#include "Hooks/Actor/ActorValueOwner.hpp"
#include "Hooks/Util/HookUtil.hpp"
#include "Managers/Attributes.hpp"
#include "Managers/AI/AIFunctions.hpp"

namespace Hooks {

	struct GetActorValue {

		static constexpr std::size_t funcIndex = 0x01;

		template<int ID>
		static float thunk(ActorValueOwner* a_owner, ActorValue a_akValue) {

			float value = func<ID>(a_owner, a_akValue);

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("ActorValueOwner::GetActorValue", ID);

				const auto actor = skyrim_cast<Actor*>(a_owner);
				if (actor) {
					if (a_akValue == ActorValue::kCarryWeight) {
						value = AttributeManager::AlterCarryWeightAV(actor, a_akValue, value);
					}
					else if (a_akValue == ActorValue::kSpeedMult && actor->formID != 0x14) {
						value = GetNPCSpeedOverride(actor, value);
					}
				}
			}

			return value;
		}

		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;

	};

	//Unused
	struct GetPermanentActorValue {

		static constexpr std::size_t funcIndex = 0x02;

		template<int ID>
		static float thunk(ActorValueOwner* a_owner, ActorValue a_akValue) {

			float value = func<ID>(a_owner, a_akValue);

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("ActorValueOwner::GetPermanentActorValue", ID);
			}
			
			return value;
		}

		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;

	};

	struct GetBaseActorValue {

		static constexpr std::size_t funcIndex = 0x03;

		template<int ID>
		static float thunk(ActorValueOwner* a_owner, ActorValue a_akValue) {

			float value = func<ID>(a_owner, a_akValue);

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("ActorValueOwner::GetBaseActorValue", ID);

				const auto actor = skyrim_cast<Actor*>(a_owner);
				if (actor && actor->formID == 0x14) { // Player Exclusive
					value = AttributeManager::AlterGetBaseAv(actor, a_akValue, value);
				}

			}

			return value;
		}

		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;

	};

	struct SetBaseActorValue {

		static constexpr std::size_t funcIndex = 0x04;

		template<int ID>
		static void thunk(ActorValueOwner* a_owner, ActorValue a_akValue, float a_value) {

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("ActorValueOwner::SetBaseActorValue", ID);

				const auto actor = skyrim_cast<Actor*>(a_owner);
				if (actor) {
					a_value = AttributeManager::AlterSetBaseAv(actor, a_akValue, a_value);
				}
			}

			func<ID>(a_owner, a_akValue, a_value);

		}

		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;

	};

	void Hook_ActorValueOwner::Install() {

		logger::info("Installing ActorValueOwner VTABLE MultiHooks...");

		//stl::write_vfunc_unique<GetActorValue, 0>(VTABLE_Actor[5]);
		stl::write_vfunc_unique<GetActorValue, 1>(VTABLE_Character[5]);
		stl::write_vfunc_unique<GetActorValue, 2>(VTABLE_PlayerCharacter[5]);

		//Unused
		/*stl::write_vfunc_unique<GetPermanentActorValue, 0>(VTABLE_Actor[5]);
		stl::write_vfunc_unique<GetPermanentActorValue, 1>(VTABLE_Character[5]);
		stl::write_vfunc_unique<GetPermanentActorValue, 2>(VTABLE_Actor[5]);*/

		//stl::write_vfunc_unique<GetBaseActorValue, 0>(VTABLE_Actor[5]);
		stl::write_vfunc_unique<GetBaseActorValue, 1>(VTABLE_Character[5]);
		stl::write_vfunc_unique<GetBaseActorValue, 2>(VTABLE_PlayerCharacter[5]);

		//stl::write_vfunc_unique<SetBaseActorValue, 0>(VTABLE_Actor[5]);
		stl::write_vfunc_unique<SetBaseActorValue, 1>(VTABLE_Character[5]);
		stl::write_vfunc_unique<SetBaseActorValue, 2>(VTABLE_PlayerCharacter[5]);

	}
}
