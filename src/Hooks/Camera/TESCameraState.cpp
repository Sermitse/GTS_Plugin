#include "Hooks/Camera/TESCameraState.hpp"
#include "Hooks/Util/HookUtil.hpp"

namespace Hooks {

	struct TESCameraStateUpdate {
		
		static constexpr std::size_t funcIndex = 0x03;

		template<int ID>
		static void thunk(TESCameraState* a_this, BSTSmartPointer<TESCameraState>& a_nextState) {

			GTS_PROFILE_ENTRYPOINT_UNIQUE("Camera::TESCameraStateUpdate", ID);

			func<ID>(a_this, a_nextState);
			GTS::EventDispatcher::DoCameraUpdate();
		}

		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;

	};

	void Hook_TESCameraState::Install() {

		logger::info("Installing TESCameraState::Update VTABLE Hooks...");

		stl::write_vfunc_unique<TESCameraStateUpdate, 0>(VTABLE_TESCameraState[0]);
		stl::write_vfunc_unique<TESCameraStateUpdate, 1>(VTABLE_ThirdPersonState[0]);
		stl::write_vfunc_unique<TESCameraStateUpdate, 2>(VTABLE_DragonCameraState[0]);
		stl::write_vfunc_unique<TESCameraStateUpdate, 3>(VTABLE_HorseCameraState[0]);
		stl::write_vfunc_unique<TESCameraStateUpdate, 4>(VTABLE_FirstPersonState[0]);
		stl::write_vfunc_unique<TESCameraStateUpdate, 5>(VTABLE_FreeCameraState[0]);
		stl::write_vfunc_unique<TESCameraStateUpdate, 6>(VTABLE_PlayerCameraTransitionState[0]);
		stl::write_vfunc_unique<TESCameraStateUpdate, 7>(VTABLE_BleedoutCameraState[0]);
		stl::write_vfunc_unique<TESCameraStateUpdate, 8>(VTABLE_VATSCameraState[0]);
		stl::write_vfunc_unique<TESCameraStateUpdate, 9>(VTABLE_FurnitureCameraState[0]);
		stl::write_vfunc_unique<TESCameraStateUpdate, 10>(VTABLE_AutoVanityState[0]);

		//IronSightsState has no RE'd Class member so just pass its vtable REL ID
		stl::write_vfunc_unique<TESCameraStateUpdate, 11>(VTABLE_IronSightsState[0]);

		//Disabled, Causes single frame camera missplacement when exiting the tween menu
		//stl::write_vfunc_unique<TESCameraStateUpdate, 12>(VTABLE_TweenMenuCameraState[0]);
	}

}
