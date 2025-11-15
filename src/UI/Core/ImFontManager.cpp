#include "UI/Core/ImFontManager.hpp"
#include "UI/Lib/imgui_impl_dx11.h"

namespace GTS {

    void ImFontManager::Init() {

        if (m_init.exchange(true)) {
           return;
		}
	
        static Font2 FontSet_Regular = { "NotoRegular", { g_Noto_Regular, g_Noto_Regular_JP, g_Noto_Regular_KR, g_Noto_Regular_SC  }};
        static Font2 FontSet_Light =   { "NotoLight",   { g_Noto_Light,   g_Noto_Light_JP,   g_Noto_Light_KR,   g_Noto_Light_SC    }};
        static Font2 FontSet_Medium =  { "NotoMedium",  { g_Noto_Medium,  g_Noto_Medium_JP,  g_Noto_Medium_KR,  g_Noto_Medium_SC   }};

    	TextTypeMap = {
            { kText,        TextType(&FontSet_Regular, 18.0f) },
		    { kTitle,       TextType(&FontSet_Medium,  48.0f) },
		    { kFooter,      TextType(&FontSet_Medium,  28.0f) },
		    { kSubText,     TextType(&FontSet_Regular, 16.0f) },
		    { kSidebar,     TextType(&FontSet_Regular, 28.0f) },
		    { kLargeText,   TextType(&FontSet_Regular, 22.0f) },
		    { kWidgetBody,  TextType(&FontSet_Medium,  18.0f) },
		    { kWidgetTitle, TextType(&FontSet_Light,   32.0f) },
            { kIconText,    TextType(&FontSet_Medium,  16.0f) },
        };

    }

    void ImFontManager::Push(ActiveFontType a_ActiveFontType, float a_scaleOverride_mult) {
    	const auto& Type = TextTypeMap.find(a_ActiveFontType)->second;
        ImGui::PushFont(Type.FontSet->EN, Type.Scale * a_scaleOverride_mult);
    }

    void ImFontManager::Pop() {
        ImGui::PopFont();
    }

    void ImFontManager::Pop(size_t a_amt) {
        if (a_amt == 0) return;

        while (a_amt-- > 0) {
            ImGui::PopFont();
        }
    }
}
