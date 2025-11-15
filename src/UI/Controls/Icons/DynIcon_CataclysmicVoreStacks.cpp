#include "UI/Controls/Icons/DynIcon_CataclysmicVoreStacks.hpp"

#include "Config/Config.hpp"

#include "UI/Controls/Text.hpp"
#include "UI/Core/ImColorUtils.hpp"
#include "UI/Core/ImFontManager.hpp"
#include "UI/Core/ImGraphics.hpp"
#include "UI/Core/ImUtil.hpp"

namespace ImGuiEx {

	DynIconCataclysmicVoreStacks::DynIconCataclysmicVoreStacks(uint32_t a_size) : IDynIcon(ImageList::BuffIcon_VoreStacks, a_size) {
		m_transform = std::make_unique<GTS::ImGraphics::ImageTransform>();
		m_transform->cutoffDir = GTS::ImGraphics::CutoffDirection::BottomToTop;
		m_transform->recolorEnabled = true;
	}

	bool DynIconCataclysmicVoreStacks::Draw(int a_stacks) const {

		if (a_stacks <= 0) return false;

		GTS::ImFontManager::Push(GTS::ImFontManager::kIconText, m_size / 64.0f);
		{
			constexpr int overflow = 20;
			m_transform->targetColor = a_stacks > overflow ? ImUtil::Colors::fRGBToImVec4(GTS::Config::UI.f3IconOverflowColor) : ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
			m_transform->cutoffPercent = static_cast<float>(std::min(a_stacks, overflow)) / static_cast<float>(overflow);

			const std::string txt = fmt::format("{:d}x", a_stacks);

			GTS::ImGraphics::RenderTransformed(m_name, *m_transform.get(), { static_cast<float>(m_size), static_cast<float>(m_size) });
			const auto pos = ImUtil::GetCenteredTextPos(ImGui::GetItemRectMin(), ImGui::GetItemRectSize(), txt.c_str());
			ImGuiEx::TextShadowExNoDummy(pos, txt.c_str());

		}
		GTS::ImFontManager::Pop();

		return true;
	}
}
