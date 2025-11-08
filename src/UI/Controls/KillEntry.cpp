#include "UI/Controls/KillEntry.hpp"
#include "UI/Lib/imgui.h"

namespace ImGuiEx {

    void DrawKillfeedEntry(const KillEntry& entry) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();

        // Calculate fade based on lifetime
        float fade = 1.0f;
        if (entry.lifetime > entry.max_lifetime - 1.0f) {
            fade = (entry.max_lifetime - entry.lifetime);
        }

        ImVec4 text_color = ImVec4(1.0f, 1.0f, 1.0f, fade);
        ImVec4 weapon_color = ImVec4(0.8f, 0.8f, 0.8f, fade);

        float padding = 8.0f;
        float spacing = 12.0f;
        float total_width = 0.0f;
        float height = ImGui::GetTextLineHeight() + padding * 2;

        // Calculate width based on whether it's an env kill
        if (entry.attacker.empty()) {
            // Format: [weapon_icon] victim
            ImVec2 text_size_weapon = ImGui::CalcTextSize(entry.weapon_icon.c_str());
            ImVec2 text_size_victim = ImGui::CalcTextSize(entry.victim.c_str());
            total_width = text_size_weapon.x + text_size_victim.x + spacing + padding * 2;
        }
        else {
            // Format: attacker [weapon_icon] victim
            ImVec2 text_size_attacker = ImGui::CalcTextSize(entry.attacker.c_str());
            ImVec2 text_size_weapon = ImGui::CalcTextSize(entry.weapon_icon.c_str());
            ImVec2 text_size_victim = ImGui::CalcTextSize(entry.victim.c_str());
            total_width = text_size_attacker.x + text_size_weapon.x + text_size_victim.x + spacing * 2 + padding * 2;
        }

        // Draw background
        ImVec4 bg_color = ImVec4(0.0f, 0.0f, 0.0f, 0.6f * fade);
        draw_list->AddRectFilled(pos, ImVec2(pos.x + total_width, pos.y + height),
            ImGui::GetColorU32(bg_color), 4.0f);

        // Position cursor with padding
        ImGui::SetCursorScreenPos(ImVec2(pos.x + padding, pos.y + padding));

        if (entry.attacker.empty()) {
            // Environmental kill: [weapon_icon] victim
            ImGui::TextColored(weapon_color, "%s", entry.weapon_icon.c_str());
            ImGui::SameLine(0, spacing);
            ImGui::TextColored(text_color, "%s", entry.victim.c_str());
        }
        else {
            // Player kill: attacker [weapon_icon] victim
            ImGui::TextColored(text_color, "%s", entry.attacker.c_str());
            ImGui::SameLine(0, spacing);
            ImGui::TextColored(weapon_color, "%s", entry.weapon_icon.c_str());
            ImGui::SameLine(0, spacing);
            ImGui::TextColored(text_color, "%s", entry.victim.c_str());
        }

        // Move cursor to next line
        ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + height + 4.0f));
    }
}