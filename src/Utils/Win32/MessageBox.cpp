#include "Utils/Win32/Messagebox.hpp"
#include "Utils/Text/Text.hpp"

namespace GTS {

	const wchar_t* const CloseMsg = L"\nThe game will now close.";

	void ReportAndExit(std::string_view a_message) {
		std::wstring wideMessage = Utf8ToUtf16(a_message);
		wideMessage += CloseMsg;
		
		MessageBoxW(
			nullptr,
			wideMessage.c_str(),
			L"Size Matters - GtsPlugin.dll",
			MB_OK | MB_ICONERROR | MB_TOPMOST
		);

		WinAPI::TerminateProcess(WinAPI::GetCurrentProcess(), EXIT_FAILURE);
	}

	void ReportAndExit(std::wstring_view a_message) {
		std::wstring wideMessage (a_message);
		wideMessage += CloseMsg;

		MessageBoxW(
			nullptr,
			a_message.data(),
			L"Size Matters - GtsPlugin.dll",
			MB_OK | MB_ICONERROR | MB_TOPMOST
		);

		WinAPI::TerminateProcess(WinAPI::GetCurrentProcess(), EXIT_FAILURE);
	}

	void ReportInfo(std::string_view a_message) {
		std::wstring wideMessage = Utf8ToUtf16(a_message);

		MessageBoxW(
			nullptr,
			wideMessage.c_str(),
			L"Size Matters - GtsPlugin.dll",
			MB_OK | MB_ICONINFORMATION | MB_TOPMOST
		);
	}

	void ReportInfo(std::wstring_view a_message) {
		MessageBoxW(
			nullptr,
			a_message.data(),
			L"Size Matters - GtsPlugin.dll",
			MB_OK | MB_ICONINFORMATION | MB_TOPMOST
		);
	}
}