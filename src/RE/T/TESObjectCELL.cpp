#include "TESObjectCELL.hpp"

namespace RE {

	void TESObjectCELLEx::ForEachReferenceEx(TESObjectCELL* a_this, const std::function<BSContainer::ForEachResult(TESObjectREFR*)>& a_callback) {
		auto& runtimeData = a_this->GetRuntimeData();
		BSSpinLockGuard locker(runtimeData.spinLock);
		for (const auto& ref : runtimeData.references) {
			if (ref && a_callback(ref.get()) == BSContainer::ForEachResult::kStop) {
				break;
			}
		}
	}

	void TESObjectCELLEx::ForEachReferenceInRangeEx(TESObjectCELL* a_this, const NiPoint3& a_origin, float a_radius, const std::function<BSContainer::ForEachResult(TESObjectREFR*)>& a_callback) {
		const float squaredRadius = a_radius * a_radius;
		TESObjectCELLEx::ForEachReferenceEx(a_this, [&](TESObjectREFR* ref) {
			const auto distance = a_origin.GetSquaredDistance(ref->GetPosition());
			return distance <= squaredRadius ? a_callback(ref) : BSContainer::ForEachResult::kContinue;
		});
	}
}