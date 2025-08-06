#include "Managers/Console/ConsoleManager.hpp"
#include "Version.hpp"
#include "git.h"


namespace GTS {

	void ConsoleManager::RegisterCommand(std::string_view a_cmdName, const std::function<void()>& a_callback, const std::string& a_desc) {

		auto& me = GetSingleton();

		std::string name(a_cmdName);

		me.RegisteredCommands.try_emplace(name, a_callback, a_desc);

		log::info("Registered Console Command \"{} {}\"", me.Default_Preffix, name);
	}


	bool ConsoleManager::Process(const std::string& a_msg) {

		auto& me = GetSingleton();

		[[unlikely]] if (me.RegisteredCommands.empty()) return false;

		std::stringstream Msg(trim(str_tolower(a_msg)));

		std::vector<std::string> Args{};
		std::string TmpArg;

		while (Msg >> TmpArg) {

			//If subcommands are ever needed just increase this value
			if (Args.size() == 2) {
				break;
			}

			Args.emplace_back(TmpArg);

			//no "gts" ? then its not our problem to deal with
			if (Args.at(0) != me.Default_Preffix) {
				return false;
			}
		}

		//if 1 arg show help
		if (Args.size() < 2) {
			CMD_Help();
			return true;
		}

		for (const auto& registered_command : me.RegisteredCommands) {
			if (registered_command.first == Args.at(1)) {
				if (registered_command.second.callback) {
					registered_command.second.callback();
					return true;
				}
				else {
					logger::warn("Command {} has no function assigned to it", registered_command.first);
					return false;
				}
			}
		}

		Cprint("Command not found type {} help for a list of commands.", me.Default_Preffix);
		return true;
	}

	void ConsoleManager::CMD_Help() {
		auto& me = GetSingleton();
		Cprint("--- List of available commands ---");

		for (const auto& key : me.RegisteredCommands) {
			Cprint("* {} {} - {} ", me.Default_Preffix, key.first, key.second.desc);
		}
	}

	void ConsoleManager::CMD_Version() {
		Cprint("--- Giantess Mod: Size Matters ---");
		Cprint("Version: {}", GTSPlugin::ModVersion.string());
		Cprint("Dll Build Date: {} {}", __DATE__, __TIME__);
		Cprint("Git Commit Date: {}", git_CommitDate());
	}

	void ConsoleManager::CMD_Unlimited() {
		auto Player = PlayerCharacter::GetSingleton();
		if (Player) {
			if (Runtime::HasPerk(Player, "GTSPerkColossalGrowth")) {
				Persistent::GetSingleton().UnlockMaxSizeSliders.value = !Persistent::GetSingleton().UnlockMaxSizeSliders.value;
				Cprint("Max Size Sliders unlocked: {}", Persistent::GetSingleton().UnlockMaxSizeSliders.value);
			}
			else {
				Cprint("You need to obtain Colossal Growth perk to use this command");
			}
		}
	}
}


