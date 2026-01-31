#include "UI/Core/ImWindow.hpp"    

namespace GTS {

	void ImWindow::DebugDraw() {
		//Nothing
	}

	bool ImWindow::IsDebugging() {
		return false;
	}

	float ImWindow::GetFadingAlpha() const {
		return m_fadeSettings.fadeAlpha;
	}

	void ImWindow::ResetFadeState() {
		m_fadeSettings.visibilityTimer = 0.0f;
		m_fadeSettings.fadeAlpha = 1.0f;
		m_fadeSettings.isFading = false;
	}

    void ImWindow::UpdateFade(float deltaTime) {
        if (!m_fadeSettings.enabled) {
			m_fadeSettings.fadeAlpha = 1.0f;
            return;
        }

        if (!m_fadeSettings.isFading) {
            m_fadeSettings.visibilityTimer += deltaTime;
            if (m_fadeSettings.visibilityTimer >= m_fadeSettings.visibilityDuration) {
                m_fadeSettings.isFading = true;
				m_fadeSettings.visibilityTimer = 0.0f;
            }
        }
        else {
			m_fadeSettings.visibilityTimer += deltaTime;
			float fadeProgress = std::clamp(m_fadeSettings.visibilityTimer / m_fadeSettings.fadeDuration, 0.0f, 1.0f);
            m_fadeSettings.fadeAlpha = 1.0f - fadeProgress;
        }
    }

	bool ImWindow::IsFadeComplete() const {
		return m_fadeSettings.enabled && m_fadeSettings.isFading && m_fadeSettings.fadeAlpha <= 0.0f;
	}


	ImVec2 ImWindow::GetAnchorPos(WindowAnchor a_position, ImVec2 a_padding, bool a_allowCenterY) {

        auto v = ImGui::GetMainViewport();
        auto s = ImGui::GetWindowSize();

        //Get Total size first then subtract the viewport position to cancel out any offsets
        ImVec2 Origin = v->Size;
        Origin.x -= v->Pos.x;
        Origin.y -= v->Pos.y;

        //Subtract the window size to get the top left corner of the window
        Origin.x -= s.x;
        Origin.y -= s.y;

        switch (a_position) {
            case WindowAnchor::kTopLeft:
                return { a_padding.x, a_padding.y };
            case WindowAnchor::kTopRight:
                return { Origin.x - a_padding.x, a_padding.y };
            case WindowAnchor::kBottomLeft:
                return { a_padding.x, Origin.y - a_padding.y };
            case WindowAnchor::kBottomRight:
                return { Origin.x - a_padding.x, Origin.y - a_padding.y };
            case WindowAnchor::kCenter: default:
                if (a_allowCenterY) {
                    return { Origin.x * 0.5f, Origin.y - a_padding.y };
                }
        	return { Origin.x * 0.5f, Origin.y * 0.5f };
        }
    }
}