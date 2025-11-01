#pragma once

#include "UI/ImGui/Core/ImWindow.hpp"

namespace GTS {

    class SizeBarWindow final : public ImConfigurableWindow<SizeBarWindow> {

		public:
        SizeBarWindow(const std::string& a_instanceName, const std::string& a_basePrefix = "UI") : ImConfigurableWindow(a_instanceName, a_basePrefix) {}

		private:
        void Draw() override;
        void Init() override;
        void RequestClose() override;
        bool WantsToDraw() override;
        virtual float GetBackgroundAlpha() override;
        void DeriveIdentityFromName();
        uint8_t m_identity = 0;
		WindowSettingsSizeBar_t m_extraSettings;
        float m_prevSize = 0.0f;
        const bool* m_isConfiguring = nullptr;
        const bool* m_settingsVisible = nullptr;
    };
}