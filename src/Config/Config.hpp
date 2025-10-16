#pragma once

#include "Config/Util/SettingsHandler.hpp"

#include "Config/Settings/SettingsAdvanced.hpp"
#include "Config/Settings/SettingsAI.hpp"
#include "Config/Settings/SettingsAudio.hpp"
#include "Config/Settings/SettingsBalance.hpp"
#include "Config/Settings/SettingsCamera.hpp"
#include "Config/Settings/SettingsGameplay.hpp"
#include "Config/Settings/SettingsGeneral.hpp"
#include "Config/Settings/SettingsHidden.hpp"
#include "Config/Settings/SettingsUI.hpp"

namespace GTS {

    class Config : public CInitSingleton<Config>, public SettingsHandler, public EventListener {

        private:
        static constexpr const char* _LegacyConfigFile = "Settings.toml";
        static inline std::filesystem::path LegacyConfigFilePath = FileUtils::_basePath / _LegacyConfigFile;
        static inline FileUtils _fileManager = {};
        static inline toml::ordered_value TomlData = {};

        public:
        static inline SettingsHidden_t Hidden = {};
        static inline SettingsAdvanced_t Advanced = {};
        static inline SettingsGeneral_t General = {};
        static inline SettingsGameplay_t Gameplay = {};
        static inline SettingsBalance_t Balance = {};
        static inline SettingsAudio_t Audio = {};
        static inline SettingsAI_t AI = {};
        static inline SettingsCamera_t Camera = {};
        static inline SettingsUI_t UI = {};

        void DataReady() override;
        std::string DebugName() override;
        void OnGameLoaded() override;

        static bool SerializeStructsToTOML();
        static bool DeserializeStructsFromTOML();
        static void ResetToDefaults();
        static bool LoadSettingsFromString();
        static bool SaveSettingsToString();
        static bool ExportSettings();
        static bool LoadFromExport(const std::filesystem::path& exportPath);
        static std::vector<std::filesystem::path> GetExportedFiles();
        static bool DeleteExport(const std::filesystem::path& exportPath);
        static bool CleanOldExports(int keepCount);
        static void CopyLegacySettings();
    };

}