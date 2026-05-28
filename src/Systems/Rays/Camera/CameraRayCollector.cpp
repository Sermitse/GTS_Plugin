#include "Systems/Rays/Camera/CameraRayCollector.hpp"

namespace GTS::CameraCol {


	void CamRayCollector::AddRayHit(const RE::hkpCdBody& body, const RE::hkpShapeRayCastCollectorOutput& hitInfo) {
		HitResult hit{};
		hit.hitFraction = hitInfo.hitFraction;
		hit.normal = {
			hitInfo.normal.quad.m128_f32[0],
			hitInfo.normal.quad.m128_f32[1],
			hitInfo.normal.quad.m128_f32[2]
		};

		const RE::hkpCdBody* obj = &body;
		while (obj && obj->parent) {
			obj = obj->parent;
		}

		hit.body = obj;
		if (!hit.body) {
			return;
		}

		earlyOutHitFraction = 1.0f; // Never early out, collect everything
		hits.push_back(hit);
	}

	const std::vector<CamRayCollector::HitResult>& CamRayCollector::GetHits() {
		return hits;
	}

	void CamRayCollector::Reset() {
		earlyOutHitFraction = 1.0f;
		hits.clear();
		objectFilter.clear();
	}

	RE::NiAVObject* CamRayCollector::HitResult::getAVObject() const {
		return body ? GetHkpAVObject(body) : nullptr;
	}

	RE::NiAVObject* GetHkpAVObject(const RE::hkpCdBody* body) {
		typedef RE::NiAVObject* (*_GetUserData)(const RE::hkpCdBody*);
		static auto getAVObject = REL::Relocation<_GetUserData>(REL::RelocationID(76160, 77988, NULL));
		return body ? getAVObject(body) : nullptr;
	}

	HkpRayResult HkpCastRay(RE::Actor* a_owner, const glm::vec4& start, const glm::vec4& end) noexcept {
		constexpr float hkpScale = 0.0142875f;
		const glm::vec4 dif = end - start;
		constexpr float one = 1.0f;

		const auto from = start * hkpScale;
		const auto to = dif * hkpScale;

		RE::hkpWorldRayCastInput pickRayInput{};
		pickRayInput.from = RE::hkVector4(from.x, from.y, from.z, one);
		pickRayInput.to = RE::hkVector4(to.x, to.y, to.z, one);  // 'to' here = end * hkpScale
		pickRayInput.filterInfo = 0;
		pickRayInput.enableShapeCollectionFilter = false;

		CamRayCollector collector;
		collector.Reset();

		RE::bhkPickData pickData{};
		pickData.rayInput = pickRayInput;
		pickData.ray = RE::hkVector4(to.x, to.y, to.z, one);
		pickData.rayHitCollectorA8 = reinterpret_cast<RE::hkpClosestRayHitCollector*>(&collector);

		auto cell = a_owner->GetParentCell();
		if (!cell) return {};

		try {
			auto physicsWorld = cell->GetbhkWorld();
			if (physicsWorld) physicsWorld->PickObject(pickData);
		}
		catch (...) {}

		HkpRayResult result;
		result.hitArray = collector.GetHits();

		// Find the closest hit to determine rayLength.
		float bestFraction = 1.0f;
		for (const auto& hit : result.hitArray) {
			if (hit.hitFraction < bestFraction) {
				bestFraction = hit.hitFraction;
			}
		}

		if (bestFraction < 1.0f) {
			result.hit = true;
			result.rayLength = glm::length(glm::vec3(dif)) * bestFraction;
		}

		return result;
	}

}