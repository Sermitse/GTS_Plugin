#pragma once

#include "Config/SettingsList.hpp"

namespace GTS {

    class Config {

        private:

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
        const std::string _ConfigFile = "Settings.toml";

        //Currentpath Resolves to the Skyrim root folder where the .exe is.
        const std::filesystem::path ConfigFile = std::filesystem::current_path() / _Subfolder / _ConfigFile;

        toml::basic_value<toml::ordered_type_config> TomlData;
        toml::basic_value<toml::ordered_type_config> TomlDataGlobal;

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

        [[nodiscard]] static inline Config& GetSingleton() {
            static Config Instance;

            static std::atomic_bool Initialized;
            static std::latch Latch(1);

            if (!Initialized.exchange(true)) {
                logger::info("Loading TOML Settings in .ctor...");

                if (!Instance.LoadSettingsFromFile()) {
                    ReportInfo("Settings.toml is either invalid or corrupted.\n"
							   "Click OK to clear out the settings file.\n"
							   "This will reset the mod's settings."
                    );

                    Instance.ResetToDefaults();

                    //Delete the config file
                    if (!DeleteFile(Instance.ConfigFile)) {
                        ReportAndExit("Could not delete Settings.toml\n"
									  "Check GTSPlugin.log for more info.\n"
									  "The game will now close."
                        );
                    }

                    //Recreate the config file and start with a fresh table.
                    if (!CheckFile(Instance.ConfigFile)) {
                        ReportAndExit("Could not create a blank Settings.toml file.\n"
                                      "Check GTSPlugin.log for more info.\n"
                                      "The game will now close."
                        );
                    }
                }

                //Incase the File is empty/missing newly added data...
                //Explicitly Ignore the [[Nodiscard]]
                auto res = Instance.SaveSettings();

                logger::info(".ctor Load OK");

                Latch.count_down();
            }
            Latch.wait();

            return Instance;
        }

        [[nodiscard]] bool LoadSettingsFromFile();
        bool SaveSettingsToString();
        bool SaveSettingsToFile();

        [[nodiscard]] bool SaveSettings();

        void ResetToDefaults();
        bool LoadSettings();
        bool LoadSettingsFromString();
    };
}