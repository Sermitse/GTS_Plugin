#include "Hooks/Util/Patch.hpp"
#include "Hooks/Util/Asm.hpp"



namespace {

	std::uintptr_t ImageEnd() {
		const auto base = REL::Module::get().base();
		const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
		const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
		return base + nt->OptionalHeader.SizeOfImage;
	}
}

namespace Hooks::detail {

	Site Resolve(std::uintptr_t a_address) {
		return { a_address, a_address ? std::format("0x{:X}", a_address) : "a null address" };
	}

	bool InText(std::uintptr_t a_address) {
		const auto text = REL::Module::get().segment(REL::Segment::textx);
		return a_address >= text.address() && a_address < text.address() + text.size();
	}

	bool InImage(std::uintptr_t a_address) {
		return a_address >= REL::Module::get().base() && a_address < ImageEnd();
	}

	std::optional<std::uintptr_t> ForeignHook(std::uintptr_t a_address) {
		return Asm::ForeignBranch(a_address, REL::Module::get().base(), ImageEnd());
	}

	void RequireTrampoline(std::size_t a_bytes, std::string_view a_hook) {
		const auto free = SKSE::GetTrampoline().free_size();
		if (free < a_bytes) {
			SKSE::stl::report_and_fail(std::format("{} needs {} trampoline bytes but only {} are left. Raise Const::Plugin::TRAMPOLINE_ALLOC_BYTES.", a_hook, a_bytes, free));
		}
	}

	long AttachDetour(void** a_original, void* a_thunk) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		if (const auto result = DetourAttach(a_original, a_thunk); result != NO_ERROR) {
			DetourTransactionAbort();
			return result;
		}
		return DetourTransactionCommit();
	}

	bool SafeWrite(const Site& a_site, const void* a_data, std::size_t a_size, std::span<const std::uint8_t> a_expected) {
		if (!a_site) {
			logger::info("safe_write skipped: {}", a_site.where);
			return false;
		}

		if (!a_expected.empty() && !Asm::MatchBytes(a_site.address, a_expected)) {
			logger::error("safe_write at {} skipped: expected [{}], found [{}]",
				a_site.where, Asm::HexBytes(reinterpret_cast<std::uintptr_t>(a_expected.data()), a_expected.size()), Asm::HexBytes(a_site.address, a_expected.size())
			);
			return false;
		}

		REL::safe_write(a_site.address, a_data, a_size);

		logger::debug("safe_write of {} bytes at {}", a_size, a_site.where);
		return true;
	}
}
