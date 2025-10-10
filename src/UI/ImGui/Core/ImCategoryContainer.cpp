#include "UI/ImGui/Core/ImCategoryContainer.hpp"
#include "UI/ImGui/ImUtil.hpp"
#include "UI/ImGui/Lib/imgui.h"

namespace GTS {

	std::vector<std::shared_ptr<ImCategory>>& ImCategoryContainer::GetCategories() {
        return m_categories;
    }

    void ImCategoryContainer::AddCategory(std::shared_ptr<ImCategory> category) {
        m_categories.push_back(std::move(category));
    }

    float ImCategoryContainer::GetLongestCategory() const {

        if (!ImUtil::ValidState()) return -1.0f;

        float longest = 0.0f;
        const float scale = ImGui::GetStyle().FontScaleMain;
        const float paddingX = (ImGui::GetStyle().FramePadding.x + 64.f) * scale;

        for (auto& category : m_categories) {
            if (!category.get()->IsVisible()) continue;
            auto len = ImGui::CalcTextSize(category.get()->GetTitle().c_str());
            longest = std::max(len.x + paddingX, longest);
        }

        return longest;
    }
}
