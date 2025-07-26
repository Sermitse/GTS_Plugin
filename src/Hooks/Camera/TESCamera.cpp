#include "Hooks/Camera/TESCamera.hpp"
#include "Hooks/Util/HookUtil.hpp"

using namespace GTS;

namespace Hooks {

	struct TESCameraUpdate {

		static constexpr std::size_t funcIndex = 0x02;

		static void thunk(RE::TESCamera* a_this) {

			GTS_PROFILE_ENTRYPOINT("Camera::TESCameraUpdate");

			func(a_this);
		}

		FUNCTYPE_VFUNC func;

	};


	void Hook_TESCamera::Install() {
		//Currently Unused
		log::info("Installing TESCamera::Update VTABLE Hooks...");
		stl::write_vfunc<TESCameraUpdate>(VTABLE_TESCamera[0]);
	}

}
