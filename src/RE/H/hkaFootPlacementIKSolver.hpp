#pragma once

#include "RE/H/hkVector4.h"

namespace RE {

	class hkaFootPlacementIKSolver : public hkReferencedObject {
		public:
		inline static constexpr auto RTTI = RTTI_hkaFootPlacementIkSolver;

		struct Setup {
			const hkaSkeleton* m_skeleton;
			std::uint16_t m_hipIndex;
			std::uint16_t m_kneeIndex;
			std::uint16_t m_ankleIndex;
			hkVector4 m_kneeAxisLS;
			hkVector4 m_footEndLS;
			hkVector4 m_worldUpDirectionWS;
			hkVector4 m_modelUpDirectionMS;
			float m_originalGroundHeightMS;
			float m_footPlantedAnkleHeightMS;
			float m_footRaisedAnkleHeightMS;
			float m_maxAnkleHeightMS;
			float m_minAnkleHeightMS;
			float m_cosineMaxKneeAngle;
			float m_cosineMinKneeAngle;
			float m_raycastDistanceUp;
			float m_raycastDistanceDown;
			bool m_useFootLocking;
		};

		Setup m_setup;
		float m_currentWeight;
		float m_previousGroundHeightWS;
		hkVector4 m_previousGroundNormalWS;
		float m_previousVerticalDisplacement;
		bool m_isFootLocked;
		hkVector4 m_lockedFootPositionInWS;
		hkVector4 m_lockedFootEndPositionInWS;
		hkVector4 m_footUnlockingOffset;
	};
}