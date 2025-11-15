#include "UI/GTSMenu.hpp"
#include "UI/Windows/Other/KillFeedWindow.hpp"

#include "UI/Controls/KillEntry.hpp"
#include "UI/Lib/imgui.h"

#include "UI/Core/ImFontManager.hpp"

#include "UI/Windows/Settings/SettingsWindow.hpp"

#include "Utils/KillDataUtils.hpp"

namespace GTS {

	void KillFeedWindow::Init() {

		m_windowType = WindowType::kWidget;
		m_name = "KillFeed";
		m_title = "KillFeed";

		m_flags =
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoNavInputs |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoBringToFrontOnFocus;


		//Construct Base defaults for this Window
		m_settingsHolder->SetBaseDefaults({
			.bLock = true,
			.f2Position = { 20.0f, 20.0f },
			.sAnchor = "kTopRight",
			.fAlpha = 0.9f,
			.fBGAlphaMult = 0.0f,
			.fWindowSizePercent = 0.0f, //Unused
			.bVisible = true,
			.bEnableFade = false, //Unused
			.fFadeAfter = 6.0f,
			.fFadeDelta = 0.00f, //Unused
		});


		this->RegisterExtraSettings(m_extraSettings);
		m_settingsHolder->SetCustomDefaults<WindowSettingsKillFeed_t>({
			.iIconSize = 48,
			.iFlags = 0,
			.fWidth = 400.f,
			.iMaxVisibleEntries = 8
		});

		if (const auto& wSettings = dynamic_cast<SettingsWindow*>(GTSMenu::WindowManager->wSettings)) {
			m_isConfiguring = &wSettings->m_isConfiguringWidgets;
			m_settingsVisible = &wSettings->m_show;
		}

		EventDispatcher::AddListener(this);

	}

	void KillFeedWindow::RequestClose() {}

	bool KillFeedWindow::WantsToDraw() {

		if (!State::InGame()) {
			return false;
		}

		if (!GetBaseSettings().bVisible) {
			return false;
		}

		//Always draw all if the widget page is open in settings
		if (*m_isConfiguring && *m_settingsVisible) {
			this->ResetFadeState();
			return true;
		}

		return true;
	}

	float KillFeedWindow::GetBackgroundAlpha() {
		return 0.0f;
	}

	std::string KillFeedWindow::DebugName() {
		return "::KillFeedWindow";
	}

	void KillFeedWindow::DeathEvent(Actor* a_killer, Actor* a_victim, bool a_dead) {
		if (!a_dead) {
			// I think the dead bool indicates whether the actor is in a critical stage
			// We don't really need this event the normal death event which fires slightly earlier is more than enough for this use case.
			AddKillEntry(a_killer, a_victim, DeathType::kVanilla);
		}
	}

	void KillFeedWindow::AddKillEntry(Actor* a_attacker, Actor* a_victim, DeathType a_type) {

		std::lock_guard lock(_Lock);
		if (!a_victim) return;

		auto Exists = [a_victim](const auto& entry) {
			return entry && entry->victimptr == reinterpret_cast<uintptr_t>(a_victim);
		};

		// Find existing entry in KillEntries or PendingKillEntries
		std::unique_ptr<ImGuiEx::KillEntry>* targetEntry = nullptr;

		if (auto it = ranges::find_if(KillEntries, Exists); it != KillEntries.end()) {
			targetEntry = &(*it);
		}
		else if (auto it2 = ranges::find_if(PendingKillEntries, Exists); it2 != PendingKillEntries.end()) {
			targetEntry = &(*it2);
		}

		// If no existing entry, create new
		if (!targetEntry) {
			PendingKillEntries.emplace_back(std::make_unique<ImGuiEx::KillEntry>());
			targetEntry = &PendingKillEntries.back();
		}

		ImGuiEx::KillEntry* entry = targetEntry->get();
		entry->victim = a_victim->GetName();
		entry->victimptr = reinterpret_cast<uintptr_t>(a_victim);
		entry->attacker.clear();

		if (a_attacker) entry->attacker = a_attacker->GetName();

		// Compute death type string
		std::string DeathTypeStr = "[World]";

		if (a_attacker) {

			switch (a_type) {

				case DeathType::kShrunkToNothing: {
					DeathTypeStr = "Shrunk To Nothing";
					break;
				}

				case DeathType::kVoreAbsorbed:
				case DeathType::kAbsorbed:
				case DeathType::kBreastAbsorbed: {
					DeathTypeStr = "Absorbed";
					break;
				}

				case DeathType::kThighSuffocated:
				case DeathType::kBreastSuffocated: {
					DeathTypeStr = "Suffocated";
					break;
				}

				case DeathType::kCrushed:
				case DeathType::kBreastCrushed:
				case DeathType::kHugCrushed:
				case DeathType::kGrabCrushed:
				case DeathType::kButtCrushed:
				case DeathType::kThighCrushed:
				case DeathType::kFingerCrushed: {
					DeathTypeStr = "Crushed";
					break;
				}


				case DeathType::kThighSandwiched: {
					DeathTypeStr = "Sandwiched";
					break;
				}

				case DeathType::kGrinded:
				case DeathType::kThighGrinded: {
					DeathTypeStr = "Pulverized";
					break;
				}

				case DeathType::kErasedFromExistence: {
					DeathTypeStr = "Erased";
					break;
				}

				case DeathType::kKicked: {
					DeathTypeStr = "Kicked";
					break;
				}

				case DeathType::kEaten: {
					DeathTypeStr = "Ate";
					break;
				}

				//Generic "Killed"
				default:
				case DeathType::kVanilla:
				case DeathType::kOtherSources: {
					DeathTypeStr = "Killed";
					break;
				}
			}
		}

		// Only update the type if the existing entry has the fallback "[World] or the generic Killed Event"
		// We don't want to override the custom death messages with the generic ones.
		if (entry->type.empty() || entry->type == "[World]" || entry->type == "Killed") {
			entry->type = DeathTypeStr;
		}

	}

	void KillFeedWindow::Draw() {
		

		auto& BaseSettings = GetBaseSettings();
		auto& ExtraSettings = GetExtraSettings<WindowSettingsKillFeed_t>();

		const ImVec2 Size {ExtraSettings.fWidth, 0};
		ImGui::SetWindowSize(Size);

		m_fadeSettings.enabled = BaseSettings.bEnableFade;
		m_fadeSettings.visibilityDuration = BaseSettings.fFadeAfter;
		bool Configuring = *m_isConfiguring && *m_settingsVisible;

		const ImVec2 Offset{ BaseSettings.f2Position[0], BaseSettings.f2Position[1] };
		ImGui::SetWindowPos(GetAnchorPos(StringToEnum<WindowAnchor>(BaseSettings.sAnchor), Offset, true));

		ImFontManager::Push(ImFontManager::ActiveFontType::kLargeText);

		// Demo entries if configuring
		if (Configuring) {
			static std::unique_ptr<ImGuiEx::KillEntry> DemoEntry = std::make_unique<ImGuiEx::KillEntry>(
				"Attacker", "Victim", 0, "Killed"
			);

			for (uint8_t i = 0; i < ExtraSettings.iMaxVisibleEntries; i++) {
				ImGuiEx::DrawKillfeedEntry(DemoEntry, BaseSettings.fFadeAfter, true);
			}
		}
		else {

			std::lock_guard lock(_Lock);

			// Move entries from pending to visible if there's space
			while (!PendingKillEntries.empty() &&
				KillEntries.size() < ExtraSettings.iMaxVisibleEntries) {
				KillEntries.emplace_back(std::move(PendingKillEntries.front()));
				PendingKillEntries.pop_front();
			}

			// Draw visible entries and update lifetime
			for (auto& Entry : KillEntries) {
				Entry->lifetime += Time::WorldTimeDelta();
				ImGuiEx::DrawKillfeedEntry(Entry, BaseSettings.fFadeAfter, false);
			}

			// Remove expired entries
			std::erase_if(KillEntries, [BaseSettings](const auto& entry) {
				return entry->lifetime >= BaseSettings.fFadeAfter;
			});

		}

		ImGui::Dummy({});
		ImFontManager::Pop();
	}

}
