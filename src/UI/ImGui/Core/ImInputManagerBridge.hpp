#pragma once

#include <functional>
#include <string>
#include <unordered_map>

namespace GTS {

    struct ManagedInputEvent;

    class ImWindowInputBridge {

        private:
        using CallbackFunc = std::function<void(const ManagedInputEvent&)>;

        struct CallbackEntry {
            std::string windowName;
            CallbackFunc callback;
        };

        static inline std::unordered_map<std::string, CallbackEntry> s_callbacks;

        public:
        static void RegisterCallback(const std::string& windowName, const std::string& eventName, CallbackFunc callback) {
            s_callbacks[eventName] = { windowName, std::move(callback) };
        }

        static void UnregisterCallback(const std::string& windowName, const std::string& eventName) {
            auto it = s_callbacks.find(eventName);
            if (it != s_callbacks.end() && it->second.windowName == windowName) {
                s_callbacks.erase(it);
            }
        }

        static void DispatchEvent(const std::string& eventName, const ManagedInputEvent& data) {
            auto it = s_callbacks.find(eventName);
            if (it != s_callbacks.end()) {
                it->second.callback(data);
            }
        }

        static void Clear() {
            s_callbacks.clear();
        }
    };

}