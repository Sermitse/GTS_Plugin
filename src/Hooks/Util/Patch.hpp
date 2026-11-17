#pragma once

#include "Hooks/Util/Target.hpp"

namespace Hooks::detail {

	//A resolved hook target. address == 0 means the hook does not apply here and `where` says why.
	struct Site {
		std::uintptr_t address = 0;
		std::string where;

		explicit operator bool() const noexcept { return address != 0; }
	};

	[[nodiscard]] Site Resolve(std::uintptr_t a_address);

	template <AddressSource Id, OffsetSource Off>
	[[nodiscard]] Site Resolve(const Id& a_id, const Off& a_offset, const When& a_when) {
		if (!a_when.Matches()) {
			return { 0, std::format("limited to {}, running {}", a_when.Describe(), REL::Module::get().version().string(".")) };
		}

		const std::uintptr_t base = a_id.address();
		if (!base) {
			return { 0, std::format("no address on {}", ToString(CurrentEpoch())) };
		}

		std::size_t offset = 0;
		if constexpr (std::integral<Off>) {
			offset = static_cast<std::size_t>(a_offset);
		}
		else {
			offset = a_offset.offset();
		}

		const auto address = base + offset;
		if constexpr (requires { a_id.id(); }) {
			return { address, std::format("ID {} + 0x{:X} ({}) = 0x{:X}", a_id.id(), offset, ToString(CurrentEpoch()), address) };
		}
		else {
			return { address, std::format("0x{:X} + 0x{:X} ({}) = 0x{:X}", base, offset, ToString(CurrentEpoch()), address) };
		}
	}

	[[nodiscard]] bool InText(std::uintptr_t a_address);
	[[nodiscard]] bool InImage(std::uintptr_t a_address);

	//Where the function at a_address branches to if another module already hooked it.
	[[nodiscard]] std::optional<std::uintptr_t> ForeignHook(std::uintptr_t a_address);

	//Fails with a readable message instead of CommonLib's generic one when the trampoline is exhausted.
	void RequireTrampoline(std::size_t a_bytes, std::string_view a_hook);

	//Detours transaction on the current thread. Returns the Detours error code, NO_ERROR on success.
	[[nodiscard]] long AttachDetour(void** a_original, void* a_thunk);

	bool SafeWrite(const Site& a_site, const void* a_data, std::size_t a_size, std::span<const std::uint8_t> a_expected);
}
