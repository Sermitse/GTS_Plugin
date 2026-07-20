#pragma once

namespace GTS {
        struct ControllerShapeData {
            hkpShape* shape;
            hkTransform transform; // translation is controller pos in Havok units, rotation is identity
            float angleZ;          // actor yaw, applied to capsule vertices before translation
        };
    
        bool SphereOverlapsCapsule(const hkpCapsuleShape* capsule, const hkVector4& center, float radiusSq, const hkTransform& transform, float angleZ);
        bool SphereOverlapsConvex(hkpConvexVerticesShape* convex, const hkVector4& center, float radiusSq, const hkTransform& transform);
        bool SphereOverlapsShape(const hkpShape* shape, const hkVector4& center, float radiusSq, const hkTransform& transform, float angleZ);
        hkpShape* GetControllerShape(Actor* actor);
        void DebugDrawShape(const hkpShape* shape, const hkTransform& transform, float angleZ, float toSkyrim, int duration);

        std::optional<ControllerShapeData> GetControllerShapeData(Actor* actor, float toHavok);
}