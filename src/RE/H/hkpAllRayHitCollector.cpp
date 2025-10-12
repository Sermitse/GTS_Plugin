// Holds RE and dummy classes
#include "RE/H/hkpAllRayHitCollector.hpp"

namespace RE {

	//00
	hkpAllRayHitCollector::~hkpAllRayHitCollector() {
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpAllRayHitCollector[0] };
		constexpr auto a_idx = 0x00;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<void(hkpRayHitCollector*)> func(result);
		func(this);
	}

	//01
	void hkpAllRayHitCollector::AddRayHit(const hkpCdBody& a_body, const hkpShapeRayCastCollectorOutput& a_hitInfo) {
		REL::Relocation<std::uintptr_t> vtable{VTABLE_hkpAllRayHitCollector[0]};
		constexpr auto a_idx = 0x01;
		const auto addr = vtable.address() + (sizeof(void *) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t *>(addr);
		REL::Relocation<void(hkpRayHitCollector*)> func(result);
		func(this);
	}

	hkpAllRayHitCollector* hkpAllRayHitCollector::Create() {
		std::uintptr_t a_vtbl = hkpAllRayHitCollector::VTABLE[0].address();
		hkpAllRayHitCollector* memory = new hkpAllRayHitCollector();
		reinterpret_cast<std::uintptr_t*>(memory)[0] = a_vtbl; // HERE BE DRAGONS
		auto result =  static_cast<hkpAllRayHitCollector*>(memory);
		result->Reset();
		return result;
	}

	std::unique_ptr<hkpAllRayHitCollector> hkpAllRayHitCollector::CreatePtr() {
		return std::unique_ptr<hkpAllRayHitCollector>(hkpAllRayHitCollector::Create());
	}

	hkpRayHitCollector::~hkpRayHitCollector() {
		// Dummy
	}
}
