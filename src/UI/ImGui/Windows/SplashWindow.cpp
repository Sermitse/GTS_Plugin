#include "UI/ImGui/Windows/SplashWindow.hpp"
#include "UI/ImGui/Core/ImFontManager.hpp"
#include "Version.hpp"

namespace GTS {

	void SplashWindow::Draw() {

		ImFontManager::Push(ImFontManager::kLargeText);

		ImGui::Text("Size Matters");
		ImGui::SameLine();

		ImFontManager::Push(ImFontManager::kSubText);
		ImGui::Text(GTSPlugin::ModVersion.string(".").c_str());
		ImGui::Text("Press F1 (By default) to open the configuration menu while ingame");

		ImFontManager::Pop(2);

	}

	bool SplashWindow::ShouldDraw() {
		return true;
	}

	void SplashWindow::Init() {
		m_windowType = kWidget;

		m_flags = ImGuiWindowFlags_NoDecoration | 
		ImGuiWindowFlags_NoSavedSettings | 
		ImGuiWindowFlags_NoFocusOnAppearing | 
		ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_NoNav;

		m_name = "SplashScreen";
	}

	float SplashWindow::GetAlphaMult() {
		return 1.0f;
	}

	float SplashWindow::GetBGAlphaMult() {
		return 1.0f;
	}

}
