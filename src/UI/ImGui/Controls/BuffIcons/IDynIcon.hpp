#pragma once
#include "UI/ImGui/Core/ImGraphics.hpp"

namespace ImGuiEx {

	class IDynIcon {
		public:

		IDynIcon(const std::string& a_name, uint32_t a_size) :
			m_name(a_name), m_size(a_size) {
		}

		virtual ~IDynIcon() = default;
		//virtual void Draw(std::any a_var) = 0;

		std::unique_ptr<GTS::ImGraphics::ImageTransform> m_transform = nullptr;
		const std::string m_name = {};
		uint32_t m_size = 32;
	};

}
