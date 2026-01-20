#pragma once
#include "DynamicController.hpp"

namespace GTS {

	constexpr uint8_t Vertex_Top = 9;
	constexpr uint8_t Vertex_Bot = 8;
	constexpr std::array<uint8_t, 8> Vertex_RingTop = { 1, 3, 4,  5,  7, 11, 13, 16 };
	constexpr std::array<uint8_t, 8> Vertex_RingBot = { 0, 2, 6, 10, 12, 14, 15, 17 };

	//For Top Vertex
	constexpr std::string_view HeadBoneName = "NPC Head [Head]";

	//For Top Rings
	constexpr std::string_view UppderArmBoneRName = "NPC R UpperArm [RUar]";
	constexpr std::string_view UpperArmBoneLName = "NPC L UpperArm [LUar]";

	//For Bottom Rings
	constexpr std::string_view CalfBoneRName = "NPC R RearCalf [RrClf]";
	constexpr std::string_view CalfBoneLName = "NPC L RearCalf [LrClf]";

	const std::vector<std::pair<std::string_view, std::string_view>> RingWidthBonePairList =
	{
		{UppderArmBoneRName, UpperArmBoneLName},
		//{"NPC R Hand [RHnd]",  "NPC L Hand [LHnd]"},
		//{"NPC R FrontThigh 1", "NPC L FrontThigh 1"},
		//{"NPC R Foot [Rft ]",  "NPC L Foot [Lft ]"}
	};

	static constexpr RE::hkpConvexVerticesShape::BuildConfig BuildConfig
	{
		.createConnectivity = false,
		.shrinkByConvexRadius = false,
		.useOptimizedShrinking = true,
		.convexRadius = 0.05f,
		.maxVertices = 0,
		.maxRelativeShrink = 0.00f,
		.maxShrinkingVerticesDisplacement = 0.00f,
		.maxCosAngleForBevelPlanes = -0.1f,
	};




	bool GetShapes(RE::bhkCharacterController* a_charController, RE::hkpConvexVerticesShape*& a_outConvexShape, std::vector<RE::hkpCapsuleShape*>& a_OutCollisionShapes);
	bool GetConvexShape(RE::bhkCharacterController* a_charController, RE::hkpCharacterProxy*& a_outProxy, RE::hkpCharacterRigidBody*& a_outRigidBody, RE::hkpListShape*& a_outListShape, RE::hkpConvexVerticesShape*& a_outConvexShape);
	hkpCapsuleShape* CloneCapsule(const hkpCapsuleShape* src);
	hkpListShape* CloneListShapeDeep(const hkpListShape* src, hkpShape* (*cloneFn)(hkpShape*));
	hkpShape* DeepCloneShape(hkpShape* s);
	bool GetCapsules(bhkCharacterController* a_controller, std::vector<hkpCapsuleShape*>& a_outCapsules);
	bool GetNPCCapsules(bhkShape* a_bhkshape, std::vector<hkpCapsuleShape*>& a_outCapsules);
	void ToggleCharacterBumper(RE::Actor* a_actor, bool a_enable);

	void FillCloningProcess(RE::NiCloningProcess& a_cloningProcess);
	
	void DrawCollisionShapes(RE::bhkCharacterController* a_controller, const RE::Actor* a_actor);

	glm::vec3 hkVec4ToGlVec3(const RE::hkVector4& a);
	glm::vec3 NiPointToVec3(const RE::NiPoint3& a);
	hkVector4 NiPointToHk4(const RE::NiPoint3& a);
	NiPoint3 hkVec4ToNiPoint(const RE::hkVector4& a);
	NiPoint3 HkVectorToNiPoint(const RE::hkVector4& a_vector);

	template <class T>
	T* Clone(T* a_object) {

		if (!a_object) {
			return nullptr;
		}

		NiCloningProcess cloningProcess{};
		FillCloningProcess(cloningProcess);

		auto clone = reinterpret_cast<T*>(a_object->Clone(cloningProcess));
		return clone;
	}
	__m128 NormalizeXYAndScale(__m128 vec, float scale, float zValue);
	float GetVerticesWidthMult(RE::Actor* actor);
	bool MoveVertZ(RE::Actor* a_actor, std::vector<RE::hkVector4>& a_modVerts, const uint8_t a_vertIdx, const std::string_view& a_boneToFollow, absl::flat_hash_map<std::string, RE::NiAVObject*>& a_boneCache, const float a_refPos);
	float GetLargestBoneDistance(RE::Actor* a_actor, absl::flat_hash_map<std::string, RE::NiAVObject*>& a_boneCache);
	bool MoveRingsZAggregate(RE::Actor* a_actor, std::vector<RE::hkVector4>& a_modVerts, const std::array<uint8_t, 8>& ringIdxs, const std::vector<std::string_view>& bonesToFollow, absl::flat_hash_map<std::string, RE::NiAVObject*>& a_boneCache, const float& a_botOfs, const float& a_distance);
	void ScaleCapsuleFromData(RE::hkpCapsuleShape& outCapsule, const GTS::DynamicController::CapsuleData& src, float scale);
	bool UpdateCapsuleScale(RE::bhkShape* a_shape, const GTS::DynamicController::ShapeData& a_data, const float scale);
	bool UpdatePlayerCapsuleScale(RE::bhkCharacterController* a_controller, const GTS::DynamicController::ShapeData& a_data, const float scale);
	void UpdateControllerData(RE::bhkCharacterController* a_controller, const GTS::DynamicController::ShapeData& a_data, const float& a_currentScale);
	void CreateAndSetVerticesShape(RE::bhkWorld* a_world, RE::hkpConvexVerticesShape* a_convexShape, RE::hkpListShape* a_listShape, RE::hkpCharacterProxy* a_proxy, RE::hkpCharacterRigidBody* a_rigidBody, std::vector<RE::hkVector4>& a_modVerts);
	void CorrectCollapsedVertexShape(std::vector<RE::hkVector4>& a_modVerts);
	bool ScaleorStateChange(const RE::ActorState::ActorState1& a_currentState, const RE::ActorState::ActorState1& a_Prevstate, const float& a_currentScale, const float& a_prevScale);
}
