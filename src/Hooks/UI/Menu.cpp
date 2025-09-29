#include "Hooks/UI/Menu.hpp"
#include "UI/UIManager.hpp"
#include "Hooks/Util/HookUtil.hpp"

namespace Hooks {

	//HudMenu Present
	//
	struct HUDMenuPostDisplay {
		static constexpr std::size_t funcIndex = 0x06;

		static void thunk(RE::IMenu* a_menu) {

			func(a_menu);
			{
				GTS_PROFILE_ENTRYPOINT("Menu::HUDMenuPostDisplay");
				UIManager::GetSingleton().Present(ImWindow::DrawLevel::kHUD);
			}

		}

		FUNCTYPE_VFUNC func;

	};

	//CursorMenu Present
	struct CursorMenuPostdisplay {

		static constexpr std::size_t funcIndex = 0x06;

		static void thunk(RE::IMenu* a_menu) {

			{
				GTS_PROFILE_ENTRYPOINT("Menu::CursorMenuPostDisplay");
				UIManager::GetSingleton().Present(ImWindow::DrawLevel::kCursor);
			}

			func(a_menu);

		}

		FUNCTYPE_VFUNC func;

	};

	struct MenuManagerUpdate {

		static void thunk(RE::BSScaleformManager* a_this) {

			{
				GTS_PROFILE_ENTRYPOINT("EnginePresent::MenuManagerUpdate");
				UIManager::GetSingleton().Update();
			}

			func(a_this);

		}

		FUNCTYPE_CALL func;

	};

	void Hook_Menu::Install() {

		logger::info("Installing Menu Hooks...");

		stl::write_vfunc<HUDMenuPostDisplay>(VTABLE_HUDMenu[0]);
		stl::write_vfunc<CursorMenuPostdisplay>(VTABLE_CursorMenu[0]);
		stl::write_call<MenuManagerUpdate>(REL::RelocationID(35556, 36555, NULL), REL::VariantOffset(0x3ab, 0x371, NULL));

	}
}