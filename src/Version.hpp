#pragma once

namespace GTS {

	struct DLLVersion {
		uint8_t Major, Minor, Patch, Revision;

		// Operator to convert Version to std::string
		explicit operator std::string() const {
			return fmt::format("v{}.{}.{}.{}", Major, Minor, Patch, Revision);
		}
	};

	//Increment the dll's version Here vvvvv
	constexpr DLLVersion PluginVersion { 3, 0, 4, 0 };
}

template <>
struct fmt::formatter<GTS::DLLVersion> {
	// No extra state: parse simply returns the beginning of the format string.
	static constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}

	// The format function MUST be const-qualified.
	template <typename FormatContext>
	auto format(const GTS::DLLVersion& ver, FormatContext& ctx) const {
		return format_to(ctx.out(), "v{}.{}.{}.{}", ver.Major, ver.Minor, ver.Patch, ver.Revision);
	}
};// namespace fmt

template <>
struct std::formatter<GTS::DLLVersion, char> {
	// We don't support any custom format specifiers, so just return the beginning.
	static constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}

	// Note: This function MUST be const-qualified.
	template <typename FormatContext>
	auto format(const GTS::DLLVersion& ver, FormatContext& ctx) const {
		return format_to(ctx.out(), "v{}.{}.{}.{}", ver.Major, ver.Minor, ver.Patch, ver.Revision);
	}
}; // namespace std