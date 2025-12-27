#include "Utils/Text/Text.hpp"

namespace GTS {

	void NotifyWithSound(Actor* actor, std::string_view message) {
		if (actor->IsPlayerRef()|| IsTeammate(actor)) {
			static Timer Cooldown = Timer(1.2);
			if (Cooldown.ShouldRun()) {
				const float falloff = 0.13f * get_visual_scale(actor);
				Runtime::PlaySoundAtNode_FallOff(Runtime::SNDR.GTSSoundFail, actor, 0.4f, "NPC COM [COM ]", falloff);
				Notify(message);
			}
		}
	}

	std::wstring Utf8ToUtf16(std::string_view a_utf8) {
		if (a_utf8.empty()) return {};
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, a_utf8.data(), static_cast<int>(a_utf8.size()), nullptr, 0);
		std::wstring result(size_needed, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, a_utf8.data(), static_cast<int>(a_utf8.size()), result.data(), size_needed);
		return result;
	}

	std::string Utf16ToUtf8(std::wstring_view a_utf16) {
		if (a_utf16.empty()) return {};
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, a_utf16.data(), static_cast<int>(a_utf16.size()), nullptr, 0, nullptr, nullptr);
		std::string result(size_needed, '\0');
		WideCharToMultiByte(CP_UTF8, 0, a_utf16.data(), static_cast<int>(a_utf16.size()), result.data(), size_needed, nullptr, nullptr);
		return result;
	}

	bool starts_with(std::string_view arg, std::string_view prefix) {
		return arg.compare(0, prefix.size(), prefix);
	}

	bool matches(std::string_view str, std::string_view reg){
		re2::StringPiece text(str.data(), str.size());
		re2::StringPiece pattern(reg.data(), reg.size());

		const re2::RE2 re(pattern);
		return RE2::FullMatch(text, re);
	}

	std::string str_tolower(std::string s) {
		ranges::transform(s, s.begin(),[](unsigned char c){
			return std::tolower(c);
		});
		return s;
	}

	std::string str_toupper(std::string s) {
		ranges::transform(s, s.begin(),[](unsigned char c){
			return std::toupper(c);
		});
		return s;
	}

	// courtesy of https://stackoverflow.com/questions/5878775/how-to-find-and-replace-string
	void replace_first(std::string& s, std::string const& toReplace, std::string const& replaceWith) {
		std::size_t pos = s.find(toReplace);
		if (pos == std::string::npos) {
			return;
		}
		s.replace(pos, toReplace.length(), replaceWith);
	}

	std::string remove_whitespace(std::string s) {
		std::erase(s,' ');
		return s;
	}

	// Trims whitespace from the beginning and end of the string
	std::string trim(const std::string& s) {
		auto start = s.begin();
		while (start != s.end() && std::isspace(static_cast<unsigned char>(*start))) {
			++start;
		}

		auto end = s.end();
		do {
			--end;
		}
		while (end != start && std::isspace(static_cast<unsigned char>(*end)));

		return string(start, end + 1);
	}

	// In-place trimming functions for a std::string
	void ltrim(std::string& s) {
		s.erase(s.begin(), ranges::find_if(s,[](unsigned char ch) {
			return !std::isspace(ch);
		}));
	}

	void rtrim(std::string& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(),
			[](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
	}

    std::string HumanizeString(std::string_view name) {
        if (name.empty()) {
            return {};
        }

        std::string result;

        // Remove a leading 'k', if present.
        if (name.front() == 'k') {
            name.remove_prefix(1);
        }

        // Process each character by index.
        for (size_t i = 0; i < name.size(); ++i) {
            char c = name[i];

            if (c == '_') {
                // Replace underscore with a space (avoiding duplicate spaces).
                if (result.empty() || result.back() != ' ')
                    result += ' ';
                continue;
            }

            // For uppercase letters (except at the very start), add a space if the previous character
            // was NOT uppercase. This prevents adding spaces between sequential uppercase letters.
            if (i > 0 && std::isupper(static_cast<unsigned char>(c)) &&
                !std::isupper(static_cast<unsigned char>(name[i - 1]))) {
                if (result.empty() || result.back() != ' ')
                    result += ' ';
            }

            result += c;
        }

        // Trim leading and trailing spaces.
        size_t start = result.find_first_not_of(' ');
        if (start == std::string::npos) {
            return "";
        }
        size_t end = result.find_last_not_of(' ');
        result = result.substr(start, end - start + 1);

        // Collapse any consecutive spaces (if any)
        std::string final_result;
        bool prev_space = false;
        for (char ch : result) {
            if (ch == ' ') {
                if (!prev_space) {
                    final_result += ' ';
                    prev_space = true;
                }
            }
            else {
                final_result += ch;
                prev_space = false;
            }
        }

        return final_result;
    }

    bool ContainsString(const std::string& a1, const std::string& a2) {
        auto to_lower = [](unsigned char c) { return std::tolower(c); };
        auto a1_view = a1 | std::views::transform(to_lower);
        auto a2_view = a2 | std::views::transform(to_lower);
        auto result = std::ranges::search(a1_view, a2_view);
        return result.begin() != a1_view.end();
    }
}
