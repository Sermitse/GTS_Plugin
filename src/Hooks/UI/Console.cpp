#include "Hooks/UI/Console.hpp"
#include "Managers/Console/ConsoleManager.hpp"
#include "Hooks/Util/HookUtil.hpp"

namespace Hooks {

	struct ConsoleScriptCompiler {

		static void thunk(RE::Script* a_script, RE::ScriptCompiler* a_compiler, RE::COMPILER_NAME a_name, RE::TESObjectREFR* a_targetRef) {

			GTS_PROFILE_ENTRYPOINT("UIConsole::ConsoleScriptCompiler");

			//logger::info("Entered Console Text: \"{}\"", a_script->text);
			//If true text was a plugin command 
			if (ConsoleManager::Process(a_script->text)) {
				return;
			}

			func(a_script, a_compiler, a_name, a_targetRef);

		}

		FUNCTYPE_CALL func;
	};

	void Hook_Console::Install() {
		log::info("Installing Console Script Compiler Hook...");
		stl::write_call<ConsoleScriptCompiler>(REL::VariantID(52065, 52952, NULL), REL::VariantOffset(0xE2, 0x52, NULL));
	}

}
