#pragma once

#include "RE/H/hkVector4.h"
#include "RE/H/hkpConvexShape.h"

namespace RE {

	class hkpConvexTransformShapeBase : public hkpConvexShape {
		public:
		inline static constexpr auto RTTI = RTTI_hkpConvexTransformShapeBase;

		~hkpConvexTransformShapeBase() override;  // 00

		hkpSingleShapeContainer childShape;
		mutable std::int32_t childSize;
	};

	class hkpConvexTransformShape : public hkpConvexTransformShapeBase {

		public:
		inline static constexpr auto RTTI = RTTI_hkpConvexTransformShape;

		~hkpConvexTransformShape() override;                                                                                           // 00

		// override (hkpConvexShape)
		void                     CalcContentStatistics(hkStatisticsCollector* a_collector, const hkClass* a_class) const override;     // 02
		const hkpShapeContainer* GetContainer() const override;                                                                        // 04
		std::int32_t             CalcSizeForSpu(const CalcSizeForSpuInput& a_input, std::int32_t a_spuBufferSizeLeft) const override;  // 06
		void                     GetAabbImpl(const hkTransform& a_localToWorld, float a_tolerance, hkAabb& a_out) const override;      // 07
		bool                     CastRayImpl(const hkpShapeRayCastInput& a_input, hkpShapeRayCastOutput& a_output) const override;     // 08
		void                     CastRayWithCollectorImpl(const hkpShapeRayCastInput& a_input, const hkpCdBody& a_cdBody, hkpRayHitCollector& a_collector) const override;               // 09
		std::int32_t             GetNumCollisionSpheresImpl() override;                                                                // 0B
		const hkpSphere* GetCollisionSpheresImpl(hkSphere* a_sphereBuffer) override;                                                   // 0C

		// members
		hkQsTransform transform;
		hkVector4 extraScale;
	};
}