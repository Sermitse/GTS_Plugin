#pragma once

#include "UI/Core/ImGraphics.hpp"

namespace ImGuiEx {

	class IDynIcon {
		public:

		IDynIcon(const std::string& a_name, uint32_t a_size) :
			m_name(a_name), m_size(a_size) {
		}

		virtual ~IDynIcon() = default;

		std::unique_ptr<GTS::ImGraphics::ImageTransform> m_transform = nullptr;
		const std::string m_name = {};
		static constexpr uint32_t m_referenceSize = 64;
		uint32_t m_size = 32;

		void Resize(uint32_t a_size) {
			m_size = a_size;
		}

	};

}
