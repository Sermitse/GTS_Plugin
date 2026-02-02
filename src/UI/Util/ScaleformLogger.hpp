#pragma once

namespace GTS {

	class ScaleformLogger final : public RE::GFxLog {
		public:
		static inline const std::filesystem::path _baseSwfPath = std::filesystem::current_path() / R"(Data/Interface)";
		void LogMessageVarg(LogMessageType, const char* a_fmt, std::va_list a_argList) override;
	};
}