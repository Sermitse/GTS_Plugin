#include "ImUtil.hpp"

#include "Config/Config.hpp"

#include "UI/GTSMenu.hpp"

namespace ImUtil {
	//------------------------------------
    //  Modified/Extended Imgui Controls
    //------------------------------------

    const bool Button(const char* a_label, const char* a_Tooltip, const bool a_disabled, const float a_padding){
        
        const float paddingX = ImGui::GetStyle().FramePadding.x;
        const float paddingY = ImGui::GetStyle().FramePadding.y;
        const ImVec2 textSize = ImGui::CalcTextSize(a_label);
        const ImVec2 buttonSize = ImVec2(textSize.x + a_padding * (paddingX * 2), textSize.y + a_padding * (paddingY * 2));

        ImGui::BeginDisabled(a_disabled);
        const bool res = ImGui::Button(a_label, buttonSize);
        Tooltip(a_Tooltip);

        ImGui::EndDisabled();
        return res;
    }

    const bool ImageButtonEx(const char* a_label, ImTextureID a_texture , const char* a_Tooltip, const bool a_disabled, ImVec2 a_imgSize) {

        ImGui::BeginDisabled(a_disabled);

        const bool res = ImGui::ImageButton(a_label, a_texture, a_imgSize);
        Tooltip(a_Tooltip);

        ImGui::EndDisabled();
        return res;
    }

    const bool ImageButton(const char* a_label, const std::string& a_TexName, const int a_size, const char* a_Tooltip, const bool a_disabled) {

        const float size = a_size * ImGui::GetStyle().FontScaleMain;
        const auto [texID, _] = GTS::GTSMenu::Graphics->GetAsImGuiTexture(a_TexName);
        const bool res = ImUtil::ImageButtonEx(a_label, texID, a_Tooltip, false, { size, size });
        Tooltip(a_Tooltip);

        return res;
    }

    const bool CheckBox(const char* a_label, bool* a_state, const char* a_Tooltip, const bool a_disabled){
        ImGui::BeginDisabled(a_disabled);
        const bool res = ImGui::Checkbox(a_label, a_state);
        Tooltip(a_Tooltip);

        ImGui::EndDisabled();
        return res;
    }

    const bool SliderF(const char* a_label, float* a_value, float a_min, float a_max, const char* a_Tooltip, const char* fmt, const bool a_disabled, const bool a_alwaysclamp){
        ImGui::BeginDisabled(a_disabled);

        auto flags = GTS::Config::Advanced.bEnforceUIClamps ? ImGuiSliderFlags_AlwaysClamp : 0;

    	if (GTS::Config::Advanced.bEnforceUIClamps){}

        if(a_alwaysclamp && flags){
	        *a_value = std::min(*a_value, a_max);
	        *a_value = std::max(*a_value, a_min);
        }

        const bool res = ImGui::SliderFloat(a_label, a_value, a_min, a_max, fmt, flags);

        Tooltip(a_Tooltip);

        ImGui::EndDisabled();
        return res;
    }

    const bool SliderF3(const char* a_label, float* a_value, float a_min, float a_max, const char* a_Tooltip, const char* fmt, const bool a_disabled){
        ImGui::BeginDisabled(a_disabled);

        auto flags = GTS::Config::Advanced.bEnforceUIClamps ? ImGuiSliderFlags_AlwaysClamp : 0;

        const bool res = ImGui::SliderFloat3(a_label, a_value, a_min, a_max, fmt, flags);

        Tooltip(a_Tooltip);

        ImGui::EndDisabled();
        return res;
    }

    const bool SliderF2(const char* a_label, float* a_value, float a_min, float a_max, const char* a_Tooltip, const char* fmt, const bool a_disabled){
        ImGui::BeginDisabled(a_disabled);

        auto flags = GTS::Config::Advanced.bEnforceUIClamps ? ImGuiSliderFlags_AlwaysClamp : 0;
        const bool res = ImGui::SliderFloat2(a_label, a_value, a_min, a_max, fmt, flags);

        Tooltip(a_Tooltip);

        ImGui::EndDisabled();
        return res;
    }



    // Helper to display a little (?) mark which shows a tooltip when hovered.
    void HelpMarker(const char* a_desc) {

        ImGui::Text("[?]");
        if (ImGui::BeginItemTooltip()){

            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(a_desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    const bool ConditionalHeader(const std::string a_label, const std::string a_ConditionText, const bool a_condition, const bool a_defaultopen){
        ImGui::BeginDisabled(!a_condition);
        auto flags = ImGuiTreeNodeFlags_None | (a_condition && a_defaultopen ? ImGuiTreeNodeFlags_DefaultOpen : 0);
        
        //TODO Optimize this POS;
        const std::string _FullText = (a_label + (a_condition ? "##" : (" [" + a_ConditionText + "]")));
        
        bool Res = ImGui::CollapsingHeader(_FullText.c_str(),flags);

        ImGui::EndDisabled();

        return Res;
    }

    //------------------------------------
    //  Misc
    //------------------------------------

    const std::string HumanizeString(std::string_view name) {
        if (name.empty()) {
            return {};
        }
        
        std::string result;
        
        // Remove a leading 'k', if present.
        if (name.front() == 'k') {
            name.remove_prefix(1);
        }
        
        // Process each character by index.
        for (size_t i = 0; i < name.size(); ++i) {
            char c = name[i];
            
            if (c == '_') {
                // Replace underscore with a space (avoiding duplicate spaces).
                if (result.empty() || result.back() != ' ')
                    result += ' ';
                continue;
            }
            
            // For uppercase letters (except at the very start), add a space if the previous character
            // was NOT uppercase. This prevents adding spaces between sequential uppercase letters.
            if (i > 0 && std::isupper(static_cast<unsigned char>(c)) &&
                !std::isupper(static_cast<unsigned char>(name[i - 1]))) {
                if (result.empty() || result.back() != ' ')
                    result += ' ';
            }
            
            result += c;
        }
        
        // Trim leading and trailing spaces.
        size_t start = result.find_first_not_of(' ');
        if (start == std::string::npos) {
            return "";
        }
        size_t end = result.find_last_not_of(' ');
        result = result.substr(start, end - start + 1);
        
        // Collapse any consecutive spaces (if any)
        std::string final_result;
        bool prev_space = false;
        for (char ch : result) {
            if (ch == ' ') {
                if (!prev_space) {
                    final_result += ' ';
                    prev_space = true;
                }
            } else {
                final_result += ch;
                prev_space = false;
            }
        }
        
        return final_result;
    }

    void CenteredProgress(float fraction, const ImVec2& size_arg, const char* overlay, const float heightmult,
        float borderThickness, bool useGradient,
        float gradientDarkFactor, float gradientLightFactor,
        bool useRounding,
        bool useCustomGradientColors,
        ImU32 gradientStartColor,
        ImU32 gradientEndColor,
        bool flipGradientDirection) {

        const ImGuiWindow* window = ImGui::GetCurrentWindow();

        if (window->SkipItems) {
            return;
        }
        const ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        // Calculate progress bar dimensions
        ImVec2 pos = window->DC.CursorPos;
        const auto TextSize = ImGui::CalcTextSize(overlay);
        const auto ItemWidth = ImGui::CalcItemWidth();
        const float Width = ItemWidth > TextSize.x ? ItemWidth : TextSize.x;
        const ImVec2 ResultSize = { Width, TextSize.y };
        const ImVec2 size = ImGui::CalcItemSize(size_arg, ResultSize.x, ResultSize.y + style.FramePadding.y * 2.0f * heightmult);
        const ImVec2 possize = { pos.x + size.x, pos.y + size.y };
        const ImRect bb(pos, possize);

        // Register the item and handle clipping
        ImGui::ItemSize(size, style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, 0)) {
            return;
        }

        // Get the colors
        const ImU32 border_color = ImGui::GetColorU32(ImGuiCol_Border);
        const ImU32 bg_color = ImGui::GetColorU32(ImGuiCol_FrameBg);
        ImU32 fill_color = gradientStartColor;

        float frame_rounding = useRounding ? style.FrameRounding : 0.0f;

        // Create inner rectangle accounting for border thickness
        ImRect inner_bb(
            ImVec2(bb.Min.x + borderThickness, bb.Min.y + borderThickness),
            ImVec2(bb.Max.x - borderThickness, bb.Max.y - borderThickness)
        );

        // Render border
        if (borderThickness > 0.0f) {
            window->DrawList->AddRect(bb.Min, bb.Max, border_color, frame_rounding, ImDrawFlags_None, borderThickness);
        }

        // Render background (inner rectangle)
        window->DrawList->AddRectFilled(inner_bb.Min, inner_bb.Max, bg_color, frame_rounding);

        // Render filled portion with adjusted size for border
        if (fraction > 0.0f) {
            const float fill_width = ImMax((inner_bb.Max.x - inner_bb.Min.x) * fraction, 2.0f);
            ImRect fill_bb(
                inner_bb.Min,
                ImVec2(inner_bb.Min.x + fill_width, inner_bb.Max.y)
            );

            if (useGradient) {
            	ImU32 startColor, endColor;

                if (useCustomGradientColors) {
                    // Apply global alpha to custom colors
                    ImVec4 startColorVec4 = ImGui::ColorConvertU32ToFloat4(gradientStartColor);
                    ImVec4 endColorVec4 = ImGui::ColorConvertU32ToFloat4(gradientEndColor);

                    // Modify the alpha component while preserving the original RGB
                    startColorVec4.w *= style.Alpha;
                    endColorVec4.w *= style.Alpha;

                    // Convert back to ImU32
                    startColor = ImGui::ColorConvertFloat4ToU32(startColorVec4);
                    endColor = ImGui::ColorConvertFloat4ToU32(endColorVec4);
                }
                else {
                    ImVec4 baseColor = ImGui::ColorConvertU32ToFloat4(fill_color);
                    baseColor.w *= style.Alpha;

                    ImVec4 darkColor = ImVec4(
                        ImClamp(baseColor.x * gradientDarkFactor, 0.0f, 1.0f),
                        ImClamp(baseColor.y * gradientDarkFactor, 0.0f, 1.0f),
                        ImClamp(baseColor.z * gradientDarkFactor, 0.0f, 1.0f),
                        baseColor.w
                    );

                    ImVec4 lightColor = ImVec4(
                        ImClamp(baseColor.x * gradientLightFactor, 0.0f, 1.0f),
                        ImClamp(baseColor.y * gradientLightFactor, 0.0f, 1.0f),
                        ImClamp(baseColor.z * gradientLightFactor, 0.0f, 1.0f),
                        baseColor.w
                    );

                    startColor = ImGui::ColorConvertFloat4ToU32(darkColor);
                    endColor = ImGui::ColorConvertFloat4ToU32(lightColor);
                }

                if (flipGradientDirection) {
                    const ImU32 temp = startColor;
                    startColor = endColor;
                    endColor = temp;
                }

                // Draw gradient fill
                window->DrawList->AddRectFilledMultiColor(
                    fill_bb.Min,
                    fill_bb.Max,
                    startColor,     // Left color
                    endColor,       // Right color
                    endColor,       // Bottom right color
                    startColor      // Bottom left color
                );
            }
            else {
                // Draw regular solid fill
                window->DrawList->AddRectFilled(fill_bb.Min, fill_bb.Max, fill_color, frame_rounding);
            }
        }

        // Render centered text
        if (overlay != nullptr) {
            ImVec2 overlay_size = ImGui::CalcTextSize(overlay);
            ImVec2 text_pos = ImVec2(
                bb.Min.x + (size.x - overlay_size.x) * 0.5f,
                bb.Min.y + (size.y - overlay_size.y) * 0.5f
            );

            // Draw text with contrasting shadow
            window->DrawList->AddText(
                ImVec2(text_pos.x + 1, text_pos.y + 1),
                IM_COL32(0, 0, 0, 128 * style.Alpha),
                overlay
            );
            window->DrawList->AddText(text_pos, ImGui::GetColorU32(ImGuiCol_Text), overlay);
        }
    }

    bool ContainsString(const std::string& a1, const std::string& a2) {
        auto to_lower = [](unsigned char c) { return std::tolower(c); };

        auto a1_view = a1 | std::views::transform(to_lower);
        auto a2_view = a2 | std::views::transform(to_lower);
    
        auto result = std::ranges::search(a1_view, a2_view);
    
        return result.begin() != a1_view.end();
    }


    void TextShadow(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        const char* text, * text_end;
        ImFormatStringToTempBufferV(&text, &text_end, fmt, args);
        va_end(args);
        TextShadowImpl(text, text_end);

    }

    void TextShadowImpl(const char* text, const char* textend, ImU32 text_color, ImU32 shadow_color, float shadow_offset) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos(); // Get the absolute screen position

        // Draw shadow text
        draw_list->AddText(ImVec2(pos.x + shadow_offset, pos.y + shadow_offset), shadow_color, text, textend);
        // Draw main text
        draw_list->AddText(pos, text_color, text, textend);

        // Calculate the text size
        ImVec2 textSize = ImGui::CalcTextSize(text, textend, false, 0.0f);
        // Move the cursor by adding an invisible dummy widget of the same size as the text
        ImGui::Dummy(textSize);
    }

	/*
	 * This function assumes the existence of an active Dear ImGui window
	 */

	//Taken from https://github.com/ocornut/imgui/issues/1329
    void RenderDropShadow(ImTextureID tex_id, float size, ImU8 opacity) {
        ImVec2 p = ImGui::GetWindowPos();
        ImVec2 s = ImGui::GetWindowSize();
        ImVec2 m = { p.x + s.x, p.y + s.y };
        float uv0 = 0.0f;      // left/top region
        float uv1 = 0.333333f; // leftward/upper region
        float uv2 = 0.666666f; // rightward/lower region
        float uv3 = 1.0f;      // right/bottom region
        ImU32 col = (opacity << 24) | 0xFFFFFF;
        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->PushClipRectFullScreen();
        dl->AddImage(tex_id, { p.x - size, p.y - size }, { p.x,        p.y }, { uv0, uv0 }, { uv1, uv1 }, col);
        dl->AddImage(tex_id, { p.x,        p.y - size }, { m.x,        p.y }, { uv1, uv0 }, { uv2, uv1 }, col);
        dl->AddImage(tex_id, { m.x,        p.y - size }, { m.x + size, p.y }, { uv2, uv0 }, { uv3, uv1 }, col);
        dl->AddImage(tex_id, { p.x - size, p.y }, { p.x,        m.y }, { uv0, uv1 }, { uv1, uv2 }, col);
        dl->AddImage(tex_id, { m.x,        p.y }, { m.x + size, m.y }, { uv2, uv1 }, { uv3, uv2 }, col);
        dl->AddImage(tex_id, { p.x - size, m.y }, { p.x,        m.y + size }, { uv0, uv2 }, { uv1, uv3 }, col);
        dl->AddImage(tex_id, { p.x,        m.y }, { m.x,        m.y + size }, { uv1, uv2 }, { uv2, uv3 }, col);
        dl->AddImage(tex_id, { m.x,        m.y }, { m.x + size, m.y + size }, { uv2, uv2 }, { uv3, uv3 }, col);
        dl->PopClipRect();
    }
}
