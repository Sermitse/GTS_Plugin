#pragma once
#include <detours/detours.h>

//Boilerplate
#define FUNCTYPE_DETOUR static inline constinit decltype(thunk)*
#define FUNCTYPE_CALL static inline constinit REL::Relocation<decltype(thunk)>
#define FUNCTYPE_CALL_UNIQUE static inline constinit REL::Relocation<decltype(thunk<ID>)>
#define FUNCTYPE_VFUNC static inline constinit REL::Relocation<decltype(&thunk)>
#define FUNCTYPE_VFUNC_UNIQUE static inline constinit REL::Relocation<decltype(&thunk<ID>)>

namespace Hooks::Internal {

	template <typename T>
	constexpr std::string_view get_type_name() {
		#if defined(__clang__) or defined(__GNUC__)
			std::string_view p = __PRETTY_FUNCTION__;
			return p.substr(p.find('=') + 2, p.rfind(']') - p.find('=') - 2);
		#elif defined(_MSC_VER)
			std::string_view p = __FUNCSIG__;
			const auto start = p.find("get_type_name<") + 14;
			const auto end = p.find(">(void)");
			return p.substr(start, end - start);
		#endif
	}
}

namespace Hooks::stl {

	// ----- Write_Call

	template <class T, std::size_t Size = 5>
	void write_call(std::uintptr_t a_src) {

		logger::debug("Installing write_call<{}> at address: 0x{:X}", 
			Internal::get_type_name<T>(), a_src
		);

		auto& trampoline = SKSE::GetTrampoline();
		if constexpr (Size == 6) {
			T::func = *reinterpret_cast<uintptr_t*>(trampoline.write_call<6>(a_src, T::thunk));
		}
		else {
			T::func = trampoline.write_call<Size>(a_src, T::thunk);
		}

		logger::debug("write_call installed, original function at: 0x{:X}", 
			a_src, static_cast<std::uintptr_t>(T::func.address())
		);
	}

	template <class T, std::size_t Size = 5>
	void write_call(REL::VariantID a_varId, REL::VariantOffset a_Offs = REL::VariantOffset(0x0, 0x0, 0x0)) {
		const uintptr_t address = a_varId.address() + a_Offs.offset();

		logger::debug("Installing write_call<{}> at VariantID [0x{:X} + 0x{:X}] resolved to 0x{:X}", 
			Internal::get_type_name<T>(), a_varId.address(), a_Offs.offset(), address
		);

		auto& trampoline = SKSE::GetTrampoline();
		if constexpr (Size == 6) {
			T::func = *reinterpret_cast<uintptr_t*>(trampoline.write_call<6>(address, T::thunk));
		}
		else {
			T::func = trampoline.write_call<Size>(address, T::thunk);
		}

		logger::debug("write_call installed, original function at: 0x{:X}", 
			address, static_cast<std::uintptr_t>(T::func.address())
		);
	}

	template <class T, int ID, std::size_t Size = 5>
	void write_call_unique(std::uintptr_t a_src) {

		logger::debug("Installing write_call_unique<{}> at address: 0x{:X} ID {}", 
			Internal::get_type_name<T>(), a_src, ID
		);

		auto& trampoline = SKSE::GetTrampoline();
		if constexpr (Size == 6) {
			T::template func<ID> = *reinterpret_cast<uintptr_t*>(trampoline.write_call<6>(a_src, T::template thunk<ID>));
		}
		else {
			T::template func<ID> = trampoline.write_call<Size>(a_src, T::template thunk<ID>);
		}

		logger::debug("write_call_unique installed, original function at: 0x{:X}",
			a_src, static_cast<std::uintptr_t>(T::template func<ID>.address())
		);
	}

	template <class T, int ID, std::size_t Size = 5>
	void write_call_unique(REL::VariantID a_varId, REL::VariantOffset a_Offs = REL::VariantOffset(0x0, 0x0, 0x0)) {
		const uintptr_t address = a_varId.address() + a_Offs.offset();

		logger::debug("Installing write_call_unique<{}> at VariantID [0x{:X} + 0x{:X}] resolved to 0x{:X} ID {}", 
			Internal::get_type_name<T>(), a_varId.address(), a_Offs.offset(), address, ID
		);
		auto& trampoline = SKSE::GetTrampoline();
		if constexpr (Size == 6) {
			T::template func<ID> = *reinterpret_cast<uintptr_t*>(trampoline.write_call<6>(address, T::template thunk<ID>));
		}
		else {
			T::template func<ID> = trampoline.write_call<Size>(address, T::template thunk<ID>);
		}

		logger::debug("write_call_unique installed, original function at: 0x{:X}", 
			address, static_cast<std::uintptr_t>(T::template func<ID>.address())
		);
	}

	template <class T, int ID, std::size_t Size = 5>
	void write_call_unique(REL::RelocationID a_RelId, REL::VariantOffset a_Offs = REL::VariantOffset(0x0, 0x0, 0x0)) {
		const uintptr_t address = a_RelId.address() + a_Offs.offset();

		logger::debug("Installing write_call_unique<{}> at RelocationID({}) [0x{:X} + 0x{:X}] resolved to 0x{:X} ID {}", 
			Internal::get_type_name<T>(), a_RelId.id(), a_RelId.address(), a_Offs.offset(), address, ID
		);

		auto& trampoline = SKSE::GetTrampoline();
		if constexpr (Size == 6) {
			T::template func<ID> = *reinterpret_cast<uintptr_t*>(trampoline.write_call<6>(address, T::template thunk<ID>));
		}
		else {
			T::template func<ID> = trampoline.write_call<Size>(address, T::template thunk<ID>);
		}

		logger::debug("write_call_unique installed, original function at: 0x{:X}", 
			address, static_cast<std::uintptr_t>(T::template func<ID>.address())
		);
	}

	template <class T, std::size_t Size = 5>
	void write_call(REL::RelocationID a_RelId, REL::VariantOffset a_Offs = REL::VariantOffset(0x0, 0x0, 0x0)) {
		const uintptr_t address = a_RelId.address() + a_Offs.offset();

		logger::debug("Installing write_call<{}> at RelocationID({}) [0x{} + 0x{:X}] resolved to 0x{:X}", 
			Internal::get_type_name<T>(), a_RelId.id(), a_RelId.address(), a_Offs.offset(), address
		);

		auto& trampoline = SKSE::GetTrampoline();
		if (Size == 6) {
			T::func = *reinterpret_cast<uintptr_t*>(trampoline.write_call<6>(address, T::thunk));
		}
		else {
			T::func = trampoline.write_call<Size>(address, T::thunk);
		}

		logger::debug("write_call installed, original function at: 0x{:X}", 
			address, static_cast<std::uintptr_t>(T::func.address())
		);
	}


	//----- Write_Jmp (Write_Branch)

	template <class T, std::size_t Size = 5>
	void write_jmp(std::uintptr_t a_src) {

		logger::debug("Installing write_jmp<{}> at address: 0x{:X}", 
			Internal::get_type_name<T>(), a_src
		);

		auto& trampoline = SKSE::GetTrampoline();
		T::func = trampoline.write_branch<Size>(a_src, T::thunk);

		logger::debug("write_jmp installed, original at: 0x{:X}", 
			a_src, static_cast<std::uintptr_t>(T::func.address())
		);
	}

	template <class T, std::size_t Size = 5>
	void write_jmp(REL::RelocationID a_RelId, REL::VariantOffset a_Offs = REL::VariantOffset(0x0, 0x0, 0x0)) {
		const auto address = a_RelId.address() + a_Offs.offset();

		logger::debug("Installing write_jmp<{}> at RelocationID([]) [0x{:X} + 0x{:X}] resolved to 0x{:X}", 
			Internal::get_type_name<T>(), a_RelId.id(), a_RelId.address(), a_RelId.offset(), address
		);

		auto& trampoline = SKSE::GetTrampoline();
		T::func = trampoline.write_branch<Size>(address, T::thunk);

		logger::debug("write_jmp installed, original at: 0x{:X}",
			address, static_cast<std::uintptr_t>(T::func.address())
		);
	}

	//----- Write_vfunc

	template <class F, size_t vtblIndex, class T>
	void write_vfunc() {

		logger::debug("Installing vfunc hook [{}] to {}::[{}]",
			Internal::get_type_name<T>(), Internal::get_type_name<F>(), vtblIndex
		);

		REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[vtblIndex] };
		T::func = vtbl.write_vfunc(T::funcIndex, T::thunk);

		logger::debug("vfunc hook written, original function: 0x{:X}", 
			static_cast<std::uintptr_t>(T::func.address())
		);
	}

	template <class T>
	void write_vfunc(REL::VariantID a_varID) {

		logger::debug("Installing vfunc hook [{}] at VariantID [0x{:X} + 0x{:X}] Index: {}", 
			Internal::get_type_name<T>(), a_varID.address(), T::funcIndex * sizeof(void*), T::funcIndex
		);

		REL::Relocation<std::uintptr_t> vtbl{ a_varID };
		T::func = vtbl.write_vfunc(T::funcIndex, T::thunk);

		logger::debug("vfunc hook written, original function at: 0x{:X}", 
			static_cast<std::uintptr_t>(T::func.address())
		);
	}

	template <class F, class T>
	void write_vfunc() {
		write_vfunc<F, 0, T>();
	}

	//----- write_vfunc_unique

	template <class T, int ID>
	void write_vfunc_unique(REL::VariantID a_varID) {

		logger::debug("Installing vfunc hook [{}] at VariantID [0x{:X} + 0x{:X}] Index {} ID {}", 
			Internal::get_type_name<T>(), a_varID.address(), T::funcIndex * sizeof(void*), T::funcIndex, ID
		);

		REL::Relocation<std::uintptr_t> vtbl{ a_varID };
		T::template func<ID> = vtbl.write_vfunc(T::funcIndex, T::template thunk<ID>);

		logger::debug("vfunc hook written, original function at: 0x{:X}", 
			static_cast<std::uintptr_t>(T::template func<ID>.address())
		);
	}

	template <class F, size_t vtblIndex, class T, int ID>
	void write_vfunc_unique() {

		logger::debug("Installing vfunc hook [{}] to {}::[{}] ID {}",
			Internal::get_type_name<T>(), Internal::get_type_name<F>(), vtblIndex, ID
		);

		REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[vtblIndex] };
		T::template func<ID> = vtbl.write_vfunc(T::funcIndex, T::template thunk<ID>);

		logger::debug("vfunc hook written, original function: 0x{:X}",
			static_cast<std::uintptr_t>(T::template func<ID>.address())
		);
	}

	template <class F, class T, int ID>
	void write_vfunc_unique() {
		write_vfunc_unique<F, 0, T, ID>();
	}

	//----- Write_detour

	template <class T>
	void write_detour(REL::RelocationID a_relId) {
		uintptr_t addr = REL::RelocationID{ a_relId }.address();
		logger::debug("Installing detour hook at RelocationID({}) [0x{:X}]", 
			a_relId.id(), a_relId.address()
		);

		if (!addr) {
			SKSE::stl::report_and_fail(fmt::format("Invalid target address for detour. RelocationID: {} Address: 0x{:X}",
				a_relId.id(), a_relId.address())
			);
		}

		// Check if this function has already been detoured by another DLL
		// Examine the first few bytes for common detour signatures
		const uint8_t* funcBytes = reinterpret_cast<const uint8_t*>(addr);
		bool alreadyDetoured = false;
		std::string detourType;

		// Check for common detour signatures used by other hooking libraries
		if (funcBytes[0] == 0xE9) {
			// Relative JMP (5 bytes: 0xE9 + 4-byte offset) - most common
			alreadyDetoured = true;
			detourType = "Relative JMP (0xE9) - likely Microsoft Detours or similar";
		}
		else if (funcBytes[0] == 0xFF && funcBytes[1] == 0x25) {
			// Indirect JMP (6 bytes: 0xFF 0x25 + 4-byte address)
			alreadyDetoured = true;
			detourType = "Indirect JMP (0xFF 0x25) - common in many hooking libraries";
		}
		else if (funcBytes[0] == 0x48 && funcBytes[1] == 0xB8) {
			// MOV RAX, imm64 followed by JMP RAX (common 64-bit detour pattern)
			if (funcBytes[10] == 0xFF && funcBytes[11] == 0xE0) {
				alreadyDetoured = true;
				detourType = "MOV+JMP RAX pattern - 64-bit detour";
			}
		}
		else if (funcBytes[0] == 0x50 && funcBytes[1] == 0x48 && funcBytes[2] == 0xB8) {
			// PUSH RAX; MOV RAX, imm64; ... (some hooking libraries use this)
			alreadyDetoured = true;
			detourType = "PUSH+MOV pattern - possibly MinHook or custom";
		}
		else if (funcBytes[0] == 0x68) {
			// PUSH imm32 (sometimes used for trampolines)
			alreadyDetoured = true;
			detourType = "PUSH immediate - possible trampoline";
		}

		if (alreadyDetoured) {
			logger::error("Function at RelocationID({}) [0x{:X}] is already detoured by another DLL",
				a_relId.id(), addr
			);
			logger::error("Detected detour type: {}", detourType);
			logger::error("First 16 bytes: {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X}",
				funcBytes[0],  funcBytes[1],  funcBytes[2],  funcBytes[3],
				funcBytes[4],  funcBytes[5],  funcBytes[6],  funcBytes[7],
				funcBytes[8],  funcBytes[9],  funcBytes[10], funcBytes[11],
				funcBytes[12], funcBytes[13], funcBytes[14], funcBytes[15]
			);

			// You can choose to either abort or continue with a warning
			SKSE::stl::report_and_fail(fmt::format("Cannot detour function at RelocationID({}) [0x{:X}] - already detoured by another DLL. This would likely cause crashes or unexpected behavior.",
				a_relId.id(), addr)
			);
		}

		using FnPtr = decltype(T::func);
		FnPtr orig = reinterpret_cast<FnPtr>(addr);

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		auto attachResult = DetourAttach(reinterpret_cast<PVOID*>(&orig), T::thunk);
		if (attachResult != NO_ERROR) {
			DetourTransactionAbort();

			SKSE::stl::report_and_fail(fmt::format("Detour attach failed. RelocationID: {} Address: 0x{:X} - Error: {}",
				a_relId.id(), a_relId.address(), attachResult
			));
		}

		auto commitResult = DetourTransactionCommit();
		if (commitResult != NO_ERROR) {
			SKSE::stl::report_and_fail(fmt::format("Detour commit failed. RelocationID: {} Address: 0x{:X} - Error: {}",
				a_relId.id(), a_relId.address(), commitResult
			));
		}

		// Overwrite real func with the trampoline
		T::func = orig;
		logger::debug("Detour installed successfully. Original function: 0x{:X}, Trampoline: 0x{:X}",
			addr, reinterpret_cast<std::uintptr_t>(T::func)
		);
	}

}