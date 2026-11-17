#pragma once

#include "Hooks/Util/Asm.hpp"
#include "Hooks/Util/CodeCave.hpp"
#include "Hooks/Util/Patch.hpp"
#include "Hooks/Util/Target.hpp"

//Boilerplate
#define FUNCTYPE_DETOUR static inline constinit decltype(thunk)*
#define FUNCTYPE_CALL static inline constinit REL::Relocation<decltype(thunk)>
#define FUNCTYPE_VFUNC static inline constinit REL::Relocation<decltype(&thunk)>

//Every hook takes its target in one of these forms:
//	(std::uintptr_t)                               absolute address
//	(Id, Offset = {}, When = {})                   Id is any AddressSource (RelocationIDEx, VariantIDEx, REL::ID,
//	                                               RelocationIDEx, REL::VariantID, ...), Offset any OffsetSource
//	                                               (OffsetEx, REL::Offset, OffsetEx, an integer)
//	(Id, When)                                     same, no offset
//When converts from an Epoch or a REL::Version:
//	stl::write_call<Hook>(RelocationIDEx(1, 2, 3, 4), OffsetEx(1, 2, 3, 4));
//	stl::write_call<Hook>(RelocationIDEx(3), OffsetEx(3), Epoch::k1_7);
//	stl::write_call<Hook>(RelocationIDEx(3), OffsetEx(3), SKSE::RUNTIME_SSE_1_7_104);
//	stl::write_call<Hook>(RelocationIDEx(35565, 36564), OffsetEx(0x748, 0xC26, 0));
//A hook that does not apply to the running game is skipped and logged.
//
//Unique hooks: template the hook struct on an int so each instantiation owns its thunk and func.
//	template <int ID> struct Hook { static void thunk(...) { func(...); } FUNCTYPE_CALL func; };
//	stl::write_call_unique<Hook, 0>(...);   //same as stl::write_call<Hook<0>>(...)

namespace Hooks::detail {

	template <typename T>
	constexpr std::string_view TypeName() {
		std::string_view p = __FUNCSIG__;
		const auto start = p.find("TypeName<") + 9;
		const auto end = p.find(">(void)");
		return p.substr(start, end - start);
	}

	template <class T>
	bool Begin(std::string_view a_kind, const Site& a_site) {
		if (!a_site) {
			logger::info("{}<{}> skipped: {}", a_kind, TypeName<T>(), a_site.where);
			return false;
		}
		logger::debug("Installing {}<{}> at {}", a_kind, TypeName<T>(), a_site.where);
		return true;
	}

	template <class T>
	bool ExpectBytes(std::string_view a_kind, const Site& a_site, std::span<const std::uint8_t> a_bytes) {
		if (Asm::MatchBytes(a_site.address, a_bytes)) {
			return true;
		}
		logger::error("{}<{}> skipped: {} holds [{}], expected [{}]. Wrong offset for this runtime?",
			a_kind, TypeName<T>(), a_site.where, Asm::HexBytes(a_site.address, a_bytes.size()), Asm::HexBytes(reinterpret_cast<std::uintptr_t>(a_bytes.data()), a_bytes.size())
		);
		return false;
	}

	template <class T, std::size_t Size>
	void WriteCall(const Site& a_site) {
		static_assert(Size == 5 || Size == 6, "call hooks are 5 (E8 rel32) or 6 (FF 15 [rip+disp32]) bytes");
		static constexpr std::array<std::uint8_t, 1> call5{ 0xE8 };
		static constexpr std::array<std::uint8_t, 2> call6{ 0xFF, 0x15 };

		if (!Begin<T>("write_call", a_site)) return;
		if (!ExpectBytes<T>("write_call", a_site, Size == 5 ? std::span<const std::uint8_t>(call5) : std::span<const std::uint8_t>(call6))) return;

		RequireTrampoline(Size == 5 ? 14 : 8, TypeName<T>());
		auto& trampoline = SKSE::GetTrampoline();
		if constexpr (Size == 6) {
			T::func = *reinterpret_cast<std::uintptr_t*>(trampoline.write_call<6>(a_site.address, T::thunk));
		}
		else {
			T::func = trampoline.write_call<5>(a_site.address, T::thunk);
		}
		logger::debug("write_call<{}> installed, original function at 0x{:X}", TypeName<T>(), static_cast<std::uintptr_t>(T::func.address()));
	}

	template <class T, std::size_t Size>
	void WriteJmp(const Site& a_site) {
		static_assert(Size == 5 || Size == 6, "jmp hooks are 5 (E9 rel32) or 6 (FF 25 [rip+disp32]) bytes");
		static constexpr std::array<std::uint8_t, 1> jmp5{ 0xE9 };
		static constexpr std::array<std::uint8_t, 2> jmp6{ 0xFF, 0x25 };

		if (!Begin<T>("write_jmp", a_site)) return;
		if (!ExpectBytes<T>("write_jmp", a_site, Size == 5 ? std::span<const std::uint8_t>(jmp5) : std::span<const std::uint8_t>(jmp6))) return;

		RequireTrampoline(Size == 5 ? 14 : 8, TypeName<T>());
		auto& trampoline = SKSE::GetTrampoline();
		if constexpr (Size == 6) {
			T::func = *reinterpret_cast<std::uintptr_t*>(trampoline.write_branch<6>(a_site.address, T::thunk));
		}
		else {
			T::func = trampoline.write_branch<5>(a_site.address, T::thunk);
		}
		logger::debug("write_jmp<{}> installed, original target at 0x{:X}", TypeName<T>(), static_cast<std::uintptr_t>(T::func.address()));
	}

	//a_site is the vtable itself, T::funcIndex selects the slot.
	template <class T>
	void WriteVfunc(const Site& a_site) {
		if (!Begin<T>("write_vfunc", a_site)) return;

		const auto slot = a_site.address + T::funcIndex * sizeof(void*);
		if (!InImage(a_site.address) || !*reinterpret_cast<const std::uintptr_t*>(slot)) {
			logger::error("write_vfunc<{}> skipped: {} is not a vtable with a slot {}", TypeName<T>(), a_site.where, T::funcIndex);
			return;
		}

		REL::Relocation<std::uintptr_t> vtbl{ a_site.address };
		T::func = vtbl.write_vfunc(T::funcIndex, T::thunk);
		logger::debug("write_vfunc<{}> installed at index {}, original function at 0x{:X}", TypeName<T>(), T::funcIndex, static_cast<std::uintptr_t>(T::func.address()));
	}

	template <class T>
	void WriteDetour(const Site& a_site) {
		static_assert(std::is_pointer_v<decltype(T::func)>, "detour hooks declare func with FUNCTYPE_DETOUR");

		if (!Begin<T>("write_detour", a_site)) return;

		if (!InText(a_site.address)) {
			logger::error("write_detour<{}> skipped: {} is outside the game's code", TypeName<T>(), a_site.where);
			return;
		}

		if (const auto target = ForeignHook(a_site.address)) {
			logger::info("write_detour<{}>: {} is already hooked by another module (branches to 0x{:X}), chaining onto it",
				TypeName<T>(), a_site.where, *target
			);
		}

		auto original = reinterpret_cast<decltype(T::func)>(a_site.address);
		if (const auto error = AttachDetour(reinterpret_cast<void**>(&original), reinterpret_cast<void*>(T::thunk)); error != NO_ERROR) {
			SKSE::stl::report_and_fail(std::format("Detour of {} at {} failed with error {}.", TypeName<T>(), a_site.where, error));
		}

		T::func = original;
		logger::debug("write_detour<{}> installed, original function via trampoline 0x{:X}", TypeName<T>(), reinterpret_cast<std::uintptr_t>(T::func));
	}

	template <class T>
	void WriteCave(const Site& a_site) {
		static_assert(std::derived_from<T, CodeCave>, "xbyak thunks derive from Hooks::CodeCave");
		static_assert(T::bytesToPatch >= 5, "the cave is entered through a 5 byte jmp");

		if (!Begin<T>("write_xbyak_thunk", a_site)) return;

		const auto decoded = Asm::Decode(a_site.address, T::bytesToPatch);
		if (!decoded || decoded->length != T::bytesToPatch) {
			logger::error("write_xbyak_thunk<{}> skipped: bytesToPatch {} at {} does not end on an instruction boundary ({}). Bytes: [{}]",
				TypeName<T>(), T::bytesToPatch, a_site.where, decoded ? std::format("whole instructions cover {}", decoded->length) : "undecodable", Asm::HexBytes(a_site.address, T::bytesToPatch + 8)
			);
			return;
		}

		if constexpr (requires { T::expectedBytes; }) {
			static_assert(T::expectedBytes.size() == T::bytesToPatch, "expectedBytes must cover exactly bytesToPatch");
			if (!ExpectBytes<T>("write_xbyak_thunk", a_site, T::expectedBytes)) return;
		}

		const CaveContext context{
			.site = a_site.address,
			.returnAddress = a_site.address + T::bytesToPatch,
			.stolen = { reinterpret_cast<const std::uint8_t*>(a_site.address), T::bytesToPatch },
			.stolenRelocatable = !decoded->positionDependent,
		};

		try {
			T cave(context);
			cave.ready();

			RequireTrampoline(cave.getSize() + 14, TypeName<T>());
			auto& trampoline = SKSE::GetTrampoline();
			const auto code = reinterpret_cast<std::uintptr_t>(trampoline.allocate(cave));

			trampoline.write_branch<5>(a_site.address, code);

			//int3 rather than nop
			REL::safe_fill(a_site.address + 5, 0xCC, T::bytesToPatch - 5);

			logger::debug("write_xbyak_thunk<{}> installed, cave at 0x{:X} ({} bytes), resumes at 0x{:X}", TypeName<T>(), code, cave.getSize(), context.returnAddress);
		}
		catch (const std::exception& e) {
			logger::error("write_xbyak_thunk<{}> skipped: generating the cave for {} failed: {}", TypeName<T>(), a_site.where, e.what());
		}
	}
}

namespace Hooks::stl {

	using detail::Resolve;

	//----- write_call: redirect an existing call (E8 rel32, or FF 15 [rip+disp32] with Size 6). T::func is the old callee.

	template <class T, std::size_t Size = 5>
	void write_call(std::uintptr_t a_address) {
		detail::WriteCall<T, Size>(Resolve(a_address));
	}

	template <class T, std::size_t Size = 5, AddressSource Id, OffsetSource Off = OffsetEx>
	void write_call(const Id& a_id, const Off& a_offset = {}, const When& a_when = {}) {
		detail::WriteCall<T, Size>(Resolve(a_id, a_offset, a_when));
	}

	template <class T, std::size_t Size = 5, AddressSource Id>
	void write_call(const Id& a_id, const When& a_when) {
		detail::WriteCall<T, Size>(Resolve(a_id, 0, a_when));
	}

	template <template <int> class T, int ID, std::size_t Size = 5, class... Args>
	void write_call_unique(Args&&... a_args) {
		write_call<T<ID>, Size>(std::forward<Args>(a_args)...);
	}

	//----- write_jmp: redirect an existing jmp (E9 rel32, or FF 25 [rip+disp32] with Size 6). T::func is the old target.
	//To take over a function from its first byte use write_detour.

	template <class T, std::size_t Size = 5>
	void write_jmp(std::uintptr_t a_address) {
		detail::WriteJmp<T, Size>(Resolve(a_address));
	}

	template <class T, std::size_t Size = 5, AddressSource Id, OffsetSource Off = OffsetEx>
	void write_jmp(const Id& a_id, const Off& a_offset = {}, const When& a_when = {}) {
		detail::WriteJmp<T, Size>(Resolve(a_id, a_offset, a_when));
	}

	template <class T, std::size_t Size = 5, AddressSource Id>
	void write_jmp(const Id& a_id, const When& a_when) {
		detail::WriteJmp<T, Size>(Resolve(a_id, 0, a_when));
	}

	template <template <int> class T, int ID, std::size_t Size = 5, class... Args>
	void write_jmp_unique(Args&&... a_args) {
		write_jmp<T<ID>, Size>(std::forward<Args>(a_args)...);
	}

	//----- write_vfunc: replace slot T::funcIndex. The target is the vtable, e.g. RE::VTABLE_Actor[0].

	template <class F, std::size_t VtblIndex, class T>
	void write_vfunc() {
		detail::WriteVfunc<T>(Resolve(F::VTABLE[VtblIndex].address()));
	}

	template <class F, class T>
	void write_vfunc() {
		write_vfunc<F, 0, T>();
	}

	template <class T>
	void write_vfunc(std::uintptr_t a_vtable) {
		detail::WriteVfunc<T>(Resolve(a_vtable));
	}

	template <class T, AddressSource Id>
	void write_vfunc(const Id& a_vtableId, const When& a_when = {}) {
		detail::WriteVfunc<T>(Resolve(a_vtableId, 0, a_when));
	}

	template <template <int> class T, int ID, class... Args>
	void write_vfunc_unique(Args&&... a_args) {
		write_vfunc<T<ID>>(std::forward<Args>(a_args)...);
	}

	template <class F, std::size_t VtblIndex, template <int> class T, int ID>
	void write_vfunc_unique() {
		write_vfunc<F, VtblIndex, T<ID>>();
	}

	template <class F, template <int> class T, int ID>
	void write_vfunc_unique() {
		write_vfunc<F, 0, T<ID>>();
	}

	//----- write_detour: take over a function from its first instruction. T::func calls the original.

	template <class T>
	void write_detour(std::uintptr_t a_address) {
		detail::WriteDetour<T>(Resolve(a_address));
	}

	template <class T, AddressSource Id, OffsetSource Off = OffsetEx>
	void write_detour(const Id& a_id, const Off& a_offset = {}, const When& a_when = {}) {
		detail::WriteDetour<T>(Resolve(a_id, a_offset, a_when));
	}

	template <class T, AddressSource Id>
	void write_detour(const Id& a_id, const When& a_when) {
		detail::WriteDetour<T>(Resolve(a_id, 0, a_when));
	}

	template <template <int> class T, int ID, class... Args>
	void write_detour_unique(Args&&... a_args) {
		write_detour<T<ID>>(std::forward<Args>(a_args)...);
	}

	//----- write_xbyak_thunk: divert T::bytesToPatch bytes into a Hooks::CodeCave. Caves are generated per site,
	//so one cave type can be installed at several sites without a unique variant.

	template <class T>
	void write_xbyak_thunk(std::uintptr_t a_address) {
		detail::WriteCave<T>(Resolve(a_address));
	}

	template <class T, AddressSource Id, OffsetSource Off = OffsetEx>
	void write_xbyak_thunk(const Id& a_id, const Off& a_offset = {}, const When& a_when = {}) {
		detail::WriteCave<T>(Resolve(a_id, a_offset, a_when));
	}

	template <class T, AddressSource Id>
	void write_xbyak_thunk(const Id& a_id, const When& a_when) {
		detail::WriteCave<T>(Resolve(a_id, 0, a_when));
	}

	//----- safe_write: write a trivially copyable value (a std::array of bytes, a float, ...). With a_expected the
	//site must hold those bytes first or nothing is written. Returns whether the write happened.

	template <class T>
	concept Payload = std::is_trivially_copyable_v<T> && !std::is_convertible_v<T, When>;

	template <Payload T>
	bool safe_write(std::uintptr_t a_address, const T& a_data, std::span<const std::uint8_t> a_expected = {}) {
		return detail::SafeWrite(Resolve(a_address), std::addressof(a_data), sizeof(T), a_expected);
	}

	template <AddressSource Id, OffsetSource Off, Payload T>
	bool safe_write(const Id& a_id, const Off& a_offset, const T& a_data, std::span<const std::uint8_t> a_expected = {}) {
		return detail::SafeWrite(Resolve(a_id, a_offset, {}), std::addressof(a_data), sizeof(T), a_expected);
	}

	template <AddressSource Id, OffsetSource Off, Payload T>
	bool safe_write(const Id& a_id, const Off& a_offset, const When& a_when, const T& a_data, std::span<const std::uint8_t> a_expected = {}) {
		return detail::SafeWrite(Resolve(a_id, a_offset, a_when), std::addressof(a_data), sizeof(T), a_expected);
	}
}
