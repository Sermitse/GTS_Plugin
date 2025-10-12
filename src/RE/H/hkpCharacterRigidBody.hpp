#pragma once

#include "RE/H/hkVector4.h"
#include "RE/H/hkContactPoint.h"
#include "RE/H/hkpCharacterControl.h"

namespace RE {

	class hkpCharacterRigidBodyListener;

	class hkpCharacterRigidBody : public hkReferencedObject, public hkpEntityListener, public hkpWorldPostSimulationListener {
		public:
		inline static constexpr auto RTTI = RTTI_hkpCharacterRigidBody;

		~hkpCharacterRigidBody() override;

		virtual void checkSupport(const hkStepInfo& stepInfo, hkpSurfaceInfo& ground) const;

		struct SupportInfo {
			hkContactPoint m_point;
			hkpRigidBody* m_rigidBody;
			hkVector4 m_surfaceVelocity;
		};

		virtual hkpSurfaceInfo::SupportedState getSupportInfo(const hkStepInfo& stepInfo, hkArray<SupportInfo>& supportInfo) const;
		virtual void getGround(const hkArray<SupportInfo>& supportInfo, bool useDynamicBodyVelocities, hkpSurfaceInfo& ground) const;

		// Listener methods.
		virtual void entityAddedCallback(hkpEntity* entity);
		virtual void entityRemovedCallback(hkpEntity* entity);
		virtual void postSimulationCallback(hkpWorld* world);

		hkpRigidBody* m_character;
		hkpCharacterRigidBodyListener* m_listener;
		hkVector4 m_up;

		float m_unweldingHeightOffsetFactor;
		float m_maxSlopeCosine;
		float m_maxSpeedForSimplexSolver;
		float m_supportDistance;
		float m_hardSupportDistance;
		hkVector4 m_acceleration;
		float m_maxForce;

		struct CollectorPair;

		struct VertPointInfo {
			hkContactPoint m_vertPoint;
			hkpSimpleConstraintContactMgr* m_mgr;
		};

		hkArray<VertPointInfo> m_verticalContactPoints;
	};

}