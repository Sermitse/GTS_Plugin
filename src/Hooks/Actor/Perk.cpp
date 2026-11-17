#include "Hooks/Actor/Perk.hpp"
#include "Hooks/Util/HookUtil.hpp"

namespace Hooks {

	template<int ID>
	struct AddPerk {

		static inline const std::size_t funcIndex = 0x0FB;
		
		static void thunk(RE::Actor* a_this, RE::BGSPerk* a_perk, std::uint32_t a_rank) {

			func(a_this, a_perk, a_rank);

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("ActorPerk::AddPerk", ID);

				AddPerkEvent Event = {
					.actor = a_this,
					.perk = a_perk,
					.rank = a_rank,
				};

				EventDispatcher::DoAddPerk(Event);
			}

		}
		FUNCTYPE_VFUNC func;
	};

	template<int ID>
	struct RemovePerk {

		static inline const std::size_t funcIndex = 0x0FC;

		static void thunk(RE::Actor* a_this, RE::BGSPerk* a_perk) {

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("ActorPerk::RemovePerk", ID);

				RemovePerkEvent Event = {
					.actor = a_this,
					.perk = a_perk,
				};

				EventDispatcher::DoRemovePerk(Event);
			}

			func(a_this, a_perk);
		}

		FUNCTYPE_VFUNC func;
	};


	void Hook_Perk::Install() {

		logger::info("Installing Perk VTABLE MultiHooks...");

		stl::write_vfunc_unique<AddPerk, 1>(VTABLE_Character[0]);
		stl::write_vfunc_unique<AddPerk, 2>(VTABLE_PlayerCharacter[0]);

		stl::write_vfunc_unique<RemovePerk, 1>(VTABLE_Character[0]);
		stl::write_vfunc_unique<RemovePerk, 2>(VTABLE_PlayerCharacter[0]);

	}
}
