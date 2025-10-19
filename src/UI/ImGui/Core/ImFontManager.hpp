#pragma once

#include "UI/ImGui/Lib/imgui.h"
#include "Config/Config.hpp"

namespace GTS {

    class ImFontManager {

        //Consts
        static inline const std::string _ext = ".ttf";
        static inline const std::string _basePath = R"(Data\SKSE\Plugins\GTSPlugin\Fonts\)";

        static inline const std::string g_Noto_Light = _basePath + R"(Noto\EN\NotoSans-Light)" + _ext;
        static inline const std::string g_Noto_Medium = _basePath + R"(Noto\EN\NotoSans-Medium)" + _ext;
        static inline const std::string g_Noto_Regular = _basePath + R"(Noto\EN\NotoSans-Regular)" + _ext;

        static inline const std::string g_Noto_Light_JP = _basePath + R"(Noto\JP\NotoSans-Light)" + _ext;
        static inline const std::string g_Noto_Medium_JP = _basePath + R"(Noto\JP\NotoSans-Medium)" + _ext;
        static inline const std::string g_Noto_Regular_JP = _basePath + R"(Noto\JP\NotoSans-Regular)" + _ext;

        static inline const std::string g_Noto_Light_KR = _basePath + R"(Noto\KR\NotoSans-Light)" + _ext;
        static inline const std::string g_Noto_Medium_KR = _basePath + R"(Noto\KR\NotoSans-Medium)" + _ext;
        static inline const std::string g_Noto_Regular_KR = _basePath + R"(Noto\KR\NotoSans-Regular)" + _ext;

        static inline const std::string g_Noto_Light_SC = _basePath + R"(Noto\SC\NotoSans-Light)" + _ext;
        static inline const std::string g_Noto_Medium_SC = _basePath + R"(Noto\SC\NotoSans-Medium)" + _ext;
        static inline const std::string g_Noto_Regular_SC = _basePath + R"(Noto\SC\NotoSans-Regular)" + _ext;

        const SettingsUI_t& Settings = Config::UI;

		public:

        enum ActiveFontType {
            kDefault,
            kSidebar,        //{ kSidebar,     TextType(&FontSet_Regular, 28.0f) },
            kTitle,          //{ kTitle,       TextType(&FontSet_Medium,  48.0f) },
            kFooter,         //{ kFooter,      TextType(&FontSet_Medium,  28.0f) },
            kText,           //{ kText,        TextType(&FontSet_Regular, 18.0f) },
            kLargeText,      //{ kLargeText,   TextType(&FontSet_Regular, 22.0f) },
            kSubText,        //{ kSubText,     TextType(&FontSet_Regular, 16.0f) },
            kWidgetBody,     //{ kWidgetBody,  TextType(&FontSet_Regular, 20.0f) },
            kWidgetTitle     //{ kWidgetTitle, TextType(&FontSet_Light,   32.0f) },
        };

        typedef struct Font2 {

            ImFont* EN = nullptr;
            ImFont* JP = nullptr;
            ImFont* KR = nullptr;
            ImFont* SC = nullptr;

            ImFontConfig* Config = new ImFontConfig();

            Font2(const char* a_Name, const std::array<std::string, 4>& a_paths) {

                // Check if all font files exist
                for (size_t i = 0; i < a_paths.size() - 1; i++) {
                    if (!std::filesystem::exists(a_paths[i])) {
                        ReportAndExit("Required font file not found: " + a_paths[i] + "\nReinstall the mod.\nPress OK to close the game.");
                    }
                }

                ImFontAtlas* const Atlas = ImGui::GetIO().Fonts;
                memcpy(&Config->Name, a_Name, strlen(a_Name));

                Config->OversampleH = 4;
                Config->OversampleV = 4;

                //Base Font
                EN = Atlas->AddFontFromFileTTF(a_paths[0].data(), 0.0f, Config);

                //Has to be set after atleast 1 font has been added fist.
                //Merge Glyphs From Other Langs Into Base Font.
                Config->MergeMode = true;
                JP = Atlas->AddFontFromFileTTF(a_paths[1].data(), 0.0f, Config);
                KR = Atlas->AddFontFromFileTTF(a_paths[2].data(), 0.0f, Config);
                SC = Atlas->AddFontFromFileTTF(a_paths[3].data(), 0.0f, Config);

            }

        } Font2;

        struct TextType {
            Font2* FontSet;
            float Scale;
            TextType(Font2* FontSet, float Scale) : FontSet(FontSet), Scale(Scale) {}
        };

        ~ImFontManager() = default;

        static void Init();
        static void Push(ActiveFontType a_ActiveFontType);
        static void Pop();
        static void Pop(size_t a_amt);

    private:
        static inline std::unordered_map<ActiveFontType, TextType> TextTypeMap {};
        static inline std::atomic_bool m_init = false;
    };
}