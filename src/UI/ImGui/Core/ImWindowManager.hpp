#pragma once

#include "UI/ImGui/Core/ImContextManager.hpp"
#include "UI/ImGui/Core/ImFontManager.hpp"
#include "UI/ImGui/Core/ImStyleManager.hpp"
#include "UI/ImGui/Core/ImWindow.hpp"

namespace GTS {

    class ImWindowManager {

		private:
        std::vector<std::unique_ptr<ImWindow>> Windows;
        ImStyleManager* Style = nullptr;
        ImContextManager* Context = nullptr;

        float m_lastFrameTime = 0.0f;

        public:
        [[nodiscard]] ImWindow* GetWindowByName(const std::string& a_name) const;
        [[nodiscard]] bool HasWindows() const;
        [[nodiscard]] bool HasInputConsumers();
        [[nodiscard]] bool HasActiveWindows();
        [[nodiscard]] ImWindow::WindowType GetHighestVisibleWindowType() const;
        float GetDeltaTime() const;

        void AddWindow(std::unique_ptr<ImWindow> a_window);
        void Update() const;
        void Init();

        void ApplyStyle() const;
        int8_t GetDesiredPriority() const;
        bool GetDesiredCursorState() const;
        static void PopFont();

    };

}