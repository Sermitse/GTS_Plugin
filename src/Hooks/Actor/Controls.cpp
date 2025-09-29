#include "Hooks/Actor/Controls.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Damage/TinyCalamity.hpp"
#include "Hooks/Util/HookUtil.hpp"

using namespace GTS;

namespace {

	bool IsGtsBusy_ForControls(Actor* actor) {
		bool GTSBusy = false;
		actor->GetGraphVariableBool("GTS_Busy", GTSBusy);
		// Have to use this because Hand Swipes make original bool return false
		return GTSBusy;
	}

	bool AllowToPerformSneak(RE::IDEvent* id) {
		bool allow = true;
		if (id) {
			auto player = PlayerCharacter::GetSingleton();
			if (player) {
				auto as_str = id->userEvent;
				if (as_str == "Sneak" && IsProning(player)) {
					if (player->IsSneaking()) {
						allow = false;
						AnimationManager::StartAnim("SBO_ProneOff", player);
					}
				}
				else if (as_str == "Activate") {
					if (TinyCalamity_WrathfulCalamity(player)) {
						allow = false;
					}
				}
			}
		}
		return allow;
	}

	bool CanMove() {

		auto player = PlayerCharacter::GetSingleton();
		if (!player) {
			return true;
		}

		Actor* Controlled = GetPlayerOrControlled();
		if (Controlled->formID != 0x14) {
			if (IsThighSandwiching(Controlled)) { // Disallow player movement if we have control over other actor and actor does thigh sandwich
				return false;
			} if (IsBetweenBreasts(Controlled)) {
				return false;
			}
		}
		if (IsFreeCameraEnabled()) {
			return true;
		}
		if (!AnimationsInstalled(player)) { // Don't mess with movement if user didn't install anims correctly
			return true;
		}
		if (IsTransitioning(player)) { // Disallow to move during transition
			return false;
		}
		if (IsGrabAttacking(player)) { // Allow movement for Grab Attacking
			return true;
		}
		return !IsGtsBusy_ForControls(player); // Else return GTS Busy
	}
}

namespace Hooks {

	struct CanProcess {

		static constexpr size_t funcIndex = 0x01;

		template<int ID>
		//void* is the class instance, can't use PlayerInputHandler as its abstract.
		static bool thunk(void* a_this, RE::InputEvent* a_event) {

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("ActorControls::CanProcess", ID);

				if (a_event->GetEventType() != INPUT_EVENT_TYPE::kMouseMove) {

					if (!CanMove()) {
						return false;
					}

					auto EvtID = a_event->AsIDEvent();
					if (!AllowToPerformSneak(EvtID)) {
						return false;
					}
				}
			}

			return func<ID>(a_this, a_event);
		}

		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;

	};


	void Hook_Controls::Install() {

		logger::info("Installing Control VTABLE Hooks...");

		stl::write_vfunc_unique<CanProcess, 0>(VTABLE_ActivateHandler[0]);
		stl::write_vfunc_unique<CanProcess, 1>(VTABLE_JumpHandler[0]);
		stl::write_vfunc_unique<CanProcess, 2>(VTABLE_ShoutHandler[0]);
		stl::write_vfunc_unique<CanProcess, 3>(VTABLE_SneakHandler[0]);
		stl::write_vfunc_unique<CanProcess, 4>(VTABLE_ToggleRunHandler[0]);
		stl::write_vfunc_unique<CanProcess, 5>(VTABLE_AttackBlockHandler[0]);

		//stl::write_vfunc_unique<CanProcess, 6>(VTABLE_AutoMoveHandler[0]);
		//stl::write_vfunc_unique<CanProcess, 7>(VTABLE_MovementHandler[0]);
		//stl::write_vfunc_unique<CanProcess, 8>(VTABLE_ReadyWeaponHandler[0]);
		//stl::write_vfunc_unique<CanProcess, 9>(VTABLE_RunHandler[0]);
		//stl::write_vfunc_unique<CanProcess, 10>(VTABLE_ThirdPersonState[0]);
		//stl::write_vfunc_unique<CanProcess, 11>(VTABLE_SprintHandler[0]);

	}

}