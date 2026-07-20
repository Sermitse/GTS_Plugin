#include "Managers/Contact/ColliderUtils.hpp"

namespace GTS {
	
	bool SphereOverlapsCapsule(const hkpCapsuleShape* capsule, const hkVector4& center, float radiusSq, const hkTransform& transform, float angleZ) {
		const float sinZ = std::sin(-angleZ);
		const float cosZ = std::cos(-angleZ);

		// Rotate vertex around Z then translate by controller position
		auto TransformVertex = [&](const __m128 v) -> __m128 {
			float x = v.m128_f32[0];
			float y = v.m128_f32[1];
			float z = v.m128_f32[2];

			float rx = x * cosZ - y * sinZ;
			float ry = x * sinZ + y * cosZ;

			return _mm_add_ps(
				_mm_set_ps(0.0f, z, ry, rx),
				transform.translation.quad
			);
		};

		const __m128 A = TransformVertex(capsule->vertexA.quad);
		const __m128 B = TransformVertex(capsule->vertexB.quad);
		const __m128 C = center.quad;

		const __m128 AB = _mm_sub_ps(B, A);
		const __m128 AC = _mm_sub_ps(C, A);

		float lenSq = _mm_cvtss_f32(_mm_dp_ps(AB, AB, 0x71));
		float t = (lenSq > 1e-8f) ? std::clamp(_mm_cvtss_f32(_mm_dp_ps(AC, AB, 0x71)) / lenSq, 0.0f, 1.0f) : 0.0f;

		const __m128 closest = _mm_add_ps(A, _mm_mul_ps(AB, _mm_set1_ps(t)));
		const __m128 delta = _mm_sub_ps(C, closest);
		float distSq = _mm_cvtss_f32(_mm_dp_ps(delta, delta, 0x71));

		float totalRadius = capsule->radius + std::sqrt(radiusSq);
		return distSq <= totalRadius * totalRadius;
	}

	bool SphereOverlapsConvex(hkpConvexVerticesShape* convex, const hkVector4& center, float radiusSq, const hkTransform& transform) {
		hkArray<hkVector4> verts{};
		const_cast<hkpConvexVerticesShape*>(convex)->GetOriginalVertices(verts);
		if (verts.empty()) return false;

		const __m128 tr = transform.translation.quad;
		__m128 mn = _mm_add_ps(verts[0].quad, tr);
		__m128 mx = mn;
		for (int i = 1; i < static_cast<int>(verts.size()); ++i) {
			__m128 tv = _mm_add_ps(verts[i].quad, tr);
			mn = _mm_min_ps(mn, tv);
			mx = _mm_max_ps(mx, tv);
		}

		const __m128 c = center.quad;
		const __m128 closest = _mm_min_ps(_mm_max_ps(c, mn), mx);
		const __m128 delta = _mm_sub_ps(closest, c);
		float distSq = _mm_cvtss_f32(_mm_dp_ps(delta, delta, 0x71));

		return distSq <= radiusSq;
	}

	bool SphereOverlapsShape(const hkpShape* shape, const hkVector4& center, float radiusSq, const hkTransform& transform, float angleZ) {
		if (!shape) return false;

		switch (shape->type) {
		case hkpShapeType::kCapsule:
			return SphereOverlapsCapsule(static_cast<const hkpCapsuleShape*>(shape), center, radiusSq, transform, angleZ);
		case hkpShapeType::kConvexVertices:
			return SphereOverlapsConvex(const_cast<hkpConvexVerticesShape*>(static_cast<const hkpConvexVerticesShape*>(shape)), center, radiusSq, transform);
		case hkpShapeType::kList: {
			auto* list = static_cast<const hkpListShape*>(shape);
			for (int i = 0; i < static_cast<int>(list->childInfo.size()); ++i) {
				if (SphereOverlapsShape(list->childInfo[i].shape, center, radiusSq, transform, angleZ)) return true;
			}
			return false;
		}
		case hkpShapeType::kMOPP: {
			auto* mopp = static_cast<const hkpMoppBvTreeShape*>(shape);
			return SphereOverlapsShape(mopp->child.childShape, center, radiusSq, transform, angleZ);
		}
		default: return false;
		}
	}

	hkpShape* GetControllerShape(Actor* actor) {
		bhkCharacterController* controller = actor->GetCharController();
		if (!controller) return nullptr;

		if (auto* rigid = skyrim_cast<bhkCharRigidBodyController*>(controller)) {
			hkRefPtr rigidBody{ static_cast<hkpCharacterRigidBody*>(rigid->characterRigidBody.referencedObject.get()) };
			if (rigidBody && rigidBody->m_character) {
				return const_cast<hkpShape*>(rigidBody->m_character->collidable.shape);
			}
		}
		else if (auto* proxy = skyrim_cast<bhkCharProxyController*>(controller)) {
			hkRefPtr charProxy{ static_cast<hkpCharacterProxy*>(proxy->proxy.referencedObject.get()) };
			if (charProxy && charProxy->shapePhantom) {
				return const_cast<hkpShape*>(charProxy->shapePhantom->collidable.shape);
			}
		}
		return nullptr;
	}

    void DebugDrawShape(const hkpShape* shape, const hkTransform& transform, float angleZ, float toSkyrim, int duration) {
        if (!shape) return;

        auto TransformCapsuleVertex = [&](const __m128 v) -> glm::vec3 {
            const float sinZ = std::sin(-angleZ);
            const float cosZ = std::cos(-angleZ);

            float x = v.m128_f32[0];
            float y = v.m128_f32[1];
            float z = v.m128_f32[2];

            float rx = x * cosZ - y * sinZ;
            float ry = x * sinZ + y * cosZ;

            __m128 result = _mm_add_ps(
                _mm_set_ps(0.0f, z, ry, rx),
                transform.translation.quad
            );
            result = _mm_mul_ps(result, _mm_set1_ps(toSkyrim));

            return glm::vec3(
                result.m128_f32[0],
                result.m128_f32[1],
                result.m128_f32[2]
            );
		};

        auto TransformConvexVertex = [&](const __m128 v) -> glm::vec3 {
            __m128 result = _mm_add_ps(v, transform.translation.quad);
            result = _mm_mul_ps(result, _mm_set1_ps(toSkyrim));

            return glm::vec3(
                result.m128_f32[0],
                result.m128_f32[1],
                result.m128_f32[2]
            );
            };

        switch (shape->type) {
        case hkpShapeType::kCapsule: {
            auto* capsule = static_cast<const hkpCapsuleShape*>(shape);
            glm::vec3 A = TransformCapsuleVertex(capsule->vertexA.quad);
            glm::vec3 B = TransformCapsuleVertex(capsule->vertexB.quad);
            float r = capsule->radius * toSkyrim;

            DebugDraw::DrawSphere(A, r, duration);
            DebugDraw::DrawSphere(B, r, duration);
            DebugDraw::DrawLineForMS(A, B, duration);
            break;
        }
        case hkpShapeType::kConvexVertices: {
            auto* convex = const_cast<hkpConvexVerticesShape*>(static_cast<const hkpConvexVerticesShape*>(shape));
            hkArray<hkVector4> verts{};
            convex->GetOriginalVertices(verts);
            if (verts.empty()) break;

            glm::vec3 mn = TransformConvexVertex(verts[0].quad);
            glm::vec3 mx = mn;
            for (int i = 1; i < static_cast<int>(verts.size()); ++i) {
                glm::vec3 tv = TransformConvexVertex(verts[i].quad);
                mn = glm::min(mn, tv);
                mx = glm::max(mx, tv);
            }

            glm::vec3 corners[8] = {
                { mn.x, mn.y, mn.z }, { mx.x, mn.y, mn.z },
                { mx.x, mx.y, mn.z }, { mn.x, mx.y, mn.z },
                { mn.x, mn.y, mx.z }, { mx.x, mn.y, mx.z },
                { mx.x, mx.y, mx.z }, { mn.x, mx.y, mx.z },
            };

            DebugDraw::DrawLineForMS(corners[0], corners[1], duration);
            DebugDraw::DrawLineForMS(corners[1], corners[2], duration);
            DebugDraw::DrawLineForMS(corners[2], corners[3], duration);
            DebugDraw::DrawLineForMS(corners[3], corners[0], duration);
            DebugDraw::DrawLineForMS(corners[4], corners[5], duration);
            DebugDraw::DrawLineForMS(corners[5], corners[6], duration);
            DebugDraw::DrawLineForMS(corners[6], corners[7], duration);
            DebugDraw::DrawLineForMS(corners[7], corners[4], duration);
            DebugDraw::DrawLineForMS(corners[0], corners[4], duration);
            DebugDraw::DrawLineForMS(corners[1], corners[5], duration);
            DebugDraw::DrawLineForMS(corners[2], corners[6], duration);
            DebugDraw::DrawLineForMS(corners[3], corners[7], duration);
            break;
        }
        case hkpShapeType::kList: {
            auto* list = static_cast<const hkpListShape*>(shape);
            for (int i = 0; i < static_cast<int>(list->childInfo.size()); ++i) {
                DebugDrawShape(list->childInfo[i].shape, transform, angleZ, toSkyrim, duration);
            }
            break;
        }
        case hkpShapeType::kMOPP: {
            auto* mopp = static_cast<const hkpMoppBvTreeShape*>(shape);
            DebugDrawShape(mopp->child.childShape, transform, angleZ, toSkyrim, duration);
            break;
        }
        default: break;
        }
    }

	std::optional<ControllerShapeData> GetControllerShapeData(Actor* actor, float toHavok) {
		bhkCharacterController* controller = actor->GetCharController();
		if (!controller) return std::nullopt;

		hkpShape* shape = nullptr;
		if (auto* rigid = skyrim_cast<bhkCharRigidBodyController*>(controller)) {
			hkRefPtr rigidBody{ static_cast<hkpCharacterRigidBody*>(rigid->characterRigidBody.referencedObject.get()) };
			if (rigidBody && rigidBody->m_character) {
				shape = const_cast<hkpShape*>(rigidBody->m_character->collidable.shape);
			}
		}
		else if (auto* proxy = skyrim_cast<bhkCharProxyController*>(controller)) {
			hkRefPtr charProxy{ static_cast<hkpCharacterProxy*>(proxy->proxy.referencedObject.get()) };
			if (charProxy && charProxy->shapePhantom) {
				shape = const_cast<hkpShape*>(charProxy->shapePhantom->collidable.shape);
			}
		}
		if (!shape) return std::nullopt;

		// Get controller world position in Havok units
		hkVector4 controllerPos{};
		controller->GetPosition(controllerPos, false);

		// Build transform: identity rotation + controller position
		// Vertices are local to controller and world-axis-aligned (no rotation needed for convex)
		// Capsules need -actor->data.angle.z rotation around Z applied to vertices before adding position
		hkTransform transform{};
		transform.rotation.col0.quad = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
		transform.rotation.col1.quad = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f);
		transform.rotation.col2.quad = _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f);
		transform.translation.quad = controllerPos.quad;

		return ControllerShapeData{ shape, transform, actor->data.angle.z };
	}
}
