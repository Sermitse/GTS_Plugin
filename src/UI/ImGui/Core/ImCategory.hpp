#pragma once

namespace GTS {
    
    class ImCategory {
        protected:
        std::string m_name = "Default";
        bool m_visible = true; //Visible by default

        public:
        virtual void Draw() = 0;
        [[nodiscard]] virtual bool IsVisible() const;
        virtual void SetVisible(const bool a_visible);
        [[nodiscard]] const std::string& GetTitle();
        virtual ~ImCategory() = default;
    };
}