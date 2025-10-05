#pragma once

namespace GTS {
    
    class ImCategory {
        protected:
            
        //Fields
        std::string title = "Default";

        bool visible = true;

        virtual ~ImCategory() = default;

        public:

        //Virtuals
        virtual void Draw() = 0;
        
        //Methods
        [[nodiscard]] const inline std::string& GetTitle(){
            return title;
        }

        [[nodiscard]] inline bool IsVisible() const {
            return visible;
        }

        inline void SetVisible(const bool a_visible){
            visible = a_visible;
        }

    };
}