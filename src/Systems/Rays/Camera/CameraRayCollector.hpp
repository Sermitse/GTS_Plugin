#pragma once

namespace GTS::CameraCol {

	class CamRayCollector {
		public:
		struct HitResult {
			glm::vec3 normal;
			float hitFraction;
			const RE::hkpCdBody* body;
			RE::NiAVObject* getAVObject() const;
		};

		CamRayCollector() = default;
		~CamRayCollector() = default;
		virtual void AddRayHit(const RE::hkpCdBody& body, const RE::hkpShapeRayCastCollectorOutput& hitInfo);
		inline void AddFilter(const RE::NiAVObject* obj) noexcept { objectFilter.push_back(obj); }
		const std::vector<HitResult>& GetHits();
		void Reset();

		private:
		float earlyOutHitFraction                       = 1.0f;
		std::uint32_t pad0C                             = 0;
		RE::hkpWorldRayCastOutput rayHit                = {};
		std::vector<HitResult> hits                     = {};
		std::vector<const RE::NiAVObject*> objectFilter = {};
	};

	// Extended ray result that includes per-hit data for object identification.
	struct HkpRayResult {
		bool hit = false;
		float rayLength = 0.0f;
		std::vector<CamRayCollector::HitResult> hitArray{};
	};

	RE::NiAVObject* GetHkpAVObject(const RE::hkpCdBody* body);

	// Thin (non hull sweep) raycast. Use for proximity probes and ignore list building
	// where an object's identity is needed. For hull swept camera collision use RaycastAsCamera.
	HkpRayResult HkpCastRay(RE::Actor* a_owner, const glm::vec4& start, const glm::vec4& end) noexcept;

}