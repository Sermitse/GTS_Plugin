#include "UI/GTSMenu.hpp"
#include "UI/ImGui/Windows/Settings/SettingsWindow.hpp"
#include "UI/ImGui/Lib/imgui.h"

#include "UI/ImGui/Core/ImFontManager.hpp"
#include "UI/ImGui/Core/ImStyleManager.hpp"
#include "UI/ImGui/Core/ImUtil.hpp"

#include "UI/ImGui/Controls/Button.hpp"
#include "UI/ImGui/Controls/Misc.hpp"

#include "Config/Config.hpp"
#include "Config/Keybinds.hpp"

#include "Managers/Input/InputManager.hpp"
#include "Managers/Console/ConsoleManager.hpp"

//categories
#include "UI/ImGui/Windows/Settings/Categories/Gameplay.hpp"
//#include "UI/ImGui/Windows/Settings/Categories/Info.hpp"
#include "UI/ImGui/Windows/Settings/Categories/Interface.hpp"
#include "UI/ImGui/Windows/Settings/Categories/Audio.hpp"
#include "UI/ImGui/Windows/Settings/Categories/AI.hpp"
#include "UI/ImGui/Windows/Settings/Categories/Advanced.hpp"
#include "UI/ImGui/Windows/Settings/Categories/Camera.hpp"
#include "UI/ImGui/Windows/Settings/Categories/Keybinds.hpp"
#include "UI/ImGui/Windows/Settings/Categories/General.hpp"
#include "UI/ImGui/Windows/Settings/Categories/Balance.hpp"
#include "UI/ImGui/Windows/Settings/Categories/Actions.hpp"
//#include "UI/ImGui/Windows/Settings/Categories/Widgets.hpp"

#include "Version.hpp"
#include "git.h"

namespace GTS {

	bool SettingsWindow::LoadImpl() {

		TryLockMutex guard(m_saveLoadBusy);


		if (!guard.acquired()) {
			logger::warn("SettingsWindow::LoadImpl Lock Not Aquired");
			return true; //Assume success...
		}

		try {

			if (!Config::LoadSettingsFromString()) {
				logger::error("SettingsWindow::LoadSettingsFromString Error");
				return false;
			}

			if (!Keybinds::LoadKeybinds()) {
				logger::error("SettingsWindow::LoadKeybinds Error");
				return false;
			}

			ImStyleManager::ApplyStyle();

		}
		//Should not be needed but just in case...
		catch (const exception& e) {
			logger::error("An exception occured in LoadImpl: {}", e.what());
			return false;
		}

		return true;
	}

	bool SettingsWindow::SaveImpl() {

		TryLockMutex guard(m_saveLoadBusy);
		if (!guard.acquired()) {
			logger::warn("SettingsWindow::SaveImpl Lock Not Aquired");
			return true; //Assume success...
		}

		try {

			if (!Config::SaveSettingsToString()) {
				logger::error("SettingsWindow::SaveSettings Error");
				return false;
			}

			if (!Keybinds::SaveKeybinds()) {
				logger::error("SettingsWindow::SaveKeybinds Error");
				return false;
			}

			InputManager::GetSingleton().Init();
		}
		//Should not be needed but just in case...
		catch (const exception& e) {
			logger::error("An exception occured in SaveImpl: {}",e.what());
			return false;
		}

		return true;
	}

	void SettingsWindow::Init() {

		m_title = "Size Matters - Settings";
		m_name = "Settings";
		m_busy = false;
		m_windowType = WindowType::kSettings;
		m_fadeSettings.enabled = false;

		m_flags = ImGuiWindowFlags_NoCollapse        | 
				  ImGuiWindowFlags_NoTitleBar        | 
				  ImGuiWindowFlags_NoNav             | 
				  ImGuiWindowFlags_NoNavFocus        | 
				  ImGuiWindowFlags_NoNavInputs       | 
				  ImGuiWindowFlags_NoScrollbar       | 
				  ImGuiWindowFlags_NoScrollWithMouse;


		//Construct Base defaults for this Window
		m_settingsHolder->SetBaseDefaults({
			.bLock = true,
			.f2Position = { 0.0f, 0.0f },
			.sAnchor = "kCenter",
			.fAlpha = 0.9f,
			.fBGAlphaMult = 0.7f,
			.fWindowSizePercent = 90.0f
		});


		CategoryMgr = new ImCategoryContainer();

		//Add Categories, order here defines the order they'll be shown.
		//CategoryMgr->AddCategory(std::make_unique<CategoryInfo>());
		CategoryMgr->AddCategory(std::make_unique<CategoryGeneral>());
		CategoryMgr->AddCategory(std::make_unique<CategoryGameplay>());
		CategoryMgr->AddCategory(std::make_unique<CategoryBalance>());
		CategoryMgr->AddCategory(std::make_unique<CategoryActions>());
		CategoryMgr->AddCategory(std::make_unique<CategoryAudio>());
		CategoryMgr->AddCategory(std::make_unique<CategoryAI>());
		CategoryMgr->AddCategory(std::make_unique<CategoryCamera>());
		CategoryMgr->AddCategory(std::make_unique<CategoryInterface>());
		//CategoryMgr->AddCategory(std::make_unique<CategoryWidgets>());
		CategoryMgr->AddCategory(std::make_unique<CategoryKeybinds>());
		CategoryMgr->AddCategory(std::make_unique<CategoryAdvanced>());

		BuildFooterText();

		InputManager::RegisterInputEvent("OpenModSettings", OpenSettingsKeybindCallback);
		ConsoleManager::RegisterCommand("menu", OpenSettingsConsoleCallback,"Open the settings menu");
	}

	void SettingsWindow::RequestClose() {
		//Handle closing through the esc key.
		if (!m_showErrorModal && !m_busy && m_show) {
			HandleOpenClose(false);
			return;
		}
		logger::trace("Not closed Modal:{} Busy:{}", m_showErrorModal, m_busy);
	}

	bool SettingsWindow::WantsToDraw() {
		return this->m_show;
	}

	void SettingsWindow::OpenSettingsKeybindCallback(const ManagedInputEvent& a_event) {
		if (auto Window = dynamic_cast<SettingsWindow*>(GTSMenu::WindowManager->wSettings)) {
			Window->HandleOpenClose(true);
			return;
		}
		logger::error("Can't call handler window, pointer was invalid!");
	}

	void SettingsWindow::OpenSettingsConsoleCallback() {
		if (auto Window = dynamic_cast<SettingsWindow*>(GTSMenu::WindowManager->wSettings)) {
			Window->HandleOpenClose(true);
			return;
		}
		logger::error("Can't call handler window, pointer was invalid!");
	}

	void SettingsWindow::HandleOpenClose(bool a_open) {

		if (!Plugin::Ready() && !m_show) {
			logger::warn("Can't show menu: Not Ingame!");
			Cprint("Can not open the settings menu at this time. Try again in game.");
			return;
		}

		if (Plugin::IsInBlockingMenu() && !m_show ) {
			logger::warn("Can't show menu: A Conflicting game menu is open!");
			Cprint("Can not open the settings menu at this time. A conflicting game menu is open.");
			DebugNotification("Can not open the settings menu at this time.", nullptr, false);
			return;
		}

		if (a_open) {

			if (const auto msgQueue = UIMessageQueue::GetSingleton()) {
				//The console draws above and since we disable input it becomes unclosable, we need to close it ourselves.
				msgQueue->AddMessage(RE::Console::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
			}

			m_show = true;
		}

		else {

			//If save fails don't hide self and show modal box.
			if (!SaveImpl()) {
				m_showErrorModal = true;
				return;
			}

			m_show = false;
		}

		GTSMenu::AlterTimeScale(a_open);
		GTSMenu::BlurBackground(a_open);
		GTSMenu::PauseGame(a_open);

	}

	void SettingsWindow::DisableUIInteraction(bool a_disabled) {
		m_disableUIInteraction = a_disabled;
	}

	void SettingsWindow::ShowErrorModal(bool* a_requestOpen) {

		static const char* const windowName = "##SaveLoadError";

		if (*a_requestOpen) {
			ImGui::OpenPopup(windowName);
			*a_requestOpen = false;
		}

		if (ImGui::BeginPopupModal(windowName, nullptr, m_flags | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize)) {
			ImFontManager::Push(ImFontManager::kLargeText);
			ImGui::TextColored(ImUtil::Colors::Error, "Settings could not be saved\n"
											          "Check GtsPlugin.log for more info.");
			if (ImGui::Button("OK")) {
				ImGui::CloseCurrentPopup();

				GTSMenu::AlterTimeScale(false);
				GTSMenu::BlurBackground(false);
				GTSMenu::PauseGame(false);

				m_show = false;

			}
			ImFontManager::Pop();
			ImGui::EndPopup();
		}

	}

	void SettingsWindow::Draw() {

		auto& Categories = CategoryMgr->GetCategories();
		auto& BaseSettings = GetBaseSettings();

		//Update Window Flags
		m_flags = BaseSettings.bLock ? 
			m_flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove : 
			m_flags & ~ImGuiWindowFlags_NoResize & ~ImGuiWindowFlags_NoMove;

		//Handle Fixed Position and Size
		if (BaseSettings.bLock) {
			ImGui::SetWindowSize(ImUtil::ScaleToViewport(BaseSettings.fWindowSizePercent));
			//Mousedown check Pprevents the sliders from moving around whilst resizing
			if (!ImGui::GetIO().MouseDown[0]) {
				const ImVec2 Offset{ BaseSettings.f2Position.at(0), BaseSettings.f2Position.at(1) };
				ImGui::SetWindowPos(GetAnchorPos(StringToEnum<WindowAnchor>(BaseSettings.sAnchor), Offset, false));
			}
		}

		const auto OldPos = ImGui::GetCursorPos();

		{   //Close button

			ImGui::BeginDisabled(m_disableUIInteraction);

			const ImVec2 pos = ImVec2(ImGui::GetContentRegionAvail().x - (ImGui::GetStyle().FramePadding.x * 2 + ImGui::GetStyle().CellPadding.x), ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetStyle().CellPadding.y);
			ImGui::SetCursorPos(pos);
			if (ImGuiEx::ImageButton("Close##", "generic_x", 18, "Close")) {
				HandleOpenClose(false);
			}

			ImGui::EndDisabled();

		}

		ImGui::SetCursorPos(OldPos);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4.0,0.0 });

		{  // Draw Title

			ImFontManager::Push(ImFontManager::ActiveFontType::kTitle);
			ImGui::Text(m_title.c_str());
			ImFontManager::Pop();
		}

		ImGui::PopStyleVar();
		ImGuiEx::SeperatorH();

		{  // Draw Sidebar

			ImGui::BeginChild("Sidebar", ImVec2(CategoryMgr->GetLongestCategory(), ImGui::GetContentRegionAvail().y), true);
			ImGui::BeginDisabled(m_disableUIInteraction);
			ImFontManager::Push(ImFontManager::ActiveFontType::kSidebar);

			// Display the categories in the sidebar
			for (uint8_t i = 0; i < static_cast<uint8_t>(Categories.size()); i++) {
				ImCategory* category = Categories[i].get();

				//If nullptr / invisible, Do not draw.
				if (!category) continue;
				if (!category->IsVisible()) continue;

				if (ImGui::Selectable(category->GetTitle().c_str(), CategoryMgr->m_activeIndex == i)) {
					CategoryMgr->m_activeIndex = i;
				}

			}

			ImFontManager::Pop();
			ImGui::EndDisabled();
			ImGui::EndChild();
		}

		ImGuiEx::SeperatorV();

		{ // Content Area, Where the category contents are drawn

			ImGui::BeginChild("Content", ImVec2(0, ImGui::GetContentRegionAvail().y), true); // Remaining width

			// Validate selectedCategory to ensure it's within bounds
			if (CategoryMgr->m_activeIndex < Categories.size()) {
				ImCategory* selected = Categories[CategoryMgr->m_activeIndex].get();
				selected->Draw(); // Call the Draw method of the selected category
			}
			else {
				ImGui::TextColored(ImUtil::Colors::Error, "Invalid category or no categories exist!");
			}

			ImGui::EndChild();
		}

		{   //Footer - Mod Info

			ImFontManager::Push(ImFontManager::ActiveFontType::kSubText);

			// Get window draw position and size
			ImVec2 windowPos = ImGui::GetWindowPos();
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImVec2 textSize = ImGui::CalcTextSize(m_footerText.c_str());

			auto padding = ImGui::GetStyle().FramePadding;
			ImVec2 textPos = {
				windowPos.x + padding.x,
				windowPos.y + windowSize.y - textSize.y - padding.y
			};

			// Set the cursor to the calculated position
			ImGui::SetCursorScreenPos(textPos);
			ImGui::PushStyleColor(ImGuiCol_Text, ImUtil::Colors::Subscript);
			ImGui::TextWrapped(m_footerText.c_str());
			ImGui::PopStyleColor();
			ImFontManager::Pop();
		}

		ShowErrorModal(&m_showErrorModal);

		//Sectet advanced settings toggle.
		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) &&
			ImGui::IsKeyDown(ImGuiKey_LeftAlt)  &&
			ImGui::IsKeyDown(ImGuiKey_F12)      &&
			!m_showErrorModal                   &&
			!m_busy) {
			Config::Hidden.IKnowWhatImDoing = true;
		}

	}

	void SettingsWindow::BuildFooterText() {

		for (auto it = GTSPlugin::ModVersion.begin(); it != GTSPlugin::ModVersion.end(); ++it) {
			m_footerText += std::to_string(*it);
			if (std::next(it) != GTSPlugin::ModVersion.end()) {
				m_footerText += ".";
			}
		}

		if (git::AnyUncommittedChanges()) {
			m_footerText += "\nDevelopment Build";
			m_footerText += "\n" + fmt::format("{} {}", __DATE__, __TIME__);
		}
	}
}
