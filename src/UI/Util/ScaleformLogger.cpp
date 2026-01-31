#include "UI/Util/ScaleformLogger.hpp"

namespace GTS {

	void ScaleformLogger::LogMessageVarg(LogMessageType a_logMessage, const char* a_fmt, std::va_list a_argList) {

		std::string fmt(a_fmt ? a_fmt : "");

		while (!fmt.empty() && fmt.back() == '\n') {
			fmt.pop_back();
		}

		std::va_list args;
		va_copy(args, a_argList);
		std::vector<char> buf(static_cast<std::size_t>(std::vsnprintf(0, 0, fmt.c_str(), a_argList) + 1));
		std::vsnprintf(buf.data(), buf.size(), fmt.c_str(), args);
		va_end(args);

		logger::info("SCALEFORM: {}"sv, buf.data());
	}

}
