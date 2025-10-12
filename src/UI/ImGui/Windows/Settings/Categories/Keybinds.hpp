
#pragma once

#include "UI/ImGUI/Core/ImCategory.hpp"
#include "UI/ImGui/Lib/imgui.h"
#include "Config/Keybinds.hpp"

namespace GTS {


    class CategoryKeybinds final : public ImCategory {
        public:
        CategoryKeybinds();
        void Draw() override;

        private:
        void DrawOptions();
        void DrawContent();
        static void SetWindowBusy(const bool a_busy);

        int Div = 2;
        std::string SearchRes;
        bool HideFiltered = false;
        volatile uint8_t ColExpState = 0;
        std::unordered_map<std::string, bool> HeaderStateMap = {};

        //InputEvent Rebinding
        std::string VisualKeyString;
        std::vector<std::string> TempKeys = {};
        int RebindIndex = 0;
        int CurEventIndex = UINT16_MAX;
        float Width = 0.0f;
        bool DrawInputEvent(BaseEventData_t& Event, const std::string& a_name, float columnNameWidth);
        const int HeaderFlags =  ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX ;

    };

}