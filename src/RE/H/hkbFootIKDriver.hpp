#pragma once

#include "RE/H/hkVector4.h"
#include "RE/H/hkaFootPlacementIKSolver.hpp"

namespace RE {

	class hkbFootIKDriver : public hkReferencedObject {
		public:
		inline static constexpr auto RTTI = RTTI_hkbFootIkDriver;

		struct InternalLegData {
			hkVector4 m_groundPosition;
			hkVector4 m_groundNormal;
			hkaFootPlacementIKSolver* m_footIkSolver;
			float m_verticalError;
			bool m_hitSomething;
			bool m_isPlantedMS;
		};

		hkArray<InternalLegData> m_internalLegData;
		float m_worldFromModelFeedback;
		float m_prevIsFootIkEnabled;
		bool m_isSetUp;
		bool m_isGroundPositionValid;

	};
}