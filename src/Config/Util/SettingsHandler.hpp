#pragma once
#include "Config/Util/FileUtils.hpp"

namespace GTS {

    class SettingsHandler {

        protected:
        static inline std::mutex _ReadWriteLock = {};

        template<typename T>
        static bool LoadStructFromTOML(const auto& a_toml, T& a_data, std::string a_tableName) {
            static_assert(std::is_class_v<T>, "a_data must be a struct or class type");
            try {
                std::lock_guard<std::mutex> lock(_ReadWriteLock);

                a_data = toml::find_or<T>(a_toml, a_tableName, T{});
                logger::info("Struct: {} Parsed!", a_tableName);
                return true;
            }
            catch (toml::exception& e) {
                logger::error("Could not parse the toml table into a struct: {}", e.what());
                return false;
            }
            catch (...) {
                logger::error("LoadStructFromTOML() -> Something really bad happened with {} and not even TOML11's Handler caught it", reflect::type_name<T&>(a_data));
                return false;
            }
        }

        template<typename T>
        static bool UpdateTOMLFromStruct(toml::ordered_value& a_toml, T& a_data, std::string a_tableName) {
            static_assert(std::is_class_v<T>, "a_data must be a struct or class type");
            try {
                std::lock_guard<std::mutex> lock(_ReadWriteLock);

                toml::ordered_value table = a_data;
                a_toml.as_table()[a_tableName] = table;
                logger::info("TOML Data for Table {} Has been Replaced", a_tableName);
                return true;
            }
            catch (toml::exception& e) {
                logger::error("Could not parse the struct into a TOML table: {}", e.what());
                return false;
            }
            catch (...) {
                logger::error("UpdateTOMLFromStruct() -> Something really bad happened with {} and not even TOML11's Handler caught it", reflect::type_name<T&>(a_data));
                return false;
            }
        }

        static bool SaveTOMLToFile(const toml::ordered_value& a_toml, const std::filesystem::path& a_file) {
            try {
                if (!FileUtils::CheckOrCreateFile(a_file)) {
                    logger::error("Settings file was missing and could not be created");
                    return false;
                }
                std::ofstream file(a_file);
                file.exceptions(std::ofstream::failbit);
                if (file.is_open()) {
                    file << toml::format(a_toml);
                    file.close();
                    return true;
                }
                logger::error("Could not open the settings for writing. Settings not saved!");
                return false;
            }
            catch (toml::exception& e) {
                logger::error("Could not parse the toml table when trying to save: {}", e.what());
                return false;
            }
            catch (const std::ios_base::failure& e) {
                logger::error("Could not write to file: {}", e.what());
                return false;
            }
            catch (const std::exception& e) {
                logger::error("Misc Exception: {}", e.what());
                return false;
            }
            catch (...) {
                logger::error("Unknown Exception");
                return false;
            }
        }

        static bool SaveTOMLToString(const toml::ordered_value& a_toml) {
            try {
                Persistent::ModSettings.value = toml::format(a_toml);
                return true;
            }
            catch (toml::exception& e) {
                logger::error("SaveTOMLToString() -> Could not parse the toml table when trying to save: {}", e.what());
                return false;
            }
            catch (const std::exception& e) {
                logger::error("SaveTOMLToString() -> Misc Exception: {}", e.what());
                return false;
            }
            catch (...) {
                logger::error("SaveTOMLToString() -> Unknown Exception");
                return false;
            }
        }
    };

}
