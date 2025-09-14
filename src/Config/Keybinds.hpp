#pragma once

#include "Config/Settings/SettingsKeybinds.hpp"
#include "Config/Util/TomlHandler.hpp"

namespace GTS {

    class Keybinds final : public CInitSingleton<Keybinds>, public TOMLHandler, public EventListener {

        private:
        static inline toml::basic_value<toml::ordered_type_config> TomlData;
        static inline std::filesystem::path InputFile = FileManager::_basePath / "Input.toml";

        public:
        static inline std::vector<BaseEventData_t> InputEvents = {};
        static bool LoadKeybinds();
        static bool SaveKeybinds();
        static void ResetKeybinds();
        std::string DebugName() override;
        void DataReady() override;
    };
}