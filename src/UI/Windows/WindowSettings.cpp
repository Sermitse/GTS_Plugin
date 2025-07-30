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

namespace GTS {

	void WindowSettings::LoadImpl() {

		try {
			if (!Settings.LoadSettings()) {
				SetErrorAndUnlock(kLoadSettingsError);
				return;
			}

			if (!KeyMgr.LoadKeybinds()) {
				SetErrorAndUnlock(kLoadInputError);
				return;
			}

			ErrorString.clear();
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

			if (!Settings.SaveSettings()) {
				SetErrorAndUnlock(kSaveSettingsError);
				return;
			}

			if (!KeyMgr.SaveKeybinds()) {
				SetErrorAndUnlock(kSaveInputError);
				return;
			}

			ErrorString.clear();
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
	    flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar  | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoScrollbar;

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
	}

	void WindowSettings::Draw() {

	    auto& Categories = CatMgr.GetCategories();
		ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kFooter);
	    const float Footer = ImGui::GetFrameHeightWithSpacing() + (ImGui::GetStyle().ItemSpacing.y * 4.0);  // text + separator
		ImFontManager::PopActiveFont();
	    
	    //Calc Button Width
	    std::array<const char*,3> Lables = { "Load", "Save", "Reset" };
	    const ImGuiStyle& Style = ImGui::GetStyle();


	    //Update Window Flags
	    flags = (sUI.bLock ? (flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove) : (flags & ~ImGuiWindowFlags_NoResize & ~ImGuiWindowFlags_NoMove));

	    //Handle Fixed Position and Size
	    if(sUI.bLock){
	        ImGui::SetWindowSize(ImUtil::ScaleToViewport(sUI.fWindowSize));

	        //Mousedown Check Prevents the window from moving around and messing with the slider while dragging
	        if(!ImGui::GetIO().MouseDown[0]){
	            //X,Y
	            const ImVec2 Offset {sUI.f2Offset[0], sUI.f2Offset[1]};
	            ImGui::SetWindowPos(GetAnchorPos(StringToEnum<ImWindow::WindowAnchor>(sUI.sAnchor), Offset, false));
	        }
	    }

		float TotalWidth = 0.0f;

		if (!Config::GetUI().bEnableAutoSaveOnClose) {
			//Save
			TotalWidth += Style.ItemSpacing.x + 2;
			TotalWidth += (ImGui::CalcTextSize(Lables[1]).x + 2.0f * Style.FramePadding.x);
		}

		if (!Config::GetAdvanced().bHideLoadButton) {
			//Load
			TotalWidth += Style.ItemSpacing.x + 2; // Add Seperator offset
			TotalWidth += (ImGui::CalcTextSize(Lables[0]).x + 2.0f * Style.FramePadding.x);
		}

		//Reset
		//TotalWidth += (ImGui::CalcTextSize(Lables[3]).x + 2.0f * Style.FramePadding.x) + Style.ItemSpacing.x;

		const float ButtonStartX = ImGui::GetWindowWidth() - (TotalWidth + Style.WindowPadding.x);

		//While mathematically correct 2.0 Just doesn't look right...
		const float TextCenter = ButtonStartX / 2.0f - ImGui::CalcTextSize(ErrorString.c_str()).x / 2.5f;

		const auto OldPos = ImGui::GetCursorPos();

		{

			ImVec2 pos = ImVec2(ImGui::GetContentRegionAvail().x - (ImGui::GetStyle().FramePadding.x * 2 + ImGui::GetStyle().CellPadding.x), ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetStyle().CellPadding.y);
			ImGui::SetCursorPos(pos);

			// Create the button
			if (ImUtil::Button(" X ")) {
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

	        ImGui::BeginChild("Sidebar", ImVec2(CatMgr.GetLongestCategory(), ImGui::GetContentRegionAvail().y - Footer), true);
	        ImGui::BeginDisabled(Disabled);
			ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kSidebar);

	        // Display the categories in the sidebar
	        for (uint8_t i = 0; i < static_cast<uint8_t>(Categories.size()); i++) {
	            ImCategory* category = Categories[i].get();

	            //If nullptr / invisible / or dbg category, Do not draw.

	            if(!category) continue;
	            if(!sHidden.IKnowWhatImDoing && category->GetTitle() == "Advanced") continue;
	            if(!category->IsVisible()) continue;

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

	        ImGui::BeginChild("Content", ImVec2(0, ImGui::GetContentRegionAvail().y - Footer), true); // Remaining width

	        // Validate selectedCategory to ensure it's within bounds
	        if (CatMgr.activeIndex < Categories.size()) {
	            ImCategory* selected = Categories[CatMgr.activeIndex].get();
	            selected->Draw(); // Call the Draw method of the selected category
	        } 
	        else {
	            ImGui::TextColored(ImUtil::ColorError,"Invalid category or no categories exist!");
	        }

	        ImGui::EndChild();
	    }


	    ImUtil::SeperatorH();
	    ImGui::BeginDisabled(Disabled);
	    
	    {   //Footer - Mod Info

			ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kSubText);

			const std::string FooterMessage = fmt::format(
			 "GTSPlugin {}\n"
			 "Build Date: {} {}\n"
			 "{}",
			 GTSPlugin::ModVersion.string(),
			 __DATE__,
			 __TIME__,
			git::AnyUncommittedChanges() ? "Development Version" : fmt::format("SHA1 {}",git::CommitSHA1()));

	        ImGui::TextColored(ImUtil::ColorSubscript, FooterMessage.c_str());
			ImFontManager::PopActiveFont();
	    }

	    ImGui::SameLine(TextCenter);

	    {   
	        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - (Footer / 2.0f) - Style.FramePadding.y);
			ImFontManager::PushActiveFont(ImFontManager::ActiveFontType::kError);
	        ImGui::PushStyleColor(ImGuiCol_Text,ImUtil::ColorError);
	        ImGui::Text(ErrorString.c_str());
	        ImGui::PopStyleColor();
			ImFontManager::PopActiveFont();
	    }

	    ImGui::SameLine(ButtonStartX);
		
	    
	    {   //-------------  Buttons
	        
	        volatile bool buttonstate = SaveLoadBusy.load();

			if (!Config::GetUI().bEnableAutoSaveOnClose) {

				//Save
				if (ImUtil::Button(Lables[1], "Save changes", buttonstate, 1.3f)) {
					AsyncSave();
				}

				ImGui::SameLine();

			}

			if (!Config::GetAdvanced().bHideLoadButton) {

				//Load
				if (ImUtil::Button(Lables[0], "Reload and apply the values currenly stored in Settings.toml and Input.toml", buttonstate, 1.3f)) {
					AsyncLoad();
				}
			}

	    }
	    ImGui::EndDisabled();
	}
}
