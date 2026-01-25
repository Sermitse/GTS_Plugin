#pragma once

namespace GTS {

	NiPoint3 HkVectorToNiPoint(const hkVector4& a_vector);
	NiPoint3 hkVec4ToNiPoint(const hkVector4& a_vector);
	glm::vec3 NiPointToVec3(const NiPoint3& a_point);

	float GetControllerMaxSlope(bhkCharacterController* a_controller);
	void SetControllerMaxSlope(bhkCharacterController* a_controller, float a_degrees);

	__m128 ScaleRingWidth(__m128 a_inHk4, float a_scale, float a_zHeight);
	float GetVerticesWidthMult(Actor* a_actor, bool a_boneDriven);

	void SetNewVerticesShape(bhkCharacterController* a_controller, std::vector<hkVector4>& a_modVerts);
	bool GetShapes(bhkCharacterController* a_charController, hkpConvexVerticesShape*& a_outConvexShape, std::vector<hkpCapsuleShape*>& a_OutCollisionShapes);
	bool GetCapsulesFromShape(bhkShape* a_bhkshape, std::vector<hkpCapsuleShape*>& a_outCapsules);
	bool GetCapsulesFromController(bhkCharacterController* a_controller, std::vector<hkpCapsuleShape*>& a_outCapsules);
	hkpShape* DeepCloneShape(hkpShape* a_shape);

	void DrawCollisionShapes(const Actor* a_actor, bool a_isBoneDriven);

}
