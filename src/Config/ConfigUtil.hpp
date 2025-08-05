#pragma once
#include "Utils/MessageboxUtil.hpp"

/*
    TOML11 Reflection Macro & Templates. Based on https://github.com/ToruNiina/toml11/blob/main/examples/reflect/reflect.hpp
*/

namespace toml::refl {

	template<typename T, typename TC>
	T from(const basic_value<TC>& v) {
		T x;
		reflect::for_each([&v, &x](auto I) {
			using member_type = std::remove_cvref_t<decltype(reflect::get<I>(x))>;
			const auto key = std::string(reflect::member_name<I>(x));
			//Changed to find_or. If the value is not found return the struct's value.
			reflect::get<I>(x) = toml::find_or<member_type>(v, key,reflect::get<I>(x));
		}, x);
		return x;
	}

	template<typename TC = toml::ordered_type_config, typename T>
	basic_value<TC> into(const T& x) {
		basic_value<TC> v(toml::table{});
		reflect::for_each([&v, &x](auto I) {
			using member_type = std::remove_cvref_t<decltype(reflect::get<I>(x))>;
			const auto key = std::string(reflect::member_name<I>(x));
			v[key] = reflect::get<I>(x);
		}, x);
		return v;
	}
}

#define TOML_SERIALIZABLE(X)                               \
namespace toml {                                           \
    template<>                                             \
    struct into<X>                                         \
    {                                                      \
        template<typename TC = toml::ordered_type_config>  \
        static toml::basic_value<TC> into_toml(const X& x) \
        {                                                  \
            return refl::into(x);                          \
        }                                                  \
    };                                                     \
    template<>                                             \
    struct from<X>                                         \
    {                                                      \
        template<typename TC = toml::ordered_type_config>  \
        static X from_toml(const toml::basic_value<TC>& v) \
        {                                                  \
            return refl::from<X>(v);                       \
        }                                                  \
    };                                                     \
} /* toml */

/*
    Helper functions Used For TOML SerDe
*/

namespace GTS {

    [[nodiscard]] static inline bool CheckOrCreateFile(const std::filesystem::path& a_file) {
        try {
            // Check if the file exists
            if (std::filesystem::exists(a_file)) {
                return true;
            }

            // Create parent directories if they don't exist
            if (std::filesystem::create_directories(a_file.parent_path())) {
                logger::critical("Plugin folder was mising and was created, MOD BROKEN.");
                ReportAndExit("The GTSPlugin folder was missing and had to be created.\n"
                "This indicates that the mod was not installed correctly.\n"
				"The mod will not work if the Font Folder and Runtime.toml are missing.\n"
				"The game will now close");
            }

            // Try to create the file
            std::ofstream file(a_file);
            file.exceptions(std::ofstream::failbit);
            if (file) {
                file.close();
                logger::warn("Configuration file did not exist but was successfully created");
                return true;
            }

            return false;
        }
        catch (const std::filesystem::filesystem_error& e) {
            logger::error("CheckFile() Filesystem error: {}", e.what());
            return false;
        }
        catch (const std::exception& e) {
            logger::error("CheckFile() -> Exception: {}", e.what());
            return false;
        }
        catch (...) {
            logger::error("CheckFile() -> Unknown Exception");
            return false;
        }
    }

    [[nodiscard]] static inline bool DeleteFile(const std::filesystem::path& a_file) {
        try {
            // Check if the file exists
            if (std::filesystem::exists(a_file)) {
                std::filesystem::remove(a_file);
                logger::info("Configuration file was successfully deleted");
                return true;
            }
            return false;

        }
        catch (const std::filesystem::filesystem_error& e) {
            logger::error("DeleteFile() Filesystem error: {}", e.what());
            return false;
        }
        catch (const std::exception& e) {
            logger::error("DeleteFile() -> Exception: {}", e.what());
            return false;
        }
        catch (...) {
            logger::error("DeleteFile() -> Unknown Exception");
            return false;
        }
    }
}