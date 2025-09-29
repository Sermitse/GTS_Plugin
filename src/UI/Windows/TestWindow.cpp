#include "TestWindow.hpp"
#include "Managers/Input/InputManager.hpp"

namespace {


}

void GTS::TestWindow::OpenSettingsK(const GTS::ManagedInputEvent& data) {
	SetVisible(!IsVisible());
}

bool GTS::TestWindow::RequestShow() {
	return false;
}

bool GTS::TestWindow::RequestHide() {
	return false;
}

void GTS::TestWindow::OnWindowShow() {

}

void GTS::TestWindow::OnWindowHide() {

}

void GTS::TestWindow::Draw() {
	ImGui::Text("Aeugh");
	static float test = 1.0f;
	ImGui::InputFloat("Test", &test);

	if (ImGui::Button("Present")) {
		this->m_drawLevel = DrawLevel::kPresent;
	}

	if (ImGui::Button("Cursor")) {
		this->m_drawLevel = DrawLevel::kCursor;
	}
}

bool GTS::TestWindow::ShouldDraw() {
	return IsVisible();
}

void GTS::TestWindow::Init() {
	this->m_drawLevel = DrawLevel::kCursor;
	this->m_consumeInput = true;
	this->SetVisible(true);
}

float GTS::TestWindow::GetAlphaMult() {
	return 1.0f;
}

float GTS::TestWindow::GetBGAlphaMult() {
	return 1.0f;
}
