#include "RE/H/hkpConvexShape.h"

namespace RE {

	// 00
	hkpConvexShape::~hkpConvexShape() {
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpConvexShape[0] };
		constexpr auto a_idx = 0x00;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<void(hkpConvexShape*)> func(result);
		func(this);
	}

	// 03
	float hkpConvexShape::GetMaximumProjection(const hkVector4& a_direction) const { 
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpConvexShape[0] };
		constexpr auto a_idx = 0x03;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<decltype(&hkpConvexShape::GetMaximumProjection)> func(result);
		return func(this, a_direction);
	}

	// 05 - { return true; }
	bool hkpConvexShape::IsConvex() const {
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpConvexShape[0] };
		constexpr auto a_idx = 0x05;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<decltype(&hkpConvexShape::IsConvex)> func(result);
		return func(this);
	}

	// 09
	void hkpConvexShape::CastRayWithCollectorImpl(const hkpShapeRayCastInput& a_input, const hkpCdBody& a_cdBody, hkpRayHitCollector& a_collector) const {
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpConvexShape[0] };
		constexpr auto a_idx = 0x09;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<decltype(&hkpConvexShape::CastRayWithCollectorImpl)> func(result);
		return func(this, a_input, a_cdBody, a_collector);
	}

	// 10 - { return 2; }
	std::uint32_t hkpConvexShape::Unk_10(void) {
		// REL::Relocation<std::uintptr_t> vtable{VTABLE_hkpConvexShape[0]};
		// const auto a_idx = 0x10;
		// const auto addr = vtable.address() + (sizeof(void *) * a_idx);
		// const auto result = *reinterpret_cast<std::uintptr_t *>(addr);
		// REL::Relocation<decltype(&hkpConvexShape::Unk_10)> func(result);
		// return func(this);
		return 0;
	}

	// 11
	void hkpConvexShape::Unk_11(void) {
		// REL::Relocation<std::uintptr_t> vtable{VTABLE_hkpConvexShape[0]};
		// const auto a_idx = 0x11;
		// const auto addr = vtable.address() + (sizeof(void *) * a_idx);
		// const auto result = *reinterpret_cast<std::uintptr_t *>(addr);
		// REL::Relocation<decltype(&hkpConvexShape::Unk_11)> func(result);
		// func(this);
	}

}