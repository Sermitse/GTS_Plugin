#pragma once
#include "UI/ImGui/Lib/imgui.h"

namespace ImGuiEx {

    // Custom widget that combines a collapsing header with tab bar
    class CollapsingTabHeader {

        public:

        struct TabData {
            std::string label;
            bool visible = true;

            TabData(const std::string& a_lbl);
        };

        private:
        std::vector<TabData> tabs = {};
        int activeTab = 0;
        bool isOpen = false;
        bool isDisabled = false;
        std::string headerLabel;
        std::string headerLabelExtra;
        ImGuiID id;


		public:

        CollapsingTabHeader(const std::string& a_label, bool a_defaultOpen = true);
        CollapsingTabHeader(const std::string& a_label, const std::vector<std::string>& a_categories, bool a_defaultOpen = true);
        CollapsingTabHeader(const std::string& a_label, std::initializer_list<std::string> a_categories, bool a_defaultOpen = true);

        void AddTab(const std::string& a_tabLabel);
        void ClearTabs();

        void SetActiveTab(int a_index);
        int GetActiveTab() const;
        const std::string& GetActiveTabLabel() const;

        void SetExtraInfo(const std::string& a_infoText);
        void SetDisabledState(bool a_disabled);

        bool Begin();
        static void End();
    };

    bool BeginCollapsingTabHeader(CollapsingTabHeader& header);
    void EndCollapsingTabHeader(CollapsingTabHeader& header);
}
