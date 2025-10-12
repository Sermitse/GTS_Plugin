#include "RE/H/hkpSphereRepShape.h"

namespace RE {

	hkpSphereRepShape::~hkpSphereRepShape() {
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpShape[0] };
		constexpr auto a_idx = 0x00;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<void(hkpSphereRepShape*)> func(result);
		func(this);
	}
}