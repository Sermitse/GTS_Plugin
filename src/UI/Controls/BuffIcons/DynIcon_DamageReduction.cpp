#include "UI/Controls/BuffIcons/DynIcon_DamageReduction.hpp"

#include "UI/Controls/Text.hpp"
#include "UI/Core/ImColorUtils.hpp"
#include "UI/Core/ImFontManager.hpp"
#include "UI/Core/ImUtil.hpp"

namespace ImGuiEx {

	DynIconDamageReduction::DynIconDamageReduction(uint32_t a_size) : IDynIcon(ImageList::BuffIcon_DamageReduction, a_size) {
		m_transform = std::make_unique<GTS::ImGraphics::ImageTransform>();
		m_transform->cutoffDir = GTS::ImGraphics::CutoffDirection::BottomToTop;
		m_transform->recolorEnabled = true;
	}

	bool DynIconDamageReduction::Draw(float a_percent) const {

		if (a_percent - 1.0f <= 0.f) return false;

		GTS::ImFontManager::Push(GTS::ImFontManager::kIconText, m_size / 64.0f);
		{
			constexpr float overflow = 90.f;
			m_transform->targetColor = a_percent > overflow ? ImUtil::Colors::fRGBToImVec4(GTS::Config::UI.f3IconOverflowColor) : ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
			m_transform->cutoffPercent = static_cast<float>(std::min(a_percent, overflow)) / static_cast<float>(overflow);

			const std::string txt = fmt::format("{:.1f}%", a_percent);
			GTS::ImGraphics::RenderTransformed(m_name, *m_transform.get(), { static_cast<float>(m_size), static_cast<float>(m_size) });
			auto pos = ImUtil::GetCenteredTextPos(ImGui::GetItemRectMin(), ImGui::GetItemRectSize(), txt.c_str());

			//CalcText Counts the double % escape as 2 chars messing with the centering, so this uglyness is needed.
			ImGuiEx::TextShadowExNoDummy(pos, std::string(txt + ("%")).c_str());

		}
		GTS::ImFontManager::Pop();

		return true;
	}
}
