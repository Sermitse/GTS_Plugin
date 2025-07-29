#pragma once

// https://gitlab.com/colorglass/fully-dynamic-game-engine/-/blob/main/Trueflame/include/FDGE/Hook/BranchHook.h
// Deprecated

namespace Hooks {

	template <class Signature>
	class BranchHook {};

	template <class Return, class ... Args>
	
	class BranchHook<Return(Args...)> {
		public:
			inline BranchHook(uint64_t functionID, uint32_t callOffset, Return(*hook)(Args ...)) {
				Initialize(REL::ID(functionID).address() + callOffset, hook);
			}

			inline BranchHook(REL::ID functionID, uint32_t callOffset, Return(*hook)(Args ...)) {
				Initialize(functionID.address() + callOffset, hook);
			}

			inline BranchHook(REL::RelocationID functionIDs, uint32_t callOffset, Return(*hook)(Args ...)) {
				Initialize(functionIDs.address() + callOffset, hook);
			}

			inline BranchHook(REL::VariantID functionIDs, uint32_t callOffset, Return(*hook)(Args ...)) {
				Initialize(functionIDs.address() + callOffset, hook);
			}

			inline BranchHook(REL::VariantOffset offsets, uint32_t callOffset, Return(*hook)(Args ...)) {
				Initialize(offsets.address() + callOffset, hook);
			}

			inline ~BranchHook() {
				uintptr_t base = REL::Module::get().base();
			}

			inline Return operator()(Args... args) const noexcept {
				if constexpr (std::is_void_v<Return>) {
					_original(args ...);
				} else {
					return _original(args ...);
				}
			}

		private:
			void Initialize(uintptr_t address, Return (*hook)(Args...)) {
				_address = address;
				uintptr_t base = REL::Module::get().base();
				_trampoline.create(32);
				_original = _trampoline.write_branch<5>(address, hook);
			}

			SKSE::Trampoline _trampoline{"FuDGE Branch Hook"};
			uintptr_t _address;
			REL::Relocation<Return(Args...)> _original;
	};
}
