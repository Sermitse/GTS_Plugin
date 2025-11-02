#pragma once
#include "UI/Lib/imgui.h"

namespace GTS {

	#define IM_VK_KEYPAD_ENTER (VK_RETURN + 256)

    class ImInput {

        private:
        class CharEvent final : public RE::InputEvent {
            public:
            uint32_t m_keyCode;
        };

        struct KeyEvent final {
            explicit KeyEvent(const RE::ButtonEvent* a_event) : m_keyCode(a_event->GetIDCode()), m_device(a_event->GetDevice()), m_eventType(a_event->GetEventType()), m_value(a_event->Value()), m_heldDownSecs(a_event->HeldDuration()) {}
            explicit KeyEvent(const CharEvent* a_event) : m_keyCode(a_event->m_keyCode), m_device(a_event->GetDevice()), m_eventType(a_event->GetEventType()) {}
            [[nodiscard]] constexpr bool IsPressed() const noexcept;
            [[nodiscard]] constexpr bool IsRepeating() const noexcept;
            [[nodiscard]] constexpr bool IsDown() const noexcept;
            [[nodiscard]] constexpr bool IsHeld() const noexcept;
            [[nodiscard]] constexpr bool IsUp() const noexcept;
            uint32_t m_keyCode;
            RE::INPUT_DEVICE m_device;
            RE::INPUT_EVENT_TYPE m_eventType;
            float m_value = 0;
            float m_heldDownSecs = 0;
        };

        // Input handling members
        std::shared_mutex m_inputMutex;
        std::vector<KeyEvent> m_keyEventQueue{};

        public:
        static void UnstickKeys();
        void ProcessInputEventQueue();
        void ResetImGuiInputState();
        void ProcessInputEvents(RE::InputEvent* const* a_events);
        static void RemoveAllKeyEvents(RE::InputEvent** a_event);
        static ImGuiKey VirtualKeyToImGuiKey(WPARAM a_wParam);
        static std::string ImGuiKeyToDIKString(ImGuiKey a_key);
        static uint32_t DIKToVK(uint32_t a_DIK);
        static void UpdateMousePos();

    };
}