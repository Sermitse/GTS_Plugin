#include "UI/Categories/Keybinds.hpp"
#include "UI/ImGui/Lib/imgui.h"
#include "UI/imGui/ImUtil.hpp"
#include "UI/ImGui/Lib/imgui_stdlib.h"
#include "UI/ImGui/ImInput.hpp"
#include "UI/Windows/WindowSettings.hpp"

namespace GTS {

	void CategoryKeybinds::Draw() {

		//New Render Loop. Reset Index.
		CurEventIndex = 65535;

		//Calc the correct width
		Width = ImGui::GetContentRegionAvail().x - ((ImGui::GetStyle().CellPadding.x * 2 + ImGui::GetStyle().FramePadding.x * 2) * Div);

		//Draw top bar
		DrawOptions();

		//Draw the list containing each input event.
		DrawContent();
	}

	void CategoryKeybinds::DrawOptions() {

		const char* T0 = "Filter based on an actions' name.";

		ImGui::BeginChild("Options", { -FLT_MIN, 0.0f }, ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_FrameStyle);

		ImGui::BeginDisabled(RebindIndex > 0);

		{
			ImGui::InputText("Search", &SearchRes);
			ImUtil::Tooltip(T0);
		}

		ImGui::SameLine();

		if (ImUtil::Button("Reset All", "Reset all keybinds and their settings to default.")) {
			KeyMgr.ResetKeybinds();
		}

		ImGui::SameLine();

		static bool constinit singleColumn = false;
		ImUtil::CheckBox("Single Column", &singleColumn, "List all keybinds in a single column");
		Div = 2 - singleColumn;

		ImGui::EndDisabled();
		ImGui::EndChild();
	}

	void CategoryKeybinds::DrawContent() {

		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 4.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 4.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 6.0f));

		ImGui::BeginChild("InputEvents", ImVec2(0, 0));

		static const auto categoryMap = []() {
			std::unordered_map<std::string, InputCategory> m;
			for (auto const& ce : GTS::DefaultEvents) {
				m.emplace(ce.Event.Event, ce.Category);
			}
			return m;
		}();

		std::unordered_map<InputCategory, std::vector<GTSInputEvent*>> groups;
		for (auto& ev : KeyMgr.InputEvents) {

			// filter by search
			if (!ImUtil::ContainsString(ImUtil::HumanizeString(ev.Event), SearchRes)) {
				continue;
			}

			// find its category (default to kDefault if missing)
			auto it = categoryMap.find(ev.Event);
			InputCategory cat = (it != categoryMap.end())? it->second : InputCategory::kDefault;

			groups[cat].push_back(&ev);
		}

		for (auto cat : magic_enum::enum_values<InputCategory>()) {
			auto git = groups.find(cat);
			if (git == groups.end() || git->second.empty()) {
				continue;
			}

			
			std::string catName = ImUtil::HumanizeString(std::string{ magic_enum::enum_name(cat) });

			//ImGui::NewLine();
			ImGui::PushFont(nullptr, 24.0f);
			ImGui::SeparatorText(catName.c_str());
			ImGui::PopFont();

			// split into Div columns
			auto& list = git->second;
			std::vector<std::vector<GTSInputEvent*>> Columns(Div);
			for (size_t i = 0; i < list.size(); ++i) {
				Columns[i % Div].push_back(list[i]);
			}

			// measure widest name
			std::vector ColumnNameWidths(Div, 0.0f);
			for (int c = 0; c < Div; ++c) {
				for (auto evt : KeyMgr.InputEvents) {
					float w = ImGui::CalcTextSize(ImUtil::HumanizeString(evt.Event).c_str()).x;
					ColumnNameWidths[c] = std::max(ColumnNameWidths[c], w);
				}
			}

			// render child for each column
			for (int c = 0; c < Div; ++c) {

				if (c > 0) {
					ImUtil::SeperatorV();
					ImGui::SameLine(0, 24.0f);
				}

				ImGui::BeginChild(static_cast<ImGuiID>((__COUNTER__ | (static_cast<int>(cat) << 4) | c)),
					{ Width / Div, 0 },
					ImGuiChildFlags_AlwaysAutoResize |
					ImGuiChildFlags_AutoResizeY |
					ImGuiChildFlags_AutoResizeX
				);

				for (auto* ptr : Columns[c]) {
					DrawInputEvent(*ptr, ImUtil::HumanizeString(ptr->Event), ColumnNameWidths[c]);
				}

				ImGui::EndChild();
			}

		}

		// If nothing printed at all
		bool any = ranges::any_of(groups, [](auto& kv) {
			return !kv.second.empty();
		});

		if (!any) {
			ImGui::Text("No results matching search string.");
		}

		ImGui::PopStyleVar(3);

		ImGui::EndChild();
	}

	bool CategoryKeybinds::DrawInputEvent(GTSInputEvent& Event, const std::string& a_name, float columnNameWidth) {

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
			"Pressing \"Rebind\" Will allow you to enter a new key combination for this action.\n"
			"You don't have to hold down the keys if creating a key combination. Pressing a key once will append it to the list\n."
			"After entering the new key combination press \"Confirm\" to apply it.\n\n";

		const char* T6 = "Click to open advanced settings for this keybind.";

		ImGui::BeginChild(CurEventIndex++, { Width / Div, 0.0f }, ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX);

		const bool IsRebinding = RebindIndex == CurEventIndex && RebindIndex != 0;

		ImGui::BeginDisabled(RebindIndex != CurEventIndex && RebindIndex != 0);

		// Draw the keybind as a horizontal layout with proper column sizing

		const std::string ButtonText = IsRebinding ? "Confirm" : "Rebind";

		constexpr float keyInputWidth = 230.0f; //TODO Unhardcode
		const float buttonWidth = ImGui::CalcTextSize(ButtonText.c_str()).x + ImGui::GetStyle().FramePadding.x + 2.0f;
		const float optionsButtonWidth = ImGui::CalcTextSize("Options").x + ImGui::GetStyle().FramePadding.x;
		const float padding = ImGui::GetStyle().ItemSpacing.x + ImGui::GetStyle().FramePadding.x;

		ImGui::BeginTable("KeybindRow", 4, ImGuiTableFlags_SizingFixedFit);
		{
			// Setup column widths
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, columnNameWidth + padding);
			ImGui::TableSetupColumn("Keys", ImGuiTableColumnFlags_WidthFixed, keyInputWidth);
			ImGui::TableSetupColumn("Rebind", ImGuiTableColumnFlags_WidthFixed, buttonWidth);
			ImGui::TableSetupColumn("Options", ImGuiTableColumnFlags_WidthFixed, optionsButtonWidth);

			// Column 1: Action Name
			ImGui::TableNextColumn();
			ImGui::AlignTextToFramePadding();

			ImGui::PushFont(nullptr, 21.0f);
			{
				// Apply disabled styling if the event is disabled
				if (Event.Disabled) {
					//Slightly Increase FontSize
					ImGui::PushStyleColor(ImGuiCol_Text, ImUtil::ColorDisabled); // Red color for disabled
					ImGui::Text("%s", a_name.c_str());
					ImGui::PopStyleColor();
				}
				else {
					ImGui::Text("%s", a_name.c_str());
				}
			}
			ImGui::PopFont();

			// Column 2: Current Keys
			ImGui::TableNextColumn();

			VisualKeyString = "";
			const std::vector<std::string>& VisualKeyList = IsRebinding ? TempKeys : Event.Keys;
			const int NumOfKeys = static_cast<int>(VisualKeyList.size());

			for (int i = 0; i < NumOfKeys; i++) {
				VisualKeyString += VisualKeyList[i];
				if (i != NumOfKeys - 1) {
					VisualKeyString += " + ";
				}
			}

			std::string InputText = VisualKeyString.empty() ? "Press any Key or ESC To Cancel" : VisualKeyString;

			ImGui::SetNextItemWidth(keyInputWidth);
			ImGui::BeginDisabled(Event.Disabled);
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::InputText("##KeyRebind", &InputText, ImGuiInputTextFlags_ReadOnly);
			ImGui::PopItemFlag();
			ImGui::EndDisabled();

			// Column 3: Rebind Button
			ImGui::TableNextColumn();

			if (auto MainWindow = dynamic_cast<WindowSettings*>(ImWindowManager::GetSingleton().GetWindowByName("Settings"))) {
				MainWindow->SetDisabled(RebindIndex > 0);
			}

			// Set the busy flag so UIMain does not close the window when we press esc
			SetWindowBusy(RebindIndex > 0);

			ImGui::BeginDisabled((TempKeys.empty() && IsRebinding) || Event.Disabled);
			ImGui::SetNextItemWidth(buttonWidth);

			if (ImUtil::Button(ButtonText.c_str(), T5)) {
				// Start new key rebind for this index.
				RebindIndex = CurEventIndex;

				if (IsRebinding) {
					if (!TempKeys.empty()) {
						// Save new keybind
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

			// Column 4: Options Button
			ImGui::TableNextColumn();

			const std::string OptionsButtonID = "Options##" + std::to_string(CurEventIndex);
			ImGui::SetNextItemWidth(optionsButtonWidth);
			if (ImUtil::Button("Options", T6)) {
				ImGui::OpenPopup(OptionsButtonID.c_str());
			}

			// Options Popup
			if (ImGui::BeginPopup(OptionsButtonID.c_str())) {
				ImGui::Text("Advanced Settings: %s", a_name.c_str());
				ImGui::Separator();

				ImGui::PushItemWidth(200.0f);

				ImUtil::CheckBox("Disabled", &Event.Disabled, T0);

				if (!Event.Disabled) {
					ImGui::BeginDisabled(IsRebinding);
					ImUtil::CheckBox("Exclusive", &Event.Exclusive, T1);
					ImUtil::ComboEx<TriggerType>("Trigger Type", Event.Trigger, T2);
					ImUtil::ComboEx<BlockInputTypes>("Block Input", Event.BlockInput, T3);

					ImGui::InputFloat("Trigger After", &Event.Duration, 0.1f, 0.01f, "%.2f Seconds");
					ImUtil::Tooltip(T4);
					Event.Duration = std::clamp(Event.Duration, 0.0f, 10.0f);
					ImGui::EndDisabled();
				}

				ImGui::PopItemWidth();
				ImGui::EndPopup();
			}
		}
		ImGui::EndTable();

		// Handle rebinding logic
		if (IsRebinding) {

			// If Escape Stop Rebind
			if (ImGui::IsKeyReleased(ImGuiKey_Escape)) {
				RebindIndex = 0;
				TempKeys.clear();
			}

			// Check if mouse is hovering over any interactive elements to block input
			else if (!(ImGui::IsAnyItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))) {

				// Go though all the imgui keys.
				for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key++) {

					if (key == ImGuiKey_Escape) continue;

					// Detect a new key press
					if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(key))) {

						// Convert the key to a string representation.
						std::string keyName = ImGuiKeyToDIKString(static_cast<ImGuiKey>(key));
						if (keyName == "INVALID") continue;

						// Only append if it's not already in TempKeys.
						if (ranges::find(TempKeys, keyName) == TempKeys.end() && TempKeys.size() < 5) {
							TempKeys.push_back(keyName);
						}

						// Sort TempKeys in descending order based on string length.
						ranges::sort(TempKeys, [](const std::string& a, const std::string& b) {
							return a.size() > b.size();
						});
					}
				}
			}
		}

		ImGui::EndDisabled();
		ImGui::EndChild();

		return true;
	}
}