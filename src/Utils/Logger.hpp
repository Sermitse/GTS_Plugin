#pragma once

namespace SKSE::log {

	void Initialize();
	void SetLevel(spdlog::level::level_enum a_level);
	void SetLevel(const char* a_level);
	void LoadConfig();

	__forceinline bool HasConsole() {
		return GetConsoleWindow() != nullptr;
	}

}