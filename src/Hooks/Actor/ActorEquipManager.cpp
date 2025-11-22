#include "Hooks/Actor/ActorEquipManager.hpp"
#include "Managers/RipClothManager.hpp"
#include "Hooks/Util/HookUtil.hpp"

namespace Hooks {

	struct EquipObject {

		static void thunk(RE::ActorEquipManager* a_this, RE::Actor* a_actor, RE::TESBoundObject* a_object, std::uint64_t a_unk) {

			bool allow = true;

			{
				GTS_PROFILE_ENTRYPOINT("ActorEquipManager::EquipObject");
				if (ClothManager::ShouldPreventReEquip(a_actor, a_object)) {
					allow = false;
				}
			}

			if (allow) {
				func(a_this, a_actor, a_object, a_unk);
			}
		}
		FUNCTYPE_CALL func;
	};

	void Hook_ActorEquipManager::Install() {
		logger::info("Installing ActorEquipManager Hooks...");
		stl::write_call<EquipObject>(REL::RelocationID(37938, 38894, NULL), REL::VariantOffset(0xE5, 0x170, NULL));
	}
}