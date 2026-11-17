#pragma once

namespace GTS {

	NiPoint3 HkVectorToNiPoint(const hkVector4& a_vector);
	NiPoint3 hkVec4ToNiPoint(const hkVector4& a_vector);
	glm::vec3 NiPointToVec3(const NiPoint3& a_point);

	// The player uses a bhkCharProxyController, every other actor a bhkCharRigidBodyController
	// unless bUseCharacterRB:HAVOK is off or its race has kAlwaysUseProxyController.
	enum class CharControllerKind : uint8_t {
		Unknown,
		Proxy,
		RigidBody,
	};

	// Costs an RTTI cast, so resolve the kind once and hand it to the two accessors below.
	CharControllerKind GetControllerKind(bhkCharacterController* a_controller);
	hkpCharacterProxy* GetControllerProxy(bhkCharacterController* a_controller, CharControllerKind a_kind);
	hkpCharacterRigidBody* GetControllerRigidBody(bhkCharacterController* a_controller, CharControllerKind a_kind);

	float GetControllerMaxSlope(bhkCharacterController* a_controller);
	// The raw value for bhkCharacterController::maxSlope.
	std::uint32_t EncodeControllerMaxSlope(float a_degrees);

	// Highest obstacle the proxy step test will walk over, and how far ahead it looks for one,
	// both in havok units. The game sets them once when the controller is built and never scales
	// them, so every actor gets 31 and 28.9 game units no matter how large it is.
	float GetControllerStepHeight(bhkCharacterController* a_controller);
	void SetControllerStepHeight(bhkCharacterController* a_controller, float a_havokUnits);
	float GetControllerStepReach(bhkCharacterController* a_controller);
	void SetControllerStepReach(bhkCharacterController* a_controller, float a_havokUnits);

	__m128 ScaleRingWidth(__m128 a_inHk4, float a_scale, float a_zHeight);
	float GetVerticesWidthMult(Actor* a_actor, bool a_boneDriven);

	float GetScaledConvexRadius(float a_scale);
	// Returns the hull now installed; pass it back next time so it can be updated in place.
	const hkpConvexVerticesShape* SetNewVerticesShape(bhkCharacterController* a_controller, std::vector<hkVector4>& a_modVerts, float a_convexRadius, const hkpConvexVerticesShape* a_ownedHull);
	bool GetShapes(bhkCharacterController* a_charController, hkpConvexVerticesShape*& a_outConvexShape, std::vector<hkpCapsuleShape*>& a_OutCollisionShapes);

	// World position of an actor's character controller and the vertical half extent of its collision
	// shape, both in game units. Havok depenetrates the controller every step, so this is the one
	// point on an actor that is guaranteed not to be inside geometry.
	//
	// Covers both shape kinds GetShapes returns; list and MOPP shapes are already flattened by it.
	bool GetControllerExtent(Actor* a_actor, NiPoint3& a_outPosition, float& a_outHalfHeight);
	// A MOPP tree is compiled from its children's bounds. Editing the children in place leaves the tree
	// at the old size, so queries cull whatever grew past it. This compiles a new tree for the list
	// under a_shape as it is now and swaps it in. Call it under the world's write lock, on a thread
	// with havok's memory router (the main thread): the compiler allocates through it.
	bool RebuildMoppCode(hkpMoppBvTreeShape* a_shape);
	void GetMoppShapes(hkpShape* a_shape, std::vector<hkpMoppBvTreeShape*>& a_outShapes);
	bool GetCapsulesFromShape(bhkShape* a_bhkshape, std::vector<hkpCapsuleShape*>& a_outCapsules);
	bool GetCapsulesFromController(bhkCharacterController* a_controller, std::vector<hkpCapsuleShape*>& a_outCapsules);
	// Whether DeepCloneShape copies a_shape. It hands any other shape back as the same object.
	bool IsDeepCloneable(const hkpShape* a_shape);
	hkpShape* DeepCloneShape(hkpShape* a_shape);

	void DrawCollisionShapes(const Actor* a_actor, bool a_isBoneDriven);

}
