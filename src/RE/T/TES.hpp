#pragma once

namespace RE {

	class TESEx final : TES {
	public:
		static void ForEachReferenceInRangeEx(TESObjectREFR* a_origin, float a_radius, const std::function<BSContainer::ForEachResult(TESObjectREFR* a_ref)>& a_callback);
		static void ForEachReferenceEx(const std::function<BSContainer::ForEachResult(TESObjectREFR* a_ref)>& a_callback);

		//All TES Members => 0x140 got moved by 0x08 on > .1130, this incluudes the current world cell ptr.
		[[nodiscard]] static inline TESWorldSpace* GetCurrentWorldSpace() noexcept {
			return REL::RelocateMemberIfNewer<TESWorldSpace*>(GTS::RUNTIME_SSE_1_6_1130, RE::TES::GetSingleton(), 0x140, 0x148);
		}
	};

}
