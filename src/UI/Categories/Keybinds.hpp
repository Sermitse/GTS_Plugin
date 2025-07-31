
#pragma once

#include "UI/ImGUI/ImCategory.hpp"
#include "Config/Config.hpp"
#include "Config/Keybinds.hpp"
#include "UI/ImGui/ImWindowManager.hpp"

namespace GTS {

    class CategoryKeybinds final : public ImCategory {
        public:
        CategoryKeybinds(){
            title = "Keybinds";
            for(auto& e : DefaultEvents){
                HeaderStateMap.emplace(e.Event.Event,false);
            }
        }
        void Draw() override;


        private:
        const Config& Settings = Config::GetSingleton();
        Keybinds& KeyMgr = Keybinds::GetSingleton();

        //UI

        void DrawOptions();
        void DrawContent();

        static inline void SetWindowBusy(const bool a_busy) {
            if (auto Window = ImWindowManager::GetSingleton().GetWindowByName("Settings")) {
                Window->Busy = a_busy;
            }
        }


        int Div = 2;
        std::string SearchRes;
        bool HideFiltered = false;
        volatile uint8_t ColExpState = 0;

        std::unordered_map<std::string, bool> HeaderStateMap = {};
        //InputEvent Rebinding
        std::string VisualKeyString;
        std::vector<std::string> TempKeys = {};
        int RebindIndex = 0;
        int CurEventIndex = 1000;
        float Width = 0.0f;
        bool DrawInputEvent(GTSInputEvent& Event, const std::string& a_name, float columnNameWidth);
        const int HeaderFlags =  ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX ;

    };

}