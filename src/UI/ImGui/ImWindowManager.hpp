#pragma once

#include "UI/ImGui/ImWindow.hpp"
#include "UI/ImGui/ImFontManager.hpp"
#include "UI/ImGui/ImStyleManager.hpp"

namespace GTS {

    class ImWindowManager {

        private:
        std::vector<std::unique_ptr<IImWindow>> windows;
        const std::string wmName = "GTSPluginWM";

        ImFontManager& FontMgr = ImFontManager::GetSingleton();
        ImStyleManager& StyleMgr = ImStyleManager::GetSingleton(); 

        ~ImWindowManager() = default;

        public:

        bool ShowMetrics = false;
        bool ShowStack = false;
        bool ShowDemoWindow = false;

        [[nodiscard]] static inline ImWindowManager& GetSingleton() {
            static ImWindowManager instance;
            return instance;
        }

        [[nodiscard]] IImWindow* GetWindowByName(const std::string& a_name) const;
        
        [[nodiscard]] inline bool HasWindows() const {
            return !windows.empty();
        }

        [[nodiscard]] inline bool HasInputConsumers() {
            return std::ranges::any_of(windows, [](const auto& window) {
                return window->Show && window->ConsumeInput;
            });
        }

        [[nodiscard]] inline bool HasActiveWindows() {
            return std::ranges::any_of(windows, [](const auto& window) {
                return window->Show;
            });
        }

        void AddWindow(std::unique_ptr<IImWindow> a_window);
        void Update();

        private:
        ImWindowManager() = default;
        ImWindowManager(const ImWindowManager&) = delete;
        ImWindowManager& operator=(const ImWindowManager&) = delete;
    };

}