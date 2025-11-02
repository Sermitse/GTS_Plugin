#include "UI/Core/ImCategory.hpp"

namespace GTS {

	const std::string& ImCategory::GetTitle() {
		return m_name;
	}

	bool ImCategory::IsVisible() const {
		return m_visible;
	}

	void ImCategory::SetVisible(const bool a_visible) {
		m_visible = a_visible;
	}
}
