#include "RE/H/hkpShape.h"

namespace RE {

	// 00
	hkpShape::~hkpShape(){
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpShape[0] };
		constexpr auto a_idx = 0x00;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<void(hkpShape*)> func(result);
		func(this);
	}

	// 03
	float hkpShape::GetMaximumProjection(const hkVector4& a_direction) const {

		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpShape[0] };
		constexpr auto a_idx = 0x03;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<decltype(&hkpShape::GetMaximumProjection)> func(result);
		return func(this, a_direction);
	}

	// 04 - { return 0; }
	const hkpShapeContainer* hkpShape::GetContainer() const {
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpShape[0] };
		constexpr auto a_idx = 0x04;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<decltype(&hkpShape::GetContainer)> func(result);
		return func(this);
	}

	// 05 - { return false; }
	bool hkpShape::IsConvex() const {
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpShape[0] };
		constexpr auto a_idx = 0x05;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<decltype(&hkpShape::IsConvex)> func(result);
		return func(this);
	}

	// 06 - { return -1; }
	std::int32_t hkpShape::CalcSizeForSpu(const CalcSizeForSpuInput& a_input, std::int32_t a_spuBufferSizeLeft) const {
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpShape[0] };
		constexpr auto a_idx = 0x06;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<decltype(&hkpShape::CalcSizeForSpu)> func(result);
		return func(this, a_input, a_spuBufferSizeLeft);
	}

	// 0A
	hkVector4Comparison hkpShape::CastRayBundleImpl(const hkpShapeRayBundleCastInput& a_input, hkpShapeRayBundleCastOutput& a_output, const hkVector4Comparison& a_mask) const {
		REL::Relocation<std::uintptr_t> vtable{ VTABLE_hkpShape[0] };
		constexpr auto a_idx = 0x0A;
		const auto addr = vtable.address() + (sizeof(void*) * a_idx);
		const auto result = *reinterpret_cast<std::uintptr_t*>(addr);
		REL::Relocation<decltype(&hkpShape::CastRayBundleImpl)> func(result);
		return func(this, a_input, a_output, a_mask);
	}

}