#pragma once

namespace GTS {

	class AllRayCollectorOutput {
		public:
		float hitFraction{ 1.0F };
		const hkpCollidable* rootCollidable{ nullptr };
		NiPoint3 position;
	};

	class AllRayCollector : public hkpClosestRayHitCollector {

		public:
		void AddRayHit(const hkpCdBody& a_body, const hkpShapeRayCastCollectorOutput& a_hitInfo) override;

		// Create the collector
		static unique_ptr<AllRayCollector> Create();

		// Reset, if you want to resuse the collector
		void Reset() noexcept;

		// Check if it has hit anything
		bool HasHit() const;

		// Get the hits
		std::vector<AllRayCollectorOutput>& GetHits();

		// Where they are colelcted to
		std::vector<AllRayCollectorOutput> hits;
		// This will be used for the filter in the ray cast
		std::uint32_t filterInfo{ 0 };
	};

}
