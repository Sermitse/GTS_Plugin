#include "Hooks/Engine/Main.hpp"
#include "Hooks/Util/HookUtil.hpp"



namespace Hooks {
	namespace {
		bool s_smpBridgeActive = false;
		bool s_smpInstalled = false;
		std::string s_smpBridgeStatus = "not installed";

		//True when a_target lands inside SkyrimSE.exe. A call site still pointing into the game
		//module has not been hooked by anyone yet.
		bool PointsIntoGameModule(std::uintptr_t a_target) {

			const std::uintptr_t base = REL::Module::get().base();

			if (!base || a_target < base) {
				return false;
			}

			const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
			const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);

			return a_target < base + nt->OptionalHeader.SizeOfImage;
		}

		//Current destination of the rel32 call at a_site.
		std::uintptr_t ReadCallTarget(std::uintptr_t a_site) {
			const auto rel = *reinterpret_cast<const std::int32_t*>(a_site + 1);
			return a_site + 5 + static_cast<std::uintptr_t>(rel);
		}
	}

	struct MainUpdatePost {

		static inline std::atomic_bool started = false;

		static void thunk(RE::Main* a_this, float a_deltaTime) {

			func(a_this, a_deltaTime);

			{
				GTS_PROFILE_ENTRYPOINT("EngineMain::MainUpdatePost");

				State::SetOnMainThread(true);

				if (State::Live()) {

					// We are not loading or in the mainmenu
					// Player loaded and not paused
					if (started.exchange(true)) {
						// Not first updated
						Time::Update();
						EventDispatcher::DoUpdate();
					}
					else {
						// First update this load
						EventDispatcher::DoStart();
					}
				}
				else if (!State::InGame()) {
					// Loading or in main menu
					started.store(false);
				}
				State::SetOnMainThread(false);
			}

		}

		FUNCTYPE_CALL func;
	};

	struct SMPPostPhysics {
		static void thunk(void* a_this) {
			func(a_this);
			EventDispatcher::DoPostPhysicsUpdate();
		}

		FUNCTYPE_CALL func;
	};
	void Hook_MainUpdate::Install() {
		logger::info("Installing Main Update Hooks...");
		//Update happens at the end of the Main::Update loop right before a BSLightingShader subroutine.
		stl::write_call<MainUpdatePost>(RelocationIDEx(35565, 36564, 36564, NULL), OffsetEx(0x748, 0xC2B, 0xC3D, NULL));
	}

	bool SMPBridgeActive() { return s_smpBridgeActive; }
	bool SMPInstalled() {return s_smpInstalled;}
	std::string_view SMPBridgeStatus() { return s_smpBridgeStatus; }

	void InstallSMPBridge() {
		if (s_smpBridgeActive) {
			return;
		}

		const RelocationIDEx site{ 35565, 36564, 36564, NULL };
		const OffsetEx offset{ 0x56D, 0x9DC, 0x9EE, NULL };
		const std::uintptr_t address = site.address() + offset.offset();

		if (!address) {
			s_smpBridgeStatus = "call site could not be resolved";
			logger::warn("SMP bridge: {}", s_smpBridgeStatus);
			return;
		}

		//Whoever wrote this call last is outermost. If it still points into SkyrimSE.exe then SMP
		//has not hooked it, which means either SMP is absent or it will wrap us later - and in that
		//second case we would run before its write and read the animation pose, silently.
		const std::uintptr_t existing = ReadCallTarget(address);
		const bool unhooked = PointsIntoGameModule(existing);

		stl::write_call<SMPPostPhysics>(site, offset);
		s_smpBridgeActive = true;
		s_smpInstalled = !unhooked;
		if (unhooked) {
			s_smpBridgeStatus = "installed, but nothing had hooked the call site - either HDT-SMP is not present, or it will wrap us and we will run before its write";
			logger::warn("SMP bridge: {}", s_smpBridgeStatus);
		}
		else {
			s_smpBridgeStatus = "installed outermost, running after HDT-SMP's transform write";
			logger::info("SMP bridge: {} (previous target 0x{:X})", s_smpBridgeStatus, existing);
		}
	}
}