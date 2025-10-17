#pragma once
#include "Button.hpp"

#include "UI/ImGui/Lib/imgui.h"

namespace ImGuiEx {

    class InfoCard {
        public:
        enum class Section {
            None,
            Section1,
            Section2
        };

        InfoCard() : expanded_section_(Section::None) {}

        void Render(const char* id, const ImVec2& card_size = ImVec2(400, 400)) {
            ImGui::PushID(id);

            ImGui::BeginChild("Card", card_size,
                ImGuiChildFlags_Borders      |
                ImGuiChildFlags_FrameStyle   |
                ImGuiChildFlags_NavFlattened | 
                ImGuiChildFlags_AutoResizeY  |
                ImGuiChildFlags_AlwaysAutoResize,

                ImGuiWindowFlags_NoSavedSettings
            );

            // Calculate layout
            float content_width = ImGui::GetContentRegionAvail().x;
            float button_width = 60.0f;
            float spacing = 10.0f;

            // Main content area (left side)
            ImGui::BeginChild("MainContent", ImVec2(content_width - button_width - spacing, 0), false);
            RenderMainContent();

            // Expanded sections
            if (expanded_section_ == Section::Section1) {
                ImGui::Separator();
                RenderSection1();
            }
            else if (expanded_section_ == Section::Section2) {
                ImGui::Separator();
                RenderSection2();
            }

            ImGui::EndChild();

            // Button column (top right)
            ImGui::SameLine();
            ImGui::BeginGroup();

            bool section1_active = expanded_section_ == Section::Section1;
            if (section1_active) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
            if (ImGuiEx::Button("[Ex]", "Show More Info.")) {
                expanded_section_ = section1_active ? Section::None : Section::Section1;
            }
            if (section1_active) ImGui::PopStyleColor();

            bool section2_active = expanded_section_ == Section::Section2;
            if (section2_active) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
            if (ImGuiEx::Button("[Ki]", "Show Kill Count Info")) {
                expanded_section_ = section2_active ? Section::None : Section::Section2;
            }
            if (section2_active) ImGui::PopStyleColor();

            if (ImGuiEx::Button("[Sp]","Spectate NPC")) {
                // Placeholder
            }

            ImGui::EndGroup();

            ImGui::EndChild();

            ImGui::PopID();
        }

        // Override these to customize card content
        virtual void RenderMainContent() {
            ImGui::Text("Main Card Content");
            ImGui::Text("Override RenderMainContent()");
        }

        virtual void RenderSection1() {
            ImGui::Text("Section 1 Content");
            ImGui::Text("Override RenderSection1()");
        }

        virtual void RenderSection2() {
            ImGui::Text("Section 2 Content");
            ImGui::Text("Override RenderSection2()");
        }

        private:
        Section expanded_section_;
    };
}
