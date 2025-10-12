#pragma once

#include "UI/ImGui/Core/ImContextManager.hpp"
#include "UI/ImGui/Core/ImWindow.hpp"

namespace GTS {

    class ImWindowManager {

		private:
        std::vector<std::unique_ptr<ImWindow>> Windows;
        ImContextManager* Context = nullptr;

        float m_lastFrameTime = 0.0f;

        public:
		// Accessors for common windows
        ImWindow* wSplash = nullptr;
        ImWindow* wSettings = nullptr;
        ImWindow* wActionMenu = nullptr;
        ImWindow* wQuestWidget = nullptr;
        ImWindow* wPlayerWidget = nullptr;

        //TODO Follower Ones Will be runtime created for each follower
        //If that wont work out then just make a fixed amount of them

        [[nodiscard]] ImWindow* GetWindowByName(const std::string& a_name) const;
        [[nodiscard]] bool HasWindows() const;
        [[nodiscard]] bool HasInputConsumers();
        [[nodiscard]] bool HasActiveWindows();
        void CloseInputConsumers() const;
        [[nodiscard]] ImWindow::WindowType GetHighestVisibleWindowType() const;
        float GetDeltaTime() const;

        void AddWindow(std::unique_ptr<ImWindow> a_window, ImWindow** a_accessor = nullptr);
        void Update() const;
        void Init();
        int8_t GetDesiredPriority() const;
        bool GetDesiredCursorState() const;

    };

}