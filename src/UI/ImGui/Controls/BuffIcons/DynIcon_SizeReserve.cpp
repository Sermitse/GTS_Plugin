#include "UI/ImGui/Controls/BuffIcons/DynIcon_SizeReserve.hpp"

#include "UI/ImGui/Controls/Text.hpp"
#include "UI/ImGui/Core/ImColorUtils.hpp"
#include "UI/ImGui/Core/ImFontManager.hpp"
#include "UI/ImGui/Core/ImGraphics.hpp"
#include "UI/ImGui/Core/ImUtil.hpp"

namespace ImGuiEx {

	DynIconSizeReserve::DynIconSizeReserve(uint32_t a_size) : IDynIcon("placeholder", a_size) {
		m_transform = std::make_unique<GTS::ImGraphics::ImageTransform>();
		m_transform->cutoffDir = GTS::ImGraphics::CutoffDirection::BottomToTop;
		m_transform->recolorEnabled = true;
	}

	bool DynIconSizeReserve::Draw(float a_amount) const {

		if (a_amount <= 0) return false;

		GTS::ImFontManager::Push(GTS::ImFontManager::kIconText, m_size / 64.0f);
		{
			constexpr float overflow = 10.0f;
			m_transform->targetColor = a_amount > overflow ? ImUtil::Colors::fRGBToImVec4(GTS::Config::UI.f3IconOverflowColor) : ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
			m_transform->cutoffPercent = static_cast<float>(std::min(a_amount, overflow)) / static_cast<float>(overflow);

			const std::string txt = fmt::format("{:.2f}x", a_amount);

			GTS::ImGraphics::RenderTransformed(m_name, *m_transform.get(), { static_cast<float>(m_size), static_cast<float>(m_size) });
			const auto pos = ImUtil::GetCenteredTextPos(ImGui::GetItemRectMin(), ImGui::GetItemRectSize(), txt.c_str());
			ImGuiEx::TextShadowExNoDummy(pos, txt.c_str());

		}
		GTS::ImFontManager::Pop();

		return true;
	}
}
