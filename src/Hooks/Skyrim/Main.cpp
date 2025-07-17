#include "Hooks/Skyrim/Main.hpp"
#include "Data/Time.hpp"
#include "Data/Plugin.hpp"



using namespace GTS;

namespace {
	void ExperimentalStuff() {
		if (Plugin::Live()) {
			EventDispatcher::DoBoneUpdate();
		}
	}
}

namespace Hooks {

	void Hook_MainUpdate::Hook(Trampoline& trampoline) {

		// Credits to Ersh for this hook, DCA source code
		REL::Relocation<uintptr_t> hook{REL::RelocationID(35565, 36564)}; 

		// ^ 5B2FF0, 5DACE0, main update
		log::info("Applying Main Update Hook at {:#X}", hook.address());
		_Update = trampoline.write_call<5>(hook.address() + RELOCATION_OFFSET(0x748, 0xC26), Update);

	}

	void Hook_MainUpdate::Update(RE::Main* a_this, float a2) {

		_Update(a_this, a2);

		static std::atomic_bool started = std::atomic_bool(false);

		Plugin::SetOnMainThread(true);

		if (Plugin::Live()) {

			//Cache all currently loaded Actors
			cache_actorList();

			// We are not loading or in the mainmenu
			// Player loaded and not paused
			if (started.exchange(true)) {
				// Not first updated
				Time::GetSingleton().Update();
				EventDispatcher::DoUpdate();
			} else {
				// First update this load
				EventDispatcher::DoStart();
			}
		} 
		else if (!Plugin::InGame()) {
			// Loading or in main menu
			started.store(false);
		}
		Plugin::SetOnMainThread(false);

		/*if (Profiler::ProfilerEnabled) {
			static Timer timer = Timer(5.0);
			if (timer.ShouldRun()) {
				Profilers::Report();
			}
		}*/
	}
}
