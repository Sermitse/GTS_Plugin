#include "Hooks/Camera/PlayerCamera.hpp"
#include "Hooks/Util/HookUtil.hpp"

namespace Hooks {


	struct PlayerCameraUpdate {

		static constexpr std::size_t funcIndex = 0x02;

		static void thunk(RE::PlayerCamera* a_this) {

			GTS_PROFILE_ENTRYPOINT("Camera::PlayerCameraUpdate");

			func(a_this);
		}

		FUNCTYPE_VFUNC func;
	};

	void Hook_PlayerCamera::Install() {
		//Currently Unused
		log::info("Installing PlayerCamera::Update VTABLE Hooks...");
		stl::write_vfunc<PlayerCameraUpdate>(VTABLE_PlayerCamera[0]);
	}

}
