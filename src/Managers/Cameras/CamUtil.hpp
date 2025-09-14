#pragma once
#include "Config/Settings/SettingsCamera.hpp"

// #define ENABLED_SHADOW

namespace GTS {
	BoneTarget GetBoneTargets(CameraTracking Camera_Anim, LCameraTrackBone_t Camera_MCM);

	float HighHeelOffset();

	void SetINIFloat(std::string_view name, float value);

	float GetINIFloat(std::string_view name);

	void EnsureINIFloat(std::string_view name, float value);

	void UpdateThirdPerson();

	NiCamera* GetNiCamera();
	void UpdateWorld2ScreetMat(NiCamera* niCamera);

	Actor* GetCameraActor();

	NiMatrix3 GetCameraRotation();

	void UpdateSceneManager(NiPoint3 camLoc);

	void UpdateRenderManager(NiPoint3 camLoc);

	void UpdateNiCamera(NiPoint3 camLoc);

	void UpdatePlayerCamera(NiPoint3 camLoc);

	NiMatrix3 QuatToMatrix(const NiQuaternion& q);

	NiPoint3 FirstPersonPoint();

	NiPoint3 ThirdPersonPoint();

	float ZoomFactor();

	NiPoint3 ComputeLookAt(float zoomScale = 0.95f);

	float GetFrustrumNearDistance();

	// Get's camera position relative to the player
	NiPoint3 GetCameraPosLocal();

	void UpdateCamera(float a_ActorScale, NiPoint3 a_CameraLocalOffset, NiPoint3 a_ActorLocalOffset);
}
