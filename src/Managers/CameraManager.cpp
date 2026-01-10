#include "Managers/CameraManager.hpp"

#include "Managers/Cameras/CamUtil.hpp"
#include "Managers/Input/InputManager.hpp"
#include "API/SmoothCam.hpp"

#include "Config/Config.hpp"

#include "Systems/Rays/raycast.hpp"

using namespace GTS;

namespace {

	void HorizontalResetEvent(const ManagedInputEvent& data) {
		auto& camera = CameraManager::GetSingleton();
		camera.ResetLeftRight();
	}

	void VerticalResetEvent(const ManagedInputEvent& data) {
		auto& camera = CameraManager::GetSingleton();
		camera.ResetUpDown();
	}

	void CamUpEvent(const ManagedInputEvent& data) {
		auto& camera = CameraManager::GetSingleton();
		float size = get_visual_scale(PlayerCharacter::GetSingleton());
		camera.AdjustUpDown(0.6f + (size * 0.05f - 0.05f));
	}

	void CamDownEvent(const ManagedInputEvent& data) {
		auto& camera = CameraManager::GetSingleton();
		float size = get_visual_scale(PlayerCharacter::GetSingleton());
		camera.AdjustUpDown(-(0.6f + (size * 0.05f - 0.05f)));
	}

	void CamLeftEvent(const ManagedInputEvent& data) {
		auto& camera = CameraManager::GetSingleton();
		float size = get_visual_scale(PlayerCharacter::GetSingleton());
		camera.AdjustLeftRight(-(0.6f + (size * 0.05f - 0.05f)));
	}

	void CamRightEvent(const ManagedInputEvent& data) {
		auto& camera = CameraManager::GetSingleton();
		float size = get_visual_scale(PlayerCharacter::GetSingleton());
		camera.AdjustLeftRight(0.6f + (size * 0.05f - 0.05f));
	}

	void SwitchCameraMode(const ManagedInputEvent& data) {
		int& Mode = Persistent::TrackedCameraState.value;

		// Increment by 1 and wrap around if we exceed the enum range
		constexpr int totalModes = static_cast<int>(magic_enum::enum_count<LCameraMode_t>());
		Mode = (Mode + 1) % totalModes;

		// Convert the integer mode to the corresponding enum value
		auto currentMode = static_cast<LCameraMode_t>(Mode);

		// Use magic_enum to get the enum name and humanize it
		const std::string Msg = fmt::format("Camera Mode: {}", HumanizeString(std::string(magic_enum::enum_name(currentMode))));

		Notify(Msg.c_str());
	}

	bool AutoCamEnabledCondition() {
		return Config::Camera.bAutomaticCamera;
	}

}

namespace GTS {

	std::string CameraManager::DebugName() {
		return "::CameraManager";
	}

	void CameraManager::DataReady() {
		InputManager::RegisterInputEvent("HorizontalCameraReset", HorizontalResetEvent, AutoCamEnabledCondition);
		InputManager::RegisterInputEvent("VerticalCameraReset", VerticalResetEvent, AutoCamEnabledCondition);

		InputManager::RegisterInputEvent("CameraUp", CamUpEvent, AutoCamEnabledCondition);
		InputManager::RegisterInputEvent("CameraDown", CamDownEvent, AutoCamEnabledCondition);
		InputManager::RegisterInputEvent("CameraLeft", CamLeftEvent, AutoCamEnabledCondition);
		InputManager::RegisterInputEvent("CameraRight", CamRightEvent, AutoCamEnabledCondition);

		//Ported From Papyrus
		InputManager::RegisterInputEvent("SwitchCameraMode", SwitchCameraMode, AutoCamEnabledCondition);

	}

	void CameraManager::CameraUpdate() {

		GTS_PROFILE_SCOPE("CameraManager: CameraUpdate");
		CameraState* CurrentState = this->GetCameraState();

		if (SmoothCam::Enabled()) {
			if (auto TPState = reinterpret_cast<ThirdPersonCameraState*>(GetCameraStateTP())) {
				if ((TPState == &this->CamStateFootL ||
					TPState == &this->CamStateFootR  ||
					TPState == &this->CamStateFoot   ||
					!TPState->GetBoneTarget().boneNames.empty())) { //Checks for Valid states when using Normal or Alt Cam
					//Take control from SC so we can do our own thing if one of these conditions match
					SmoothCam::RequestConrol();
				}
				else {
					//If not in one of the above states. Return camera control to SC.
					SmoothCam::ReturnControl();
					return;
				}
			}
		}

		if (CurrentState != this->TrackedState) {

			if (this->TrackedState) {
				this->TrackedState->ExitState();
			}

			if (CurrentState) {
				CurrentState->EnterState();
			}

			CameraState* PreviousState = this->TrackedState;
			this->TrackedState = CurrentState;
			if (PreviousState) {
				if (CurrentState) {
					if (CurrentState->PermitTransition() && PreviousState->PermitTransition()) {
						this->TransitionState.reset(new TransState(PreviousState, CurrentState));
						CurrentState = this->TransitionState.get();
					}
					else {
						this->TransitionState.reset(nullptr);
					}
				}
				else {
					this->TransitionState.reset(nullptr);
				}
			}
			else {
				this->TransitionState.reset(nullptr);
			}
		}
		else {
			if (this->TransitionState) {
				if (!this->TransitionState->IsDone()) {
					CurrentState = this->TransitionState.get();
				}
				else {
					this->TransitionState.reset(nullptr);
				}
			}
		}

		// Handles updating the camera
		if (CurrentState) {

			auto player = PlayerCharacter::GetSingleton();
			bool IsCurrentlyCrawling = AnimationVars::Crawl::IsCrawling(player);
			if (IsGtsBusy(player) && AnimationVars::Crawl::IsCrawling(player) && GetCameraOverride(player)) {
				IsCurrentlyCrawling = false;
			}
			else if (AnimationVars::Prone::IsProne(player)) {
				IsCurrentlyCrawling = true;
			}

			// Get scale based on camera state
			float scale = CurrentState->GetScale();

			// Get current camera position in player space
			auto cameraPosLocal = GetCameraPosLocal();

			// Get either normal or combat offset
			NiPoint3 offset;
			if (player != nullptr && player->AsActorState()->IsWeaponDrawn()) {
				offset = CurrentState->GetCombatOffset(cameraPosLocal, IsCurrentlyCrawling);
			}
			else {
				offset = CurrentState->GetOffset(cameraPosLocal, IsCurrentlyCrawling);
			}

			NiPoint3 playerLocalOffset = CurrentState->GetPlayerLocalOffset(cameraPosLocal, IsCurrentlyCrawling);

			if (CurrentState->PermitManualEdit()) {
				this->SpringSmoothOffset.target = this->ManualEditOffsets;
			}

			offset += this->SpringSmoothOffset.value;
			this->SpringSmoothScale.target = scale;

			// Apply camera scale and offset
			if (CurrentState->PermitCameraTransforms()) {
				ComputeAndApplyFinalCameraTransforms(this->SpringSmoothScale.value, offset, playerLocalOffset);
			}
		}
	}

	CameraState* CameraManager::GetCameraStateTP() {

		auto Mode = static_cast<LCameraMode_t>(Persistent::TrackedCameraState.value);

		switch (Mode) {

			case LCameraMode_t::kNormal: {
				return &this->CamStateNormal;
			}

			case LCameraMode_t::kAlternative: {
				return &this->CamStateAlt;
			}

			case LCameraMode_t::kFeetCenter: {
				return &this->CamStateFoot;
			}

			case LCameraMode_t::kFootLeft: {
				return &this->CamStateFootL;
			}

			case LCameraMode_t::kFootRight: {
				return &this->CamStateFootR;
			}

			default: {
				return nullptr;
			}
		}
	}

	CameraState* CameraManager::GetCameraStateFP() {
		//Other states are now deprecated
		return &this->CamStateFP;
	}

	// Decide which camera state to use
	CameraState* CameraManager::GetCameraState() {

		if (!Config::Camera.bAutomaticCamera || IsFreeCameraEnabled()) {
			return nullptr;
		}

		bool AllowFpCamera = true;
		auto playerCamera = PlayerCamera::GetSingleton();
		if (!playerCamera) {
			return nullptr;
		}

		if (Config::General.bConversationCamCompat && Runtime::IsAltConversationCamInstalled()) {
			auto ui = RE::UI::GetSingleton();
			if (ui) {
				if (ui->IsMenuOpen(DialogueMenu::MENU_NAME)) {
					if (GetCameraActor() && GetCameraActor()->formID != 0x14) {
						return nullptr;
					}
				}
			}
		}

		auto playerCameraState = playerCamera->currentState;
		if (!playerCameraState) {
			return nullptr;
		}
		RE::CameraState playerCameraMode = playerCameraState->id;

		switch (playerCameraMode) {
			// Fp state
			case RE::CameraState::kFirstPerson: {
				if (AllowFpCamera) {
					return this->GetCameraStateFP();
				} else {
					return nullptr;
				}
			}
			// All these are TP like states
			case RE::CameraState::kThirdPerson:
			case RE::CameraState::kAutoVanity:
			case RE::CameraState::kFurniture:
			case RE::CameraState::kMount:
			case RE::CameraState::kBleedout:
			case RE::CameraState::kDragon: {
				return this->GetCameraStateTP();
			}
			// These ones should be scaled but not adjusted
			// any other way like pointing at feet when using
			// kIronSights
			case RE::CameraState::kVATS:
			case RE::CameraState::kFree:
			case RE::CameraState::kPCTransition:
			case RE::CameraState::kIronSights: {
				return &this->CamStateVanillaScaled;
			}
			// These should not be touched at all
			case RE::CameraState::kTween:
			case RE::CameraState::kAnimated: {
				return nullptr;
			}
			// Catch all in case I forgot something
			default: {
				return nullptr;
			}
		}
	}

	void CameraManager::AdjustUpDown(float amt) {
		this->ManualEditOffsets.z += amt;
	}
	void CameraManager::ResetUpDown() {
		this->ManualEditOffsets.z = 0.0f;
	}

	void CameraManager::AdjustLeftRight(float amt) {
		this->ManualEditOffsets.x += amt;
	}
	void CameraManager::ResetLeftRight() {
		this->ManualEditOffsets.x = 0.0f;
	}

	void CameraManager::Reset() {

		SpringSmoothScale = Spring(0.3f, 0.5f);
		SpringSmoothOffset = Spring3(NiPoint3(0.30f, 0.30f, 0.30f), 0.50f);

		CamStateNormal.SpringSmoothScale = Spring(1.0f, 0.5f);
		CamStateNormal.SpringSmoothedBonePos = Spring3(NiPoint3(0.0f, 0.0f, 0.0f), 0.5f);

		CamStateAlt.SpringSmoothScale = Spring(1.0f, 0.5f);
		CamStateAlt.SpringSmoothedBonePos = Spring3(NiPoint3(0.0f, 0.0f, 0.0f), 0.5f);

		CamStateFoot.SpringSmoothScale = Spring(1.0f, 0.5f);
		CamStateFoot.SpringSmoothedBonePos = Spring3(NiPoint3(0.0f, 0.0f, 0.0f), 0.5f);

		CamStateFootR.SpringSmoothScale = Spring(1.0f, 0.5f);
		CamStateFootR.SpringSmoothedBonePos = Spring3(NiPoint3(0.0f, 0.0f, 0.0f), 0.5f);

		CamStateFootL.SpringSmoothScale = Spring(1.0f, 0.5f);
		CamStateFootL.SpringSmoothedBonePos = Spring3(NiPoint3(0.0f, 0.0f, 0.0f), 0.5f);

		ManualEditOffsets = { 0,0,0 };

		TrackedState = nullptr;
		TransitionState.reset(nullptr);

		logger::info("CameraManager Reset");
	}

	void CameraManager::ComputeAndApplyFinalCameraTransforms(float a_ActorScale, NiPoint3 a_CameraLocalOffset, NiPoint3 a_ActorLocalOffset) {
		static PlayerCamera* const PlayerCamera = PlayerCamera::GetSingleton();
		NiPointer<NiNode>& CameraRoot = PlayerCamera->cameraRoot;
		Actor* const CameraTargetActor = GetCameraActor();
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
						if (abs(RayCastHitPosition.GetDistance(RayStart)) <= GetFrustrumNearDistance() + 1e-2) {
							RayCastHitPosition = LocalSpacePosition;
						}

					}

					// Apply final transformations
					SetCameraNearFarPlanes(a_ActorScale);
					UpdatePlayerCamera(RayCastHitPosition);
					UpdateNiCamera(RayCastHitPosition);
				}
			}
		}
	}

	void CameraManager::Update() {
		EnforceCameraINISettings();
	}
}
