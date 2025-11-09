
#pragma once

#include "UI/Core/ImCategory.hpp"
#include "UI/Lib/imgui.h"
#include "Config/Keybinds.hpp"

namespace GTS {


    class CategoryKeybinds final : public ImCategory {
        public:
        CategoryKeybinds();
        void Draw() override;

        private:
        void DrawOptions();
        void DrawContent();
        static void SetWindowBusy(bool a_busy);
        bool DrawInputEvent(BaseEventData_t& Event, const std::string& a_name, const char* a_description, float columnNameWidth);

        std::string SearchRes;
        std::unordered_map<std::string, bool> HeaderStateMap = {};
        std::vector<std::string> TempKeys = {};

        int Div = 2;
        int RebindIndex = 0;
        int CurEventIndex = UINT16_MAX;
        const int HeaderFlags =  ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX ;
        float Width = 0.0f;
        bool singleColumn = true;

        inline static std::unordered_map<std::string, size_t> categoryMap = {};
        inline static std::unordered_map<std::string, bool> hiddenMap = {};
        inline static std::unordered_map<std::string, const char*> uiNameMap = {};
        inline static std::unordered_map<std::string, const char*> uiDescriptionMap = {};

    };

}