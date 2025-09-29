#pragma once

#include "UI/ImGui/ImFontManager.hpp"
#include "UI/ImGui/ImStyleManager.hpp"
#include "UI/ImGui/ImWindow.hpp"

namespace GTS {

    class ImWindowManager {

		private:
        std::vector<std::unique_ptr<ImWindow>> m_windows;
        ImFontManager* FontManager = nullptr;
        ImStyleManager* StyleManager = nullptr;

        public:
        [[nodiscard]] ImWindow* GetWindowByName(const std::string& a_name) const;
        [[nodiscard]] bool HasWindows() const;
        [[nodiscard]] bool HasInputConsumers();
        [[nodiscard]] bool HasActiveWindows();
        [[nodiscard]] ImWindow::DrawLevel GetHighestDrawLevel() const;

        void AddWindow(std::unique_ptr<ImWindow> a_window);
        void AddWindow(std::unique_ptr<ImConfigurableWindow<ImWindow>> a_window);
        void Update() const;
        void Init();

        void ApplyStyle() const;
        void SetFont(ImFontManager::ActiveFontType a_fontType) const;
        static void PopFont();

    };

}