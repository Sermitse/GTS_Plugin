#include "Managers/Contact/ContactManager.hpp"

using namespace REL;
using namespace GTS;



namespace GTS {

	std::string ContactManager::DebugName() {
		return "::ContactManager";
	}

	void ContactManager::HavokUpdate() {
		auto playerCharacter = PlayerCharacter::GetSingleton();
		auto cell = playerCharacter->GetParentCell();
		if (!cell) {
			return;
		}
		auto world = RE::NiPointer<RE::bhkWorld>(cell->GetbhkWorld());
		if (!world) {
			return;
		}
		ContactListener& contactListener = this->listener;
		if (contactListener.world != world) {
			contactListener.detach();
			contactListener.attach(world);
			contactListener.ensure_last();
			contactListener.enable_biped_collision();
		}
		contactListener.sync_camera_collision_groups();
	}

	void ContactManager::UpdateCameraContacts() {
		ContactListener& contactListener = this->listener;
		contactListener.sync_camera_collision_groups();
	}
}
