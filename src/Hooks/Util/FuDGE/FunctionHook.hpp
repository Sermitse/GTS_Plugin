#pragma once

//#include <gluino/type_traits.h>

namespace Hooks {
	
	template <class Signature>
	class FunctionHook {
		//static_assert(gluino::dependent_false<Signature>, "Signature template must be a function signature.");
	};

	template <>
	class FunctionHook<void> {
		static void Attach(void** target, void* hook);

		static void Detach(void** target, void* hook);

		template <class Signature>
		friend class FunctionHook;
	};

	template <class Return, class ... Args>
	class FunctionHook<Return(Args...)> {
		public:
			inline FunctionHook(uint64_t functionID, Return (*hook)(Args ...))
				: _target(reinterpret_cast<void*>(REL::ID(functionID).address())), _hook(reinterpret_cast<void*>(hook)) {
				FunctionHook<void>::Attach(&_target, _hook);
			}

			inline FunctionHook(REL::ID functionID, Return (*hook)(Args ...))
				: _target(reinterpret_cast<void*>(functionID.address())), _hook(reinterpret_cast<void*>(hook)) {
				FunctionHook<void>::Attach(&_target, _hook);
			}

			inline FunctionHook(REL::Offset offset, Return (*hook)(Args ...))
				: _target(reinterpret_cast<void*>(offset.address())), _hook(reinterpret_cast<void*>(hook)) {
				FunctionHook<void>::Attach(&_target, _hook);
			}

			inline FunctionHook(Return (*function)(Args ...), Return (*hook)(Args ...))
				: _target(reinterpret_cast<void*>(function)), _hook(reinterpret_cast<void*>(hook)) {
				FunctionHook<void>::Attach(&_target, _hook);
			}

			inline FunctionHook(REL::RelocationID functionIDs, Return (*hook)(Args ...))
				: _target(reinterpret_cast<void*>(functionIDs.address())), _hook(reinterpret_cast<void*>(hook)) {
				FunctionHook<void>::Attach(&_target, _hook);
			}

			inline FunctionHook(REL::RelocationID functionIDs, REL::Offset offset, Return (*hook)(Args ...))
				: _target(reinterpret_cast<void*>(functionIDs.address() + offset.offset())), _hook(reinterpret_cast<void*>(hook)) {
				FunctionHook<void>::Attach(&_target, _hook);
			}

			inline FunctionHook(REL::VariantID functionIDs, Return (*hook)(Args ...))
				: _target(reinterpret_cast<void*>(functionIDs.address())), _hook(reinterpret_cast<void*>(hook)) {
				FunctionHook<void>::Attach(&_target, _hook);
			}

			inline FunctionHook(REL::VariantOffset offsets, Return (*hook)(Args ...))
				: _target(reinterpret_cast<void*>(offsets.address())), _hook(reinterpret_cast<void*>(hook)) {
				FunctionHook<void>::Attach(&_target, _hook);
			}

			inline ~FunctionHook() {
				FunctionHook<void>::Detach(&_target, _hook);
			}

			inline Return operator()(Args... args) const noexcept {
				if constexpr (std::is_void_v<Return>) {
					reinterpret_cast<Return (*)(Args...)>(_target)(args ...);
				} else {
					return reinterpret_cast<Return (*)(Args...)>(_target)(args ...);
				}
			}

		private:
			void* _target;
			void* _hook;
	};
}
