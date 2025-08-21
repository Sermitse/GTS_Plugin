#pragma once

#include "Config/SettingsList.hpp"

namespace GTS {

    class Config {

        private:

        // Helper methods for export functionality
        static std::string GetTimestamp();
        bool EnsureExportDirectoryExists() const;

        // Core serialization/deserialization methods
        bool SerializeStructsToTOML();
        bool DeserializeStructsFromTOML();

        //Create structs with default values.
        //These act as sane defaults in case new data is loaded or the toml itself is corrupted.
        SettingsGeneral General = {};
        SettingsAdvanced Advanced = {};
        SettingsAI AI = {};
        SettingsAudio Audio = {};
        SettingsBalance Balance = {};
        SettingsCamera Camera = {};
        SettingsGameplay Gameplay{};
        SettingsUI GtsUI = {};
        SettingsHidden Hidden = {};

        template <typename T>
        constexpr std::string_view GetStructName(const T&) {
            return reflect::type_name<T>();
        }

	    #define REGISTER_STRUCT_NAME(TYPE, NAME)                        \
	    template <>                                                     \
	    constexpr std::string_view GetStructName<TYPE>(const TYPE&) {   \
			return NAME;                                                \
		}

        //Too much effort to make a parser for it, So Define the names here.
        REGISTER_STRUCT_NAME(SettingsGeneral, "General")
        REGISTER_STRUCT_NAME(SettingsAdvanced, "Advanced")
        REGISTER_STRUCT_NAME(SettingsAI, "AI")
        REGISTER_STRUCT_NAME(SettingsAudio, "Audio")
        REGISTER_STRUCT_NAME(SettingsBalance, "Balance")
        REGISTER_STRUCT_NAME(SettingsCamera, "Camera")
        REGISTER_STRUCT_NAME(SettingsGameplay, "Gameplay")
        REGISTER_STRUCT_NAME(SettingsUI, "UI")
        REGISTER_STRUCT_NAME(SettingsHidden, "Hidden")

        const std::string _Subfolder = R"(Data\SKSE\Plugins\GTSPlugin)";
        const std::string _Exports = R"(Exports)";
        const std::string _LegacyConfigFile = "Settings.toml";

        //Currentpath Resolves to the Skyrim root folder where the .exe is.
        const std::filesystem::path LegacyConfigFilePath = std::filesystem::current_path() / _Subfolder / _LegacyConfigFile;
        const std::filesystem::path ModConfigPath = std::filesystem::current_path() / _Subfolder;

        toml::basic_value<toml::ordered_type_config> TomlData;
        //toml::basic_value<toml::ordered_type_config> TomlDataGlobal;

        std::mutex _ReadWriteLock;

        Config() = default;
        Config(const Config&) = delete;
        Config& operator=(const Config&) = delete;

        template<typename T>
        [[nodiscard]] bool LoadStructFromTOML(const auto& a_toml, T& a_data);

        template<typename T>
        [[nodiscard]] bool UpdateTOMLFromStruct(auto& a_toml, T& a_data);

        [[nodiscard]] bool SaveTOMLToFile(const auto& a_toml, const std::filesystem::path& a_file);

        [[nodiscard]] bool SaveTOMLToString(const auto& a_toml);


    public:

        //Static Accessors (Helpers)
        //They're wrapped this way to ensure that the singleton has run first.
        //Sideeffect is that if you call these in the singleton it will deadlock wating on the latch.
        [[nodiscard]] static inline SettingsGeneral& GetGeneral() {
            return GetSingleton().General;
        }

        [[nodiscard]] static inline SettingsAdvanced& GetAdvanced() {

            return GetSingleton().Advanced;
        }

        [[nodiscard]] static inline SettingsAI& GetAI() {
            return GetSingleton().AI;
        }

        [[nodiscard]] static inline SettingsAudio& GetAudio() {
            return GetSingleton().Audio;
        }

        [[nodiscard]] static inline SettingsBalance& GetBalance() {
            return GetSingleton().Balance;
        }

        [[nodiscard]] static inline SettingsCamera& GetCamera() {
            return GetSingleton().Camera;
        }

        [[nodiscard]] static inline SettingsGameplay& GetGameplay() {
            return GetSingleton().Gameplay;
        }

        [[nodiscard]] static inline SettingsUI& GetUI() {
            return GetSingleton().GtsUI;
        }

        [[nodiscard]] static inline SettingsHidden& GetHidden() {
            return GetSingleton().Hidden;
        }

        static inline void CopyLegacySettings(Config& Instance) {

            // Check if the file exists else don't do anything
            if (!std::filesystem::exists(Instance.LegacyConfigFilePath)) {
                return;
            }
            if (!Instance.EnsureExportDirectoryExists()) {
                return;
            }

            try {
                // Create destination path in exports folder
                auto exportDir = std::filesystem::path(Instance.ModConfigPath / Instance._Exports);
                auto destinationPath = exportDir / "LegacySettings.toml";

                // Copy the legacy config file to exports folder
                std::filesystem::copy_file(Instance.LegacyConfigFilePath, destinationPath,
                    std::filesystem::copy_options::overwrite_existing);

                // Delete the original legacy file after successful copy
                std::filesystem::remove(Instance.LegacyConfigFilePath);

            }
            catch (const std::filesystem::filesystem_error& e) {
                logger::error("CopyLegacySettings() FS error {}", e.what());
            }
        }

        [[nodiscard]] static inline Config& GetSingleton() {
            static Config Instance;
            return Instance;
        }

        // Export management methods
        bool ExportSettings(const std::string& a_customName = "");
        std::vector<std::filesystem::path> GetExportedFiles() const;
        bool LoadFromExport(const std::filesystem::path& a_exportPath);
        static bool DeleteExport(const std::filesystem::path& a_exportPath);
        bool CleanOldExports(int a_keepCount = 5) const;

        bool SaveSettingsToString();
        bool LoadSettingsFromString();

        void ResetToDefaults();

    };
}