#include "UI/ImGui/Windows/Settings/Categories/Keybinds.hpp"
#include "UI/ImGui/Windows/Settings/SettingsWindow.hpp"

#include "UI/ImGui/Controls/Button.hpp"
#include "UI/ImGui/Controls/CheckBox.hpp"
#include "UI/ImGui/Controls/ComboBox.hpp"
#include "UI/ImGui/Controls/Misc.hpp"
#include "UI/ImGui/Controls/ToolTip.hpp"

#include "UI/imGui/Core/ImUtil.hpp"
#include "UI/ImGui/Core/ImInput.hpp"

#include "UI/ImGui/Lib/imgui_internal.h"
#include "UI/ImGui/Lib/imgui_stdlib.h"
#include "UI/ImGui/Lib/imgui.h"

#include "Config/Keybinds.hpp"

#include "Config/Config.hpp"

#include "UI/GTSMenu.hpp"
#include "UI/ImGui/Controls/Text.hpp"

namespace {

	PSString T0 = "Disable this input event.\n"
		          "Disabled events are completely ignored by the game and will never trigger.";

	PSString T1 = "When an action is marked as exclusive it means it will only activate if its exact key combination is being pressed.\n"
		          "(eg. If an action requires ALT+E to activate and you're also holding W while trying to trigger it with this flag set, nothing will happen unless you stop pressing W.)";

	PSString T2 = "The action trigger type modifies the activation behavior for an action.\n\n"
		          "- Once: Trigger an action once upon pressing its key combo.\n"
		          "- Release: The action will only trigger when you release its keys after pressing them.\n"
		          "- Continuous: As long as you are holding down the key combination the action event will be fired every game frame.";

	PSString T3 = "Normaly when you press a key combo. whatever keys you are holding down are sent to the mod and the game at the same time\n"
		          "Depending on what keys you press this may have undesired effects. Which is why this option exists.\n\n"
		          "- Automatic: Prevent the game from reading the pressed action keys only when said GTS action would be valid. (eg. When you have the relevant perk/the action is possible to do).\n"
		          "  (NOTE: Some actions are not compatible with this setting. These are by default set to \"Never\" On purpose.)\n"
		          "- Never: Never prevent the game from reading the pressed keys for this action even if the action would be valid.\n"
		          "- Always: Will always prevent the game from reading this key combination regardless if the action would trigger/do something or not.";

	PSString T4 = "This adds a time delay before an action gets triggerd if its keys are pressed.\n"
		          "(eg. if the trigger type is once and this value is set to 1.0 you'd need to hold the correct key combination for atleast 1 second before this event's action will fire.)";

	PSString T5 = "Change the key combination to trigger this event.\n"
		          "You don't have to hold down the keys if creating a key combination. Pressing a key once will append it to the list\n"
		          "After entering the new key combination press this button again to save it.\n"
		          "Pressing ESC will cancel the keybind reassignment.";

	PSString T6 = "Click to open advanced settings for this keybind.";


    PSString TH0 = "Filter based on an actions' name.";
    PSString TH1 = "Reset all keybinds and their settings to default.";

}

namespace GTS {

	CategoryKeybinds::CategoryKeybinds() {
		m_name = "Keybinds";
		for(auto& e : DefaultEvents){
			HeaderStateMap.emplace(e.Event.Event,false);
		}
	}

	void CategoryKeybinds::Draw() {

		//New Render Loop. Reset Index.
		CurEventIndex = UINT16_MAX;

		//Calc the correct width
		Width = ImGui::GetContentRegionAvail().x - ((ImGui::GetStyle().CellPadding.x * 2 + ImGui::GetStyle().FramePadding.x * 2) * Div);

		//Draw top bar
		DrawOptions();

		//Draw the list containing each input event.
		DrawContent();
	}

	void CategoryKeybinds::DrawOptions() {

        ImGui::SetNextWindowBgAlpha(0.1f);

		ImGui::BeginChild("Options", { -FLT_MIN, 0.0f }, ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_FrameStyle);
        {
            ImGui::BeginDisabled(RebindIndex > 0);
            {
                {
                    ImGui::InputText("Search", &SearchRes);
                    ImGuiEx::Tooltip(TH0);
                }

                ImGui::SameLine();

                static bool constinit singleColumn = false;
                ImGuiEx::CheckBox("Compact", &singleColumn, "List all keybinds in a single column");
                Div = 2 - singleColumn;

                ImGuiEx::SeperatorV();

                // Right-align the Reset button
                float avail = ImGui::GetContentRegionAvail().x;
                float buttonWidth = 18.0f + ImGui::GetStyle().FramePadding.x * 2;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + avail - buttonWidth);
                if (ImGuiEx::ImageButton("Reset", ImageList::Generic_Reset, 18, TH1)) {
                    Keybinds::ResetKeybinds();
                }

            }
			ImGui::EndDisabled();
        }
		ImGui::EndChild();
	}

	void CategoryKeybinds::DrawContent() {

		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 4.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 4.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 6.0f));

		ImGui::BeginChild("InputEvents", ImVec2(0, 0));
        {

            static const auto categoryMap = [] {
                std::unordered_map<std::string, LInputCategory_t> m;
                for (auto const& ce : DefaultEvents) {
                    m.emplace(ce.Event.Event, ce.UICategory);
                }
                return m;
            }();

            static const auto hiddenMap = [] {
                std::unordered_map<std::string, bool> m;
                for (auto const& ce : DefaultEvents) {
                    m.emplace(ce.Event.Event, ce.UIHidden);
                }
                return m;
            }();

            std::unordered_map<LInputCategory_t, std::vector<BaseEventData_t*>> groups;

            for (auto& ev : Keybinds::InputEvents) {

                // Check if hidden (skip if not in advanced mode)
                if (!Config::Hidden.IKnowWhatImDoing) {
                    auto hit = hiddenMap.find(ev.Event);
                    if (hit != hiddenMap.end() && hit->second) {
                        continue;
                    }
                }

                // filter by search
                if (!ContainsString(HumanizeString(ev.Event), SearchRes)) {
                    continue;
                }

                // find its category (default to kDefault if missing)
                auto it = categoryMap.find(ev.Event);
                LInputCategory_t cat = (it != categoryMap.end()) ? it->second : LInputCategory_t::kDefault;

                groups[cat].push_back(&ev);
            }

            for (auto cat : magic_enum::enum_values<LInputCategory_t>()) {

                auto git = groups.find(cat);
                if (git == groups.end() || git->second.empty()) {
                    continue;
                }

                const std::string catName = HumanizeString(std::string{ magic_enum::enum_name(cat) });

                ImGui::PushFont(nullptr, 24.0f);
                {
                    ImGui::SeparatorText(catName.c_str());
                }
                ImGui::PopFont();

                // split into Div columns
                auto& list = git->second;
                std::vector<std::vector<BaseEventData_t*>> Columns(Div);
                for (size_t i = 0; i < list.size(); ++i) {
                    Columns[i % Div].push_back(list[i]);
                }

                // measure widest name
                std::vector ColumnNameWidths(Div, 0.0f);
                for (int c = 0; c < Div; ++c) {
                    for (auto evt : Keybinds::InputEvents) {

                        ImGui::PushFont(nullptr, 21.0f);
                        {
                            float w = ImGui::CalcTextSize(HumanizeString(evt.Event).c_str()).x;
                            ColumnNameWidths[c] = std::max(ColumnNameWidths[c], w);
                        }
                        ImGui::PopFont();

                    }
                }

                // render child for each column
                for (int c = 0; c < Div; ++c) {

                    if (c > 0) {
                        ImGuiEx::SeperatorV();
                        ImGui::SameLine(0, 24.0f);
                    }

                    ImGui::BeginChild(static_cast<ImGuiID>(__COUNTER__ | static_cast<int>(cat) << 4 | c), { Width / Div, 0 },  ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX);
                    {

                        for (auto* ptr : Columns[c]) {
                            DrawInputEvent(*ptr, HumanizeString(ptr->Event), ColumnNameWidths[c]);
                        }
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
        }
		ImGui::EndChild();
	}

	void CategoryKeybinds::SetWindowBusy(const bool a_busy) {
		if (SettingsWindow* Window = dynamic_cast<SettingsWindow*>(GTSMenu::WindowManager->wSettings)) {
			Window->m_busy = a_busy;
            Window->m_disableUIInteraction = a_busy;
		}
	}

	bool CategoryKeybinds::DrawInputEvent(BaseEventData_t& Event, const std::string& a_name, float columnNameWidth) {

        const float ButtonImageSize = 18 * ImGui::GetStyle().FontScaleMain;
        const float ButtonSize = ButtonImageSize + ImGui::GetStyle().ItemSpacing.x + (ImGui::GetStyle().FramePadding.x * 2.0f);
        const bool IsRebinding = (RebindIndex == CurEventIndex && RebindIndex != 0);

        // Extra padding between name and key input
        const float nameToKeyPadding = ImGui::GetStyle().FramePadding.x * 2 + ImGui::GetStyle().ItemSpacing.x * 2;

        // Get available region width inside the child
        float contentWidth = ImGui::GetContentRegionAvail().x - ImGui::GetStyle().FramePadding.x * 2 - ImGui::GetStyle().CellPadding.x * 2;
        float nameColWidth = columnNameWidth + nameToKeyPadding;

        // Keys column takes remaining space
        float keysColWidth = contentWidth - nameColWidth - (ButtonSize * 2);

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));

        ImGui::BeginChild(CurEventIndex, { Width / Div, 0.0f }, ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX);
		{
        	ImGui::BeginDisabled(RebindIndex != CurEventIndex && RebindIndex != 0);
            {
                ImGui::BeginTable("KeybindRow", 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoPadInnerX);
                {
                    // Setup column widths
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, nameColWidth);
                    ImGui::TableSetupColumn("Keys", ImGuiTableColumnFlags_WidthFixed, keysColWidth);
                    ImGui::TableSetupColumn("Rebind", ImGuiTableColumnFlags_WidthFixed, ButtonSize);
                    ImGui::TableSetupColumn("Options", ImGuiTableColumnFlags_WidthFixed, ButtonSize);

                    // Column 1: Action Name
                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();

                    ImGui::PushFont(nullptr, 21.0f);
                    {
                    	ImGuiEx::TextColorShadow(Event.Disabled ? ImUtil::Colors::Message : ImGui::GetStyle().Colors[ImGuiCol_Text] ,"%s", a_name.c_str());
                    }
                    ImGui::PopFont();

                    // Column 2: Current Keys
                    ImGui::TableNextColumn();
                    std::string VisualKeyString;
                    auto& VisualKeyList = IsRebinding ? TempKeys : Event.Keys;
                    for (size_t i = 0; i < VisualKeyList.size(); ++i) {
                        VisualKeyString += VisualKeyList[i];
                        if (i + 1 < VisualKeyList.size()) VisualKeyString += " + ";
                    }
                    std::string InputText = VisualKeyString.empty() ? "Press any Key or ESC To Cancel" : VisualKeyString;

                    ImGui::SetNextItemWidth(keysColWidth - ImGui::GetStyle().CellPadding.x * 2);
                    ImGui::BeginDisabled(Event.Disabled);
                    {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::InputText("##KeyRebind", &InputText, ImGuiInputTextFlags_ReadOnly);
                        ImGui::PopItemFlag();
                    }
                    ImGui::EndDisabled();

                    // Column 3: Rebind Button
                    ImGui::TableNextColumn();
                    SetWindowBusy(RebindIndex > 0);

                    ImGui::BeginDisabled((TempKeys.empty() && IsRebinding) || Event.Disabled);
                    {
                        if (ImGuiEx::ImageButton(("Rebind##" + std::to_string(CurEventIndex)).c_str(), IsRebinding ? ImageList::Generic_OK : ImageList::Keybind_EditKeybind, 18, T5)) {
                            RebindIndex = CurEventIndex;
                            if (IsRebinding) {
                                if (!TempKeys.empty()) {
                                    Event.Keys = TempKeys;
                                    TempKeys.clear();
                                }
                                RebindIndex = 0;
                            }
                        }
                    }
                    ImGui::EndDisabled();

                    // Column 4: Options Button
                    ImGui::BeginDisabled(IsRebinding);
                    ImGui::TableNextColumn();
				    if (ImGuiEx::ImageButton(("Options##" + std::to_string(CurEventIndex)).c_str(), ImageList::Keybind_ShowAdvanced, 18, T6)) {
                        ImGui::OpenPopup(("Options##" + std::to_string(CurEventIndex)).c_str());
				    }
                    ImGui::EndDisabled();

                    // Options Popup
                    if (ImGui::BeginPopup(("Options##" + std::to_string(CurEventIndex)).c_str())) {
                        ImGui::Text("Extra Options: %s", a_name.c_str());
                        ImGui::Separator();
                        ImGui::PushItemWidth(250.0f);
                        ImGuiEx::CheckBox("Disabled", &Event.Disabled, T0);
                        if (!Event.Disabled) {
                            ImGui::BeginDisabled(IsRebinding);
                            ImGuiEx::CheckBox("Exclusive", &Event.Exclusive, T1);
                            ImGuiEx::ComboEx<LTriggerType_t>("Trigger Type", Event.Trigger, T2);
                            ImGuiEx::ComboEx<LBlockInputTypes_t>("Block Input", Event.BlockInput, T3);
                            ImGui::InputFloat("Trigger After", &Event.Duration, 0.1f, 0.01f, "%.2f Seconds");
                            ImGuiEx::Tooltip(T4);
                            Event.Duration = std::clamp(Event.Duration, 0.0f, 10.0f);
                            ImGui::EndDisabled();
                        }
                        ImGui::PopItemWidth();
                        ImGui::EndPopup();
                    }
                }
                ImGui::EndTable();
                ImGui::PopStyleVar(2);

                // Rebind Handling
                if (IsRebinding) {
                    if (ImGui::IsKeyReleased(ImGuiKey_Escape)) {
                        RebindIndex = 0;
                        TempKeys.clear();
                    }
                    else if (!(ImGui::IsAnyItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))) {
                        for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; ++key) {
                            if (key == ImGuiKey_Escape) continue;
                            if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(key))) {
                                auto keyName = ImInput::ImGuiKeyToDIKString(static_cast<ImGuiKey>(key));
                                if (keyName == "INVALID") continue;
                                if (ranges::find(TempKeys, keyName) == TempKeys.end()
                                    && TempKeys.size() < 5) {
                                    TempKeys.push_back(keyName);
                                    ranges::sort(TempKeys, [](auto& a, auto& b) {
                                        return a.size() > b.size();
                                    });
                                }
                            }
                        }
                    }
                }
                CurEventIndex++;
            }
            ImGui::EndDisabled();
        }
        ImGui::EndChild();
        return true;
    }
}