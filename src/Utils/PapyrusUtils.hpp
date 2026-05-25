#pragma once

namespace GTS {

	using VM = RE::BSScript::Internal::VirtualMachine;
	using ObjectPtr = RE::BSTSmartPointer<RE::BSScript::Object>;

	inline RE::VMHandle GetVMHandle(RE::TESForm* a_form) {
		auto vm = VM::GetSingleton();
		auto policy = vm->GetObjectHandlePolicy();
		return policy->GetHandleForObject(a_form->GetFormType(), a_form);
	}

	inline ObjectPtr GetVMObjectPtr(RE::TESForm* a_form, const char* a_class, bool a_create) {
		auto vm = VM::GetSingleton();
		auto handle = GetVMHandle(a_form);

		ObjectPtr object = nullptr;
		bool found = vm->FindBoundObject(handle, a_class, object);
		if (!found && a_create) {
			vm->CreateObject2(a_class, object);
			vm->BindObject(object, handle, false);
		}

		return object;
	}

	template <class ... Args>
	void CallVMFunctionOn(TESForm* a_form, std::string_view formKind, std::string_view function, Args... a_args) {
		const auto skyrimVM = RE::SkyrimVM::GetSingleton();
		auto vm = skyrimVM ? skyrimVM->impl : nullptr;
		if (vm) {
			RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
			auto args = RE::MakeFunctionArguments(std::forward<Args>(a_args)...);
			auto objectPtr = GetVMObjectPtr(a_form, std::string(formKind).c_str(), false);
			if (!objectPtr) {
				logger::error("Could not bind form");
			}
			vm->DispatchMethodCall(objectPtr, std::string(function).c_str(), args, callback);
		}
	}

	template <class ... Args>
	void CallVMFunction(std::string_view functionClass, std::string_view function, Args... a_args) {
		const auto skyrimVM = RE::SkyrimVM::GetSingleton();
		auto vm = skyrimVM ? skyrimVM->impl : nullptr;
		if (vm) {
			RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
			auto args = RE::MakeFunctionArguments(std::forward<Args>(a_args)...);
			vm->DispatchStaticCall(std::string(functionClass).c_str(), std::string(function).c_str(), args, callback);
		}
	}

	// Callback functor that captures the return value
	struct VMCallbackFunctor : RE::BSScript::IStackCallbackFunctor {
		std::function<void(RE::BSScript::Variable)> handler;

		VMCallbackFunctor(std::function<void(RE::BSScript::Variable)> a_handler)
			: handler(std::move(a_handler)) {
		}

		void operator()(RE::BSScript::Variable a_result) override {
			handler(a_result);
		}

		bool CanSave() const override { return false; }
		void SetObject(const RE::BSTSmartPointer<RE::BSScript::Object>&) override {}
	};

	template <class ReturnT, class... Args>
	void CallVMFunctionOnReturn(TESForm* a_form, std::string_view formKind, std::string_view function, std::function<void(ReturnT)> a_callback, Args... a_args) {
		const auto skyrimVM = RE::SkyrimVM::GetSingleton();
		auto vm = skyrimVM ? skyrimVM->impl : nullptr;
		if (vm) {
			auto args = RE::MakeFunctionArguments(std::forward<Args>(a_args)...);
			auto objectPtr = GetVMObjectPtr(a_form, std::string(formKind).c_str(), false);
			if (!objectPtr) {
				logger::error("Could not bind form");
				return;
			}

			auto functor = RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>{
				new VMCallbackFunctor([a_callback](const RE::BSScript::Variable& a_result) {
					// Unpack from the Variable union based on ReturnT
					if constexpr (std::is_same_v<ReturnT, bool>) {
						a_callback(a_result.GetBool());
					}
					else if constexpr (std::is_same_v<ReturnT, int>) {
						a_callback(a_result.GetSInt());
					}
					else if constexpr (std::is_same_v<ReturnT, float>) {
						a_callback(a_result.GetFloat());
					}
					else if constexpr (std::is_same_v<ReturnT, RE::BSFixedString>) {
						a_callback(a_result.GetString());
					}
					else if constexpr (std::is_base_of_v<RE::TESForm, std::remove_pointer_t<ReturnT>>) {
						a_callback(a_result.Unpack<ReturnT>());
					}
				})
			};

			vm->DispatchMethodCall(objectPtr, std::string(function).c_str(), args, functor);
		}
	}
}
