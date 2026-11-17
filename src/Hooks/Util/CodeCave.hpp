#pragma once


namespace Hooks {

	struct CaveContext {
		std::uintptr_t site = 0;                    //first patched byte
		std::uintptr_t returnAddress = 0;           //first byte after the patched range
		std::span<const std::uint8_t> stolen;       //original bytes the branch overwrites
		bool stolenRelocatable = false;             //false if a stolen instruction is RIP relative or a relative branch
	};

	//Base for stl::write_xbyak_thunk hooks. Derive, declare
	//	static constexpr std::size_t bytesToPatch = N;                        //>= 5, whole instructions only
	//	static constexpr std::array<std::uint8_t, N> expectedBytes{ ... };    //optional, verified before patching
	//and emit the cave in a constructor taking const CaveContext&.
	//
	//The code is generated in a scratch buffer and then copied into the trampoline, so a plain jmp(addr) or
	//call(addr) encodes a rel32 against the scratch buffer and lands somewhere random. Reach absolute
	//addresses through JmpAbs/CallAbs/JumpBack. Jumps to labels inside the cave are fine.
	class CodeCave : public Xbyak::CodeGenerator {
		protected:
		explicit CodeCave(const CaveContext& a_ctx) : m_ctx(a_ctx) {}

		//Re-emits the overwritten instructions verbatim. Throws (and the hook is skipped) when they are
		//position dependent; re-emit those by hand with their operands rewritten.
		void EmitStolenBytes() {
			if (!m_ctx.stolenRelocatable) {
				throw std::runtime_error("stolen bytes are position dependent, re-emit them by hand");
			}
			for (const auto byte : m_ctx.stolen) {
				db(byte);
			}
		}

		//Resumes the original code after the patched range. Clobbers no registers.
		void JumpBack() {
			JmpAbs(m_ctx.returnAddress);
		}

		void JmpAbs(std::uintptr_t a_target) {
			Xbyak::Label target;
			jmp(ptr[rip + target]);
			L(target);
			dq(a_target);
		}

		void CallAbs(std::uintptr_t a_target) {
			Xbyak::Label target;
			Xbyak::Label done;
			call(ptr[rip + target]);
			jmp(done);
			L(target);
			dq(a_target);
			L(done);
		}

		[[nodiscard]] const CaveContext& Context() const noexcept {
			return m_ctx;
		}

		private:
		CaveContext m_ctx;
	};
}
