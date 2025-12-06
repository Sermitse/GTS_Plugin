#include "Hooks/Actor/Actor.hpp"
#include "Hooks/Util/HookUtil.hpp"
#include "Managers/AttributeManager.hpp"
#include "Managers/Damage/SizeHitEffects.hpp"

namespace Hooks {

	struct HandleHealthDamage {

		static inline const std::size_t funcIndex = REL::Relocate(0x104, 0x104, 0x106);

		template<int ID>
		static void thunk(RE::Actor* a_this, RE::Actor* a_attacker, float a_damage) {

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("Actor::HandleHealthDamage", ID);

				if (a_attacker) {

					SizeHitEffects::ApplyEverything(a_attacker, a_this, a_damage); // Apply bonus damage, overkill, stagger resistance

					if (Runtime::HasPerkTeam(a_this, Runtime::PERK.GTSPerkSizeReserveAug1)) { // Size Reserve Augmentation
						auto Cache = Persistent::GetActorData(a_this);
						if (Cache) {
							Cache->fSizeReserve += -a_damage / 3000;
						}
					}
				}
			}

			func<ID>(a_this, a_attacker, a_damage);  // Just reports the value, can't override it.
		}
		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;
	};

	struct Move {

		static inline const std::size_t funcIndex = REL::Relocate(0x0C8, 0x0C8, 0x0CA);

		// Override Movement Speed
		template<int ID>
		static bhkCharacterController* thunk(RE::Actor* a_this, float a_arg2, const RE::NiPoint3& a_position) {

			float bonus = 1.0f;

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("Actor::Move", ID);
				if (a_this && a_this->Get3D1(false) && !a_this->IsInKillMove()) {
					bonus = AttributeManager::AlterMovementSpeed(a_this);
				}
			}

			return func<ID>(a_this, a_arg2, a_position * bonus);

		}
		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;
	};

	struct Update {

		static constexpr std::size_t funcIndex = 0xAD;
		template<int ID>
		static void thunk(RE::Actor* a_this, float a_deltaTime) {

			func<ID>(a_this, a_deltaTime);

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("Actor::Update", ID);
				EventDispatcher::DoActorUpdate(a_this);
			}

		}
		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;
	};

	void Hook_Actor::Install() {

		logger::info("Installing Actor VTABLE MultiHooks...");

		stl::write_vfunc_unique<HandleHealthDamage, 1>(VTABLE_Character[0]);
		stl::write_vfunc_unique<HandleHealthDamage, 2>(VTABLE_PlayerCharacter[0]);

		stl::write_vfunc_unique<Move, 1>(VTABLE_Character[0]);
		stl::write_vfunc_unique<Move, 2>(VTABLE_PlayerCharacter[0]);

		stl::write_vfunc_unique<Update, 1>(VTABLE_Character[0]);
		stl::write_vfunc_unique<Update, 2>(VTABLE_PlayerCharacter[0]);
	}
}
