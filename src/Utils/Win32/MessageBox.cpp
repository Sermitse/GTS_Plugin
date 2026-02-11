#include "Utils/Win32/Messagebox.hpp"
#include "Utils/Text/Text.hpp"

namespace {

	HWND GetHandle(){

		const auto* renderer = RE::BSRenderManager::GetSingleton();
		if (!renderer) {
			return nullptr;
		}

		auto* swapChain = renderer->GetRuntimeData().swapChain;
		if (!swapChain) {
			return nullptr;
		}

		DXGI_SWAP_CHAIN_DESC desc{};
		if (FAILED(swapChain->GetDesc(&desc))) {
			return nullptr;
		}

		return desc.OutputWindow;
	}

	auto force_show_cursor = [] {
		while (ShowCursor(TRUE) < 0) {}
	};

	auto restore_cursor_hide = [] {
		while (ShowCursor(FALSE) >= 0) {}
	};
}

namespace GTS {

	const wchar_t* const CloseMsg = L"\nThe game will now close.";

	void ReportAndExit(std::string_view a_message) {
		std::wstring wideMessage = Utf8ToUtf16(a_message);
		wideMessage += CloseMsg;
		
		force_show_cursor();

		MessageBoxW(
			GetHandle(),
			wideMessage.data(),
			L"Size Matters - GtsPlugin.dll",
			MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND
		);

		restore_cursor_hide();

		WinAPI::TerminateProcess(WinAPI::GetCurrentProcess(), EXIT_FAILURE);
	}

	void ReportAndExit(std::wstring_view a_message) {
		std::wstring wideMessage (a_message);
		wideMessage += CloseMsg;

		force_show_cursor();

		MessageBoxW(
			GetHandle(),
			wideMessage.data(),
			L"Size Matters - GtsPlugin.dll",
			MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND
		);

		restore_cursor_hide();

		WinAPI::TerminateProcess(WinAPI::GetCurrentProcess(), EXIT_FAILURE);
	}

	void ReportInfo(std::string_view a_message) {
		std::wstring wideMessage = Utf8ToUtf16(a_message);

		force_show_cursor();

		MessageBoxW(
			GetHandle(),
			wideMessage.data(),
			L"Size Matters - GtsPlugin.dll",
			MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND
		);

		restore_cursor_hide();
	}

	void ReportInfo(std::wstring_view a_message) {
		std::wstring wideMessage(a_message);
		wideMessage += CloseMsg;

		force_show_cursor();

		MessageBoxW(
			GetHandle(),
			wideMessage.data(),
			L"Size Matters - GtsPlugin.dll",
			MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND
		);

		restore_cursor_hide();
	}
}