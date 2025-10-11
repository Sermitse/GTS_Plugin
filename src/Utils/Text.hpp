#pragma once

//Helper
#define PSString static const char* const

namespace GTS {

	template<typename ... Args>
	void Notify(std::string_view rt_fmt_str, Args&&... args) {
		try {
			DebugNotification(std::vformat(rt_fmt_str, std::make_format_args(args ...)).c_str());
		} catch (const std::format_error &e) {
			log::info("Could not format notification, check valid format string: {}", e.what());
		}
	}

	template<typename ... Args>
	void PrintMessageBox(std::string_view rt_fmt_str, Args&&... args) {
		try {
			DebugMessageBox(std::vformat(rt_fmt_str, std::make_format_args(args ...)).c_str());
		} catch (const std::format_error &e) {
			log::info("Could not format notification, check valid format string: {}", e.what());
		}
	}

	template<typename ... Args>
	void Cprint(std::string_view rt_fmt_str, Args&&... args) {
		try {
			ConsoleLog::GetSingleton()->Print("%s", std::vformat(rt_fmt_str, std::make_format_args(args ...)).c_str());
		} catch (const std::format_error &e) {
			log::info("Could not format console log, check valid format string: {}", e.what());
		}
	}

	bool starts_with(std::string_view arg, std::string_view prefix);
	bool matches(std::string_view str, std::string_view reg);
	std::string str_tolower(std::string s);
	std::string str_toupper(std::string s);
	std::string trim(const std::string& s);
	void ltrim(std::string& s);
	void rtrim(std::string& s);
	void replace_first(std::string& s, std::string const& toReplace, std::string const& replaceWith);
	std::string remove_whitespace(std::string s);
	std::string HumanizeString(std::string_view name);
	bool ContainsString(const std::string& a1, const std::string& a2);
}
