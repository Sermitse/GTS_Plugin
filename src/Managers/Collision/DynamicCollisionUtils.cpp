#include "Managers/Collision/DynamicCollisionUtils.hpp"
#include "DynamicController.hpp"
#include "Config/Config.hpp"


namespace {
	const float* gWorldScaleInverse = reinterpret_cast<float*>(RE::Offset::Havok::WorldScaleInverse.address());
}

namespace GTS {

	__m128 NormalizeXYAndScale(__m128 vec, float scale, float zValue) {

		vec = _mm_and_ps(vec, _mm_castsi128_ps(_mm_setr_epi32(-1, -1, 0, 0)));

		const __m128 squared = _mm_mul_ps(vec, vec);
		const float length = _mm_cvtss_f32(_mm_sqrt_ss(_mm_hadd_ps(squared, squared)));

		// Normalize and scale
		if (length > 0.0f) {
			vec = _mm_mul_ps(vec, _mm_set1_ps(scale / length));
		}
		vec.m128_f32[2] = zValue;

		return vec;
	}

	float GetVerticesWidthMult(RE::Actor* actor) {

		if (GTS::AnimationVars::Crawl::IsCrawling(actor)) {
			return GTS::Config::Advanced.fColliderWidthMultCrawling;
		}
		if (GTS::AnimationVars::Prone::IsProne(actor)) {
			return GTS::Config::Advanced.fColliderWidthMultProning;
		}
		if (actor->IsSneaking()) {
			return GTS::Config::Advanced.fColliderWidthMultSneaking;
		}
		if (actor->AsActorState()->IsSwimming()) {
			return GTS::Config::Advanced.fColliderWidthMultSwimming;
		}
		return GTS::Config::Advanced.fColliderWidthMultBase;
	};

	bool MoveVertZ(RE::Actor* a_actor, std::vector<RE::hkVector4>& a_modVerts, const uint8_t a_vertIdx,
		const std::string_view& a_boneToFollow,
		absl::flat_hash_map<std::string, RE::NiAVObject*>& a_boneCache, const float a_refPos) {

		RE::NiAVObject* bone = nullptr;

		// Try to get from cached bones first
		auto it = a_boneCache.find(a_boneToFollow);
		if (it != a_boneCache.end()) {
			bone = it->second;
		}
		else {
			// Find bone and cache it
			bone = GTS::find_node(a_actor, a_boneToFollow.data(), false);
			if (!bone) {
				return false; // bone not found
			}
			a_boneCache[a_boneToFollow] = bone;
		}

		const RE::NiPoint3 BonePos = (bone->world.translate - a_actor->GetPosition()) / *gWorldScaleInverse;
		a_modVerts[a_vertIdx].quad.m128_f32[2] = BonePos.z + a_refPos; //Correct Height
		return true;
	};

	float GetLargestBoneDistance(RE::Actor* a_actor, absl::flat_hash_map<std::string, RE::NiAVObject*>& a_boneCache) {
		std::vector<std::pair<RE::NiAVObject*, RE::NiAVObject*>> BonePairObjectList = {};
		float Longest = 0.05f;

		for (const auto& stringPair : RingWidthBonePairList) {
			std::pair<RE::NiAVObject*, RE::NiAVObject*> BonePairs = {};
			//First Bone
			{
				// Try to get from cached bones first
				auto it = a_boneCache.find(stringPair.first);
				if (it != a_boneCache.end()) {
					BonePairs.first = it->second;
				}
				else {
					// Find bone and cache it
					BonePairs.first = GTS::find_node(a_actor, stringPair.first.data(), false);
					if (!BonePairs.first) {
						return Longest; // bone not found
					}
					a_boneCache[stringPair.first] = BonePairs.first;
				}
			}

			//Second Bone
			{
				auto it = a_boneCache.find(stringPair.second);
				if (it != a_boneCache.end()) {
					BonePairs.second = it->second;
				}
				else {
					// Find bone and cache it
					BonePairs.second = GTS::find_node(a_actor, stringPair.second.data(), false);
					if (!BonePairs.second) {
						return Longest; // bone not found
					}
					a_boneCache[stringPair.second] = BonePairs.second;
				}
			}
			BonePairObjectList.push_back(std::move(BonePairs));
		}

		for (std::pair bonePair : BonePairObjectList) {
			const float CurDist = abs(bonePair.first->world.translate.GetDistance(bonePair.second->world.translate)) * 0.5f;
			if (CurDist > Longest) {
				Longest = CurDist;
			}
		}

		return Longest;
	};

	bool MoveRingsZAggregate(RE::Actor* a_actor, std::vector<RE::hkVector4>& a_modVerts,
		const std::array<uint8_t, 8>& ringIdxs, const std::vector<std::string_view>& bonesToFollow,
		absl::flat_hash_map<std::string, RE::NiAVObject*>& a_boneCache, const float& a_botOfs,
		const float& a_distance) {

		std::vector<RE::NiAVObject*> boneList = {};
		boneList.reserve(2);

		for (const std::string_view& boneName : bonesToFollow) {
			const auto it = a_boneCache.find(boneName);
			if (it != a_boneCache.end()) {
				boneList.push_back(it->second);
			}
			else {
				if (RE::NiAVObject* const& bone = GTS::find_node(a_actor, boneName, false)) {
					a_boneCache[boneName] = bone;
					boneList.push_back(bone);
				}
			}
		}

		float aggregateBoneZPos = 0.0f;
		if (!boneList.empty()) {
			for (RE::NiAVObject* const& bone : boneList) {
				if (bone) aggregateBoneZPos += bone->world.translate.z;
			}
			aggregateBoneZPos = ((aggregateBoneZPos / static_cast<float>(boneList.size())) - a_actor->GetPosition().z) / *gWorldScaleInverse;
		}

		if (aggregateBoneZPos <= 0.0f) {
			return false;
		}

		// Adjust ring vertices
		for (uint8_t idx : ringIdxs) {
			a_modVerts[idx] = NormalizeXYAndScale(a_modVerts[idx].quad, (a_distance / *gWorldScaleInverse) * GetVerticesWidthMult(a_actor), aggregateBoneZPos + a_botOfs);
		}
		return true;
	};

	void ScaleCapsuleFromData(
		RE::hkpCapsuleShape& outCapsule,
		const GTS::DynamicController::CapsuleData& src,
		float scale)
	{
		const float r0 = src.m_radius;

		// Original endpoints (unscaled)
		const float a0x = src.m_pointA.quad.m128_f32[0];
		const float a0y = src.m_pointA.quad.m128_f32[1];
		const float a0z = src.m_pointA.quad.m128_f32[2];

		const float b0x = src.m_pointB.quad.m128_f32[0];
		const float b0y = src.m_pointB.quad.m128_f32[1];
		const float b0z = src.m_pointB.quad.m128_f32[2];

		const float bottom0 = std::min(a0z, b0z) - r0;

		// Scale radius + endpoints about origin (your current behavior)
		const float r1 = r0 * scale;

		float a1x = a0x * scale;
		float a1y = a0y * scale;
		float a1z = a0z * scale;

		float b1x = b0x * scale;
		float b1y = b0y * scale;
		float b1z = b0z * scale;

		if (src.m_isBumper) {
			const float bottom1 = std::min(a1z, b1z) - r1;
			const float dz = bottom0 - bottom1;

			a1z += dz;
			b1z += dz;
		}

		outCapsule.radius = r1;

		outCapsule.vertexA.quad.m128_f32[0] = a1x;
		outCapsule.vertexA.quad.m128_f32[1] = a1y;
		outCapsule.vertexA.quad.m128_f32[2] = a1z;

		outCapsule.vertexB.quad.m128_f32[0] = b1x;
		outCapsule.vertexB.quad.m128_f32[1] = b1y;
		outCapsule.vertexB.quad.m128_f32[2] = b1z;
	}

	bool UpdateCapsuleScale(RE::bhkShape* a_shape, const GTS::DynamicController::ShapeData& a_data, const float scale) {
		if (a_data.m_capsules.empty()) return true; // Actor has no capsules to update

		std::vector<RE::hkpCapsuleShape*> CurrentCapsules{};
		CurrentCapsules.reserve(3);
		if (GTS::GetNPCCapsules(a_shape, CurrentCapsules)) {
			if (CurrentCapsules.empty() || CurrentCapsules.size() != a_data.m_capsules.size()) {
				return false;
			}
			for (size_t i = 0; i < CurrentCapsules.size(); ++i) {
				ScaleCapsuleFromData(*CurrentCapsules[i], a_data.m_capsules[i], scale);
			}
		}
		return false;
	};

	bool UpdatePlayerCapsuleScale(RE::bhkCharacterController* a_controller,
		const GTS::DynamicController::ShapeData& a_data, const float scale) {

		if (a_data.m_capsules.empty()) return true; // Actor has no capsules to update

		std::vector<RE::hkpCapsuleShape*> CurrentCapsules{};
		CurrentCapsules.reserve(3);
		if (GTS::GetCapsules(a_controller, CurrentCapsules)) {
			if (CurrentCapsules.empty() || CurrentCapsules.size() != a_data.m_capsules.size()) {
				return false;
			}
			for (size_t i = 0; i < CurrentCapsules.size(); ++i) {
				ScaleCapsuleFromData(*CurrentCapsules[i], a_data.m_capsules[i], scale);
			}
			return true;
		}
		return false;
	};

	void UpdateControllerData(RE::bhkCharacterController* a_controller, const GTS::DynamicController::ShapeData& a_data, const float& a_currentScale) {

		a_controller->actorHeight = a_data.m_originalControllerHeight * a_currentScale;
		a_controller->scale = a_data.m_originalControllerScale * a_currentScale;

		constexpr float maxSlopeAt = 10.0f;
		float normalizedScale = std::clamp(a_currentScale / maxSlopeAt, 0.0f, 1.0f);
		float value = std::lerp(60, 90, normalizedScale);
		const float radians = value * std::numbers::pi / 180.0f;

		if (RE::bhkCharProxyController* proxyController = skyrim_cast<RE::bhkCharProxyController*>(a_controller)) {
			if (RE::hkpCharacterProxy* proxy = static_cast<RE::hkpCharacterProxy*>(proxyController->proxy.referencedObject.get())) {
				proxy->maxSlopeCosine = std::cos(radians);
				a_controller->maxSlope = static_cast<uint32_t>(value);
			}
		}

		if (RE::bhkCharRigidBodyController* rigidBodyController = skyrim_cast<RE::bhkCharRigidBodyController*>(a_controller)) {
			if (RE::hkpCharacterRigidBody* rigidBody = static_cast<RE::hkpCharacterRigidBody*>(rigidBodyController->characterRigidBody.referencedObject.get())) {
				rigidBody->m_maxSlopeCosine = std::cos(radians);
				a_controller->maxSlope = static_cast<uint32_t>(value);
			}
		}
	};

	void CreateAndSetVerticesShape(RE::bhkWorld* a_world, RE::hkpConvexVerticesShape* a_convexShape,
		RE::hkpListShape* a_listShape, RE::hkpCharacterProxy* a_proxy,
		RE::hkpCharacterRigidBody* a_rigidBody, std::vector<RE::hkVector4>& a_modVerts) {
		RE::BSWriteLockGuard lock(a_world->worldLock);

		const RE::hkStridedVertices StridedVerts(a_modVerts.data(), static_cast<int>(a_modVerts.size()));

		RE::hkpConvexVerticesShape* NewShape = static_cast<RE::hkpConvexVerticesShape*>(RE::hkMemoryRouter::hkHeapAlloc(sizeof(RE::hkpConvexVerticesShape)));
		NewShape->ctor(StridedVerts, BuildConfig);  // sets refcount to 1

		// it's actually a hkCharControllerShape not just a hkpConvexVerticesShape
		reinterpret_cast<std::uintptr_t*>(NewShape)[0] = RE::VTABLE_hkCharControllerShape[0].address();

		if (RE::bhkShape* Wrapper = a_convexShape->userData) {
			Wrapper->SetReferencedObject(NewShape);
		}

		// The listshape does not use a hkRefPtr but it's still setup to add a reference upon construction and remove one on destruction
		if (a_listShape) {
			a_listShape->childInfo[0].shape = NewShape;
			a_convexShape->RemoveReference();  // this will usually call the dtor on the old shape
		}
		else {
			if (a_proxy) {
				a_proxy->shapePhantom->SetShape(NewShape);
			}
			else if (a_rigidBody) {
				a_rigidBody->m_character->SetShape(NewShape);
			}
			NewShape->RemoveReference();
		}
	};

	void CorrectCollapsedVertexShape(std::vector<RE::hkVector4>& a_modVerts) {

		constexpr float buffer = 0.05f;
		const float& topPos = a_modVerts[Vertex_Top].quad.m128_f32[2];
		const float& botPos = a_modVerts[Vertex_Bot].quad.m128_f32[2];
		const float& topRing = a_modVerts[1].quad.m128_f32[2];
		const float& botRing = a_modVerts[0].quad.m128_f32[2];

		//Bottom Ring
		if (botRing - botPos < buffer) {
			for (uint8_t idx : Vertex_RingBot) {
				a_modVerts[idx].quad.m128_f32[2] = botPos + buffer;
			}
		}

		//Top Ring
		if (topRing - botRing < buffer) {
			for (uint8_t idx : Vertex_RingTop) {
				a_modVerts[idx].quad.m128_f32[2] = botRing + buffer;
			}
		}

		//Top Vertex
		if (topPos - topRing < buffer) {
			a_modVerts[Vertex_Top].quad.m128_f32[2] = topRing + buffer;
		}
	};

	bool ScaleorStateChange(const RE::ActorState::ActorState1& a_currentState,
		const RE::ActorState::ActorState1& a_Prevstate, const float& a_currentScale,
		const float& a_prevScale) {

		if (a_currentScale != a_prevScale) {
			return true;
		}
		if (a_currentState.swimming != a_Prevstate.swimming || a_currentState.sneaking != a_Prevstate.sneaking) {
			return true;
		}
		return false;
	};



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

	bool GetConvexShape(bhkCharacterController* a_charController, hkpCharacterProxy*& a_outProxy, hkpCharacterRigidBody*& a_outRigidBody, hkpListShape*& a_outListShape, hkpConvexVerticesShape*& a_outConvexShape) {

		if (bhkCharProxyController* proxyController = skyrim_cast<bhkCharProxyController*>(a_charController)) {
			a_outProxy = static_cast<hkpCharacterProxy*>(proxyController->proxy.referencedObject.get());
			if (a_outProxy) {
				a_outListShape = skyrim_cast<hkpListShape*>(const_cast<hkpShape*>(a_outProxy->shapePhantom->collidable.shape));
				a_outConvexShape = skyrim_cast<hkpConvexVerticesShape*>(const_cast<hkpShape*>(a_outListShape ? a_outListShape->childInfo[0].shape : a_outProxy->shapePhantom->collidable.shape));
				return a_outListShape && a_outConvexShape;
			}
		}
		else if (bhkCharRigidBodyController* rigidBodyController = skyrim_cast<bhkCharRigidBodyController*>(a_charController)) {
			a_outRigidBody = static_cast<hkpCharacterRigidBody*>(rigidBodyController->characterRigidBody.referencedObject.get());
			if (a_outRigidBody) {
				a_outListShape = skyrim_cast<hkpListShape*>(const_cast<hkpShape*>(a_outRigidBody->m_character->collidable.shape));
				a_outConvexShape = skyrim_cast<hkpConvexVerticesShape*>(const_cast<hkpShape*>(a_outListShape ? a_outListShape->childInfo[0].shape : a_outRigidBody->m_character->collidable.shape));
				//Some creatures have a vertex shape but don't have a list shape
				return a_outConvexShape;
			}
		}

		return false;
	}

	static hkpMoppBvTreeShape* CloneMopp_SharedCode_DeepChild(const hkpMoppBvTreeShape* src, hkpShape* (*cloneFn)(hkpShape*)) {
		auto* dst = static_cast<hkpMoppBvTreeShape*>(RE::hkMemoryRouter::hkHeapAlloc(sizeof(hkpMoppBvTreeShape)));

		// Copy scalars/pointers
		std::memcpy(dst, src, sizeof(hkpMoppBvTreeShape));
		reinterpret_cast<std::uintptr_t*>(dst)[0] = RE::VTABLE_hkpMoppBvTreeShape[0].address();
		dst->referenceCount = 1;

		if (dst->code) {
			dst->code->AddReference();
		}

		// Clone the child shape graph and repoint container
		hkpShapeBuffer buf{};
		const hkpShapeKey key = src->child.GetFirstKey();
		const hkpShape* child = src->child.GetChildShape(key, buf);

		if (child) {
			hkpShape* childClone = cloneFn(const_cast<hkpShape*>(child));
			dst->child.childShape = childClone; // transfer ownership of that ref
		}
		else {
			dst->child.childShape = nullptr;
		}

		return dst;
	}

	hkpCapsuleShape* CloneCapsule(const hkpCapsuleShape* src) {
		auto* dst = static_cast<hkpCapsuleShape*>(RE::hkMemoryRouter::hkHeapAlloc(sizeof(hkpCapsuleShape)));

		std::memcpy(dst, src, sizeof(hkpCapsuleShape));
		reinterpret_cast<std::uintptr_t*>(dst)[0] = RE::VTABLE_hkpCapsuleShape[0].address();
		dst->referenceCount = 1;

		return dst;
	}

	hkpListShape* CloneListShapeDeep(const hkpListShape* src, hkpShape* (*cloneFn)(hkpShape*)) {
		auto* dst = static_cast<hkpListShape*>(RE::hkMemoryRouter::hkHeapAlloc(sizeof(hkpListShape)));

		// Shallow copy scalars first
		std::memcpy(dst, src, sizeof(hkpListShape));
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
					hkpShape* childClone = cloneFn(const_cast<hkpShape*>(newBuf[i].shape));
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


	hkpShape* DeepCloneShape(hkpShape* s) {

		if (!s) return nullptr;

		switch (s->type) {
			case hkpShapeType::kCapsule: return CloneCapsule(static_cast<hkpCapsuleShape*>(s));

			case hkpShapeType::kList: return CloneListShapeDeep(static_cast<hkpListShape*>(s), &DeepCloneShape);

			case hkpShapeType::kMOPP: return CloneMopp_SharedCode_DeepChild(static_cast<hkpMoppBvTreeShape*>(s), &DeepCloneShape);

			default:
				s->AddReference();
				return s;
		}
	}

	bool GetCapsules(bhkCharacterController* a_controller, std::vector<hkpCapsuleShape*>& a_outCapsules) {

		const auto ReadShape = [&](const auto& a_self, const hkpShape* a_shape) {

			if (a_shape == nullptr) return;

			switch (a_shape->type) {

				case hkpShapeType::kCapsule: 
				{
					if (hkpCapsuleShape* CapsuleShape = const_cast<hkpCapsuleShape*>(skyrim_cast<const hkpCapsuleShape*>(a_shape))) {
						a_outCapsules.emplace_back(CapsuleShape);
					}
					
				} break;

				case hkpShapeType::kList: 
				{
					if (hkpListShape* ListShape = skyrim_cast<hkpListShape*>(const_cast<hkpShape*>(a_shape))) {
						for (hkpListShape::ChildInfo& ChildInfo : ListShape->childInfo) {
							if (auto Child = ChildInfo.shape) {
								a_self(a_self, Child);
							}
						}
					}
				} break;

				case hkpShapeType::kMOPP: 
				{
					if (hkpMoppBvTreeShape* MOPPShape = const_cast<hkpMoppBvTreeShape*>(skyrim_cast<const hkpMoppBvTreeShape*>(a_shape))) {
						hkpShapeBuffer ShapeBuffer{};
						hkpShapeKey ShapeKey = MOPPShape->child.GetFirstKey();
						for (int i = 0; i < MOPPShape->child.GetNumChildShapes(); ++i) {
							if (auto ChildShape = MOPPShape->child.GetChildShape(ShapeKey, ShapeBuffer)) {
								a_self(a_self, ChildShape);
							}
							ShapeKey = MOPPShape->child.GetNextKey(ShapeKey);
						}
					}
				} break;

				default: break;
			}
		};

		if (bhkCharProxyController* ProxyController = skyrim_cast<bhkCharProxyController*>(a_controller)) {
			if (hkpCharacterProxy* CharacterProxy = static_cast<hkpCharacterProxy*>(ProxyController->proxy.referencedObject.get())) {
				ReadShape(ReadShape, CharacterProxy->shapePhantom->collidable.shape);
				return !a_outCapsules.empty();
			}
		}

		else if (bhkCharRigidBodyController* RigidBodyController = skyrim_cast<bhkCharRigidBodyController*>(a_controller)) {
			if (hkpCharacterRigidBody* RigidBody = static_cast<hkpCharacterRigidBody*>(RigidBodyController->characterRigidBody.referencedObject.get())) {
				ReadShape(ReadShape, RigidBody->m_character->collidable.shape);
				return !a_outCapsules.empty();
			}
		}

		return false;
	}

	bool GetNPCCapsules(bhkShape* a_bhkshape, std::vector<hkpCapsuleShape*>& a_outCapsules) {

		if (!a_bhkshape) return false;

		const auto ReadShape = [&](const auto& a_self, const hkpShape* a_shape) {

			if (a_shape == nullptr) return;

			switch (a_shape->type) {

				case hkpShapeType::kCapsule:
				{
					if (hkpCapsuleShape* CapsuleShape = const_cast<hkpCapsuleShape*>(skyrim_cast<const hkpCapsuleShape*>(a_shape))) {
						a_outCapsules.emplace_back(CapsuleShape);
					}

				} break;

				case hkpShapeType::kList:
				{
					if (hkpListShape* ListShape = skyrim_cast<hkpListShape*>(const_cast<hkpShape*>(a_shape))) {
						for (hkpListShape::ChildInfo& ChildInfo : ListShape->childInfo) {
							if (auto Child = ChildInfo.shape) {
								a_self(a_self, Child);
							}
						}
					}
				} break;

				case hkpShapeType::kMOPP:
				{
					if (hkpMoppBvTreeShape* MOPPShape = const_cast<hkpMoppBvTreeShape*>(skyrim_cast<const hkpMoppBvTreeShape*>(a_shape))) {
						hkpShapeBuffer ShapeBuffer{};
						hkpShapeKey ShapeKey = MOPPShape->child.GetFirstKey();
						for (int i = 0; i < MOPPShape->child.GetNumChildShapes(); ++i) {
							if (auto ChildShape = MOPPShape->child.GetChildShape(ShapeKey, ShapeBuffer)) {
								a_self(a_self, ChildShape);
							}
							ShapeKey = MOPPShape->child.GetNextKey(ShapeKey);
						}
					}
				} break;

				default: break;
			}
		};

		ReadShape(ReadShape, static_cast<hkpShape*>(a_bhkshape->referencedObject.get()));
		return !a_outCapsules.empty();
	}

	void ToggleCharacterBumper(Actor* a_actor, bool a_enable) {
		if (bhkCharacterController* characterController = a_actor->GetCharController()) {
			const float height = a_actor->GetRelevantWaterHeight();

			characterController->ToggleCharacterBumper(a_enable);

			if (LOADED_REF_DATA* loadedData = a_actor->loadedData) {
				if (height > loadedData->relevantWaterHeight) {
					loadedData->relevantWaterHeight = height;
				}
			}
		}
	}

	NiPoint3 HkVectorToNiPoint(const hkVector4& a_vector) {
		NiPoint3 Out = { a_vector.quad.m128_f32[0], a_vector.quad.m128_f32[1], a_vector.quad.m128_f32[2] };
		return Out;
	}

	void FillCloningProcess(NiCloningProcess& a_cloningProcess) {

		static constexpr auto One = NiPoint3(1.0f, 1.0f, 1.0f);

		uintptr_t cloneMap = reinterpret_cast<uintptr_t>(&a_cloningProcess.cloneMap);
		void** value1 = reinterpret_cast<void**>(cloneMap + 0x18);
		*value1 = reinterpret_cast<void*>(RELOCATION_ID(501133, 359452).address());;

		uintptr_t processMap = reinterpret_cast<uintptr_t>(&a_cloningProcess.processMap);
		void** value2 = reinterpret_cast<void**>(processMap + 0x18);
		*value2 = reinterpret_cast<void*>(RELOCATION_ID(501132, 359451).address());

		a_cloningProcess.copyType = 1;
		a_cloningProcess.appendChar = '$';
		a_cloningProcess.scale = One;
	}

	glm::vec3 hkVec4ToGlVec3(const hkVector4& a) {
		return glm::vec3(a.quad.m128_f32[0], a.quad.m128_f32[1], a.quad.m128_f32[2]);
	}

	NiPoint3 hkVec4ToNiPoint(const hkVector4& a) {
		return NiPoint3(a.quad.m128_f32[0], a.quad.m128_f32[1], a.quad.m128_f32[2]);
	}

	glm::vec3 NiPointToVec3(const NiPoint3& a) {
		return glm::vec3(a.x, a.y, a.z);
	}

	hkVector4 NiPointToHk4(const NiPoint3& a) {
		return hkVector4(a.x, a.y, a.z, 0.0f);
	}

	void DrawCollisionShapes(bhkCharacterController* a_controller, const Actor* a_actor) {

		if (a_controller && a_actor) {
			if (!a_actor->IsDead()) {
				if (TESObjectCELL* Cell = a_actor->GetParentCell()) {
					if (NiPointer<bhkWorld> World = NiPointer(Cell->GetbhkWorld())) {

						hkpConvexVerticesShape* CollisionConvexVertexShape = nullptr;
						std::vector<hkpCapsuleShape*> CollisionCapsules{};
						CollisionCapsules.reserve(6);

						{
							BSReadLockGuard WorldLock(World->worldLock);
							GetShapes(a_controller, CollisionConvexVertexShape, CollisionCapsules);
						}

						hkVector4 ControllerPosition;
						a_controller->GetPosition(ControllerPosition, false);
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

							DebugDraw::DrawConvexVertices(
								Verts,
								NiPointToVec3(ContollerNiPosition),
								glm::mat4(1.0f),
								10, { 1.0f, 0.0f, 1.0f, 1.0f }, 1.0f
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
							glm::vec4 color = { 1.0f, 1.0f, 0.0f, 1.0f }; // Default yellow
							if (bhkShape* Shape = Capsule->userData) {
								if (Shape->materialID == MATERIAL_ID::kCharacterBumper) {
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