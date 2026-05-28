#pragma once

namespace GTS::CameraCol {

	struct hkpGenericShapeData {
		intptr_t* unk;
		uint32_t shapeType;
	};
	static_assert(sizeof(hkpGenericShapeData) == 0x10);

	struct rayHitShapeInfo {
		hkpGenericShapeData* hitShape;  // 0x00 8 bytes exactly
	};
	static_assert(sizeof(rayHitShapeInfo) == 0x08);

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
	} CamRayResult;
	static_assert(sizeof(bhkRayResult) == 0x70);


	//Game default fneardistance is 15.0
	//Needs to be half of fneardistance otherwise the camera will clip through.
	constexpr float defaultCamHullSize = 15.0f / 2.0f;

	//Camera
	NiPoint3 ComputeCameraCollision(RE::Actor* cameraActor, const NiPoint3& rayStart, const NiPoint3& rayEnd, const float hullMult = -1.0f, const float rayMult = -1.0f);
	CamRayResult RaycastAsCamera(glm::vec4 start, glm::vec4 end, float traceHullSize) noexcept;

}