#include "Managers/Collision/DynamicCollisionUtils.hpp"
#include "Config/Config.hpp"

namespace {

	constexpr RE::hkpConvexVerticesShape::BuildConfig BuildConfig
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

	void ReadShape(const RE::hkpShape* a_shape, std::vector<RE::hkpCapsuleShape*>& a_outCapsules) {
		if (!a_shape) {
			return;
		}

		switch (a_shape->type) {
			case RE::hkpShapeType::kCapsule:
			{
				auto* capsule =const_cast<RE::hkpCapsuleShape*>(skyrim_cast<const RE::hkpCapsuleShape*>(a_shape));

				if (capsule) {
					a_outCapsules.emplace_back(capsule);
				}
			} break;

			case RE::hkpShapeType::kList:
			{
				auto* listShape =skyrim_cast<RE::hkpListShape*>(const_cast<RE::hkpShape*>(a_shape));

				if (!listShape) {
					return;
				}

				for (RE::hkpListShape::ChildInfo& childInfo : listShape->childInfo) {
					if (childInfo.shape) {
						ReadShape(childInfo.shape, a_outCapsules);
					}
				}
			} break;

			case RE::hkpShapeType::kMOPP:
			{
				if (RE::hkpMoppBvTreeShape* moppShape = const_cast<RE::hkpMoppBvTreeShape*>(skyrim_cast<const RE::hkpMoppBvTreeShape*>(a_shape))) {

					RE::hkpShapeBuffer shapeBuffer{};
					RE::hkpShapeKey shapeKey = moppShape->child.GetFirstKey();

					const int numChildren = moppShape->child.GetNumChildShapes();
					for (int i = 0; i < numChildren; ++i) {
						if (const RE::hkpShape* child =
							moppShape->child.GetChildShape(shapeKey, shapeBuffer)) {
							ReadShape(child, a_outCapsules);
						}
						shapeKey = moppShape->child.GetNextKey(shapeKey);
					}
				}
			} break;

			default:break;
		}
	}

	RE::hkpMoppBvTreeShape* CloneMopp_SharedCode_DeepChild(const RE::hkpMoppBvTreeShape* src, RE::hkpShape* (*cloneFn)(RE::hkpShape*)) {
		auto* dst = static_cast<RE::hkpMoppBvTreeShape*>(RE::hkMemoryRouter::hkHeapAlloc(sizeof(RE::hkpMoppBvTreeShape)));

		// Copy scalars/pointers
		std::memcpy(dst, src, sizeof(RE::hkpMoppBvTreeShape));
		reinterpret_cast<std::uintptr_t*>(dst)[0] = RE::VTABLE_hkpMoppBvTreeShape[0].address();
		dst->referenceCount = 1;

		if (dst->code) {
			dst->code->AddReference();
		}

		// Clone the child shape graph and repoint container
		RE::hkpShapeBuffer buf{};
		const RE::hkpShapeKey key = src->child.GetFirstKey();
		const RE::hkpShape* child = src->child.GetChildShape(key, buf);

		if (child) {
			RE::hkpShape* childClone = cloneFn(const_cast<RE::hkpShape*>(child));
			dst->child.childShape = childClone; // transfer ownership of that ref
		}
		else {
			dst->child.childShape = nullptr;
		}

		return dst;
	}

	RE::hkpCapsuleShape* CloneCapsule(const RE::hkpCapsuleShape* src) {
		auto* dst = static_cast<RE::hkpCapsuleShape*>(RE::hkMemoryRouter::hkHeapAlloc(sizeof(RE::hkpCapsuleShape)));

		std::memcpy(dst, src, sizeof(RE::hkpCapsuleShape));
		reinterpret_cast<std::uintptr_t*>(dst)[0] = RE::VTABLE_hkpCapsuleShape[0].address();
		dst->referenceCount = 1;

		return dst;
	}

	RE::hkpListShape* CloneListShapeDeep(const RE::hkpListShape* src, RE::hkpShape* (*cloneFn)(RE::hkpShape*)) {
		RE::hkpListShape* dst = static_cast<RE::hkpListShape*>(RE::hkMemoryRouter::hkHeapAlloc(sizeof(RE::hkpListShape)));

		// Shallow copy scalars first
		std::memcpy(dst, src, sizeof(RE::hkpListShape));
		reinterpret_cast<std::uintptr_t*>(dst)[0] = RE::VTABLE_hkpListShape[0].address();
		dst->referenceCount = 1;

		// Deep-copy childInfo backing storage so we don't share hkArray buffer
		const int n = src->childInfo.size();
		if (n > 0) {
			using ChildInfoT = std::remove_reference_t<decltype(dst->childInfo[0])>;

			auto* newBuf = static_cast<ChildInfoT*>(RE::hkMemoryRouter::hkHeapAlloc(sizeof(ChildInfoT) * n));

			// Copy entries (including filter info etc.)
			std::memcpy(newBuf, src->childInfo.data(), sizeof(ChildInfoT) * n);

			// Rebind hkArray storage to the new buffer
			dst->childInfo._data = newBuf;
			dst->childInfo._size = n;

			// Preserve the upper flag bits, replace capacity with n.
			// (Havok stores flags in the high bits of capacityAndFlags.)
			const int capFlags = src->childInfo._capacityAndFlags;
			dst->childInfo._capacityAndFlags = (capFlags & 0xC0000000) | n;

			// Now clone each child and patch pointer in the NEW buffer
			for (int i = 0; i < n; ++i) {
				if (newBuf[i].shape) {
					RE::hkpShape* childClone = cloneFn(const_cast<RE::hkpShape*>(newBuf[i].shape));
					newBuf[i].shape = childClone;
				}
			}
		}
		else {
			// Ensure a consistent empty array state
			dst->childInfo._data = nullptr;
			dst->childInfo._size = 0;
			dst->childInfo._capacityAndFlags = 0;
		}
		return dst;
	}
}

namespace GTS {

	NiPoint3 HkVectorToNiPoint(const hkVector4& a_vector) {
		return { a_vector.quad.m128_f32[0], a_vector.quad.m128_f32[1], a_vector.quad.m128_f32[2] };
	}

	NiPoint3 hkVec4ToNiPoint(const hkVector4& a_vector) {
		return { a_vector.quad.m128_f32[0], a_vector.quad.m128_f32[1], a_vector.quad.m128_f32[2] };
	}

	glm::vec3 NiPointToVec3(const NiPoint3& a_point) {
		return { a_point.x, a_point.y, a_point.z };
	}

	// Beth doesn't use the maxslopeCosine from havok,
	// It instead is handled by the bhkCharacterController as an inverted radian value.
	// maxSlope = (pi/2) - radians
	// maxSlope is also wrong in clib, it has it listed as a uint32_t instead of a float
	// The default "uint32" value is 1060018034 which makes no sense for an int angle.
	// When that int value is intepreted as a float it produces the value 0.6819984 which ends up being a radian of ~39 degrees (0.6819984 (180/pi) = 39.07 degrees)
	// The value is also inverted in regards to the slope calc, so the actual formula for the max slope for the controller in the vanilla game is (pi/2) - 0.6819984 = 0.8887982 (~50.92 degrees)
	float GetControllerMaxSlope(bhkCharacterController* a_controller) {
		const float radians = std::bit_cast<float>(a_controller->maxSlope);
		const float maxSlopeRadians = (std::numbers::pi / 2.0f) - radians;
		return maxSlopeRadians * 180.0f / std::numbers::pi;
	}

	void SetControllerMaxSlope(bhkCharacterController* a_controller, float a_degrees) {
		const float maxSlopeRadians = a_degrees * std::numbers::pi / 180.0f;
		const float invertedRadians = (std::numbers::pi / 2.0f) - maxSlopeRadians;
		a_controller->maxSlope = std::bit_cast<uint32_t>(invertedRadians);
	}

	__m128 ScaleRingWidth(__m128 a_inHk4, float a_scale, float a_zHeight) {

		a_inHk4 = _mm_and_ps(a_inHk4, _mm_castsi128_ps(_mm_setr_epi32(-1, -1, 0, 0)));

		const __m128 squared = _mm_mul_ps(a_inHk4, a_inHk4);
		const float length = _mm_cvtss_f32(_mm_sqrt_ss(_mm_hadd_ps(squared, squared)));

		// Normalize and scale
		if (length > 0.0f) {
			a_inHk4 = _mm_mul_ps(a_inHk4, _mm_set1_ps(a_scale / length));
		}
		a_inHk4.m128_f32[2] = a_zHeight;

		return a_inHk4;
	}

	float GetVerticesWidthMult(Actor* a_actor, bool a_boneDriven) {

		if (AnimationVars::Crawl::IsCrawling(a_actor)) {
			return a_boneDriven ? Config::Collision.fBoneDrivenWidthMultCrawling : Config::Collision.fSimpleDrivenWidthMultCrawling;
		}

		if (AnimationVars::Prone::IsProne(a_actor) && a_boneDriven) {
			return Config::Collision.fBoneDrivenWidthMultProning;
		}

		if (a_actor->IsSneaking()) {
			return a_boneDriven ? Config::Collision.fBoneDrivenWidthMultSneaking : Config::Collision.fSimpleDrivenWidthMultSneaking;
		}

		if (a_actor->AsActorState()->IsSwimming()) {
			return a_boneDriven ? Config::Collision.fBoneDrivenWidthMultSwimming : Config::Collision.fSimpleDrivenWidthMultSwimming;
		}

		return a_boneDriven ? Config::Collision.fBoneDrivenWidthMultBase : Config::Collision.fSimpleDrivenWidthMultBase;
	}

	void SetNewVerticesShape(bhkCharacterController* a_controller, std::vector<hkVector4>& a_modVerts) {

		hkpListShape* ListShape = nullptr;
		hkpCharacterProxy* CharProxy = nullptr;
		hkpConvexVerticesShape* ConvexShape = nullptr;
		hkpCharacterRigidBody* CharRigidBody = nullptr;

		if (bhkCharProxyController* proxyController = skyrim_cast<bhkCharProxyController*>(a_controller)) {
			CharProxy = static_cast<hkpCharacterProxy*>(proxyController->proxy.referencedObject.get());
			if (CharProxy) {
				ListShape = skyrim_cast<hkpListShape*>(const_cast<hkpShape*>(CharProxy->shapePhantom->collidable.shape));
				ConvexShape = skyrim_cast<hkpConvexVerticesShape*>(const_cast<hkpShape*>(ListShape ? ListShape->childInfo[0].shape : CharProxy->shapePhantom->collidable.shape));
			}
		}
		else if (bhkCharRigidBodyController* rigidBodyController = skyrim_cast<bhkCharRigidBodyController*>(a_controller)) {
			CharRigidBody = static_cast<hkpCharacterRigidBody*>(rigidBodyController->characterRigidBody.referencedObject.get());
			if (CharRigidBody) {
				ListShape = skyrim_cast<hkpListShape*>(const_cast<hkpShape*>(CharRigidBody->m_character->collidable.shape));
				ConvexShape = skyrim_cast<hkpConvexVerticesShape*>(const_cast<hkpShape*>(ListShape ? ListShape->childInfo[0].shape : CharRigidBody->m_character->collidable.shape));
			}
		}

		const hkStridedVertices StridedVerts(a_modVerts.data(), static_cast<int>(a_modVerts.size()));

		hkpConvexVerticesShape* NewShape = static_cast<hkpConvexVerticesShape*>(hkMemoryRouter::hkHeapAlloc(sizeof(hkpConvexVerticesShape)));
		NewShape->ctor(StridedVerts, BuildConfig);  // sets refcount to 1

		// it's actually a hkCharControllerShape not just a hkpConvexVerticesShape
		reinterpret_cast<std::uintptr_t*>(NewShape)[0] = VTABLE_hkCharControllerShape[0].address();

		if (bhkShape* Wrapper = ConvexShape->userData) {
			Wrapper->SetReferencedObject(NewShape);
		}

		// The listshape does not use a hkRefPtr but it's still setup to add a reference upon construction and remove one on destruction
		if (ListShape) {
			ListShape->childInfo[0].shape = NewShape;
			ConvexShape->RemoveReference();  // this will usually call the dtor on the old shape
		}
		else {
			if (CharProxy) {
				CharProxy->shapePhantom->SetShape(NewShape);
			}
			else if (CharRigidBody) {
				CharRigidBody->m_character->SetShape(NewShape);
			}
			NewShape->RemoveReference();
		}
	}

	bool GetShapes(bhkCharacterController* a_charController, hkpConvexVerticesShape*& a_outConvexShape, std::vector<hkpCapsuleShape*>& a_OutCollisionShapes) {

		const auto readShape = [&](const auto& a_self, const hkpShape* a_shape) -> bool {
			if (a_shape) {
				switch (a_shape->type) {
					case hkpShapeType::kCapsule:
					{
						if (hkpCapsuleShape* capsuleShape = const_cast<hkpCapsuleShape*>(skyrim_cast<const hkpCapsuleShape*>(a_shape))) {
							a_OutCollisionShapes.emplace_back(capsuleShape);
						}
					} break;
					case hkpShapeType::kConvexVertices:
					{
						if (hkpConvexVerticesShape* convexVerticesShape = const_cast<hkpConvexVerticesShape*>(skyrim_cast<const hkpConvexVerticesShape*>(a_shape))) {
							a_outConvexShape = convexVerticesShape;
							return true;
						}
					} break;
					case hkpShapeType::kList:
					{
						if (hkpListShape* listShape = skyrim_cast<hkpListShape*>(const_cast<hkpShape*>(a_shape))) {
							for (hkpListShape::ChildInfo& childInfo : listShape->childInfo) {
								if (const hkpShape* child = childInfo.shape) {
									a_self(a_self, child);
								}
							}
						}
					} break;
					case hkpShapeType::kMOPP:
					{
						if (hkpMoppBvTreeShape* moppShape = const_cast<hkpMoppBvTreeShape*>(skyrim_cast<const hkpMoppBvTreeShape*>(a_shape))) {
							hkpShapeBuffer buf{};
							hkpShapeKey shapeKey = moppShape->child.GetFirstKey();
							for (uint32_t i = 0; i < moppShape->child.GetNumChildShapes(); ++i) {
								if (const hkpShape* child = moppShape->child.GetChildShape(shapeKey, buf)) {
									a_self(a_self, child);
								}
								shapeKey = moppShape->child.GetNextKey(shapeKey);
							}
						}
					} break;

					default: break;
				}
			}
			return false;
		};

		//Player
		if (bhkCharProxyController* proxyController = skyrim_cast<bhkCharProxyController*>(a_charController)) {
			if (hkpCharacterProxy* charProxy = static_cast<hkpCharacterProxy*>(proxyController->proxy.referencedObject.get())) {
				readShape(readShape, charProxy->shapePhantom->collidable.shape);

				return a_outConvexShape || !a_OutCollisionShapes.empty();
			}
		}
		//Everything else
		else if (bhkCharRigidBodyController* rigidBodyController = skyrim_cast<bhkCharRigidBodyController*>(a_charController)) {
			if (hkpCharacterRigidBody* rigidBody = static_cast<hkpCharacterRigidBody*>(rigidBodyController->characterRigidBody.referencedObject.get())) {
				readShape(readShape, rigidBody->m_character->collidable.shape);

				return a_outConvexShape || !a_OutCollisionShapes.empty();
			}
		}

		return false;
	}

	hkpShape* DeepCloneShape(hkpShape* a_shape) {

		if (!a_shape) return nullptr;

		switch (a_shape->type) {

			case hkpShapeType::kCapsule: return CloneCapsule(static_cast<hkpCapsuleShape*>(a_shape));
			case hkpShapeType::kList:    return CloneListShapeDeep(static_cast<hkpListShape*>(a_shape), &DeepCloneShape);
			case hkpShapeType::kMOPP:    return CloneMopp_SharedCode_DeepChild(static_cast<hkpMoppBvTreeShape*>(a_shape), &DeepCloneShape);

			default:
			{
				a_shape->AddReference();
				return a_shape;
			}
		}
	}

	bool GetCapsulesFromShape(bhkShape* a_bhkshape, std::vector<hkpCapsuleShape*>& a_outCapsules) {
		if (!a_bhkshape) return false;
		ReadShape(static_cast<hkpShape*>(a_bhkshape->referencedObject.get()), a_outCapsules);
		return !a_outCapsules.empty();
	}

	bool GetCapsulesFromController(bhkCharacterController* a_controller, std::vector<hkpCapsuleShape*>& a_outCapsules) {

		if (!a_controller) return false;

		if (bhkCharProxyController* ProxyController = skyrim_cast<bhkCharProxyController*>(a_controller)) {
			if (hkpCharacterProxy* CharacterProxy = static_cast<hkpCharacterProxy*>(ProxyController->proxy.referencedObject.get())) {
				ReadShape(CharacterProxy->shapePhantom->collidable.shape, a_outCapsules);
				return !a_outCapsules.empty();
			}
		}
		else if (bhkCharRigidBodyController* RigidBodyController = skyrim_cast<bhkCharRigidBodyController*>(a_controller)) {
			if (hkpCharacterRigidBody* RigidBody = static_cast<hkpCharacterRigidBody*>(RigidBodyController->characterRigidBody.referencedObject.get())) {
				ReadShape(RigidBody->m_character->collidable.shape, a_outCapsules);
				return !a_outCapsules.empty();
			}
		}

		return false;
	}

	void DrawCollisionShapes(const Actor* a_actor, bool a_isBoneDriven) {

		static const float* gWorldScaleInverse = reinterpret_cast<float*>(RE::Offset::Havok::WorldScaleInverse.address());

		if (a_actor) {
			if (!a_actor->IsDead()) {
				if (bhkCharacterController* controller = a_actor->GetCharController()) {
					if (TESObjectCELL* Cell = a_actor->GetParentCell()) {
						if (NiPointer<bhkWorld> World = NiPointer(Cell->GetbhkWorld())) {

							hkpConvexVerticesShape* CollisionConvexVertexShape = nullptr;
							std::vector<hkpCapsuleShape*> CollisionCapsules{};
							CollisionCapsules.reserve(6);

							{
								BSReadLockGuard WorldLock(World->worldLock);
								GetShapes(controller, CollisionConvexVertexShape, CollisionCapsules);
							}

							hkVector4 ControllerPosition;
							controller->GetPosition(ControllerPosition, false);
							NiPoint3 ContollerNiPosition = hkVec4ToNiPoint(ControllerPosition) * *reinterpret_cast<float*>(Offset::Havok::WorldScaleInverse.address());

							if (CollisionConvexVertexShape) {
								hkArray<hkVector4> Verts{};
								CollisionConvexVertexShape->GetOriginalVertices(Verts);

								// Scale vertices from Havok units to game world units and add controller position
								for (int32_t i = 0; i < Verts.size(); ++i) {
									Verts[i].quad.m128_f32[0] = Verts[i].quad.m128_f32[0] * *gWorldScaleInverse;
									Verts[i].quad.m128_f32[1] = Verts[i].quad.m128_f32[1] * *gWorldScaleInverse;
									Verts[i].quad.m128_f32[2] = Verts[i].quad.m128_f32[2] * *gWorldScaleInverse;
								}

								const glm::vec4& color = a_isBoneDriven ? glm::vec4{ 0.0f, 1.0f, 1.0f, 1.0f } : glm::vec4{ 1.0f, 0.0f, 1.0f, 1.0f };
								DebugDraw::DrawConvexVertices(
									Verts,
									NiPointToVec3(ContollerNiPosition),
									glm::mat4(1.0f),
									10, color, 1.0f
								);
							}

							for (hkpCapsuleShape*& Capsule : CollisionCapsules) {
								constexpr NiPoint3 UpVector{ 0.f, 0.f, 1.f };

								const float ColCapsuleRadius = Capsule->radius * *gWorldScaleInverse;
								NiPoint3 A = hkVec4ToNiPoint(Capsule->vertexA) * *gWorldScaleInverse;
								NiPoint3 B = hkVec4ToNiPoint(Capsule->vertexB) * *gWorldScaleInverse;

								A = RotateAngleAxis(A, -a_actor->data.angle.z, UpVector);
								A += ContollerNiPosition;

								B = RotateAngleAxis(B, -a_actor->data.angle.z, UpVector);
								B += ContollerNiPosition;

								// Determine color based on material
								glm::vec4 color = { 1.0f, 1.0f, 0.0f, 1.0f }; // Yellow for collision capsules
								if (bhkShape* Shape = Capsule->userData) {
									if (Shape->materialID == MATERIAL_ID::kCharacterBumper) {
										if (!Config::Collision.bDrawBumpers) continue;
										color = { 0.0f, 0.25f, 0.53f, 1.0f }; // Blue for bumper
									}
								}

								const float distance = PlayerCharacter::GetSingleton()->GetPosition().GetDistance(a_actor->GetPosition());

								// Configure LOD based on distance
								int32_t longitudinal_steps, latitude_steps;

								if (distance < 512.0f) {
									//high detail
									longitudinal_steps = 8;
									latitude_steps = 4;
								}
								else if (distance < 1024.0f) {
									//normal detail
									longitudinal_steps = 6;
									latitude_steps = 3;
								}
								else if (distance < 1536.0f) {
									//low detail
									longitudinal_steps = 4;
									latitude_steps = 2;
								}
								else {
									//minimal detail
									longitudinal_steps = 1;
									latitude_steps = 1;
								}

								DebugDraw::DrawCapsule(
									NiPointToVec3(A),
									NiPointToVec3(B),
									ColCapsuleRadius,
									glm::mat4(1.0f),
									10, color, 1.0f,
									longitudinal_steps,
									latitude_steps
								);
							}
						}
					}
				}	
			}
		}
	}
}