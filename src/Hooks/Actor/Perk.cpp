#include "Hooks/Actor/Perk.hpp"
#include "Hooks/Util/HookUtil.hpp"

namespace Hooks {

	struct AddPerk {

		static inline const std::size_t funcIndex = REL::Relocate(0x0FB, 0x0FB, 0x0FD);

		template<int ID>
		static void thunk(RE::Actor* a_this, RE::BGSPerk* a_perk, std::uint32_t a_rank) {

			GTS_PROFILE_ENTRYPOINT_UNIQUE("ActorPerk::AddPerk", ID);

			func<ID>(a_this, a_perk, a_rank);

			AddPerkEvent Event = {
				.actor = a_this,
				.perk = a_perk,
				.rank = a_rank,
			};

			EventDispatcher::DoAddPerk(Event);
		}

		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;

	};

	struct RemovePerk {


		static inline const std::size_t funcIndex = REL::Relocate(0x0FC, 0x0FC, 0x0FE);

		template<int ID>
		static void thunk(RE::Actor* a_this, RE::BGSPerk* a_perk) {

			GTS_PROFILE_ENTRYPOINT_UNIQUE("ActorPerk::RemovePerk", ID);

			RemovePerkEvent Event = {
				.actor = a_this,
				.perk = a_perk,
			};

			EventDispatcher::DoRemovePerk(Event);

			func<ID>(a_this, a_perk);
		}

		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;
	};


	void Hook_Perk::Install() {

		logger::info("Installing Perk VTABLE MultiHooks...");

		stl::write_vfunc_unique<AddPerk, 0>(VTABLE_Actor[0]);
		stl::write_vfunc_unique<AddPerk, 1>(VTABLE_Character[0]);
		stl::write_vfunc_unique<AddPerk, 2>(VTABLE_PlayerCharacter[0]);

		stl::write_vfunc_unique<RemovePerk, 0>(VTABLE_Actor[0]);
		stl::write_vfunc_unique<RemovePerk, 1>(VTABLE_Character[0]);
		stl::write_vfunc_unique<RemovePerk, 2>(VTABLE_PlayerCharacter[0]);

	}
}
