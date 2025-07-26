#pragma once

namespace GTS {

	// This function allocates a new console and redirects the standard streams
	static inline void AllocateConsole() {

		// Allocate a new console for the calling process.
		if (!AllocConsole()) {
			std::cerr << "Failed to allocate console" << '\n';
			ReportInfo("Could not allocate a console window...");
			return;
		}

		// Redirect unbuffered STDOUT to the console.
		FILE* fpOut = nullptr;
		if (freopen_s(&fpOut, "CONOUT$", "w", stdout) != 0) {
			std::cerr << "Failed to redirect stdout" << '\n';
			ReportInfo("stdout could not be redirected.");
		}

		// Redirect unbuffered STDERR to the console.
		FILE* fpErr = nullptr;
		if (freopen_s(&fpErr, "CONOUT$", "w", stderr) != 0) {
			std::cerr << "Failed to redirect stderr" << '\n';
		}

		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		// Enable ANSI escape sequences for colored output (requires Windows 10+).
		DWORD mode = 0;
		if (GetConsoleMode(hConsole, &mode)) {
			mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			SetConsoleMode(hConsole, mode);
		}

		// Synchronize the C++ standard streams with the C standard streams.
		std::ios::sync_with_stdio();

	}

	static inline void VersionCheck(const LoadInterface* a_skse) {
		if (a_skse->RuntimeVersion() < SKSE::RUNTIME_SSE_1_5_80 || REL::Module::IsVR()) {
			ReportAndExit("This mod does not support Skyrim VR or versions of Skyrim older than 1.5.80.");
		}
	}

	static inline void CPrintPluginInfo() {
		Cprint("[GTSPlugin.dll]: [ Giantess Mod {} was succesfully initialized. Waiting for New Game/Save Load. ]", GTSPlugin::ModVersion.string());
		Cprint("[GTSPlugin.dll]: Dll Build Date: {} {}", __DATE__, __TIME__);
		Cprint("[GTSPlugin.dll]: Git Info:");
		Cprint("\t -- Commit: {}", git_CommitSubject());
		Cprint("\t -- SHA1: {}", git_CommitSHA1());
		Cprint("\t -- Date: {}", git_CommitDate());
		Cprint("\t -- Uncommited Changes: {}", git_AnyUncommittedChanges() ? "Yes" : "No");
		Cprint("!---READ INFO BELOW---!");
		Cprint("!---Giantess mod got rid of MCM menu after version 3.0.0.0!");
		Cprint("!---To open mod settings, type GTS MENU into the console when in game or press F1 (keybind can be changed)");
		Cprint("!---Command can be entered like gts menu or GTS MENU");
	}

	static void LogPrintPluginInfo() {

		logger::info("GTSPlugin {}", GTSPlugin::ModVersion.string());
		logger::info("Dll Build Date: {} {}", __DATE__, __TIME__);

		const std::string_view git_commit = fmt::format("\t -- Commit: {}", git_CommitSubject());
		const std::string_view git_sha1 = fmt::format("\t -- SHA1: {}", git_CommitSHA1());
		const std::string_view git_date = fmt::format("\t -- Date: {}", git_CommitDate());
		const std::string_view git_ditry = fmt::format("\t -- Uncommited Changes: {}", git_AnyUncommittedChanges() ? "Yes" : "No");

		logger::info("Git Info:\n{}\n{}\n{}\n{}", git_commit, git_sha1, git_date, git_ditry);
	}
}