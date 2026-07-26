#include "UI/Controls/KillEntry.hpp"

#include "UI/Core/ImColorUtils.hpp"
#include "UI/Core/ImFontManager.hpp"

namespace ImGuiEx {

    namespace {

        std::string TruncateToWidth(const std::string& a_text, float a_maxWidth) {
            if (a_text.empty()) return a_text;

            if (ImGui::CalcTextSize(a_text.c_str()).x <= a_maxWidth) return a_text;

            constexpr const char* Ellipsis = "...";
            float EllipsisWidth = ImGui::CalcTextSize(Ellipsis).x;
            if (a_maxWidth <= EllipsisWidth) return Ellipsis;

            std::string Result = a_text;
            while (!Result.empty()) {
                Result.pop_back();
                if (ImGui::CalcTextSize(Result.c_str()).x + EllipsisWidth <= a_maxWidth) {
                    return Result + Ellipsis;
                }
            }
            return Ellipsis;
        }
    }

    void DrawKillfeedEntry(std::unique_ptr<KillEntry>& entry, const KillFeedStyle& a_style) {

        GTS::ImFontManager::Push(GTS::ImFontManager::kWidgetBody, a_style.fontScale);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 avail = ImGui::GetContentRegionAvail();
        const ImGuiStyle& style = ImGui::GetStyle();
        float alpha = style.Alpha;

        float fade = 1.0f;
        if (!a_style.neverFade) {
            if (entry->lifetime > a_style.visDuration - 1.0f) {
                fade = (a_style.visDuration - entry->lifetime);
            }
            fade = ImClamp(fade, 0.0f, 1.0f);
        }

        std::string KilledBy = entry->type;

        ImVec4 attackerColor = ImUtil::Colors::CompensateForDarkColors(ImGui::ColorConvertU32ToFloat4(a_style.attackerCol), 0.3f);
        attackerColor.w = fade * alpha;

        ImVec4 victimColor = ImUtil::Colors::CompensateForDarkColors(ImGui::ColorConvertU32ToFloat4(a_style.victimCol), 0.3f);
        victimColor.w = fade * alpha;

        ImVec4 deathTypeColor = ImUtil::Colors::CompensateForDarkColors(ImGui::ColorConvertU32ToFloat4(a_style.deathTypeCol), 0.3f);
        deathTypeColor.w = fade * alpha;

        ImVec4 bg_color = ImGui::ColorConvertU32ToFloat4(a_style.bgColor);
        bg_color.w *= fade * alpha * a_style.bgAlpha; // Apply fade to the alpha channel

        ImVec4 border_color = { 1.0f, 1.0f, 1.0f, 0.3f * fade * alpha * a_style.bgAlpha };

        float padding = style.FramePadding.y; // respect ImGui padding
        float height = ImGui::GetTextLineHeight() + padding * 2;
        float total_width = avail.x;

        const bool showKiller = !(a_style.flags & KillFeedEntryFlag_NoKiller) && !entry->attacker.empty();
        const bool showKillType = !(a_style.flags & KillFeedEntryFlag_NoKillType);

        int elements = (showKiller ? 1 : 0) + (showKillType ? 1 : 0) + 1; // victim always shown
        constexpr float MinSpacing = 4.0f;
        float available_text_width = std::max(total_width - padding * 2 - MinSpacing * (elements - 1), 0.0f);

        std::string AttackerText = showKiller ? entry->attacker : std::string();
        std::string TypeText = showKillType ? KilledBy : std::string();
        std::string VictimText = entry->victim;
        
    	// Calculate text sizes
        ImVec2 text_size_attacker = AttackerText.empty() ? ImVec2(0, 0) : ImGui::CalcTextSize(AttackerText.c_str());
        ImVec2 text_size_weapon = TypeText.empty() ? ImVec2(0, 0) : ImGui::CalcTextSize(TypeText.c_str());
        ImVec2 text_size_victim = ImGui::CalcTextSize(VictimText.c_str());

        float text_total = text_size_attacker.x + text_size_weapon.x + text_size_victim.x;

        // Truncate each field proportionally to its share of the total if it can't all fit.
        if (text_total > available_text_width && text_total > 0.0f) {
            float scale = available_text_width / text_total;

            if (!AttackerText.empty()) {
                AttackerText = TruncateToWidth(AttackerText, text_size_attacker.x * scale);
                text_size_attacker = ImGui::CalcTextSize(AttackerText.c_str());
            }
            if (!TypeText.empty()) {
                TypeText = TruncateToWidth(TypeText, text_size_weapon.x * scale);
                text_size_weapon = ImGui::CalcTextSize(TypeText.c_str());
            }
            VictimText = TruncateToWidth(VictimText, text_size_victim.x * scale);
            text_size_victim = ImGui::CalcTextSize(VictimText.c_str());

            text_total = text_size_attacker.x + text_size_weapon.x + text_size_victim.x;
        }

        float spacing = elements > 1 ? (total_width - padding * 2 - text_total) / (elements - 1) : 0.0f;
        spacing = std::max(spacing, MinSpacing);

        draw_list->AddRectFilled(pos, ImVec2(pos.x + total_width, pos.y + height), ImGui::GetColorU32(bg_color), style.FrameRounding);
        draw_list->AddRect(pos, ImVec2(pos.x + total_width, pos.y + height), ImGui::GetColorU32(border_color), style.FrameRounding);

        float cursor_x = pos.x + padding;
        float cursor_y = pos.y + padding;

        if (!AttackerText.empty()) {
            draw_list->AddText(ImVec2(cursor_x, cursor_y), ImGui::GetColorU32(attackerColor), AttackerText.c_str());
            cursor_x += text_size_attacker.x + spacing;
        }
        if (!TypeText.empty()) {
            draw_list->AddText(ImVec2(cursor_x, cursor_y), ImGui::GetColorU32(deathTypeColor), TypeText.c_str());
            cursor_x += text_size_weapon.x + spacing;
        }
        draw_list->AddText(ImVec2(cursor_x, cursor_y), ImGui::GetColorU32(victimColor), VictimText.c_str());

        // Move cursor to next line
        ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + height + style.ItemSpacing.y));

        GTS::ImFontManager::Pop();

    }

}