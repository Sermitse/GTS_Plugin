#pragma once

#include "Config/KeybindList.hpp"

//---------------
// INPUT EVENT
//---------------

namespace GTS {

    class Keybinds {
        public:

        std::vector<GTSInputEvent> InputEvents = {};

        [[nodiscard]] static inline Keybinds& GetSingleton(){
            static Keybinds Instance;

            static std::atomic_bool Initialized;
            static std::latch Latch(1);
            
            if (!Initialized.exchange(true)) {
                logger::info("Loading TOML Keybinds in .ctor...");
    
                if(!Instance.LoadKeybinds()){
                    ReportInfo("Input.toml is either invalid or corrupted.\n"
								  "Click OK to clear out the file.\n"
								  "This will reset your keybinds.");
            
                    Instance.ResetKeybinds();
                    
                    //Delete the config file
                    if(!DeleteFile(Instance.InputFile)){
                        ReportAndExit("Could not delete Input.toml\n"
									  "Check GTSPlugin.log for more info.\n"
									  "The game will now close."
                        );
                    }
                    
                    //Recreate the config file and start with a fresh table.
                    if(!CheckOrCreateFile(Instance.InputFile)){
                        ReportAndExit("Could not create a blank Input.toml file.\n"
									  "Check GTSPlugin.log for more info.\n"
									  "The game will now close."
                        );
                    }
                }
    
                //Incase the File is empty/missing newly added data...
                //Explicitly Ignore the [[Nodiscard]]
                auto res = Instance.SaveKeybinds();
    
                logger::info(".ctor Load OK");
    
                Latch.count_down();
            }
            Latch.wait();

            return Instance;
        }

        [[nodiscard]] bool LoadKeybinds();
        [[nodiscard]] bool SaveKeybinds();
        void ResetKeybinds();

        private:



        Keybinds() = default;
        Keybinds(const Keybinds&) = delete;
        Keybinds& operator=(const Keybinds&) = delete;

        const std::string _Subfolder = "Data\\SKSE\\Plugins\\GTSPlugin"; 
        const std::string _ConfigFile = "Input.toml";

        //Currentpath Resolves to the Skyrim root folder where the .exe is.
        const std::filesystem::path InputFile = std::filesystem::current_path() / _Subfolder / _ConfigFile;
    
        toml::basic_value<toml::ordered_type_config> TomlData;
        std::mutex _ReadWriteLock;

    };
}