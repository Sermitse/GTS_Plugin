#pragma once

namespace GTS {

	void ReportAndExit(std::string_view a_message);
	void ReportAndExit(std::wstring_view a_message);
	void ReportInfo(std::string_view a_message);
	void ReportInfo(std::wstring_view a_message);

}