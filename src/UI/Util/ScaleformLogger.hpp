#pragma once

namespace GTS {

	class ScaleformLogger final : public RE::GFxLog {
		public:
		void LogMessageVarg(LogMessageType, const char* a_fmt, std::va_list a_argList) override;
	};
}