#pragma once
#include "UI/ImGui/ImWindow.hpp"

namespace GTS {

    struct IWindowConfigBase {
        virtual ~IWindowConfigBase() = default;
        virtual IWindowConfigBase& Base() = 0;
    };

    template<typename TExtra = void>
    struct WindowConfig : public IWindowConfigBase {
        WindowSettings base;
        TExtra extra;

        WindowSettings& Base() override {
            return base;
        }
    };

    template<typename TDerived, typename TExtra = void>
    class ImWindowConfig: public ImWindow {
        public:
        ImWindowConfig() {
            config = std::make_unique<WindowConfig<TExtra>>();
            WindowManager::Get().RegisterWindow(this);
        }

        WindowConfig<TExtra>& GetTypedConfig() {
            return *static_cast<WindowConfig<TExtra>*>(config.get());
        }
    };

}