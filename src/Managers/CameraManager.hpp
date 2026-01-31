#pragma once

// Module that handles the Camera

#include "Managers/Cameras/State.hpp"
#include "Managers/Cameras/Trans.hpp"

#include "Managers/Cameras/TP/Alt.hpp"
#include "Managers/Cameras/TP/Normal.hpp"
#include "Managers/Cameras/TP/Foot.hpp"
#include "Managers/Cameras/TP/FootL.hpp"
#include "Managers/Cameras/TP/footR.hpp"

#include "Managers/Cameras/FP/normal.hpp"

namespace GTS {

	class CameraManager : public EventListener, public CInitSingleton <CameraManager> {
		public:
		virtual std::string DebugName() override;
		virtual void DataReady() override;
		virtual void CameraUpdate() override;
		void Reset() override;
		void Update() override;

		CameraState* GetCameraState();

		void AdjustUpDown(float amt);
		void ResetUpDown();

		void AdjustLeftRight(float amt);
		void ResetLeftRight();
		static void ComputeAndApplyFinalCameraTransforms(float a_ActorScale, NiPoint3 a_CameraLocalOffset, NiPoint3 a_ActorLocalOffset);

		private:
		CameraState* GetCameraStateTP();
		CameraState* GetCameraStateFP();

		CameraState CamStateVanillaScaled;  // Like vanilla only scaled

		Normal CamStateNormal;
		Alt CamStateAlt;
		Foot CamStateFoot;
		FootR CamStateFootR;
		FootL CamStateFootL;

		FirstPerson CamStateFP;

		NiPoint3 ManualEditOffsets = {0,0,0};

		Spring SpringSmoothScale = Spring(0.3f, 0.5f);
		Spring3 SpringSmoothOffset = Spring3(NiPoint3(0.3f, 0.3f, 0.3f), 0.5f);

		CameraState* TrackedState = nullptr;
		std::unique_ptr<TransState> TransitionState = std::unique_ptr<TransState>(nullptr);
	};
}