#include "UI/ImGui/ImWindowManager.hpp"
#include "UI/UIManager.hpp"
#include "UI/ImGui/ImUtil.hpp"

namespace GTS {

    void ImWindowManager::AddWindow(std::unique_ptr<ImWindow> a_window) {

        assert(a_window != nullptr);

        if (HasWindows()) {
            for (const auto& window : windows) {
                if (window->Name == a_window->Name) {
                    logger::warn("ImWindowManager::AddWindow: Window with name {} already exists, Not Adding New Window", a_window->Name);
                    return;
                }
            }
        }

        windows.push_back(std::move(a_window));
        logger::info("ImWindowManager::AddWindow {}", windows.back()->Name);
    }


    void ImWindowManager::Update() {

       GTS_PROFILE_SCOPE("ImWindowManager Update");

		if (HasWindows()) [[likely]] {

            if (HasInputConsumers()) {
                auto& io = ImGui::GetIO();
                io.MouseDrawCursor = true;
            }
            else {
                auto& io = ImGui::GetIO();
                io.MouseDrawCursor = false;
                io.ClearInputKeys();
                io.ClearEventsQueue();
                io.ClearInputMouse();
            }

            if (ShowMetrics) {
                ImGui::ShowMetricsWindow();
            }

            if (ShowStack) {
                ImGui::ShowIDStackToolWindow();
            }

            if (ShowDemoWindow) {
                ImGui::ShowDemoWindow();
            }

            //Things Like the inventory menu draw before our hudmenu hook, this check prevents the widgets from drawing ontop of some things.
            if (Plugin::AnyWidgetMenuOpen() && !UIManager::ShouldDrawOverTop.load()) {
                return;
            }

            for (const auto& window : windows) {

                if (window->ShouldDraw()) {

                    const float BGAlpha = window->GetBGAlphaMult();
                    const float AlphaMult = window->GetAlphaMult();

                    auto BorderCol = ImGui::GetStyle().Colors[ImGuiCol_Border];
                    BorderCol.w *= BGAlpha;

                    //Set Background alpha
                    ImGui::SetNextWindowBgAlpha(BGAlpha * AlphaMult);
                    ImGui::PushStyleColor(ImGuiCol_Border, BorderCol);

                    //Set Entire Window Alpha
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, AlphaMult);

                    ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kText);

                    ImGui::Begin(window->Name.c_str(), &window->Show, window->flags);

                    window->Draw();

                	ImGui::End();

                    ImGui::PopStyleColor();
                    ImGui::PopStyleVar();
                    ImFontManager::PopActiveFont();
                }
            }
        }
    }

    //Gets a ptr to the window which fully matches the "Name" var.
    //Name var gets set in the ctor of the child window, otherwise its "Default"
    //If 2 Or more default windows exist only the earliest one will be returned
    ImWindow* ImWindowManager::GetWindowByName(const std::string& a_name) const {
        for (const auto& window : windows) {
            if (window->Name == a_name) {
                return window.get();
            }
        }
        logger::error("ImWindowManager::GetWindowByName Name: {} does not exist", a_name);
        return nullptr;
    }
}
