#pragma once

#include "UI/ImGui/Core/ImCategory.hpp"

namespace GTS {

 class ImCategoryManager {

        private:
        std::vector<std::shared_ptr<ImCategory>> m_categories;

        public:
        uint8_t m_activeIndex;
        ~ImCategoryManager() = default;

        [[nodiscard]] std::vector<std::shared_ptr<ImCategory>>& GetCategories();
        [[nodiscard]] float GetLongestCategory() const;

        void AddCategory(std::shared_ptr<ImCategory> category);

    };
}
