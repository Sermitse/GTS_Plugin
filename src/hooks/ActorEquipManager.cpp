#include "hooks/ActorEquipManager.hpp"
#include "managers/RipClothManager.hpp"

using namespace RE;
using namespace Gts;

namespace Hooks {

	void Hook_ActorEquipManager::Hook(Trampoline& trampoline) {
		log::info("Hooking ActorEquipManager");
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(37938, 38894), REL::VariantOffset(0xE5, 0x170, 0xE5) };
		_ObjectEquip = trampoline.write_call<5>(target.address(), (uintptr_t)ObjectEquip);
	}

    void Hook_ActorEquipManager::ObjectEquip(RE::ActorEquipManager* a_self, RE::Actor* a_actor, RE::TESBoundObject* a_object, std::uint64_t a_unk) {
		
		if (!ClothManager::GetSingleton().ShouldPreventReEquip(a_actor, a_object)) {
			_ObjectEquip(a_self, a_actor, a_object, a_unk);
			//RE::DebugMessageBox("Blocked Equip");
		}
    }
}