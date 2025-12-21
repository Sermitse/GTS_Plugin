#include "UI/Controls/KillEntry.hpp"

#include "UI/Core/ImColorUtils.hpp"
#include "UI/Core/ImFontManager.hpp"
#include "UI/Lib/imgui_internal.h"

namespace ImGuiEx {

    void DrawKillfeedEntry(std::unique_ptr<KillEntry>& entry, const KillFeedStyle& a_style) {

        GTS::ImFontManager::Push(GTS::ImFontManager::kWidgetBody, a_style.fontScale);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 avail = ImGui::GetContentRegionAvail(); // available width
        const ImGuiStyle& style = ImGui::GetStyle();
        float alpha = style.Alpha;

        // Calculate fade based on lifetime
        float fade = 1.0f;
        if (!a_style.neverFade){
	        if (entry->lifetime > a_style.visDuration - 1.0f) {
	            fade = (a_style.visDuration - entry->lifetime);
	        }
	        fade = ImClamp(fade, 0.0f, 1.0f);
		}

        std::string KilledBy;

        //if (!entry->type.empty()) {
        //    KilledBy = "[" + entry->type + "]";
        //}

    	KilledBy = entry->type;


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

        // Calculate text sizes
        ImVec2 text_size_attacker = entry->attacker.empty() ? ImVec2(0,0) : ImGui::CalcTextSize(entry->attacker.c_str());
        ImVec2 text_size_weapon = ImGui::CalcTextSize(KilledBy.c_str());
        ImVec2 text_size_victim = ImGui::CalcTextSize(entry->victim.c_str());

        // Draw text
        float cursor_x = pos.x + padding;
        float cursor_y = pos.y + padding;
        const bool showKiller = !(a_style.flags & KillFeedEntryFlag_NoKiller);
        const bool showKillType = !(a_style.flags & KillFeedEntryFlag_NoKillType);

        // Determine spacing
        int elements = 0;
        float text_total = 0.0f;

        if (showKiller && !entry->attacker.empty()) {
            elements++;
            text_total += text_size_attacker.x;
        }
        if (showKillType) {
            elements++;
            text_total += text_size_weapon.x;
        }
        elements++; // victim always shown
        text_total += text_size_victim.x;

        float spacing = elements > 1 ? (total_width - padding * 2 - text_total) / (elements - 1) : 0.0f;
        spacing = std::max(spacing, 4.0f); // avoid negative spacing

        // Draw background with border (framed child style)
        draw_list->AddRectFilled(pos, ImVec2(pos.x + total_width, pos.y + height), ImGui::GetColorU32(bg_color), style.FrameRounding);
        draw_list->AddRect(pos, ImVec2(pos.x + total_width, pos.y + height), ImGui::GetColorU32(border_color), style.FrameRounding);

        if (entry->attacker.empty() || !showKiller) {
            if (showKillType) {
                draw_list->AddText(ImVec2(cursor_x, cursor_y), ImGui::GetColorU32(deathTypeColor), KilledBy.c_str());
                cursor_x += text_size_weapon.x + spacing;
            }
            draw_list->AddText(ImVec2(cursor_x, cursor_y), ImGui::GetColorU32(victimColor), entry->victim.c_str());
        }
        else {
            draw_list->AddText(ImVec2(cursor_x, cursor_y), ImGui::GetColorU32(attackerColor), entry->attacker.c_str());
            cursor_x += text_size_attacker.x + spacing;
            if (showKillType) {
                draw_list->AddText(ImVec2(cursor_x, cursor_y), ImGui::GetColorU32(deathTypeColor), KilledBy.c_str());
                cursor_x += text_size_weapon.x + spacing;
            }
            draw_list->AddText(ImVec2(cursor_x, cursor_y), ImGui::GetColorU32(victimColor), entry->victim.c_str());
        }

        // Move cursor to next line
        ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + height + style.ItemSpacing.y));

        GTS::ImFontManager::Pop();

    }


}
