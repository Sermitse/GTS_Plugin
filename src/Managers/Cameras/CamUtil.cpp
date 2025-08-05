#include "Managers/Cameras/CamUtil.hpp"
#include "TPState.hpp"
#include "Config/Config.hpp"
#include "Hooks/Engine/Settings.hpp"
#include "Managers/Camera.hpp"
#include "Rays/raycast.hpp"
#include "UI/DebugAPI.hpp"

using namespace GTS;

namespace {

	enum class CameraDataMode {
		State,
		Transform,
	};

	constexpr CameraDataMode currentMode = CameraDataMode::State;

	const BoneTarget GetBoneTarget_Anim(CameraTracking Camera_Anim) {

		switch (Camera_Anim) {
			case CameraTracking::None: {
				return BoneTarget();
			}
			case CameraTracking::Butt: {
				return BoneTarget {
					.boneNames = {"NPC L Butt","NPC R Butt",},
					.zoomScale = ZoomIn_butt,
				};
			}
			case CameraTracking::Knees: {
				return BoneTarget {
					.boneNames = {"NPC L Calf [LClf]","NPC R Calf [RClf]",},
					.zoomScale = ZoomIn_knees,
				};
			}
			case CameraTracking::Breasts_02: {
				return BoneTarget {
					.boneNames = {"L Breast02","R Breast02",},
					.zoomScale = ZoomIn_Breast02,
				};
			}
			case CameraTracking::Thigh_Crush: {
				return BoneTarget {
					.boneNames = {"NPC R PreRearCalf","NPC R Foot [Rft ]","NPC L PreRearCalf","NPC L Foot [Lft ]",},
					.zoomScale = ZoomIn_ThighCrush,
				};
			}
			case CameraTracking::Thigh_Sandwich: {
				return BoneTarget {
					.boneNames = {"AnimObjectA",},
					.zoomScale = ZoomIn_ThighSandwich,
				};
			}
			case CameraTracking::Hand_Right: {
				return BoneTarget {
					.boneNames = {"NPC R Hand [RHnd]",},
					.zoomScale = ZoomIn_RightHand,
				};
			}
			case CameraTracking::Hand_Left: {
				return BoneTarget {
					.boneNames = {"NPC L Hand [LHnd]",},
					.zoomScale = ZoomIn_LeftHand,
				};
			}
			case CameraTracking::Grab_Left: {
				return BoneTarget {
					.boneNames = {"NPC L Finger02 [LF02]",},
					.zoomScale = ZoomIn_GrabLeft,
				};
			}
			case CameraTracking::L_Foot: {
				return BoneTarget {
					.boneNames = {"NPC L Foot [Lft ]",},
					.zoomScale = ZoomIn_LeftFoot,
				};
			}
			case CameraTracking::R_Foot: {
				return BoneTarget {
					.boneNames = {"NPC R Foot [Rft ]",},
					.zoomScale = ZoomIn_RightFoot,
				};
			}
			case CameraTracking::Mid_Butt_Legs: {
				return BoneTarget {
					.boneNames = {"NPC L Butt","NPC R Butt","NPC L Foot [Lft ]","NPC R Foot [Rft ]",},
					.zoomScale = ZoomIn_ButtLegs,
				};
			}
			case CameraTracking::VoreHand_Right: {
				return BoneTarget {
					.boneNames = {"AnimObjectA",},
					.zoomScale = ZoomIn_VoreRight,
				};
			}
			case CameraTracking::Finger_Right: {
				return BoneTarget {
					.boneNames = {"NPC R Finger12 [RF12]",},
					.zoomScale = ZoomIn_FingerRight,
				};
			}
			case CameraTracking::Finger_Left: {
				return BoneTarget {
					.boneNames = {"NPC L Finger12 [LF12]",},
					.zoomScale = ZoomIn_FingerLeft,
				};
			}
			case CameraTracking::ObjectA: {
				return BoneTarget {
					.boneNames = {"AnimObjectA",},
					.zoomScale = ZoomIn_ObjectA,
				};
			}
			case CameraTracking::ObjectB: {
				return BoneTarget {
					.boneNames = {"AnimObjectB",},
					.zoomScale = ZoomIn_ObjectB,
				};
			}
			case CameraTracking::ObjectL: {
				return BoneTarget {
					.boneNames = {"AnimObjectL",},
					.zoomScale = ZoomIn_ObjectL,
				};
			}
		}
		return BoneTarget();
	}

	const BoneTarget GetBoneTargetFromSettings(CameraTrackingSettings a_CamSetting) {
		if (HasFirstPersonBody()) {
			return BoneTarget();
		}
		switch (a_CamSetting) {

			case CameraTrackingSettings::kNone: {
				return BoneTarget();
			}

			case CameraTrackingSettings::kSpine: {
				return BoneTarget {
					.boneNames = {"NPC Spine2 [Spn2]","NPC Neck [Neck]"},
					.zoomScale = ZoomIn_Cam_Spine,
				};
			}
			case CameraTrackingSettings::kClavicle: {
				return BoneTarget {
					.boneNames = {"NPC R Clavicle [RClv]","NPC L Clavicle [LClv]"},
					.zoomScale = ZoomIn_Cam_Clavicle,
				};
			}
			case CameraTrackingSettings::kBreasts:
			{
				return BoneTarget{
					.boneNames = {"NPC L Breast01","NPC R Breast01"},
					.zoomScale = ZoomIn_Cam_Breasts,
				};
			}
			case CameraTrackingSettings::kBreasts_00:
			{
				return BoneTarget{
					.boneNames = {"L Breast00","R Breast00"},
					.zoomScale = ZoomIn_Cam_3BABreasts_00,
				};
			}
			case CameraTrackingSettings::kBreasts_01: {
				return BoneTarget {
					.boneNames = {"L Breast01","R Breast01"},
					.zoomScale = ZoomIn_Cam_3BABreasts_01,
				};
			}
			case CameraTrackingSettings::kBreasts_02: {
				return BoneTarget {
					.boneNames = {"L Breast02","R Breast02"},
					.zoomScale = ZoomIn_Cam_3BABreasts_02,
				};
			}
			case CameraTrackingSettings::kBreasts_03: {
				return BoneTarget {
					.boneNames = {"L Breast03","R Breast03"},
					.zoomScale = ZoomIn_Cam_3BABreasts_03,
				};
			}
			case CameraTrackingSettings::kBreasts_04:
			{
				return BoneTarget{
					.boneNames = {"L Breast04","R Breast04"},
					.zoomScale = ZoomIn_Cam_3BABreasts_04,
				};
			}
			case CameraTrackingSettings::kNeck: {
				return BoneTarget {
					.boneNames = {"NPC Neck [Neck]"},
					.zoomScale = ZoomIn_Cam_Neck,
				};
			}
			case CameraTrackingSettings::kButt: {
				return BoneTarget {
					.boneNames = {"NPC L Butt","NPC R Butt"},
					.zoomScale = ZoomIn_Cam_Butt,
				};
			}
			case CameraTrackingSettings::kGenitals:
			{
				return BoneTarget{
					.boneNames = {"Genitals"},
					.zoomScale = ZoomIn_Cam_Genitals,
				};
			}
			case CameraTrackingSettings::kBelly:
			{
				return BoneTarget{
					.boneNames = {"NPC Belly"},
					.zoomScale = ZoomIn_Cam_Belly,
				};
			}
		}
		return BoneTarget();
	}

	//https://www.desmos.com/calculator/5adrwyld6l
	float CalcLOGFnear(float scale, const float a_ref = 15.0f) {
		// Clamp scale between 0.05 and 1.0
		scale = std::max(0.05f, std::min(scale, 1.0f));

		// Normalize scale to [0, 1]
		float t = (scale - 0.05f) / 0.95f;

		// Exponential interpolation from 1.0 to 15.0
		float result = std::pow(a_ref, t);
		return result;
	}

	void ComputeFrustrumNearDistance(const float a_ActorScale) {

		if (!Config::GetCamera().bEnableAutoFNearDist || a_ActorScale > 1.0f) return;

		if (auto niCamera = GetNiCamera()) {

			auto fnear = CalcLOGFnear(a_ActorScale);

			niCamera->GetRuntimeData2().viewFrustum.fNear = fnear;
		}
	}
}

namespace GTS {

	BoneTarget GetBoneTargets(CameraTracking Camera_Anim, CameraTrackingSettings Camera_MCM) {
		if (HasFirstPersonBody()) {
			return {};
		}
		if (Camera_Anim != CameraTracking::None) { // must take priority
			return GetBoneTarget_Anim(Camera_Anim);
		}
		else {
			return GetBoneTargetFromSettings(Camera_MCM);
		}
	}

	void SetINIFloat(std::string_view name, float value) {
		auto ini_conf = INISettingCollection::GetSingleton();
		Setting* setting = ini_conf->GetSetting(name);
		if (setting) {
			setting->data.f=value; // If float
			ini_conf->WriteSetting(setting);
		}
	}

	float GetINIFloat(std::string_view name) {
		auto ini_conf = INISettingCollection::GetSingleton();
		Setting* setting = ini_conf->GetSetting(name);
		if (setting) {
			return setting->data.f;
		}
		return -1.0f;
	}

	void EnsureINIFloat(std::string_view name, float value) {
		auto currentValue = GetINIFloat(name);
		if (fabs(currentValue - value) > 1e-3) {
			SetINIFloat(name, value);
		}
	}

	void UpdateThirdPerson() {
		auto camera = PlayerCamera::GetSingleton();
		auto player = GetCameraActor();
		if (camera && player) {
			camera->UpdateThirdPerson(player->AsActorState()->IsWeaponDrawn());
		}
	}

	NiCamera* GetNiCamera() {
		auto camera = PlayerCamera::GetSingleton();
		auto cameraRoot = camera->cameraRoot.get();
		NiCamera* niCamera = nullptr;
		for (auto& child : cameraRoot->GetChildren()) {
			NiAVObject* node = child.get();
			if (node) {
				NiCamera* casted = netimmerse_cast<NiCamera*>(node);
				if (casted) {
					niCamera = casted;
					break;
				}
			}
		}
		return niCamera;
	}

	void UpdateWorld2ScreetMat(NiCamera* niCamera) {
		auto camNi = niCamera ? niCamera : GetNiCamera();
		typedef void (*UpdateWorldToScreenMtx)(RE::NiCamera*);
		static auto toScreenFunc = REL::Relocation<UpdateWorldToScreenMtx>(REL::RelocationID(69271, 70641).address());
		toScreenFunc(camNi);
	}

	Actor* GetCameraActor() {
		auto camera = PlayerCamera::GetSingleton();
		return camera->cameraTarget.get().get();
	}

	void UpdateSceneManager(NiPoint3 camLoc) {
		auto sceneManager = UI3DSceneManager::GetSingleton();
		if (sceneManager) {
			// Cache
			sceneManager->cachedCameraPos = camLoc;

			// Camera
			auto niCamera = sceneManager->camera;
			if (niCamera) {
				niCamera->world.translate = camLoc;
				UpdateWorld2ScreetMat(niCamera.get());
			}
		}
	}

	void UpdateRenderManager(NiPoint3 camLoc) {
		auto renderManager = UIRenderManager::GetSingleton();
		if (renderManager) {

			// Camera
			auto niCamera = renderManager->camera;
			if (niCamera) {
				niCamera->world.translate = camLoc;
				UpdateWorld2ScreetMat(niCamera.get());
			}
		}
	}

	void UpdateNiCamera(NiPoint3 camLoc) {
		auto niCamera = GetNiCamera();
		if (niCamera) {
			niCamera->world.translate = camLoc;
			UpdateWorld2ScreetMat(niCamera);
			update_node(niCamera);
		}
	}

	static NiTransform GetCameraWorldTransform() {
		auto camera = PlayerCamera::GetSingleton();
		if (camera) {
			auto& cameraRoot = camera->cameraRoot;
			if (cameraRoot) {
				return cameraRoot->world;
			}
		}
		return {};
	}

	void UpdatePlayerCamera(NiPoint3 camLoc) {
		auto camera = PlayerCamera::GetSingleton();
		if (camera) {
			auto cameraRoot = camera->cameraRoot;
			if (cameraRoot) {
				cameraRoot->local.translate = camLoc;
				cameraRoot->world.translate = camLoc;
				update_node(cameraRoot.get());
			}
		}
	}

	static NiPoint3 GetCameraPosition() {
		NiPoint3 cameraLocation;
		switch (currentMode) {
			case CameraDataMode::State: {
				auto camera = PlayerCamera::GetSingleton();
				if (camera) {
					auto currentState = camera->currentState;
					if (currentState) {
						currentState->GetTranslation(cameraLocation);
					}
				}
			}
			case CameraDataMode::Transform: {
				auto camera = PlayerCamera::GetSingleton();
				if (camera) {
					auto cameraRoot = camera->cameraRoot;
					if (cameraRoot) {
						cameraLocation = cameraRoot->world.translate;
					}
				}
			}
		}
		return cameraLocation;
	}

	NiMatrix3 GetCameraRotation() {
		NiMatrix3 cameraRot;
		switch (currentMode) {
			case CameraDataMode::State: {
				//log::info("Camera State: State");
				auto camera = PlayerCamera::GetSingleton();
				if (camera) {
					auto& currentState = camera->currentState;
					if (currentState) {
						NiQuaternion cameraQuat;
						currentState->GetRotation(cameraQuat);
						cameraRot = QuatToMatrix(cameraQuat);
					}
				}
			}
			case CameraDataMode::Transform: {
				//log::info("Camera State: Transform");
				auto camera = PlayerCamera::GetSingleton();
				if (camera) {
					auto& cameraRoot = camera->cameraRoot;
					if (cameraRoot) {
						cameraRot = cameraRoot->world.rotate;
					}
				}
			}
		}
		auto camera = PlayerCamera::GetSingleton();
		if (camera) {
			auto currentState = camera->currentState;
			if (currentState) {
				NiQuaternion cameraQuat;
				currentState->GetRotation(cameraQuat);
				cameraRot = QuatToMatrix(cameraQuat);
			}
		}
			
		return cameraRot;
	}

	// Get's camera position relative to the cameraActor
	NiPoint3 GetCameraPosLocal() {
		auto camera = PlayerCamera::GetSingleton();
		if (camera) {
			NiPointer<TESObjectREFR> Target = camera->cameraTarget.get();

			auto& currentState = camera->currentState;
			if (currentState) {
				auto player = GetCameraActor();
				if (player) {
					auto model = player->Get3D(false);
					if (model) {
						NiPoint3 cameraLocation = GetCameraPosition();
						auto playerTrans = model->world;
						playerTrans.scale = model->parent ? model->parent->world.scale : 1.0f; // Only do translation/rotation
						auto playerTransInve = playerTrans.Invert();
						// Get Scaled Camera Location
						return playerTransInve*cameraLocation;
					}
				}
			}
		}
		return {};
	}

	NiMatrix3 QuatToMatrix(const NiQuaternion& q){
		float sqw = q.w*q.w;
		float sqx = q.x*q.x;
		float sqy = q.y*q.y;
		float sqz = q.z*q.z;

		// invs (inverse square length) is only required if quaternion is not already normalised
		float invs = 1 / (sqx + sqy + sqz + sqw);
		float m00 = ( sqx - sqy - sqz + sqw)*invs; // since sqw + sqx + sqy + sqz =1/invs*invs
		float m11 = (-sqx + sqy - sqz + sqw)*invs;
		float m22 = (-sqx - sqy + sqz + sqw)*invs;

		float tmp1 = q.x*q.y;
		float tmp2 = q.z*q.w;
		float m10 = 2.0f * (tmp1 + tmp2)*invs;
		float m01 = 2.0f * (tmp1 - tmp2)*invs;

		tmp1 = q.x*q.z;
		tmp2 = q.y*q.w;
		float m20 = 2.0f * (tmp1 - tmp2)*invs;
		float m02 = 2.0f * (tmp1 + tmp2)*invs;
		tmp1 = q.y*q.z;
		tmp2 = q.x*q.w;
		float m21 = 2.0f * (tmp1 + tmp2)*invs;
		float m12 = 2.0f * (tmp1 - tmp2)*invs;

		return NiMatrix3{
			NiPoint3(m00, m01, m02),
			NiPoint3(m10, m11, m12),
			NiPoint3(m20, m21, m22)
		};
	}

	NiPoint3 FirstPersonPoint() {
		auto camera = PlayerCamera::GetSingleton();
		auto camState = camera->cameraStates[RE::CameraState::kFirstPerson].get();
		NiPoint3 cameraTrans;
		camState->GetTranslation(cameraTrans);
		return cameraTrans;
	}

	NiPoint3 ThirdPersonPoint() {
		auto camera = PlayerCamera::GetSingleton();
		auto camState = camera->cameraStates[RE::CameraState::kThirdPerson].get();
		NiPoint3 cameraTrans;
		camState->GetTranslation(cameraTrans);
		return cameraTrans;
	}

	float ZoomFactor() {
		auto camera = PlayerCamera::GetSingleton();
		auto camState = camera->cameraStates[RE::CameraState::kThirdPerson].get();
		if (camState) {
			ThirdPersonState* tpState = skyrim_cast<ThirdPersonState*>(camState);
			if (tpState) {
				return tpState->currentZoomOffset;
			}
		}
		return 0.0f;
	}

	NiPoint3 ComputeLookAt(float zoomScale) {
		NiPoint3 cameraTrans = GetCameraPosition();
		NiMatrix3 cameraRotMat = GetCameraRotation();

		float zoomOffset = ZoomFactor() * (*Hooks::Camera::fVanityModeMaxDist) * zoomScale;
		NiPoint3 zoomOffsetVec = NiPoint3(0.0f, zoomOffset + (*Hooks::Camera::fVanityModeMinDist), 0.0f);
		return cameraRotMat * zoomOffsetVec + cameraTrans;
	}

	static NiPoint3 GetAggregateBoneTarget(RE::Actor* a_actor) {

		if (CameraState* CurrentState = CameraManager::GetSingleton().GetCameraState()) {
			if (auto TPState = dynamic_cast<ThirdPersonCameraState*>(CurrentState)){

				if (TPState->GetBoneTarget().boneNames.empty()) {

					if (auto Node = find_node_any(a_actor, "NPC Neck [Neck]")) {
						return Node->world.translate;
					}

					return {};
				}

				NiAVObject* RootModel = a_actor->Get3D(false);
				NiTransform ActorTranslation = RootModel->world;
				NiTransform transform = ActorTranslation.Invert();
				ActorTranslation.scale = RootModel->parent ? RootModel->parent->world.scale : 1.0f;  // Only do translation/rotation
				BoneTarget boneTarget = TPState->GetBoneTarget();

				std::vector<NiAVObject*> bones = {};
				for (auto bone_name : boneTarget.boneNames) {
					NiAVObject* node = find_node(a_actor, bone_name);
					if (node) {
						bones.push_back(node);
					}
					else {
						log::error("Bone not found for camera target: {}", bone_name);
					}
				}

				NiPoint3 bonePos = NiPoint3();
				auto bone_count = bones.size();
				for (NiAVObject* bone : bones) {
					NiPoint3 worldPos = bone->world.translate;
					NiPoint3 localPos = transform * worldPos * get_visual_scale(a_actor);;
					bonePos += localPos * (1.0f / bone_count);
				}
				NiPoint3 worldBonePos = ActorTranslation * bonePos;

				if (IsDebugEnabled()) {
					DebugAPI::DrawSphere(glm::vec3(worldBonePos.x, worldBonePos.y, worldBonePos.z), 1.0f, 33, { 0.1f, 0.9f, 0.2f, 1.0f }, 5.0f);
				}

				return worldBonePos;
			}
		}
		return {};
	}

	float GetFrustrumNearDistance() {
		
		if (auto niCamera = GetNiCamera()) {
			return niCamera->GetRuntimeData2().viewFrustum.fNear;
		}

		return 15.0f;
	}

	void UpdateCamera(float a_ActorScale, NiPoint3 a_CameraLocalOffset, NiPoint3 a_ActorLocalOffset) {
		PlayerCamera* PlayerCamera = PlayerCamera::GetSingleton();
		NiPointer<NiNode>& CameraRoot = PlayerCamera->cameraRoot;
		Actor* CameraTargetActor = GetCameraActor();
		BSTSmartPointer<TESCameraState>& CurrentCameraState = PlayerCamera->currentState;

		if (CameraRoot && CurrentCameraState && CameraTargetActor) {
			NiTransform CameraWorldTransform = GetCameraWorldTransform();
			NiPoint3 CameraTranslation;
			CurrentCameraState->GetTranslation(CameraTranslation);

			if (a_ActorScale > 1e-4) {
				NiAVObject* Actor3D = CameraTargetActor->Get3D(false);
				if (Actor3D) {
					NiTransform ActorTransform = Actor3D->world;
					ActorTransform.scale = Actor3D->parent ? Actor3D->parent->world.scale : 1.0f;
					NiTransform InverseTransform = ActorTransform.Invert();

					// Standard transform calculations
					NiTransform ActorAdjustments = NiTransform();
					ActorAdjustments.scale = a_ActorScale;
					ActorAdjustments.translate = a_ActorLocalOffset;
					NiPoint3 TargetLocationWorld = ActorTransform * (ActorAdjustments * (InverseTransform * CameraTranslation));
					CameraWorldTransform.translate = TargetLocationWorld;

					NiTransform CameraAdjustments = NiTransform();
					CameraAdjustments.translate = a_CameraLocalOffset * a_ActorScale;
					NiPoint3 WorldShifted = CameraWorldTransform * CameraAdjustments * NiPoint3();
					NiNode* CameraRootParent = CameraRoot->parent;
					NiTransform InvertedRootTransform = CameraRootParent->world.Invert();

					const NiPoint3 LocalSpacePosition = InvertedRootTransform * WorldShifted;
					NiPoint3 RayCastHitPosition = LocalSpacePosition;

					// Collision handling
					NiPoint3 RayStart = GetAggregateBoneTarget(CameraTargetActor);
					if (RayStart != NiPoint3()) {
						RayCastHitPosition = ComputeRaycast(RayStart, LocalSpacePosition);

						//If less than frustrum it means the camera is stuck to the raycast origin bone.
						//"Revert" the colision position if this happens. Effectively disabling camera colision in this case.
						if (abs(RayCastHitPosition.GetDistance(RayStart)) <= GetFrustrumNearDistance() + std::numeric_limits<float>::epsilon()) {
							RayCastHitPosition = LocalSpacePosition;
						}

					}

					// Apply final transformations
					ComputeFrustrumNearDistance(a_ActorScale);
					UpdatePlayerCamera(RayCastHitPosition);
					UpdateNiCamera(RayCastHitPosition);

				}
			}
		}
	}
}