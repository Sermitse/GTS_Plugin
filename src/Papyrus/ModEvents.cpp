#include "Papyrus/ModEvents.hpp"
#include "Managers/ModEvent.hpp"

using namespace GTS;
using namespace RE::BSScript;

namespace {

	constexpr std::string_view PapyrusClass = "GTSEvent";

	void RegisterOnFootstep(StaticFunctionTag*, TESForm* form) {
		if (!form) {
			return;
		}
		ModEventManager::GetSingleton().m_onfootstep.Register(form);
	}

	void UnRegisterOnFootstep(StaticFunctionTag*, TESForm* form) {
		if (!form) {
			return;
		}
		ModEventManager::GetSingleton().m_onfootstep.Unregister(form);
	}
}

namespace GTS {

	bool register_papyrus_events(IVirtualMachine* vm) {
		vm->RegisterFunction("RegisterOnFootstep", PapyrusClass, RegisterOnFootstep);
		vm->RegisterFunction("UnRegisterOnFootstep", PapyrusClass, UnRegisterOnFootstep);
		return true;
	}

}
