#include "Utils/Plugin/InitUtils.hpp"
#include "Version.hpp"
#include "git.h"

namespace GTS {

	void VersionCheck(const SKSE::LoadInterface* a_skse) {

		auto ver = a_skse->RuntimeVersion();

		logger::critical("Game version: {}", ver.string());

		if (ver < SKSE::RUNTIME_SSE_1_5_97 || REL::Module::IsVR()) {
			logger::critical("Invalid Game Version: {}", ver.string());
			ReportAndExit("This mod does not support Skyrim VR or versions of Skyrim older than 1.5.97.");
		}

		auto isValidGameVer =
			ver == SKSE::RUNTIME_SSE_1_5_97   || //SE
			ver == SKSE::RUNTIME_SSE_1_6_1170 || //Last 1.6 AE
			ver == SKSE::RUNTIME_SSE_1_6_1179 || //Current (17/11/26) GOG
			ver == SKSE::RUNTIME_SSE_1_7_104;    //Current (17/11/26) Steam

		if (!isValidGameVer) {
			logger::critical("Unsupported Game version: {}", ver.string());
			logger::critical("THE PLUGIN MAY CRASH OR BEHAVE UNEXPECTENDLY. YOU HAVE BEEN WARNED.");
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