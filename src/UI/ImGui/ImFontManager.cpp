#include "UI/ImGui/ImFontManager.hpp"
#include "UI/ImGui/Lib/imgui_impl_dx11.h"

namespace GTS {

    void ImFontManager::Init() {

        static Font2 FontSet_Regular = { "NotoRegular", { g_Noto_Regular , g_Noto_Regular_JP, g_Noto_Regular_KR, g_Noto_Regular_SC } };
        static Font2 FontSet_Light = { "NotoLight", { g_Noto_Light , g_Noto_Light_JP, g_Noto_Light_KR, g_Noto_Light_SC } };
        static Font2 FontSet_Medium = { "NotoMedium", { g_Noto_Medium , g_Noto_Medium_JP, g_Noto_Medium_KR, g_Noto_Medium_SC } };

    	TextTypeMap = {
		    {ActiveFontType::kText,TextType(&FontSet_Regular, 18.0f)},
		    {ActiveFontType::kTitle,TextType(&FontSet_Medium, 48.0f)},
		    {ActiveFontType::kFooter,TextType(&FontSet_Medium, 28.0f)},
		    {ActiveFontType::kSubText,TextType(&FontSet_Regular, 16.0f)},
		    {ActiveFontType::kSidebar,TextType(&FontSet_Regular, 28.0f)},
		    {ActiveFontType::kError,TextType(&FontSet_Regular, 22.0f)},
		    {ActiveFontType::kWidgetBody,TextType(&FontSet_Regular, 20.0f)},
		    {ActiveFontType::kWidgetTitle,TextType(&FontSet_Light, 32.0f)},
        };

    }

    void ImFontManager::PushActiveFont(ActiveFontType a_ActiveFontType) {
        const auto& FontMgr = GetSingleton();
    	const auto& Type = FontMgr.TextTypeMap.find(a_ActiveFontType)->second;
        ImGui::PushFont(Type.FontSet->GetActiveScript(), Type.Scale);
    }

    void ImFontManager::PopActiveFont() {
        ImGui::PopFont();
    }

}
