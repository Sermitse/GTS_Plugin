#include "Utils/Plugin/Logger.hpp"
#include "Utils/Text/Text.hpp"

#include "Config/Config.hpp"

namespace {

	//ANSI Escape Codes: Colors
	#define BLK "\033[30m"  //Black
	#define RED "\033[31m"  //Red
	#define GRN "\033[32m"  //Green
	#define YEL "\033[33m"  //Yellow
	#define BLU "\033[34m"  //Blue
	#define MAG "\033[35m"  //Magenta
	#define CYA "\033[36m"  //Cyan
	#define WHT "\033[37m"  //White

	//ANSI Escape Codes: Formating
	#define RST "\033[0m"   //Reset
	#define BLD "\033[1m"   //Bold
	#define UDL "\033[4m"   //Underline

	constexpr const char* PatternDefault = "[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v";
	constexpr const char* PatternConsole = WHT "[" MAG "GTS" WHT "][%H:%M:%S.%e][%^%l%$][" YEL "%s:%#" WHT "]: %v" RST;

}

namespace SKSE::log {

	void Initialize() {

		auto path = log_directory();

		if (!path) {
			GTS::ReportAndExit("Could not find a valid log directory.");
		}

		*path /= PluginDeclaration::GetSingleton()->GetName();
		*path += L".log";

		std::shared_ptr <spdlog::logger> logger;

		if (HasConsole()) {
			logger = std::make_shared <spdlog::logger>("Global", std::make_shared <spdlog::sinks::stdout_color_sink_mt>());
			logger->set_pattern(PatternConsole);
		}
		/*else if (IsDebuggerPresent()) {
			logger = std::make_shared <spdlog::logger>("Global", std::make_shared <spdlog::sinks::msvc_sink_mt>());
			logger->set_pattern(PatternDefault);
		}*/
		else {
			logger = std::make_shared <spdlog::logger>("Global", std::make_shared <spdlog::sinks::basic_file_sink_mt>(path->string(), true));
			logger->set_pattern(PatternDefault);
		}

		spdlog::set_default_logger(std::move(logger));

		//If console force to trace for init.
		//Else set to info.
		//Userconfig gets parsed during game load where this setting will be overriden by the value stored in the save.
		SetLevel(HasConsole() ? "Trace" : "Info");
	}

	void SetLevel(spdlog::level::level_enum a_level) {
		spdlog::set_level(a_level);
		spdlog::flush_on(a_level);
	}

	void SetLevel(const char* a_level) {

		const auto to_level_enum = [](const char* levelStr) -> std::optional<spdlog::level::level_enum> {
			using enum spdlog::level::level_enum;

			std::string lower = GTS::str_tolower(levelStr);
			if (lower == "off")                         return off;
			if (lower == "trace")                       return trace;
			if (lower == "debug")                       return debug;
			if (lower == "info")                        return info;
			if (lower == "warning" || lower == "warn")  return warn;
			if (lower == "error" || lower == "err")     return err;
			if (lower == "critical")                    return critical;

			return std::nullopt;
		};

		if (const auto level = to_level_enum(a_level)) {
			SetLevel(*level);
		}
	}

	void LoadConfig() {
		try {
			const auto& debugConfig = GTS::Config::Advanced;
			log::info("Config Loaded from settings: Print: {} Flush: {}", debugConfig.sLogLevel, debugConfig.sLogLevel);
			SetLevel(debugConfig.sLogLevel.c_str());
		}
		catch (std::exception& e) {
			logger::critical("Could not load spdlog settings from config", e.what());
			GTS::ReportAndExit("Logger: Could not load logger settings.");
		}
	}

	bool HasConsole() {
		return GetConsoleWindow() != nullptr;
	}
}
