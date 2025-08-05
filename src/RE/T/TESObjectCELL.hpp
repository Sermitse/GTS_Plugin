#pragma once

namespace RE {

	class TESObjectCELLEx final : TESObjectCELL {
	public:
		static void ForEachReferenceEx(TESObjectCELL* a_this, const std::function<BSContainer::ForEachResult(TESObjectREFR*)>& a_callback);
		static void ForEachReferenceInRangeEx(TESObjectCELL* a_this, const NiPoint3& a_origin, float a_radius, const std::function<BSContainer::ForEachResult(TESObjectREFR*)>& a_callback);
	};

}