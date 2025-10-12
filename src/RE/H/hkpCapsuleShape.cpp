#include "RE/H/hkpCapsuleShape.h"

namespace RE {

	/*

		DRAGON SLAYING
		HERE LIES SUPER CTD INDUCING CODE
		Touch only if you want to wake the dragon that lies
		dormant in your machine
		Hack in the vtables in a super hacky way
		Seriously don't use this, just replace the vtable
		with the real one via some other hacky code

		//TODO: Note to self these overriden vtable entries are what may be causing
		//CTD's on alandtse's ClibNg Fork.

	*/


	// 00
	hkpCapsuleShape::~hkpCapsuleShape()  {
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpCapsuleShape[0] };
		constexpr auto a_idx = 0x00;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<void(hkpCapsuleShape*)> func(result);
		func(this);
	}

	// override (hkpConvexShape)
	// 02
	void hkpCapsuleShape::CalcContentStatistics(hkStatisticsCollector* a_collector, const hkClass* a_class) const {
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpCapsuleShape[0] };
		constexpr auto a_idx = 0x02;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<decltype(&hkpCapsuleShape::CalcContentStatistics)> func(result);
		return func(this, a_collector, a_class);
	}

	// 06 - { return 56; }
	std::int32_t hkpCapsuleShape::CalcSizeForSpu(const CalcSizeForSpuInput& a_input, std::int32_t a_spuBufferSizeLeft) const {
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpCapsuleShape[0] };
		constexpr auto a_idx = 0x06;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<decltype(&hkpCapsuleShape::CalcSizeForSpu)> func(result);
		return func(this, a_input, a_spuBufferSizeLeft);
	}

	// 07
	void hkpCapsuleShape::GetAabbImpl(const hkTransform& a_localToWorld, float a_tolerance, hkAabb& a_out) const {
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpCapsuleShape[0] };
		constexpr auto a_idx = 0x07;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<decltype(&hkpCapsuleShape::GetAabbImpl)> func(result);
		func(this, a_localToWorld, a_tolerance, a_out);
	}

	// 08
	bool hkpCapsuleShape::CastRayImpl(const hkpShapeRayCastInput& a_input, hkpShapeRayCastOutput& a_output) const {
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpCapsuleShape[0] };
		constexpr auto a_idx = 0x08;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<decltype(&hkpCapsuleShape::CastRayImpl)> func(result);
		return func(this, a_input, a_output);
	}

	// 0B - { return 8; }
	std::int32_t hkpCapsuleShape::GetNumCollisionSpheresImpl() {
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpCapsuleShape[0] };
		constexpr auto a_idx = 0x0B;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<decltype(&hkpCapsuleShape::GetNumCollisionSpheresImpl)> func(result);
		return func(this);
	}

	// 0C
	const hkpSphere* hkpCapsuleShape::GetCollisionSpheresImpl(hkSphere* a_sphereBuffer) {
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpCapsuleShape[0] };
		constexpr auto a_idx = 0x0C;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<decltype(&hkpCapsuleShape::GetCollisionSpheresImpl)> func(result);
		return func(this, a_sphereBuffer);
	}

	// 0D
	void hkpCapsuleShape::GetCentreImpl(hkVector4& a_centreOut) {
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpCapsuleShape[0] };
		constexpr auto a_idx = 0x0D;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<decltype(&hkpCapsuleShape::GetCentreImpl)> func(result);
		func(this, a_centreOut);
	}

	// 0E
	void hkpCapsuleShape::Unk_0E(void) {
		// REL::Relocation<std::uintptr_t> vtable{VTABLE_hkpCapsuleShape[0]};
		// const auto a_idx = 0x0E;
		// const auto addr = vtable.address() + (sizeof(void *) * a_idx);
		// const auto result = *reinterpret_cast<std::uintptr_t *>(addr);
		// REL::Relocation<decltype(&hkpCapsuleShape::Unk_0E)> func(result);
		// func(this);
	}

	// 0F
	void hkpCapsuleShape::Unk_0F(void) {
		// REL::Relocation<std::uintptr_t> vtable{VTABLE_hkpCapsuleShape[0]};
		// const auto a_idx = 0x0F;
		// const auto addr = vtable.address() + (sizeof(void *) * a_idx);
		// const auto result = *reinterpret_cast<std::uintptr_t *>(addr);
		// REL::Relocation<decltype(&hkpCapsuleShape::Unk_0F)> func(result);
		// func(this);
	}

	// 11
	void hkpCapsuleShape::Unk_11(void) {
		// REL::Relocation<std::uintptr_t> vtable{VTABLE_hkpCapsuleShape[0]};
		// const auto a_idx = 0x11;
		// const auto addr = vtable.address() + (sizeof(void *) * a_idx);
		// const auto result = *reinterpret_cast<std::uintptr_t *>(addr);
		// REL::Relocation<decltype(&hkpCapsuleShape::Unk_11)> func(result);
		// func(this);
	}
}