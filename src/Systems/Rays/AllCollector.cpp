#include "Systems/Rays/AllCollector.hpp"

using namespace GTS;

namespace GTS {

	void AllRayCollector::Reset() noexcept {
		hits.clear();
		hkpClosestRayHitCollector::Reset();
	}

	void AllRayCollector::AddRayHit(const hkpCdBody& a_body, const hkpShapeRayCastCollectorOutput& a_hitInfo) {
		AllRayCollectorOutput output;

		const hkpCdBody* body = &a_body;
		if (body->parent) {
			body = body->parent;
		}
		auto root = reinterpret_cast<const hkpCollidable*>(body);

		output.rootCollidable = root;
		output.hitFraction = a_hitInfo.hitFraction;

		this->hits.push_back(output);

		this->earlyOutHitFraction = 1.0f;
	}

	std::unique_ptr<AllRayCollector> AllRayCollector::Create() {
		return std::make_unique<AllRayCollector>();
	}

	std::vector<AllRayCollectorOutput>& AllRayCollector::GetHits() {
		return this->hits;
	}

	bool AllRayCollector::HasHit() const {
		return !this->hits.empty();
	}
}

void hkpClosestRayHitCollector::AddRayHit(const hkpCdBody& a_body, const hkpShapeRayCastCollectorOutput& a_hitInfo) {}
hkpClosestRayHitCollector::~hkpClosestRayHitCollector() = default;
ExtraDataList::ExtraDataList() = default;