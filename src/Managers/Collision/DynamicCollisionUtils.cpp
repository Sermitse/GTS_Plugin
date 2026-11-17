#include "Managers/Collision/DynamicCollisionUtils.hpp"
#include "Config/Config.hpp"

namespace {

	// Step height and step reach sit inside the blobs clib calls unk308 and unk310. Verified
	// identical on 1.5.97 and 1.6.1170.
	constexpr std::ptrdiff_t StepHeightOffset = 0x30C;
	constexpr std::ptrdiff_t StepReachOffset = 0x310;

	float* ControllerFloatAt(RE::bhkCharacterController* a_controller, std::ptrdiff_t a_offset) {
		return reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(a_controller) + a_offset);
	}

	// hkConvexShapeDefaultRadius. Havok rounds a convex hull by this, so it is the fillet on every
	// edge including the bottom rim, and it is what lets a shape ride over small obstacles instead
	// of catching on them.
	constexpr float DefaultConvexRadius = 0.05f;

	template <class T>
	void SwapArray(RE::hkArray<T>& a_lhs, RE::hkArray<T>& a_rhs) {
		std::swap(a_lhs._data, a_rhs._data);
		std::swap(a_lhs._size, a_rhs._size);
		std::swap(a_lhs._capacityAndFlags, a_rhs._capacityAndFlags);
	}

	// Everything the hull build derives from the vertices. The vtable, refcount, userData and the
	// remaining fields stay with each shape.
	void SwapHullGeometry(RE::hkpConvexVerticesShape* a_lhs, RE::hkpConvexVerticesShape* a_rhs) {
		std::swap(a_lhs->radius, a_rhs->radius);
		std::swap(a_lhs->aabbHalfExtents, a_rhs->aabbHalfExtents);
		std::swap(a_lhs->aabbCenter, a_rhs->aabbCenter);
		SwapArray(a_lhs->rotatedVertices, a_rhs->rotatedVertices);
		std::swap(a_lhs->numVertices, a_rhs->numVertices);
		SwapArray(a_lhs->planeEquations, a_rhs->planeEquations);
		std::swap(a_lhs->connectivity, a_rhs->connectivity);
	}

	RE::hkpConvexVerticesShape::BuildConfig MakeBuildConfig(float a_convexRadius) {
		return {
			.createConnectivity = false,
			.shrinkByConvexRadius = false,
			.useOptimizedShrinking = true,
			.convexRadius = a_convexRadius,
			.maxVertices = 0,
			.maxRelativeShrink = 0.00f,
			.maxShrinkingVerticesDisplacement = 0.00f,
			.maxCosAngleForBevelPlanes = -0.1f,
		};
	}

	// The hull is expanded outward by the convex radius, so pull the vertices in by whatever was
	// added on top of the default to keep the collider the size the caller asked for.
	void ShrinkForConvexRadius(std::vector<RE::hkVector4>& a_verts, float a_convexRadius) {

		const float inset = a_convexRadius - DefaultConvexRadius;

		if (inset <= 0.0f || a_verts.empty()) {
			return;
		}

		float lowest = FLT_MAX;
		float highest = -FLT_MAX;

		for (const RE::hkVector4& vertex : a_verts) {
			lowest = std::min(lowest, vertex.quad.m128_f32[2]);
			highest = std::max(highest, vertex.quad.m128_f32[2]);
		}

		if (highest - lowest <= inset * 4.0f) {
			return;
		}

		for (RE::hkVector4& vertex : a_verts) {

			float& x = vertex.quad.m128_f32[0];
			float& y = vertex.quad.m128_f32[1];
			float& z = vertex.quad.m128_f32[2];

			const float width = std::sqrt(x * x + y * y);
			if (width > inset) {
				const float factor = (width - inset) / width;
				x *= factor;
				y *= factor;
			}

			z = std::clamp(z, lowest + inset, highest - inset);
		}
	}

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
				auto* listShape = skyrim_cast<RE::hkpListShape*>(const_cast<RE::hkpShape*>(a_shape));

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

	// Clones are allocated at their own size, so the size the source was freed with must not carry over.
	// A packfile object has 0 here (never freed) and hkScaledMoppBvTreeShape has 0x80 against our 0x70.
	template <class T>
	T* AllocCopy(const T* a_src, const REL::VariantID& a_vtable) {
		auto* dst = static_cast<T*>(RE::hkMemoryRouter::hkHeapAlloc(sizeof(T)));
		std::memcpy(dst, a_src, sizeof(T));
		reinterpret_cast<std::uintptr_t*>(dst)[0] = a_vtable.address();
		dst->memSizeAndFlags = static_cast<std::uint16_t>(sizeof(T));
		dst->referenceCount = 1;
		return dst;
	}

	// The source may be Bethesda's hkScaledMoppBvTreeShape. Its query vfuncs (slots 1-13) are the same as
	// hkpMoppBvTreeShape's on 1.5.97, 1.6.1170 and 1.7.104, so the clone drops to the base class.
	// The code stays shared until RebuildMoppCode swaps in one compiled for this actor's children.
	RE::hkpMoppBvTreeShape* CloneMopp_SharedCode_DeepChild(const RE::hkpMoppBvTreeShape* src, RE::hkpShape* (*cloneFn)(RE::hkpShape*)) {
		RE::hkpMoppBvTreeShape* dst = AllocCopy(src, RE::VTABLE_hkpMoppBvTreeShape[0]);

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
		return AllocCopy(src, RE::VTABLE_hkpCapsuleShape[0]);
	}

	RE::hkpListShape* CloneListShapeDeep(const RE::hkpListShape* src, RE::hkpShape* (*cloneFn)(RE::hkpShape*)) {
		RE::hkpListShape* dst = AllocCopy(src, RE::VTABLE_hkpListShape[0]);

		// Deep-copy childInfo backing storage so we don't share the hkArray buffer
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

	CharControllerKind GetControllerKind(bhkCharacterController* a_controller) {
		if (!a_controller) {
			return CharControllerKind::Unknown;
		}
		if (skyrim_cast<bhkCharProxyController*>(a_controller)) {
			return CharControllerKind::Proxy;
		}
		if (skyrim_cast<bhkCharRigidBodyController*>(a_controller)) {
			return CharControllerKind::RigidBody;
		}
		return CharControllerKind::Unknown;
	}

	hkpCharacterProxy* GetControllerProxy(bhkCharacterController* a_controller, CharControllerKind a_kind) {
		if (!a_controller || a_kind != CharControllerKind::Proxy) {
			return nullptr;
		}
		return static_cast<hkpCharacterProxy*>(static_cast<bhkCharProxyController*>(a_controller)->proxy.referencedObject.get());
	}

	hkpCharacterRigidBody* GetControllerRigidBody(bhkCharacterController* a_controller, CharControllerKind a_kind) {
		if (!a_controller || a_kind != CharControllerKind::RigidBody) {
			return nullptr;
		}
		return static_cast<hkpCharacterRigidBody*>(static_cast<bhkCharRigidBodyController*>(a_controller)->characterRigidBody.referencedObject.get());
	}

	// The havok proxy's own maxSlope is left at pi/2, which disables its slope feature. Bethesda
	// runs their own in bhkCharProxyController::ProcessConstraintsCallback against this field,
	// which holds cos(angle) and is built from a hardcoded 47 degrees. clib types it as a uint32_t.
	float GetControllerMaxSlope(bhkCharacterController* a_controller) {
		const float cosine = std::clamp(std::bit_cast<float>(a_controller->maxSlope), -1.0f, 1.0f);
		return std::acos(cosine) * (180.0f / std::numbers::pi_v<float>);
	}

	std::uint32_t EncodeControllerMaxSlope(float a_degrees) {
		return std::bit_cast<std::uint32_t>(std::cos(a_degrees * (std::numbers::pi_v<float> / 180.0f)));
	}

	float GetControllerStepHeight(bhkCharacterController* a_controller) {
		return *ControllerFloatAt(a_controller, StepHeightOffset);
	}

	void SetControllerStepHeight(bhkCharacterController* a_controller, float a_havokUnits) {
		*ControllerFloatAt(a_controller, StepHeightOffset) = a_havokUnits;
	}

	float GetControllerStepReach(bhkCharacterController* a_controller) {
		return *ControllerFloatAt(a_controller, StepReachOffset);
	}

	void SetControllerStepReach(bhkCharacterController* a_controller, float a_havokUnits) {
		*ControllerFloatAt(a_controller, StepReachOffset) = a_havokUnits;
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

	// The game builds every character hull with hkConvexShapeDefaultRadius no matter how large the
	// actor is, which leaves a big actor with a proportionally razor sharp bottom rim that catches
	// on terrain a small one would ride over.
	float GetScaledConvexRadius(float a_scale) {
		const float scaling = std::clamp(Config::Collision.fConvexRadiusScaling, 0.0f, 1.0f);
		const float scale = std::clamp(a_scale, 1.0f, std::max(1.0f, Config::Collision.fMSimpleDrivenColliderMaxScale));
		return DefaultConvexRadius * std::lerp(1.0f, scale, scaling);
	}

	const hkpConvexVerticesShape* SetNewVerticesShape(bhkCharacterController* a_controller, std::vector<hkVector4>& a_modVerts, float a_convexRadius, const hkpConvexVerticesShape* a_ownedHull) {

		hkpListShape* ListShape = nullptr;
		hkpCharacterProxy* CharProxy = nullptr;
		hkpConvexVerticesShape* ConvexShape = nullptr;
		hkpCharacterRigidBody* CharRigidBody = nullptr;

		const auto resolveShapes = [&](const hkpShape* a_rootShape) {

			if (!a_rootShape) {
				return;
			}

			ListShape = skyrim_cast<hkpListShape*>(const_cast<hkpShape*>(a_rootShape));

			if (!ListShape) {
				ConvexShape = skyrim_cast<hkpConvexVerticesShape*>(const_cast<hkpShape*>(a_rootShape));
				return;
			}

			if (ListShape->childInfo.empty()) {
				ListShape = nullptr;
				return;
			}

			ConvexShape = skyrim_cast<hkpConvexVerticesShape*>(const_cast<hkpShape*>(ListShape->childInfo[0].shape));
		};

		if (bhkCharProxyController* proxyController = skyrim_cast<bhkCharProxyController*>(a_controller)) {
			CharProxy = static_cast<hkpCharacterProxy*>(proxyController->proxy.referencedObject.get());
			if (CharProxy && CharProxy->shapePhantom) {
				resolveShapes(CharProxy->shapePhantom->collidable.shape);
			}
		}
		else if (bhkCharRigidBodyController* rigidBodyController = skyrim_cast<bhkCharRigidBodyController*>(a_controller)) {
			CharRigidBody = static_cast<hkpCharacterRigidBody*>(rigidBodyController->characterRigidBody.referencedObject.get());
			if (CharRigidBody && CharRigidBody->m_character) {
				resolveShapes(CharRigidBody->m_character->collidable.shape);
			}
		}

		// Bail before allocating the replacement, otherwise a failed resolve leaks it.
		if (!ConvexShape) {
			return a_ownedHull;
		}

		ShrinkForConvexRadius(a_modVerts, a_convexRadius);

		const hkStridedVertices StridedVerts(a_modVerts.data(), static_cast<int>(a_modVerts.size()));

		hkpConvexVerticesShape* NewShape = static_cast<hkpConvexVerticesShape*>(hkMemoryRouter::hkHeapAlloc(sizeof(hkpConvexVerticesShape)));
		NewShape->ctor(StridedVerts, MakeBuildConfig(a_convexRadius));  // sets refcount to 1

		// it's actually a hkCharControllerShape not just a hkpConvexVerticesShape
		reinterpret_cast<std::uintptr_t*>(NewShape)[0] = VTABLE_hkCharControllerShape[0].address();

		// Replacing the shape removes the object from the broadphase and adds it back, which resets all of its
		// collision agents. For the rigid body that drops its contact points every
		// frame the hull follows the bones; the player's phantom is an hkpCachingShapePhantom, which keeps
		// agents too. So the new geometry is moved into the installed hull instead.
		// Only for the hull we installed last (anything else installed gets a full replace), and only at the
		// same vertex count: GSK agents cache vertex ids across frames (hkpGskCache::m_vertices), and a
		// shorter hull would leave them out of range.
		// The rigid body is told through UpdateShape, which refreshes the radius and AABB it derives from the
		// shape. Phantoms have no UpdateShape (the slot only returns), but the proxy's integrate rebuilds the
		// phantom's broadphase AABB from its shape in setPositionAndLinearCast.
		if (ConvexShape == a_ownedHull && ConvexShape->numVertices == NewShape->numVertices) {
			SwapHullGeometry(ConvexShape, NewShape);
			NewShape->RemoveReference();  // Frees the old geometry it now holds
			if (CharRigidBody) {
				CharRigidBody->m_character->UpdateShape(nullptr);
			}
			return ConvexShape;
		}

		if (bhkShape* Wrapper = ConvexShape->userData) {
			Wrapper->SetReferencedObject(NewShape);
		}

		// The listshape does not use a hkRefPtr but it's still setup to add a reference upon construction and remove one on destruction
		if (ListShape) {
			if (ListShape->childInfo[0].shape) {
				ListShape->childInfo[0].shape = NewShape;
				ConvexShape->RemoveReference();
			}

			// Swapping a list child behind havok's back keeps the old hull's agents (with its vertex indices)
			// and the old broadphase AABB. Setting the same root again rebuilds both.
			if (CharRigidBody) {
				CharRigidBody->m_character->SetShape(ListShape);
			}
			else if (CharProxy) {
				CharProxy->shapePhantom->SetShape(ListShape);
			}
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

		return NewShape;
	}

	bool GetControllerExtent(Actor* a_actor, NiPoint3& a_outPosition, float& a_outHalfHeight) {

		if (!a_actor) {
			return false;
		}

		auto* controller = a_actor->GetCharController();
		if (!controller) {
			return false;
		}

		static const float* gWorldScaleInverse = reinterpret_cast<float*>(RE::Offset::Havok::WorldScaleInverse.address());

		{
			hkVector4 hkPosition{};
			controller->GetPosition(hkPosition, false);
			a_outPosition = hkVec4ToNiPoint(hkPosition) * *gWorldScaleInverse;
		}

		//The collision manager swaps these shapes and the cached hull under the world's write lock.
		TESObjectCELL* cell = a_actor->GetParentCell();
		bhkWorld* world = cell ? cell->GetbhkWorld() : nullptr;
		if (!world) {
			return false;
		}
		BSReadLockGuard lock(world->worldLock);

		hkpConvexVerticesShape* convexShape = nullptr;
		std::vector<hkpCapsuleShape*> capsules = {};

		if (!GetShapes(controller, convexShape, capsules)) {
			return false;
		}

		//Shape coordinates are Havok units in controller local space, so both bounds are converted
		//the same way the collision manager converts its vertices.
		float lowest = std::numeric_limits<float>::max();
		float highest = std::numeric_limits<float>::lowest();

		if (convexShape) {
			if (auto* transient = Transient::GetActorData(a_actor)) {
				for (const hkVector4& vertex : transient->cachedConvexVerticesShape) {
					const float z = vertex.quad.m128_f32[2];
					lowest = std::min(lowest, z);
					highest = std::max(highest, z);
				}
			}
		}

		for (const hkpCapsuleShape* capsule : capsules) {

			if (!capsule) {
				continue;
			}

			const float a = capsule->vertexA.quad.m128_f32[2];
			const float b = capsule->vertexB.quad.m128_f32[2];

			lowest = std::min(lowest, std::min(a, b) - capsule->radius);
			highest = std::max(highest, std::max(a, b) + capsule->radius);
		}

		if (highest <= lowest) {
			return false;
		}

		a_outHalfHeight = (highest - lowest) * 0.5f * *gWorldScaleInverse;
		return true;
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

	bool IsDeepCloneable(const hkpShape* a_shape) {
		if (!a_shape) return false;
		switch (a_shape->type) {
			case hkpShapeType::kCapsule:
			case hkpShapeType::kList:
			case hkpShapeType::kMOPP:
				return true;
			default:
				return false;
		}
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

	bool RebuildMoppCode(hkpMoppBvTreeShape* a_shape) {

		const hkpShape* child = a_shape->child.childShape;
		if (!child || child->type != hkpShapeType::kList) {
			return false;
		}

		// hkpMoppCompilerInput.
		struct MoppCompilerInput {
			hkVector4 absoluteFitToleranceOfAxisAlignedTriangles;  // 00
			float relativeFitToleranceOfInternalNodes;             // 10
			float absoluteFitToleranceOfInternalNodes;             // 14
			float absoluteFitToleranceOfTriangles;                 // 18
			bool useShapeKeys;                                     // 1C
			bool enablePrimitiveSplitting;                         // 1D
			bool enableChunkSubdivision;                           // 1E
			bool enableInterleavedBuilding;                        // 1F
			bool cachePrimitiveExtents;                            // 20
		};
		static_assert(offsetof(MoppCompilerInput, cachePrimitiveExtents) == 0x20);
		static_assert(sizeof(MoppCompilerInput) == 0x30);

		// Checked in the asm on 1.5.97, 1.6.1170 and 1.7.104: buildCode(container, input, reindexInfo)
		// returns a new hkpMoppCode holding one reference. Bethesda's bhkMoppBvTreeShape passes the
		// child collection's hkpShapeContainer base (+0x20) and a null reindex array.
		using InputCtor_t = MoppCompilerInput* (*)(MoppCompilerInput*);
		using BuildCode_t = hkpMoppCode* (*)(const hkpShapeContainer*, const MoppCompilerInput*, void*);
		static auto InputCtor = REL::Relocation<InputCtor_t>(Hooks::RelocationIDEx(78796, 80780, 80780, NULL).address());
		static auto BuildCode = REL::Relocation<BuildCode_t>(Hooks::RelocationIDEx(78801, 80785, 80785, NULL).address());

		MoppCompilerInput input;
		InputCtor(&input);

		// Interleaved building (the default) works through a fixed 2 MB buffer meant for large triangle sets.
		// The havok docs advise turning it off below ~4000 primitives.
		input.enableInterleavedBuilding = false;

		const hkpShapeContainer* container = static_cast<const hkpListShape*>(child);
		hkpMoppCode* code = BuildCode(container, &input, nullptr);
		if (!code) {
			return false;
		}

		// The same four fields hkMoppBvTreeShapeBase's ctor derives from its code.
		const hkpMoppCode* oldCode = a_shape->code;
		a_shape->code = code;
		a_shape->moppData = code->data.data();
		a_shape->moppDataSize = static_cast<std::uint32_t>(code->data.size());
		a_shape->codeInfoCopy = code->info.offset;

		if (oldCode) {
			oldCode->RemoveReference();
		}

		return true;
	}

	void GetMoppShapes(hkpShape* a_shape, std::vector<hkpMoppBvTreeShape*>& a_outShapes) {

		if (!a_shape) return;

		switch (a_shape->type) {

			case hkpShapeType::kList:
			{
				for (hkpListShape::ChildInfo& childInfo : static_cast<hkpListShape*>(a_shape)->childInfo) {
					GetMoppShapes(const_cast<hkpShape*>(childInfo.shape), a_outShapes);
				}
			} break;

			case hkpShapeType::kMOPP:
			{
				auto* moppShape = static_cast<hkpMoppBvTreeShape*>(a_shape);
				if (moppShape->code) {
					a_outShapes.push_back(moppShape);
				}
				GetMoppShapes(const_cast<hkpShape*>(moppShape->child.childShape), a_outShapes);
			} break;

			default: break;
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
								std::vector<hkVector4> Verts{};
								if (auto tranData = Transient::GetActorData(const_cast<Actor*>(a_actor))) {
									Verts = tranData->cachedConvexVerticesShape;
								}

								// Scale vertices from Havok units to game world units and add controller position
								for (int32_t i = 0; i < static_cast<int32_t>(Verts.size()); ++i) {
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

								const float t = std::clamp(distance / 4096.0f, 0.0f, 1.0f);
								int32_t longitudinal_steps = static_cast<int32_t>(std::floorf(std::lerp(8.0f, 3.0f, t)));
								int32_t latitude_steps = static_cast<int32_t>(std::floorf(std::lerp(4.0f, 2.0f, t)));

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