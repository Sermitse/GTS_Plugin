#include "Managers/SpectatorManager.hpp"

using namespace RE;

namespace {
	Actor* GetCameraActor() {
		auto camera = PlayerCamera::GetSingleton();
		return camera->cameraTarget.get().get();
	}
}

namespace GTS {

	Actor* SpectatorManager::GetCameraTarget() {
		Actor* player = PlayerCharacter::GetSingleton();
		Actor* Target_Actor = TESForm::LookupByID<Actor>(Target);
		if (Target_Actor) {
			return Target_Actor;
		} else {
			return player;
		}
	}

	void SpectatorManager::SetCameraTarget(RE::Actor* a_Actor, bool aTemporary) {
		if (!a_Actor) {
			return;
		}
		auto Camera = PlayerCamera::GetSingleton();
		if (Camera) {
			auto NewTarget = a_Actor->CreateRefHandle();
			if (NewTarget) {
				Camera->cameraTarget = NewTarget;
				if (!aTemporary) {
					Target = a_Actor->formID;
					// Reset the lost flag when we deliberately set a target
					TrackedActorLost = false; 
				}
			}
		}
	}

	std::string SpectatorManager::DebugName() {
		return "::SpectatorManager";
	}

	void SpectatorManager::Update() {

		if (State::Live()) {

			auto currentCameraTarget = GetCameraActor();

			// Handle case when there's no camera target
			if (!currentCameraTarget) {
				ResetTarget(true);
				return;
			}

			// Case 1: Current camera target is not loaded - switch to player temporarily if not already
			// Normaly the player never is unloaded so if the target is the player this case should never trigger.
			// even if it did though it wont do anything undesired.
			// This soely exists because if a tracked npc leaves the currect cell there's a chance the game will just die.
			// This pressumably happens because after fininshing loading post cell transition the tracked actor's 3d is not yet loaded causing a nullptr dereference
			// meanwhile the player's 3d data appears to always be valid from testing.

			if (!currentCameraTarget->Is3DLoaded() || !currentCameraTarget->Get3D() || !currentCameraTarget->Get3D1(false)) {

				// Only temporarily reset to the player without changing Target
				if (const auto Player = PlayerCharacter::GetSingleton()) {
					SetCameraTarget(Player, true);
					TrackedActorLost = true; // Mark that our tracked actor was lost
				}
				return;
			}

			// Case 2: Camera target differs from our stored target
			if (currentCameraTarget->formID != Target) {

				// If the current target is the player, we need to check if it's because our tracked actor was lost
				// or if an external source changed to the player deliberately

				if (currentCameraTarget->IsPlayerRef()) {

					if (TrackedActorLost) {

						// We're on the player because our tracked actor was lost
						// Check if our intended target is now valid again

						const auto intendedTarget = TESForm::LookupByID<Actor>(Target);
						if (intendedTarget && intendedTarget->Is3DLoaded() && intendedTarget->Get3D() && intendedTarget->Get3D1(false)) {
							// Our previously lost target is valid and loaded again - switch back to it
							SetCameraTarget(intendedTarget, false);
						}

					}
					else {

						// We're on the player but not because a tracked actor was lost
						// This means an external source intentionally switched to the player
						// Update our target to match

						Target = 0x14;
					}
				}
				else {

					// Current target is not the player and differs from our stored target
					// This means an external source changed the target
					// If our current target is the player, update to the new external target

					if (Target == 0x14) {
						Target = currentCameraTarget->formID;
						TrackedActorLost = false;
					}
					else {

						// We had a npc target, but something external changed it to another npc
						// Check if our intended target is still valid

						auto intendedTarget = TESForm::LookupByID<Actor>(Target);
						if (!intendedTarget || !intendedTarget->Is3DLoaded() || !intendedTarget->Get3D() || !intendedTarget->Get3D1(false)) {
							// Our target is no longer valid
							ResetTarget(true);
						}
						else {
							// Our target is valid and loaded - switch back to it
							SetCameraTarget(intendedTarget, false);
						}
					}
				}
			}
			else {
				// Current camera target matches our stored target
				// If we previously marked the actor as lost, clear that flag
				if (TrackedActorLost) {
					TrackedActorLost = false;
				}
			}
		}
	}

	bool SpectatorManager::IsCameraTargetPlayer() {
		if (const auto CamTarget = GetCameraActor()) {
			return CamTarget->IsPlayerRef();
		}
		return false;
	}

	void SpectatorManager::ResetTarget(bool aDoFullReset) {
		if (const auto Player = PlayerCharacter::GetSingleton()) {
			SetCameraTarget(Player, !aDoFullReset);
			if (aDoFullReset) {
				Target = 0x14; // Set target to player's formID
				TrackedActorLost = false; // Reset the lost flag when we do a full reset
			}
		}
	}

	void SpectatorManager::Reset() {
		ResetTarget(true);
	}

	void SpectatorManager::ResetActor(Actor* actor) {
		if (const auto& CameraTarget = GetCameraActor()) {
			if (actor) {
				if (CameraTarget->formID == actor->formID) {
					ResetTarget(true);
				}
			}
		}
	}
}