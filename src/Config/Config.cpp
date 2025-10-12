#include "Config/Config.hpp"
#include "Config/Util/SettingsOperations.hpp"

namespace GTS {

    std::string Config::DebugName() {
        return "::Config";
    }

    void Config::DataReady() {
        CopyLegacySettings();
    }

    // Serialization methods using helper class
    bool Config::SerializeStructsToTOML() {
        return SettingsOperations::SerializeAllStructsToTOML(GetSingleton(), TomlData);
    }

    bool Config::DeserializeStructsFromTOML() {
        return SettingsOperations::DeserializeAllStructsFromTOML(GetSingleton(), TomlData);
    }

    void Config::ResetToDefaults() {
        SettingsOperations::ResetAllStructsToDefaults(GetSingleton());
        TomlData = toml::ordered_table();
    }

    bool Config::LoadSettingsFromString() {

        const auto Sett = &Persistent::ModSettings.value;
        if (Sett->empty()) {
            logger::info("LoadSettingsFromString(): no TOML payload, skipping load.");
            return false;
        }

        try {
            TomlData = toml::parse_str<toml::ordered_type_config>(*Sett).as_table();
        }
        catch (const toml::exception& e) {
            TomlData = toml::ordered_table();
            Sett->clear();
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
        return SaveTOMLToString(TomlData);

    }

    // Export functionality
    bool Config::ExportSettings() {

        if (!_fileManager.EnsureExportDirectoryExists()) {
            return false;
        }

        if (!SerializeStructsToTOML()) {
            logger::error("Failed to serialize settings for export");
            return false;
        }

        const std::string FileName = "Export_" + std::string(PlayerCharacter::GetSingleton()->GetName()) +  "_" + FileUtils::GetTimestamp() + ".toml";
        auto exportPath = _fileManager.GetExportPath(FileName);
        bool result = SaveTOMLToFile(TomlData, exportPath);

        if (result) {
            logger::info("Settings exported to: {}", exportPath.string());
        }
        else {
            logger::error("Failed to export settings to: {}", exportPath.string());
        }
        return result;
    }

    bool Config::LoadFromExport(const std::filesystem::path& exportPath) {

        if (!std::filesystem::exists(exportPath)) {
            logger::error("Export file does not exist: {}", exportPath.string());
            return false;
        }

        try {
            auto exportedData = toml::parse<toml::ordered_type_config>(exportPath.string());
            TomlData = std::move(exportedData).as_table();
            bool result = DeserializeStructsFromTOML();
            if (result) {
                logger::info("Settings loaded from export: {}", exportPath.string());
            }
            return result;
        }
        catch (const toml::exception& e) {
            logger::error("Could not parse export file {}: {}", exportPath.string(), e.what());
            return false;
        }
        catch (...) {
            logger::error("Unknown exception loading from export: {}", exportPath.string());
            return false;
        }
    }

    std::vector<std::filesystem::path> Config::GetExportedFiles() {
        return _fileManager.GetExportedFiles();
    }

    bool Config::DeleteExport(const std::filesystem::path& exportPath) {
        return _fileManager.DeleteExport(exportPath);
    }

    bool Config::CleanOldExports(int keepCount) {
        return _fileManager.CleanOldExports(keepCount);
    }

    void Config::CopyLegacySettings() {
        _fileManager.CopyLegacySettings(LegacyConfigFilePath);
    }

    void Config::OnGameLoaded() {
        LoadSettingsFromString();
        spdlog::set_level(spdlog::level::from_str(Advanced.sLogLevel));
    }
}
