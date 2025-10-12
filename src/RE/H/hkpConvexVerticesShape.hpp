#pragma once

#include "RE/H/hkVector4.h"
#include "RE/H/hkpConvexShape.h"
#include "RE/H/hkpConvexVerticesConnectivity.hpp"

namespace RE {

	class hkFourTransposedPoints {
		public:
		hkVector4 vertices[3];
	};

	class hkpConvexVerticesShape : public hkpConvexShape {

		public:
		inline static constexpr auto RTTI = RTTI_hkpConvexVerticesShape;

		~hkpConvexVerticesShape() override;  // 00

		// override (hkpConvexShape)
		void             CalcContentStatistics(hkStatisticsCollector* a_collector, const hkClass* a_class) const override;     // 02
		std::int32_t     CalcSizeForSpu(const CalcSizeForSpuInput& a_input, std::int32_t a_spuBufferSizeLeft) const override;  // 06
		void             GetAabbImpl(const hkTransform& a_localToWorld, float a_tolerance, hkAabb& a_out) const override;      // 07
		bool             CastRayImpl(const hkpShapeRayCastInput& a_input, hkpShapeRayCastOutput& a_output) const override;     // 08
		std::int32_t     GetNumCollisionSpheresImpl() override;                                                                // 0B
		const hkpSphere* GetCollisionSpheresImpl(hkSphere* a_sphereBuffer) override;                                           // 0C

		// members
		// Cached AABB
		hkVector4 aabbHalfExtents;
		hkVector4 aabbCenter;
		hkArray<hkFourTransposedPoints> rotatedVertices;
		std::int32_t numVertices;
		bool useSpuBuffer;

		mutable hkArray<hkVector4> planeEquations;

		// FYI: Cannot get connectivity to work
		mutable hkpConvexVerticesConnectivity* connectivity;
	};
}