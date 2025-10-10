#include "UI/ImGui/Windows/Settings/SettingsWindow.hpp"

#include "UI/ImGui/Core/ImFontManager.hpp"
#include "UI/ImGui/Core/ImStyleManager.hpp"

#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/ImUtil.hpp"

#include "Config/Config.hpp"
#include "Config/Keybinds.hpp"

//categories
//#include "UI/Windows/Settings/Categories/Gameplay.hpp"
//#include "UI/Windows/Settings/Categories/Info.hpp"
//#include "UI/Windows/Settings/Categories/Interface.hpp"
//#include "UI/Windows/Settings/Categories/Audio.hpp"
//#include "UI/Windows/Settings/Categories/AI.hpp"
//#include "UI/Windows/Settings/Categories/Advanced.hpp"
//#include "UI/Windows/Settings/Categories/Camera.hpp"
//#include "UI/Windows/Settings/Categories/Keybinds.hpp"
//#include "UI/Windows/Settings/Categories/General.hpp"
//#include "UI/Windows/Settings/Categories/Balance.hpp"
//#include "UI/Windows/Settings/Categories/Actions.hpp"
//#include "UI/Windows/Settings/Categories/Widgets.hpp"


#include "Managers/Input/InputManager.hpp"

#include "Version.hpp"
#include "git.h"

#include "Managers/Console/ConsoleManager.hpp"

#include "UI/GTSMenu.hpp"

namespace GTS {

	void SettingsWindow::AsyncSave() {

		if (m_saveLoadBusy.load()) {
			return;
		}

		m_saveLoadBusy.store(true);
		std::thread(&SettingsWindow::SaveImpl, this).detach();
	}

	void SettingsWindow::LoadImpl() {

		try {

			if (!Config::LoadSettingsFromString()) {
				logger::error("Settings.LoadSettings() Error");
				m_saveLoadBusy.store(false);
				return;
			}

			if (!Keybinds::LoadKeybinds()) {
				logger::error("KeyMgr.LoadKeybinds() Error");
				m_saveLoadBusy.store(false);
				return;
			}

			ImStyleManager::ApplyStyle();
			m_saveLoadBusy.store(false);
		}
		//Should not be needed but just in case...
		catch (...) {
			logger::error("An exception occured in LoadImpl()");
		}
	}

	void SettingsWindow::SaveImpl() {

		try {

			if (!Config::SaveSettingsToString()) {
				logger::error("Settings.SaveSettings() Error");
				m_saveLoadBusy.store(false);
				return;
			}

			if (!Keybinds::SaveKeybinds()) {
				logger::error("KeyMgr.SaveKeybinds() Error");
				m_saveLoadBusy.store(false);
				return;
			}

			InputManager::GetSingleton().Init();
			m_saveLoadBusy.store(false);
		}
		//Should not be needed but just in case...
		catch (...) {
			logger::error("An exception occured in SaveImpl()");
		}
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

		CategoryMgr = new ImCategoryContainer();

		//Add Categories, order here defines the order they'll be shown.
		//CatMgr.AddCategory(std::make_shared<CategoryInfo>());
		//CatMgr.AddCategory(std::make_shared<CategoryGeneral>());
		//CatMgr.AddCategory(std::make_shared<CategoryGameplay>());
		//CatMgr.AddCategory(std::make_shared<CategoryBalance>());
		//CatMgr.AddCategory(std::make_shared<CategoryActions>());
		//CatMgr.AddCategory(std::make_shared<CategoryAudio>());
		//CatMgr.AddCategory(std::make_shared<CategoryAI>());
		//CatMgr.AddCategory(std::make_shared<CategoryCamera>());
		//CatMgr.AddCategory(std::make_shared<CategoryInterface>());
		//CatMgr.AddCategory(std::make_shared<CategoryWidgets>());
		//CatMgr.AddCategory(std::make_shared<CategoryKeybinds>());
		//CatMgr.AddCategory(std::make_shared<CategoryAdvanced>());
		BuildFooterText();

		InputManager::RegisterInputEvent("OpenModSettings", OpenSettingsKeybindCallback);
		ConsoleManager::RegisterCommand("menu", OpenSettingsConsoleCallback,"Open the settings menu");
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
		}
		logger::error("Can't call handler window, pointer was invalid!");
	}

	void SettingsWindow::HandleOpenClose(bool a_open) {

		if (!Plugin::Live() && !m_show) {
			logger::warn("Can't show menu: Not Ingame!");
			Cprint("Can not open the settings menu at this time. Try again in game.");
			return;
		}

		if (Plugin::IsInBlockingMenu() && !m_show) {
			logger::warn("Can't show menu: A Conflicting game menu is open!");
			Cprint("Can not open the settings menu at this time. A conflicting game menu is open.");
			return;
		}

		if (a_open) {

			if (const auto msgQueue = UIMessageQueue::GetSingleton()) {
				//The console draws above and since we disable input its unclosable so we need to close it ourselves.
				msgQueue->AddMessage(RE::Console::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);

				//TODO Place any conflicting menus here.
				//msgQueue->AddMessage(RE::DialogueMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
			}

			m_show = true;
		}

		else {
			AsyncSave();
			m_show = false;
		}

		GTSMenu::AlterTimeScale(a_open);
		GTSMenu::BlurBackground(a_open);
		GTSMenu::PauseGame(a_open);

	}

	void SettingsWindow::SetDisabled(bool a_disabled) {
		m_inputDisabled = a_disabled;
	}

	void SettingsWindow::Draw() {

		if (!m_busy && ImGui::IsKeyReleased(ImGuiKey_Escape)) {
			HandleOpenClose(false);
			return;
		}

		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && 
			ImGui::IsKeyDown(ImGuiKey_LeftAlt)  && 
			ImGui::IsKeyDown(ImGuiKey_F12)) {
			//Enable Advanced Menu
			Config::Hidden.IKnowWhatImDoing = true;
		}

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

		{

			const ImVec2 pos = ImVec2(ImGui::GetContentRegionAvail().x - (ImGui::GetStyle().FramePadding.x * 2 + ImGui::GetStyle().CellPadding.x), ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetStyle().CellPadding.y);
			ImGui::SetCursorPos(pos);

			//Close button
			if (ImUtil::ImageButton("Close##", "generic_x", 18, "Close")) {
				HandleOpenClose(false);
			}

		}

		ImGui::SetCursorPos(OldPos);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4.0,0.0 });

		{  // Draw Title

			ImFontManager::Push(ImFontManager::ActiveFontType::kTitle);
			ImGui::Text(m_title.c_str());
			ImFontManager::Pop();
		}

		ImGui::PopStyleVar();
		ImUtil::SeperatorH();

		{  // Draw Sidebar

			ImGui::BeginChild("Sidebar", ImVec2(CategoryMgr->GetLongestCategory(), ImGui::GetContentRegionAvail().y), true);
			ImGui::BeginDisabled(m_inputDisabled);
			ImFontManager::Push(ImFontManager::ActiveFontType::kSidebar);

			// Display the categories in the sidebar
			for (uint8_t i = 0; i < static_cast<uint8_t>(Categories.size()); i++) {
				ImCategory* category = Categories[i].get();

				//If nullptr / invisible / or dbg category, Do not draw.
				if (!category) continue;
				if (!Config::Hidden.IKnowWhatImDoing && category->GetTitle() == "Advanced") continue;
				if (!category->IsVisible()) continue;

				if (ImGui::Selectable(category->GetTitle().c_str(), CategoryMgr->m_activeIndex == i)) {
					CategoryMgr->m_activeIndex = i;
				}

			}

			ImFontManager::Pop();
			ImGui::EndDisabled();
			ImGui::EndChild();
		}

		ImUtil::SeperatorV();

		{ // Content Area, Where the category contents are drawn

			ImGui::BeginChild("Content", ImVec2(0, ImGui::GetContentRegionAvail().y), true); // Remaining width

			// Validate selectedCategory to ensure it's within bounds
			if (CategoryMgr->m_activeIndex < Categories.size()) {
				ImCategory* selected = Categories[CategoryMgr->m_activeIndex].get();
				selected->Draw(); // Call the Draw method of the selected category
			}
			else {
				ImGui::TextColored(ImUtil::ColorError, "Invalid category or no categories exist!");
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
			ImGui::PushStyleColor(ImGuiCol_Text, ImUtil::ColorSubscript);
			ImGui::TextWrapped(m_footerText.c_str());
			ImGui::PopStyleColor();
			ImFontManager::Pop();
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
