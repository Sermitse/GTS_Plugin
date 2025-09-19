#pragma once

#include "Config/Settings/SettingsKeybinds.hpp"
#include "Config/Util/SettingsHandler.hpp"

namespace GTS {

    class Keybinds final : public CInitSingleton<Keybinds>, public SettingsHandler, public EventListener {

        private:
        static inline toml::basic_value<toml::ordered_type_config> TomlData;
        static inline std::filesystem::path InputFile = FileUtils::_basePath / "Input.toml";

        public:
        static inline std::vector<BaseEventData_t> InputEvents = {};
        static bool LoadKeybinds();
        static bool SaveKeybinds();
        static void ResetKeybinds();
        std::string DebugName() override;
        void DataReady() override;
    };
}