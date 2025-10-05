#pragma once

namespace GTS {

    class ImWindow;

    class ImContextManager {

        public:
        enum class ContextMode {
            kNone,
            kHUD,
            kSettings,
            kDebug,
            kMainMenu
        };

        struct ContextConfig {
            int depthPriority;
            bool cursorEnabled;
        };

        private:
        ContextMode m_currentMode = ContextMode::kNone;
        ContextMode m_requestedMode = ContextMode::kNone;
        volatile bool m_pendingSwitch = false;

        static constexpr ContextConfig GetConfigForMode(ContextMode mode);
        static ContextMode GetModeForWindowType(int windowType);

        public:
        void RequestSwitch(ContextMode mode);
        void RequestSwitchForWindowType(int windowType);
        bool HasPendingSwitch() const;

        ContextConfig ApplyPendingSwitch();
        ContextMode GetCurrentMode() const;
        ContextConfig GetCurrentConfig() const;
    };

}
