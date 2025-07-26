#include "Hooks/Papyrus/VM.hpp"
#include "Hooks/Util/HookUtil.hpp"

namespace Hooks {

	struct VirtualMachineSendEvent {

		static inline const std::size_t funcIndex = REL::Relocate(0x24, 0x24, 0x26);

		static void thunk(BSScript::Internal::VirtualMachine* a_this, VMHandle a_handle, const BSFixedString& a_eventName, BSScript::IFunctionArguments* a_args) {

			GTS_PROFILE_ENTRYPOINT("PapyrusVM::VirtualMachineSendEvent");

			func(a_this, a_handle, a_eventName, a_args);

			if (a_eventName == "OnUpdate") {
				EventDispatcher::DoPapyrusUpdate();
			}

		}

		FUNCTYPE_VFUNC func;
	};

	void Hook_VM::Install() {
		logger::info("Installing VM VTABLE Hooks...");
		stl::write_vfunc<VirtualMachineSendEvent>(VTABLE_BSScript__Internal__VirtualMachine[0]);
	}

}
