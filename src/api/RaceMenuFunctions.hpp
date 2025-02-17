#include "utils/papyrusUtils.hpp"
#include "data/transient.hpp"
#include "data/time.hpp"

#pragma once


// Credits to SlaveTatsNG   (https://github.com/nopse0/SlaveTatsNG/blob/423cc566bd955ec3e5f8b70112085b6bce949de2/include/nioverride_impl_old.h)

/*
   Morphs for possible future usage if we will be able to use Racemenu API:
   "Breasts"  [27] is = to displayNames[27]  = "Breasts - Size" for example
   Can be found inside RaceMenuMorphs3BB.psc
*/

/*
namespace {
	static inline std::array<int, 12> _offsets4_15{
		0xBA0B0,  // GetNodeOverrideInt  140718186930352 - 140718186168320 = 762.032 = B A0B0
		0xB9FD0,  // GetNodeOverrideFloat 140718186930128 - 140718186168320 = 761.808 = B 9FD0
		0xBA260,  // GetNodeOverrideString  140718186930784 - 140718186168320 = 762.464 = B A260
		0xB9910,  // AddNodeOverrideInt 140718186928400 - 140718186168320 = 760.080 = B 9910
		0xB96B0,  // AddNodeOverrideFloat 140718186927792 - 140718186168320 = 759.472 = B 96B0
		0xB9B60,  // AddNodeOverrideString 140718186928992 - 140718186168320 = 760.672 = B 9B60
		0x96970,  // HasNodeOverride 140718186785136 - 140718186168320 = 616.816 = 9 6970
		0x96DE0,  // RemoveNodeOverride 140718186786272 - 140718186168320 = 617.952 = 9 6DE0
		0x96920,  // ApplyNodeOverrides 140718186785056 - 140718186168320 = 616.736 = 9 6920
		0x96350,  // AddOverlays 140718186783568 - 140718186168320 = 615.248 = 9 6350
		0x96370,  // HasOverlays 140718186783600 - 140718186168320 = 615.280 = 9 6370
		0x96390   // RemoveOverlays 140718186783632 - 140718186168320 = 615.312 = 9 6390
	};

	static inline std::array<int, 12> _offsets97{
		0xBE160,
		0xBE090,
		0xBE2F0,
		0xBD9E0,
		0xBD780,
		0xBDC30, // AddNodeOverrideFloat
		0x99D80,
		0x9A1D0,
		0x99D30,
		0x99760,
		0x99780,
		0x997A0
	};

	static inline std::array<int, 12> _offsets640{
		0xCA230,
		0xCA160,
		0xCA3C0,
		0xC9AD0,
		0xC9860,
		0xC9D20,
		0xA5940,
		0xA5CA0,
		0xA5930,
		0xA5350,
		0xA53D0,
		0xA53F0
	};

	static inline std::array<int, 12> _offsets659{
		0xCA390,
		0xCA2C0,
		0xCA520,
		0xC9C30,
		0xC99C0,
		0xC9E80,
		0xA5AA0,
		0xA5E00,
		0xA5A90,
		0xA54B0,
		0xA5530,
		0xA5550
	};

	
}

namespace RaceMenu {
	class legacy_binding
	{
		static inline legacy_binding* GetSingleton() {
			static legacy_binding _instance;

			static std::atomic_bool _initialized;
			static std::latch       _latch(1);

			if (!_initialized.exchange(true)) {
				HMODULE skeeBaseAddr = GetModuleHandleA("skeevr.dll");
				if (skeeBaseAddr == NULL) {
					skeeBaseAddr = GetModuleHandleA("skee64.dll");
					logger::info("GetModuleHandleA('skee64.dll') = {}", (uintptr_t)skeeBaseAddr);
				} else {
					logger::info("GetModuleHandleA('skeevr.dll') = {}", (uintptr_t)skeeBaseAddr);
				}
				auto&   skeeOffsets = _offsets640;

				const auto skyrimVer = REL::Module::get().version();

				if (skyrimVer.major() == 1 && skyrimVer.minor() == 6 && skyrimVer.patch() == 640) {
					log::info("Using NIOverride addresses for Skyrim 1.6.640");
				} else if (skyrimVer.major() == 1 && skyrimVer.minor() == 5 && skyrimVer.patch() == 97) {
					log::info("Using NIOverride addresses for Skyrim 1.5.97");
					skeeOffsets = _offsets97;
				} else if (skyrimVer.major() == 1 && skyrimVer.minor() == 6 && skyrimVer.patch() == 659) {
					log::info("Using NIOverride addresses for Skyrim 1.6.659");
					skeeOffsets = _offsets659;
				} else if (skyrimVer.major() == 1 && skyrimVer.minor() == 4 && skyrimVer.patch() == 15) {
					log::info("Using NIOverride addresses for Skyrim 1.4.15");
					skeeOffsets = _offsets4_15;
				}

				auto addr = (char*)skeeBaseAddr + skeeOffsets[0];
				_instance._GetNodeOverrideInt = (_GetNodeOverrideInt_t)addr;
				addr = (char*)skeeBaseAddr + skeeOffsets[1];
				_instance._GetNodeOverrideFloat = (_GetNodeOverrideFloat_t)addr;
				addr = (char*)skeeBaseAddr + skeeOffsets[2];
				_instance._GetNodeOverrideString = (_GetNodeOverrideString_t)addr;
				addr = (char*)skeeBaseAddr + skeeOffsets[3];
				_instance._AddNodeOverrideInt = (_AddNodeOverrideInt_t)addr;
				addr = (char*)skeeBaseAddr + skeeOffsets[4];
				_instance._AddNodeOverrideFloat = (_AddNodeOverrideFloat_t)addr;
				addr = (char*)skeeBaseAddr + skeeOffsets[5];
				_instance._AddNodeOverrideString = (_AddNodeOverrideString_t)addr;
				addr = (char*)skeeBaseAddr + skeeOffsets[6];
				_instance._HasNodeOverride = (_HasNodeOverride_t)addr;
				addr = (char*)skeeBaseAddr + skeeOffsets[7];
				_instance._RemoveNodeOverride = (_RemoveNodeOverride_t)addr;
				addr = (char*)skeeBaseAddr + skeeOffsets[8];
				_instance._ApplyNodeOverrides = (_ApplyNodeOverrides_t)addr;
				addr = (char*)skeeBaseAddr + skeeOffsets[9];
				_instance._AddOverlays = (_AddOverlays_t)addr;
				addr = (char*)skeeBaseAddr + skeeOffsets[10];
				_instance._HasOverlays = (_HasOverlays_t)addr;
				addr = (char*)skeeBaseAddr + skeeOffsets[11];
				_instance._RemoveOverlays = (_RemoveOverlays_t)addr;

				_latch.count_down();
			}

			_latch.wait();
			return &_instance;
		}
	public:
		static void SetBodyMorph(TESObjectREFR* a_ref, string morphName, string keyName, float value) { // calls NiOverride.pex script directly
			CallFunction("NiOverride", "SetBodyMorph", a_ref, morphName, keyName, value);
			CallFunction("NiOverride", "UpdateModelWeight", a_ref);	
		}

		static void GrowBreastsOverTime(Actor* giant) { // Currently relies on doing DLL -> pex script -> other DLL method which may be slow so it's disabled
		// Should probably be capped at +50 or +100% of natural breast size
		// As a fun thing can probably even try to calculate total player weight based on morph values if we will manage to use RaceMenu functions directly
        	std::string taskname = std::format("GrowBreasts_{}", giant->formID);
			ActorHandle giantHandle = giant->CreateRefHandle();

			auto transient = Transient::GetSingleton().GetActorData(giant);
			double startTime = Time::WorldTimeElapsed();

			float duration = 3.0f;
			float total_size_add = 1.0f;
			float initial_size = 0.0f;
			if (transient) {
				initial_size = transient->breast_size_buff;
			}

			TaskManager::Run(taskname, [=](auto& progressData){
				if (!giantHandle) {
					return false;
				}

				double endTime = Time::WorldTimeElapsed();

				Actor* giant = giantHandle.get().get();
				float timepassed = endTime - startTime;
				float breast_buff = (initial_size) + (timepassed * 0.33f) * total_size_add;

				RaceMenu::legacy_binding::SetBodyMorph(giant, "Breasts", "3BBB-RaceMenuMorphs.esp", breast_buff);

				if (timepassed >= static_cast<double>(duration)) {
					if (transient) {
						transient->breast_size_buff = breast_buff;
					}
					return false;
				}
				return true;
        	});
		}

		static void OverrideNodeFloat(Actor* a_ref, bool a_female, const char* a_node, int a_key, int a_index, float a_value, bool persistent) // Sadly doesn't work with body morphs
		{	
			auto _instance = GetSingleton();
			if (_instance) {
				RE::StaticFunctionTag _staticfunctag;
				_instance->_AddNodeOverrideFloat(&_staticfunctag, a_ref, a_female, a_node, a_key, a_index, a_value, persistent);
				_instance->_ApplyNodeOverrides(&_staticfunctag, a_ref);

				log::info("{} Value for key {} is {}", a_node, a_key, _instance->_GetNodeOverrideFloat(&_staticfunctag, a_ref, a_female, a_node, a_key, a_index));
			}
		}

		typedef void              (*_AddNodeOverrideFloat_t)(RE::StaticFunctionTag*, RE::TESObjectREFR* a_ref, bool a_female, const char* a_node, int a_key, int a_index, float a_value, bool);
		typedef void              (*_AddNodeOverrideInt_t)(RE::StaticFunctionTag*, RE::TESObjectREFR* a_ref, bool a_female, const char* a_node, int a_key, int a_index, int a_value, bool);
		typedef void              (*_AddNodeOverrideString_t)(RE::StaticFunctionTag*, RE::TESObjectREFR* a_ref, bool a_female, const char* a_node, int a_key, int a_index, const char* a_value, bool);
		typedef void              (*_AddOverlays_t)(RE::StaticFunctionTag*, RE::TESObjectREFR* a_ref);
		typedef void              (*_ApplyNodeOverrides_t)(RE::StaticFunctionTag*, RE::Actor* a_actor);
		typedef float             (*_GetNodeOverrideFloat_t)(RE::StaticFunctionTag*, RE::TESObjectREFR* a_ref, bool a_female, const char* a_node, int a_key, int a_index);
		typedef int               (*_GetNodeOverrideInt_t)(RE::StaticFunctionTag*, RE::TESObjectREFR* a_ref, bool a_female, const char* a_node, int a_key, int a_index);
		typedef RE::BSFixedString (*_GetNodeOverrideString_t)(RE::StaticFunctionTag*, RE::TESObjectREFR* a_ref, bool a_female, const char* a_node, int a_key, int a_index);
		typedef bool              (*_HasNodeOverride_t)(RE::StaticFunctionTag*, RE::TESObjectREFR* a_ref, bool a_female, const char* a_node, int a_key, int a_index);
		typedef bool              (*_HasOverlays_t)(RE::StaticFunctionTag*, RE::TESObjectREFR* a_ref);
		typedef void              (*_RemoveNodeOverride_t)(RE::StaticFunctionTag*, RE::TESObjectREFR* a_ref, bool a_female, const char* a_node, int a_key, int a_index);
		typedef void              (*_RemoveOverlays_t)(RE::StaticFunctionTag*, RE::TESObjectREFR* a_ref);

		_AddNodeOverrideFloat_t  _AddNodeOverrideFloat = nullptr;
		_AddNodeOverrideInt_t    _AddNodeOverrideInt = nullptr;
		_AddNodeOverrideString_t _AddNodeOverrideString = nullptr;
		_AddOverlays_t           _AddOverlays = nullptr;
		_ApplyNodeOverrides_t    _ApplyNodeOverrides = nullptr;
		_GetNodeOverrideFloat_t  _GetNodeOverrideFloat = nullptr;
		_GetNodeOverrideInt_t    _GetNodeOverrideInt = nullptr;
		_GetNodeOverrideString_t _GetNodeOverrideString = nullptr;
		_HasNodeOverride_t       _HasNodeOverride = nullptr;
		_HasOverlays_t           _HasOverlays = nullptr;
		_RemoveNodeOverride_t    _RemoveNodeOverride = nullptr;
		_RemoveOverlays_t        _RemoveOverlays = nullptr;

		RE::StaticFunctionTag _staticfunctag;
	};
}
*/

