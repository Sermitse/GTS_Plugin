#pragma once
#include "Config/Settings/SettingsCamera.hpp"

// #define ENABLED_SHADOW

namespace GTS {

	float HighHeelOffset();
	float GetFrustrumNearDistance();
	float ZoomFactor();

	void UpdateThirdPerson();
	void UpdateWorld2ScreetMat(NiCamera* niCamera);
	void UpdateSceneManager(NiPoint3 camLoc);
	void UpdateRenderManager(NiPoint3 camLoc);
	void UpdateNiCamera(NiPoint3 camLoc);
	void UpdatePlayerCamera(NiPoint3 camLoc);
	void EnforceCameraINISettings();
	NiTransform GetCameraWorldTransform();
	NiPoint3 GetAggregateBoneTarget(RE::Actor* a_actor);
	void SetCameraNearFarPlanes(float a_ActorScale);

	BoneTarget GetBoneTargets(CameraTracking Camera_Anim, LCameraTrackBone_t Camera_MCM);

	NiCamera* GetNiCamera();

	NiMatrix3 GetCameraRotation();
	NiMatrix3 QuatToMatrix(const NiQuaternion& q);

	NiPoint3 ComputeLookAt(float zoomScale = 0.95f);
	NiPoint3 FirstPersonPoint();
	NiPoint3 ThirdPersonPoint();
	NiPoint3 GetCameraPosLocal();

	Actor* GetCameraActor();

}
