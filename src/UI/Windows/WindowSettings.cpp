#include "UI/Windows/WindowSettings.hpp"
#include "UI/ImGui/Lib/imgui.h"

//categories
#include "UI/Categories/Gameplay.hpp"
#include "UI/Categories/Info.hpp"
#include "UI/Categories/Interface.hpp"
#include "UI/Categories/Audio.hpp"
#include "UI/Categories/AI.hpp"
#include "UI/Categories/Advanced.hpp"
#include "UI/Categories/Camera.hpp"
#include "UI/Categories/Keybinds.hpp"
#include "UI/Categories/General.hpp"
#include "UI/Categories/Balance.hpp"
#include "UI/Categories/Actions.hpp"
#include "UI/Categories/Widgets.hpp"

#include "UI/ImGui/ImUtil.hpp"
#include "Managers/Input/InputManager.hpp"

#include "UI/UIManager.hpp"

#include "Version.hpp"
#include "git.h"

namespace {

	std::string FooterText;

	void BuildFooterText() {

		for (auto it = GTSPlugin::ModVersion.begin(); it != GTSPlugin::ModVersion.end(); ++it) {
			FooterText += std::to_string(*it);
			if (std::next(it) != GTSPlugin::ModVersion.end()) {
				FooterText += ".";
			}
		}

		if (git::AnyUncommittedChanges()) {
			FooterText += "\nDevelopment Build";
			FooterText += "\n" + fmt::format("{} {}", __DATE__, __TIME__);
		}
	}
}

namespace GTS {

	void WindowSettings::LoadImpl() {

		try {

			if (!Config::LoadSettingsFromString()) {
				logger::error("Settings.LoadSettings() Error");
				SaveLoadBusy.store(false);
				return;
			}

			if (!Keybinds::LoadKeybinds()) {
				logger::error("KeyMgr.LoadKeybinds() Error");
				SaveLoadBusy.store(false);
				return;
			}

			StyleMgr.LoadStyle();
			SaveLoadBusy.store(false);
		}
		//Should not be needed but just in case...
		catch (...) {
			logger::error("An exception occured in LoadImpl()");
		}
	}

	void WindowSettings::SaveImpl() {

		try {

			if (!Config::SaveSettingsToString()) {
				logger::error("Settings.SaveSettings() Error");
				SaveLoadBusy.store(false);
				return;
			}

			if (!Keybinds::SaveKeybinds()) {
				logger::error("KeyMgr.SaveKeybinds() Error");
				SaveLoadBusy.store(false);
				return;
			}

			InputManager::GetSingleton().Init();
			SaveLoadBusy.store(false);
		}
		//Should not be needed but just in case...
		catch (...) {
			logger::error("An exception occured in SaveImpl()");
		}
	}

	//Do All your Init Stuff here
	//Note: Dont do any calls to the imgui api here as the window is not yet created
	WindowSettings::WindowSettings() {

		Title = "Size Matters - Settings";
		Name = "Settings";
		Show = false;
		ConsumeInput = true;
		flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

		//Add Categories, order here defines the order they'll be shown.
		CatMgr.AddCategory(std::make_shared<CategoryInfo>());
		CatMgr.AddCategory(std::make_shared<CategoryGeneral>());
		CatMgr.AddCategory(std::make_shared<CategoryGameplay>());
		CatMgr.AddCategory(std::make_shared<CategoryBalance>());
		CatMgr.AddCategory(std::make_shared<CategoryActions>());
		CatMgr.AddCategory(std::make_shared<CategoryAudio>());
		CatMgr.AddCategory(std::make_shared<CategoryAI>());
		CatMgr.AddCategory(std::make_shared<CategoryCamera>());
		CatMgr.AddCategory(std::make_shared<CategoryInterface>());
		CatMgr.AddCategory(std::make_shared<CategoryWidgets>());
		CatMgr.AddCategory(std::make_shared<CategoryKeybinds>());
		CatMgr.AddCategory(std::make_shared<CategoryAdvanced>());

		BuildFooterText();
	}

	void WindowSettings::Draw() {

		auto& Categories = CatMgr.GetCategories();

		//Update Window Flags
		flags = (sUI.bLock ? (flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove) : (flags & ~ImGuiWindowFlags_NoResize & ~ImGuiWindowFlags_NoMove));

		//Handle Fixed Position and Size
		if (sUI.bLock) {
			ImGui::SetWindowSize(ImUtil::ScaleToViewport(sUI.fWindowSize));

			//Mousedown Check Prevents the window from moving around and messing with the slider while dragging
			if (!ImGui::GetIO().MouseDown[0]) {
				//X,Y
				const ImVec2 Offset{ sUI.f2Offset[0], sUI.f2Offset[1] };
				ImGui::SetWindowPos(GetAnchorPos(StringToEnum<WindowAnchor>(sUI.sAnchor), Offset, false));
			}
		}

		const auto OldPos = ImGui::GetCursorPos();

		{

			const ImVec2 pos = ImVec2(ImGui::GetContentRegionAvail().x - (ImGui::GetStyle().FramePadding.x * 2 + ImGui::GetStyle().CellPadding.x), ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetStyle().CellPadding.y);
			ImGui::SetCursorPos(pos);

			//Close button
			if (ImUtil::ImageButton("Close##", "generic_x", 18, "Close")) {
				UIManager::CloseSettings();
			}

		}

		ImGui::SetCursorPos(OldPos);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4.0,0.0 });

		{  // Draw Title

			ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kTitle);
			ImGui::Text(Title.c_str());
			ImFontManager::PopActiveFont();
		}

		ImGui::PopStyleVar();
		ImUtil::SeperatorH();

		{  // Draw Sidebar

			ImGui::BeginChild("Sidebar", ImVec2(CatMgr.GetLongestCategory(), ImGui::GetContentRegionAvail().y), true);
			ImGui::BeginDisabled(Disabled);
			ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kSidebar);

			// Display the categories in the sidebar
			for (uint8_t i = 0; i < static_cast<uint8_t>(Categories.size()); i++) {
				ImCategory* category = Categories[i].get();

				//If nullptr / invisible / or dbg category, Do not draw.

				if (!category) continue;
				if (!sHidden.IKnowWhatImDoing && category->GetTitle() == "Advanced") continue;
				if (!category->IsVisible()) continue;

				if (ImGui::Selectable(category->GetTitle().c_str(), CatMgr.activeIndex == i)) {
					CatMgr.activeIndex = i;
				}

			}

			ImFontManager::PopActiveFont();
			ImGui::EndDisabled();
			ImGui::EndChild();
		}

		ImUtil::SeperatorV();

		{ // Content Area, Where the category contents are drawn

			ImGui::BeginChild("Content", ImVec2(0, ImGui::GetContentRegionAvail().y), true); // Remaining width

			// Validate selectedCategory to ensure it's within bounds
			if (CatMgr.activeIndex < Categories.size()) {
				ImCategory* selected = Categories[CatMgr.activeIndex].get();
				selected->Draw(); // Call the Draw method of the selected category
			}
			else {
				ImGui::TextColored(ImUtil::ColorError, "Invalid category or no categories exist!");
			}

			ImGui::EndChild();
		}

		{   //Footer - Mod Info

			ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kSubText);

			// Get window draw position and size
			ImVec2 windowPos = ImGui::GetWindowPos();
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImVec2 textSize = ImGui::CalcTextSize(FooterText.c_str());

			auto padding = ImGui::GetStyle().FramePadding;
			ImVec2 textPos = {
				windowPos.x + padding.x,
				windowPos.y + windowSize.y - textSize.y - padding.y
			};

			// Set the cursor to the calculated position
			ImGui::SetCursorScreenPos(textPos);
			ImGui::PushStyleColor(ImGuiCol_Text, ImUtil::ColorSubscript);
			ImGui::TextWrapped(FooterText.c_str());
			ImGui::PopStyleColor();
			ImFontManager::PopActiveFont();
		}
	}
}
