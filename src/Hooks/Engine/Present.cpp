#include "Hooks/Engine/Present.hpp"
#include "Hooks/Util/HookUtil.hpp"

#include "UI/GTSMenu.hpp"

using namespace GTS;

namespace Hooks {

	struct D3DPresent {

		static void thunk(uint32_t unk_01) {

			func(unk_01);

			{
				GTS_PROFILE_ENTRYPOINT("EnginePresent::DXGISwapchainPresent");
				if (GTSMenu::m_DrawOnPresent.load()) {
					GTSMenu::GetSingleton().Present();
				}
			}

		}

		FUNCTYPE_CALL func;

	};

	void Hook_Present::Install() {

		logger::info("Installing D3DPresent Hook...");

		stl::write_call<D3DPresent>(REL::RelocationID(75461, 77246, NULL), REL::VariantOffset(0x9, 0x9, NULL));

	}

}
