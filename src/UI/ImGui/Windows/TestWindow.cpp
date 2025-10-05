#include "TestWindow.hpp"

void GTS::TestWindow::Draw() {
	ImGui::Text("Aeugh");
}

bool GTS::TestWindow::ShouldDraw() {
	return true;
}

void GTS::TestWindow::Init() {
	this->m_windowType = WindowType::kSettings;
}

float GTS::TestWindow::GetAlphaMult() {
	return 1.0f;
}

float GTS::TestWindow::GetBGAlphaMult() {
	return 1.0f;
}
