#pragma once

namespace Hooks::Asm {

	struct Decoded {
		std::size_t length = 0;          //bytes covered by the decoded whole instructions
		bool positionDependent = false;  //a RIP-relative operand or a relative branch, breaks if copied elsewhere
	};

	//Decodes whole instructions from a_address until at least a_minLength bytes are covered.
	//length == a_minLength means the range ends exactly on an instruction boundary.
	[[nodiscard]] inline std::optional<Decoded> Decode(std::uintptr_t a_address, std::size_t a_minLength) {
		Decoded result;
		while (result.length < a_minLength) {
			hde64s hs{};
			const auto length = hde64_disasm(reinterpret_cast<const void*>(a_address + result.length), &hs);
			if (length == 0 || (hs.flags & F_ERROR) != 0) {
				return std::nullopt;
			}
			const bool ripRelative = (hs.flags & F_MODRM) != 0 && hs.modrm_mod == 0 && hs.modrm_rm == 5;
			result.positionDependent = result.positionDependent || ripRelative || (hs.flags & F_RELATIVE) != 0;
			result.length += length;
		}
		return result;
	}

	[[nodiscard]] inline bool MatchBytes(std::uintptr_t a_address, std::span<const std::uint8_t> a_bytes) {
		return std::memcmp(reinterpret_cast<const void*>(a_address), a_bytes.data(), a_bytes.size()) == 0;
	}

	[[nodiscard]] inline std::string HexBytes(std::uintptr_t a_address, std::size_t a_count) {
		std::string out;
		for (std::size_t i = 0; i < a_count; ++i) {
			out += std::format("{}{:02X}", i ? " " : "", reinterpret_cast<const std::uint8_t*>(a_address)[i]);
		}
		return out;
	}

	//Where a function's first instruction branches to when it looks hooked (jmp rel32, jmp [rip+disp32],
	//mov rax, imm64 + jmp rax). A branch staying inside [a_imageBegin, a_imageEnd) is a compiler thunk,
	//not a hook, and yields nullopt.
	[[nodiscard]] inline std::optional<std::uintptr_t> ForeignBranch(std::uintptr_t a_address, std::uintptr_t a_imageBegin, std::uintptr_t a_imageEnd) {
		const auto* code = reinterpret_cast<const std::uint8_t*>(a_address);
		std::uintptr_t target = 0;

		if (code[0] == 0xE9) {
			target = a_address + 5 + *reinterpret_cast<const std::int32_t*>(code + 1);
		}
		else if (code[0] == 0xFF && code[1] == 0x25) {
			const auto slot = a_address + 6 + *reinterpret_cast<const std::int32_t*>(code + 2);
			target = *reinterpret_cast<const std::uintptr_t*>(slot);
		}
		else if (code[0] == 0x48 && code[1] == 0xB8 && code[10] == 0xFF && code[11] == 0xE0) {
			target = *reinterpret_cast<const std::uintptr_t*>(code + 2);
		}
		else {
			return std::nullopt;
		}

		if (target >= a_imageBegin && target < a_imageEnd) {
			return std::nullopt;
		}
		return target;
	}
}
