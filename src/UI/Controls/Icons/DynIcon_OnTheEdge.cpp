#include "UI/Controls/Icons/DynIcon_OnTheEdge.hpp"

#include "Config/Config.hpp"

#include "UI/GTSMenu.hpp"
#include "UI/Controls/Text.hpp"
#include "UI/Core/ImColorUtils.hpp"
#include "UI/Core/ImFontManager.hpp"
#include "UI/Core/ImUtil.hpp"
#include "UI/Windows/Widgets/StatusBarWindow.hpp"

namespace ImGuiEx {

	DynIconOnTheEdge::DynIconOnTheEdge(uint32_t a_size) : IDynIcon(ImageList::BuffIcon_OnTheEdge, a_size) {
		m_transform = std::make_unique<GTS::ImGraphics::ImageTransform>();
		m_transform->transformDirection = GTS::ImGraphics::Direction::BottomToTop;
		m_transform->recolorEnabled = true;
		m_transform->gradientFadeEnabled = true;
	}

	bool DynIconOnTheEdge::Draw(float a_percent, bool a_alwaysShow) const {

		if (a_percent - 1.0f <= 0.f && !a_alwaysShow) return false;

		float rel_scale = dynamic_cast<GTS::StatusBarWindow*>(GTS::GTSMenu::WindowManager->wStatusBar)->GetExtraSettings<WindowSettingsStatusBar_t>().fRelativeFontScale;
		GTS::ImFontManager::Push(GTS::ImFontManager::kIconText, (static_cast<float>(m_size) / static_cast<float>(m_referenceSize)) * rel_scale);
		{
			constexpr float overflow = 60.f;
			m_transform->targetColor = a_percent > overflow ? ImUtil::Colors::fRGBToImVec4(GTS::Config::UI.f3IconOverflowColor) : ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
			m_transform->gradientStartPercent = static_cast<float>(std::min(a_percent, overflow)) / static_cast<float>(overflow);
			m_transform->gradientTargetAlpha = 0.0f; //Target alpha at max fade

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
