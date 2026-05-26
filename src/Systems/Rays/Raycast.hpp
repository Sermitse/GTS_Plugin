#pragma once

namespace GTS {

	struct hkpGenericShapeData {
		intptr_t* unk;
		uint32_t shapeType;
	};

	struct rayHitShapeInfo {
		hkpGenericShapeData* hitShape;
		uint32_t unk;
	};

	typedef __declspec(align(16)) struct bhkRayResult {
		union {
			uint32_t data[16];
			struct {
				// Might be surface normal
				glm::vec4 rayUnkPos;
				// The normal vector of the ray
				glm::vec4 rayNormal;

				rayHitShapeInfo colA;
				rayHitShapeInfo colB;
			};
		};
		// Custom utility data, not part of the game

		// True if the trace hit something before reaching it's end position
		bool hit = false;
		// If the ray hit a character actor, this will point to it
		RE::Character* hitCharacter = nullptr;
		// The length of the ray from start to hitPos
		float rayLength = 0.0f;
		// The position the ray hit, in world space
		glm::vec4 hitPos{};

		// pad to 128
		uint64_t _pad{};
	} RayResult;

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


	//Game default is 15.0
	//Needs to be the same as fneardistance otherwise the camera will clip through.
	constexpr float camhullSize = 15.0f;

	//Camera
	NiPoint3 ComputeCameraCollision(RE::Actor* cameraActor, const NiPoint3& rayStart, const NiPoint3& rayEnd, const float hullMult = -1.0f, const float rayMult = -1.0f);
	RayResult RaycastAsCamera(glm::vec4 start, glm::vec4 end, float traceHullSize) noexcept;

	NiPoint3 CastRay(TESObjectREFR* ref, const NiPoint3& origin, const NiPoint3& direction, const float& length, bool& success);
	NiPoint3 CastRayStatics(TESObjectREFR* ref, const NiPoint3& origin, const NiPoint3& direction, const float& length, bool& success);
}
