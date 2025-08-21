#include "Config/Config.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace GTS {

    // Helper to get current timestamp for exports
    std::string Config::GetTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
        return ss.str();
    }

    // Helper to ensure export directory exists
    bool Config::EnsureExportDirectoryExists() const {
        auto exportDir = ModConfigPath / _Exports;
        std::error_code ec;

        if (!std::filesystem::exists(exportDir, ec)) {
            if (!std::filesystem::create_directories(exportDir, ec)) {
                logger::error("Failed to create export directory: {}", ec.message());
                return false;
            }
        }
        return true;
    }

    // Serialize all structs to TOML
    bool Config::SerializeStructsToTOML() {
        try {
            bool updateRes = true;
            // Ensure TomlData is a table
            TomlData = toml::ordered_table();


            updateRes &= UpdateTOMLFromStruct(TomlData, Hidden);
            updateRes &= UpdateTOMLFromStruct(TomlData, Advanced);
            updateRes &= UpdateTOMLFromStruct(TomlData, General);
            updateRes &= UpdateTOMLFromStruct(TomlData, Gameplay);
            updateRes &= UpdateTOMLFromStruct(TomlData, Balance);
            updateRes &= UpdateTOMLFromStruct(TomlData, Audio);
            updateRes &= UpdateTOMLFromStruct(TomlData, AI);
            updateRes &= UpdateTOMLFromStruct(TomlData, Camera);
            updateRes &= UpdateTOMLFromStruct(TomlData, GtsUI);

            if (!updateRes) {
                logger::error("One or more structs could not be serialized to TOML");
                return false;
            }
            return true;
        }
        catch (const toml::exception& e) {
            logger::error("TOML Exception during serialization: {}", e.what());
            return false;
        }
        catch (...) {
            logger::error("Unknown exception during struct serialization");
            return false;
        }
    }

    // Deserialize all structs from TOML
    bool Config::DeserializeStructsFromTOML() {
        try {
            bool loadRes = true;

            loadRes &= LoadStructFromTOML(TomlData, Hidden);
            loadRes &= LoadStructFromTOML(TomlData, Advanced);
            loadRes &= LoadStructFromTOML(TomlData, General);
            loadRes &= LoadStructFromTOML(TomlData, Gameplay);
            loadRes &= LoadStructFromTOML(TomlData, Balance);
            loadRes &= LoadStructFromTOML(TomlData, Audio);
            loadRes &= LoadStructFromTOML(TomlData, AI);
            loadRes &= LoadStructFromTOML(TomlData, Camera);
            loadRes &= LoadStructFromTOML(TomlData, GtsUI);

            if (!loadRes) {
                logger::critical("One or more structs could not be deserialized");
            }
            return loadRes;
        }
        catch (const toml::exception& e) {
            logger::error("TOML Exception during deserialization: {}", e.what());
            return false;
        }
        catch (const std::exception& e) {
            logger::error("Exception during deserialization: {}", e.what());
            return false;
        }
        catch (...) {
            logger::error("Unknown exception during struct deserialization");
            return false;
        }
    }

    template<typename T>
    bool Config::LoadStructFromTOML(const auto& a_toml, T& a_data) {
        static_assert(std::is_class_v<T>, "a_data must be a struct or class type");
        try {
            std::lock_guard<std::mutex> lock(_ReadWriteLock);
            auto _Name = std::string(GetStructName(a_data));
            a_data = toml::find_or<T>(a_toml, _Name, T{});
            logger::info("Struct: {} Parsed!", _Name);
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
    bool Config::UpdateTOMLFromStruct(auto& a_toml, T& a_data) {
        static_assert(std::is_class_v<T>, "a_data must be a struct or class type");
        try {
            std::lock_guard<std::mutex> lock(_ReadWriteLock);
            std::string _StructName = std::string(GetStructName(a_data));

            toml::ordered_value table = a_data;
            a_toml.as_table()[_StructName] = table;
            logger::info("TOML Data for Table {} Has been Replaced", _StructName);
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

    bool Config::SaveTOMLToFile(const auto& a_toml, const std::filesystem::path& a_file) {
        try {
            std::lock_guard<std::mutex> lock(_ReadWriteLock);

            if (!CheckOrCreateFile(a_file)) {
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

            logger::error("SaveTOMLToFile() -> Could not open the settings for writing. Settings not saved!");
            return false;
        }
        catch (toml::exception& e) {
            logger::error("SaveTOMLToFile() -> Could not parse the toml table when trying to save: {}", e.what());
            return false;
        }
        catch (const std::ios_base::failure& e) {
            logger::error("SaveTOMLToFile() -> Could not write to file: {}", e.what());
            return false;
        }
        catch (const std::exception& e) {
            logger::error("SaveTOMLToFile() -> Misc Exception: {}", e.what());
            return false;
        }
        catch (...) {
            logger::error("SaveTOMLToFile() -> Unknown Exception");
            return false;
        }
    }

    bool Config::SaveTOMLToString(const auto& a_toml) {
        auto& Settings = Persistent::GetSingleton().ModSettings;

        try {
            std::lock_guard<std::mutex> lock(_ReadWriteLock);
            Settings.value = toml::format(a_toml);
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

    void Config::ResetToDefaults() {
        Advanced = SettingsAdvanced{};
        General = SettingsGeneral{};
        AI = SettingsAI{};
        Audio = SettingsAudio{};
        Balance = SettingsBalance{};
        Camera = SettingsCamera{};
        Gameplay = SettingsGameplay{};
        GtsUI = SettingsUI{};
        //Hidden = SettingsHidden{};

        TomlData = toml::ordered_table();
    }

    bool Config::LoadSettingsFromString() {
        auto& Settings = Persistent::GetSingleton().ModSettings;
        if (Settings.value.empty()) {
            logger::info("LoadSettingsFromString(): no TOML payload, skipping load.");
            return false;
        }

        try {
            TomlData = toml::parse_str<toml::ordered_type_config>(Settings.value);
        }
        catch (const toml::exception& e) {
            TomlData = toml::ordered_table();
            Settings.value.clear();
            logger::error("Could not Parse Persistent Mod Settings: {}", e.what());
            return false;
        }
        catch (...) {
            logger::error("LoadSettingsFromString() -> TOML::Parse Exception Outside of TOML11's Scope");
            return false;
        }

        if (TomlData.is_empty()) {
            logger::warn("Parsed TOML is empty, skipping load.");
            return false;
        }

        return DeserializeStructsFromTOML();
    }

    bool Config::SaveSettingsToString() {
        if (!SerializeStructsToTOML()) {
            return false;
        }

        bool saveRes = SaveTOMLToString(TomlData);
        if (!saveRes) {
            logger::error("Something went wrong when trying to save the TOML data to string");
        }
        return saveRes;
    }

    bool Config::ExportSettings(const std::string& a_customName) {

        if (!EnsureExportDirectoryExists()) {
            return false;
        }

        if (!SerializeStructsToTOML()) {
            logger::error("Failed to serialize settings for export");
            return false;
        }

        std::string filename = a_customName.empty() ?
            "Export_" + GetTimestamp() + ".toml" :
            a_customName + "_" + GetTimestamp() + ".toml";

        auto exportPath = ModConfigPath / _Exports / filename;

        bool result = SaveTOMLToFile(TomlData, exportPath);
        if (result) {
            logger::info("Settings exported to: {}", exportPath.string());
        }
        else {
            logger::error("Failed to export settings to: {}", exportPath.string());
        }
        return result;
    }

    std::vector<std::filesystem::path> Config::GetExportedFiles() const {
        std::vector<std::filesystem::path> exports;
        auto exportDir = ModConfigPath / _Exports;

        if (!std::filesystem::exists(exportDir)) {
            return exports;
        }

        std::error_code ec;
        for (const auto& entry : std::filesystem::directory_iterator(exportDir, ec)) {
            if (entry.is_regular_file() && entry.path().extension() == ".toml") {
                exports.push_back(entry.path());
            }
        }

        // Sort by modification time (newest first)
        ranges::sort(exports, [](const auto& a, const auto& b) {
            std::error_code err;
            auto timeA = std::filesystem::last_write_time(a, err);
            auto timeB = std::filesystem::last_write_time(b, err);
            return timeA > timeB;
        });

        return exports;
    }

    bool Config::LoadFromExport(const std::filesystem::path& a_exportPath) {
        if (!std::filesystem::exists(a_exportPath)) {
            logger::error("Export file does not exist: {}", a_exportPath.string());
            return false;
        }

        try {
            auto exportedData = toml::parse<toml::ordered_type_config>(a_exportPath.string());
            TomlData = std::move(exportedData);

            bool result = DeserializeStructsFromTOML();
            if (result) {
                logger::info("Settings loaded from export: {}", a_exportPath.string());
            }
            return result;
        }
        catch (const toml::exception& e) {
            logger::error("Could not parse export file {}: {}", a_exportPath.string(), e.what());
            return false;
        }
        catch (...) {
            logger::error("Unknown exception loading from export: {}", a_exportPath.string());
            return false;
        }
    }

    bool Config::DeleteExport(const std::filesystem::path& a_exportPath) {
        std::error_code ec;
        bool result = std::filesystem::remove(a_exportPath, ec);

        if (result) {
            logger::info("Deleted export: {}", a_exportPath.string());
        }
        else {
            logger::error("Failed to delete export {}: {}", a_exportPath.string(), ec.message());
        }
        return result;
    }

    bool Config::CleanOldExports(int a_keepCount) const {
        auto exports = GetExportedFiles();

        if (exports.size() <= static_cast<size_t>(a_keepCount)) {
            return true; // Nothing to clean
        }

        bool allDeleted = true;
        for (size_t i = a_keepCount; i < exports.size(); ++i) {
            if (!DeleteExport(exports[i])) {
                allDeleted = false;
            }
        }

        logger::info("Cleaned {} old export files", exports.size() - a_keepCount);
        return allDeleted;
    }

}