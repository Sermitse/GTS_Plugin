#include "Hooks/Actor/MagicTarget.hpp"
#include "Hooks/Util/HookUtil.hpp"

using namespace GTS;

namespace Hooks {

	struct AddTarget {

		static constexpr std::size_t funcIndex = 0x01;

		template<int ID>
		static void thunk(Actor* a_actor, MagicTarget::AddTargetData& a_targetData) {

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("ActorMagicTarget::AddTarget", ID);

				log::info("Add magic target");
				auto effect = a_targetData.effect;
				if (effect) {
					log::info("  - Has effect");
					auto baseEffect = effect->baseEffect;
					if (baseEffect) {
						log::info("    - Has base effect: {}", baseEffect->GetFullName());
					}
				}
			}

			return func<ID>(a_actor, a_targetData);
		}

		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;
	};

	void Hook_MagicTarget::Install() {

		log::info("Installing MagicTarget VTABLE Hooks...");

		//Currently Unused
		stl::write_vfunc_unique<AddTarget, 0>(Character::VTABLE[1]);
		stl::write_vfunc_unique<AddTarget, 1>(PlayerCharacter::VTABLE[1]);
	}
}
