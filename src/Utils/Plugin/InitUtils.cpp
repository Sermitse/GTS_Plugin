#include "Utils/Plugin/InitUtils.hpp"
#include "Version.hpp"
#include "git.h"

namespace GTS {

	void VersionCheck(const LoadInterface* a_skse) {
		if (a_skse->RuntimeVersion() < SKSE::RUNTIME_SSE_1_5_97 || REL::Module::IsVR()) {
			ReportAndExit("This mod does not support Skyrim VR or versions of Skyrim older than 1.5.97.");
		}
	}

	void CPrintPluginInfo() {
		Cprint("[GTSPlugin.dll]: [ Giantess Mod {} was succesfully initialized. Waiting for New Game/Save Load. ]", GTSPlugin::ModVersion.string());
		Cprint("[GTSPlugin.dll]: Dll Build Date: {} {}", __DATE__, __TIME__);
		Cprint("[GTSPlugin.dll]: Git Info:");
		Cprint("\t -- Commit: {}", git_CommitSubject());
		Cprint("\t -- SHA1: {}", git_CommitSHA1());
		Cprint("\t -- Date: {}", git_CommitDate());
		Cprint("\t -- Uncommited Changes: {}", git_AnyUncommittedChanges() ? "Yes" : "No");
		Cprint("!---README BELOW---!");
		Cprint("!---Versions after 3.0 no longer have a MCM menu!");
		Cprint("!---To open the mod's settings, either type \"gts menu\" (case insensitive) into the console when in-game or press F1 (by default)");

	}

	void LogPrintPluginInfo() {

		logger::info("GTSPlugin {}", GTSPlugin::ModVersion.string());
		logger::info("Dll Build Date: {} {}", __DATE__, __TIME__);

		const std::string_view git_commit = fmt::format("\t -- Commit: {}", git_CommitSubject());
		const std::string_view git_sha1 = fmt::format("\t -- SHA1: {}", git_CommitSHA1());
		const std::string_view git_date = fmt::format("\t -- Date: {}", git_CommitDate());
		const std::string_view git_ditry = fmt::format("\t -- Uncommited Changes: {}", git_AnyUncommittedChanges() ? "Yes" : "No");

		logger::info("Git Info:\n{}\n{}\n{}\n{}", git_commit, git_sha1, git_date, git_ditry);
	}

}