#include "UI/Categories/Keybinds.hpp"
#include "UI/ImGui/Lib/imgui.h"
#include "UI/imGui/ImUtil.hpp"
#include "UI/ImGui/Lib/imgui_stdlib.h"
#include "UI/ImGui/ImInput.hpp"
#include "UI/Windows/WindowSettings.hpp"

namespace GTS {

	void CategoryKeybinds::Draw(){

	    //New Render Loop. Reset Index.
	    //Why is it 1000? Why not.
		//Well allright ill tell you why. It's to make sure the ID pushed to imgui is sufficiently unique
		//Unless we end up with 1000 calls to the imutil_unique wrapper that is...
	    CurEventIndex = 1000;

	    //Calc the correct width
	    Width = ImGui::GetContentRegionAvail().x - ((ImGui::GetStyle().CellPadding.x * 2 + ImGui::GetStyle().FramePadding.x * 2) * Div);
	    
	    //Draw top bar
	    DrawOptions();

	    ImUtil::SeperatorH();

	    //Draw the columns containing each input event.
	    DrawContent();

	    //Reset Collapse/Expand Flag.
	    ColExpState = 0;
	}

	void CategoryKeybinds::DrawOptions(){

	    const char* T0 = "Collapse all currently visible input actions.";
	    const char* T1 = "Expand all currently visible input actions.";
	    const char* T2 = "Filter based on an actions' name.";
	    const char* T3 = "Divide the action list in columns.";

	    ImGui::BeginChild("Options", {-FLT_MIN, 0.0f}, ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY);
	    
	    ImGui::BeginDisabled(RebindIndex > 0);

	    if(ImUtil::Button("Collapse All",T0)){
	        ColExpState = 1;
	    }

	    ImGui::SameLine();
	    
	    if(ImUtil::Button("Expand All",T1)){
	        ColExpState = 2;
	    }

	    ImUtil::SeperatorV();

	    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 8.0f);

	    {
	        ImGui::InputText("Search", &SearchRes);
	        ImUtil::Tooltip(T2);
	    }

	    ImGui::SameLine();

	    {
	        ImGui::InputInt("Columns", &Div, 1, 1);
	        ImUtil::Tooltip(T3);
	        Div = std::clamp(Div,1,4);
	    }

	    ImGui::PopItemWidth();
	    ImGui::EndDisabled();
	    ImGui::EndChild();
	    

	}

	void CategoryKeybinds::DrawContent(){

	    //4 For Loops just to display the input events in a table...
	    ImGui::BeginChild("InputEvents", ImVec2(0, 0));

	    // Build a vector of pointers for events that pass the search filter.
	    std::vector<GTSInputEvent*> VisibleEvents;
	    for (auto& Event : KeyMgr.InputEvents){
	        if (ImUtil::ContainsString(ImUtil::HumanizeString(Event.Event), SearchRes))
	        VisibleEvents.push_back(&Event);
	    }

	    // Now distribute the filtered events evenly across columns.
	    std::vector<std::vector<GTSInputEvent*>> Columns(Div);
	    for (size_t i = 0; i < VisibleEvents.size(); i++){
	        Columns[i % Div].push_back(VisibleEvents[i]);
	    }

	    //Keep a list of column indeces that have items in them.
	    std::vector<int> NonEmptyIndices;
	    for (int i = 0; i < Div; i++) {
	        if (!Columns[i].empty()) {
	            NonEmptyIndices.push_back(i);
	        }
	    }

	    //If we have items to display...
	    if (!NonEmptyIndices.empty()){

	        // Draw child windows for each column.
	        for (int i = 0; i < Div; i++){
	            
	            // Draw a separator from the second column up to if the subsequent column exists.
	            // TBH I have no idea how i managed to make this work...
	            if (i > 0 && NonEmptyIndices.size() - i > 0){
	                ImUtil::SeperatorV();
	            }
	            
	            ImGui::BeginChild(i + 1, {0, 0}, this->HeaderFlags);
	            
	            //Draw each presorted input event.
	            for (GTSInputEvent* Event : Columns[i]){
	                DrawInputEvent(*Event, ImUtil::HumanizeString(Event->Event));
	            }

	            ImGui::EndChild();
	        }
	    }
	    else{
	        ImGui::Text("No results matching search string.");
	    }

	    ImGui::EndChild();
	}

	bool CategoryKeybinds::DrawInputEvent(GTSInputEvent& Event, const std::string& a_name){

	    const char* T0 = "Disable this input event.\n"
	                     "Disabled events are completely ignored by the game and will never trigger.";

	    const char* T1 = "When an action is marked as exclusive it means it will only activate if its exact key combination is being pressed.\n"
	                     "(eg. If an action requires ALT+E to activate and you're also holding W while trying to trigger it with this flag set, nothing will happen unless you stop pressing W.)";

	    const char* T2 = "The action trigger type modifies the activation behavior for an action.\n\n"
	                     "- Once: Trigger an action once upon pressing its key combo.\n"
	                     "- Release: The action will only trigger when you release its keys after pressing them.\n"
	                     "- Continuous: As long as you are holding down the key combination the action event will be fired every game frame.";
	    
	    const char* T3 = "Normaly when you press a key combo. whatever keys you are holding down are sent to the mod and the game at the same time\n"
	                     "Depending on what keys you press this may have undesired effects. Which is why this option exists.\n\n"
	                     "- Automatic: Prevent the game from reading the pressed action keys only when said GTS action would be valid. (eg. When you have the relevant perk/the action is possible to do).\n"
	                     "  (NOTE: Some actions are not compatible with this setting. These are by default set to \"Never\" On purpose.)\n"
	                     "- Never: Never prevent the game from reading the pressed keys for this action even if the action would be valid.\n"
	                     "- Always: Will always prevent the game from reading this key combination regardless if the action would trigger/do something or not.";

	    const char* T4 = "This adds a time delay before an action gets triggerd if its keys are pressed.\n"
	                     "(eg. if the trigger type is once and this value is set to 1.0 you'd need to hold the correct key combination for atleast 1 second before this event's action will fire.)";
	    
		const char* T5 = "Here you can see the current key combination required to trigger an action as well as modify it.\n"
			"Pressing \"Rebind Action\" Will allow you to enter a new key combination for this action.\n"
			"You don't have to hold down the keys if creating a key combination. Pressing a key once will append it to the list\n."
			"After entering the new key combination press \"Confirm\" to apply it.\n\n"
			"Note: You NEED to press the \"Save\" button at the bottom if you have autosave disabled to be able to make use of the changes you've made";
	                    

	    ImGui::BeginChild(CurEventIndex++, {Width/Div, 0.0f}, ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX);

	    { // --  CollapsingHeader Control Begin
	        const bool IsRebinding = RebindIndex == CurEventIndex && RebindIndex != 0;

	        ImGui::BeginDisabled(RebindIndex != CurEventIndex && RebindIndex != 0);

	        //Not the most elegant solution but it works...
	        bool* HeaderState = nullptr;
	        if (auto Res = HeaderStateMap.find(Event.Event); Res != HeaderStateMap.end()){
	            HeaderState = &Res->second;
	        }

	        if(ColExpState != 0){
	            ImGui::SetNextItemOpen(ColExpState - 1);
	        }
	        else{
	            if(HeaderState)
	            ImGui::SetNextItemOpen(*HeaderState);
	        }

			const std::string VisualName = Event.Disabled ? fmt::format("{} [Disabled]", a_name) : a_name;
			volatile bool PushedStyle = false;

			//Too lazy to make this look better
	    	if (Event.Disabled) {
				const float Alpha = ImGui::GetStyle().Colors[ImGuiCol_Header].w;
				ImGui::PushStyleColor(ImGuiCol_Header, { 0,0,0, Alpha });
				ImGui::PushStyleColor(ImGuiCol_HeaderActive, { 0,0,0, Alpha });
				PushedStyle = true;
			}

	        if(ImGui::CollapsingHeader(VisualName.c_str())){

	            if (HeaderState){
	                *HeaderState = true;
	            }

	            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
	        	ImUtil::CheckBox("Disabled", &Event.Disabled, T0);

	            //If Disabled: Don't draw at all.
	            if(!Event.Disabled){

	                {   //-- Basic Controls
						ImGui::BeginDisabled(IsRebinding);
						ImUtil::CheckBox("Exclusive", &Event.Exclusive, T1);
						ImUtil::ComboEx<TriggerType>("Trigger Type", Event.Trigger, T2);
						ImUtil::ComboEx<BlockInputTypes>("Block Input", Event.BlockInput, T3);

						ImGui::InputFloat("Trigger After", &Event.Duration, 0.1f, 0.01f, "%.2f Seconds");
						ImUtil::Tooltip(T4);
						Event.Duration = std::clamp(Event.Duration, 0.0f, 10.0f);
						ImGui::EndDisabled();
						
	                }

	                {   //-- UI CODE

	                    VisualKeyString = "";

	                    const std::vector<std::string>& VisualKeyList = IsRebinding ? TempKeys : Event.Keys;

	                    const int NumOfKeys = static_cast<int>(VisualKeyList.size());

	                    for (int i = 0; i < NumOfKeys; i++){
	                        VisualKeyString +=  VisualKeyList[i];

	                        if(i != NumOfKeys - 1){
	                            VisualKeyString += " + ";
	                        }
	                    }
	            
	                    const std::string ButtonText = IsRebinding ? "Confirm" : "Rebind Action";
	                    std::string InputText = VisualKeyString.empty() ? "Press any Key(s) or ESC To Cancel" : VisualKeyString;
	                    constexpr float Pad = 4.0f;

	                    {   //---- Draw Keybind Textbox

	                        //Disable control wihtout graying it out... This api will break in the future...
	                        //As of v1.91.9 it works as expected.

	                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	                        ImGui::InputText("##KeyRebind", &InputText, ImGuiInputTextFlags_ReadOnly);
	                        ImGui::PopItemFlag();
	                    }


	                    ImGui::SameLine(0.f, Pad);

	                    if (auto MainWindow = dynamic_cast<WindowSettings*>(ImWindowManager::GetSingleton().GetWindowByName("Settings"))){
							MainWindow->SetDisabled(RebindIndex > 0);
	                    }

						//Set the busy flag so UIMain does not close the window when we press esc
						SetWindowBusy(RebindIndex > 0);
	                    
	                    ImGui::BeginDisabled((TempKeys.empty() && IsRebinding) );
	                    if(ImUtil::Button(ButtonText.c_str(),T5)){
	                        //Start new key rebind for this index.
	                        RebindIndex = CurEventIndex;

							if (IsRebinding) {
								if (!TempKeys.empty()) {
									//Save new keybind
									Event.Keys = TempKeys;
									TempKeys.clear();
									RebindIndex = 0;
								}
								else {
									RebindIndex = 0;
								}
							}
	                    }
	                    ImGui::EndDisabled();
	                }

	                {   //-- PARSE LOGIC

	                    //If Escape Stop Rebind
	                    if(ImGui::IsKeyReleased(ImGuiKey_Escape)){
	                        RebindIndex = 0;
	                        TempKeys.clear();
	                    }

	                    //IsItemHovered References the last control drawn in this case its the Button.
	                    //If the mouse is hovering the button ignore its left click inputs.
	                    else if (IsRebinding && !(ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))){

	                        //Go though all the imgui keys.
	                        for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key++){

								if (key == ImGuiKey_Escape) continue;

	                            // Detect a new key press
	                            if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(key))){

	                                // Convert the key to a string representation.
	                                std::string keyName = ImGuiKeyToDIKString(static_cast<ImGuiKey>(key));
	                                if(keyName == "INVALID") continue;

	                                // Only append if it's not already in TempKeys.
	                                if (ranges::find(TempKeys, keyName) == TempKeys.end() && TempKeys.size() < 5){
	                                    TempKeys.push_back(keyName);
	                                }

	                                // Sort TempKeys in descending order based on string length.
	                                ranges::sort(TempKeys, [](const std::string &a, const std::string &b) {
	                                    return a.size() > b.size();
	                                });
	                            }
	                        }
	                    }
	                }
	            }
	            ImGui::Spacing();
	            ImGui::PopItemWidth();
	        }
	        else{
	            if (HeaderState){
	                *HeaderState = false;
	            }
	        }

			if (PushedStyle) {
				ImGui::PopStyleColor(2);
			}
				
	        
	        ImGui::EndDisabled();
	    }


	    ImGui::EndChild();
	    return true;
	}
}