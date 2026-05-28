#pragma once

namespace GTS {

	struct hkpGenericShapeData {
		intptr_t* unk;
		uint32_t shapeType;
	};

	struct rayHitShapeInfo {
		hkpGenericShapeData* hitShape;  // 0x00 8 bytes exactly
	};

	// based on usage in FUN_14054f720 (.1170)
	typedef __declspec(align(16)) struct hkpRootCdPoint {
		glm::vec4 contactPos;   // 0x00 contact position, Havok scale
		glm::vec4 normal;       // 0x10 surface normal, unit vector
		float distance;         // 0x20 penetration depth / sweep fraction
		uint8_t _pad[12];       // 0x24
		rayHitShapeInfo colA;   // 0x30 hkpCdBody* + unk
		rayHitShapeInfo colB;   // 0x38 hkpCdBody* + unk
	} hkpRootCdPoint;           // sizeof == 0x40

	static_assert(sizeof(hkpRootCdPoint) == 0x40);

	typedef __declspec(align(16)) struct bhkRayResult {
		hkpRootCdPoint cdPoint{};   // pre-allocated, written by FUN_14054f720

		// Custom utility fields
		bool hit = false;
		RE::Character* hitCharacter = nullptr;
		float rayLength = 0.0f;
		glm::vec4 hitPos{};         // hit pos in ganme units, from param_4 writeback

		uint32_t _pad{};
	} RayResult;

	static_assert(sizeof(RayResult) == 112);

	class RayCollector {
	public:
		struct HitResult {
			glm::vec3 normal;
			float hitFraction;
			const RE::hkpCdBody* body;
			RE::NiAVObject* getAVObject() const;
		};

	public:
		RayCollector() = default;
		~RayCollector() = default;
		virtual void AddRayHit(const RE::hkpCdBody& body, const RE::hkpShapeRayCastCollectorOutput& hitInfo);
		inline void AddFilter(const RE::NiAVObject* obj) noexcept { objectFilter.push_back(obj); }
		const std::vector<HitResult>& GetHits();
		void Reset();

	private:
		float earlyOutHitFraction{ 1.0f };
		std::uint32_t pad0C{};
		RE::hkpWorldRayCastOutput rayHit;
		std::vector<HitResult> hits{};
		std::vector<const RE::NiAVObject*> objectFilter{};
	};

	// Extended ray result that includes per-hit data for object identification.
	struct HkpRayResult {
		bool hit = false;
		float rayLength = 0.0f;
		std::vector<RayCollector::HitResult> hitArray{};
	};

	RE::NiAVObject* GetHkpAVObject(const RE::hkpCdBody* body);

	// Thin (non-hull) ray cast. Use for proximity probes and ignore-list building
	// where object identity is needed. For hull-swept camera collision use RaycastAsCamera.
	HkpRayResult HkpCastRay(const glm::vec4& start, const glm::vec4& end) noexcept;


	//Game default fneardistance is 15.0
	//Needs to be half of fneardistance otherwise the camera will clip through.
	constexpr float defaultCamHullSize = 15.0f / 2.0f;

	//Camera
	NiPoint3 ComputeCameraCollision(RE::Actor* cameraActor, const NiPoint3& rayStart, const NiPoint3& rayEnd, const float hullMult = -1.0f, const float rayMult = -1.0f);
	RayResult RaycastAsCamera(glm::vec4 start, glm::vec4 end, float traceHullSize) noexcept;

	NiPoint3 CastRay(TESObjectREFR* ref, const NiPoint3& origin, const NiPoint3& direction, const float& length, bool& success);
	NiPoint3 CastRayStatics(TESObjectREFR* ref, const NiPoint3& origin, const NiPoint3& direction, const float& length, bool& success);
}
