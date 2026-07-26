#include "UI/GTSMenu.hpp"
#include "UI/Windows/Other/KillFeedWindow.hpp"

#include "UI/Controls/KillEntry.hpp"
#include "UI/Core/ImColorUtils.hpp"

#include "UI/Windows/Settings/SettingsWindow.hpp"

#include "Utils/KillDataUtils.hpp"


namespace {
	ImGuiEx::KillPriority GetAttackerPriority(RE::Actor* a_attacker) {
		if (!a_attacker) return ImGuiEx::KillPriority::kWorld;
		if (a_attacker->IsPlayerRef()) return ImGuiEx::KillPriority::kPlayer;
		return ImGuiEx::KillPriority::kNPC;
	}
}

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
			.fBGAlphaMult = 1.0f,
			.fWindowSizePercent = 0.0f, //Unused
			.bVisible = true,
			.bEnableFade = false,       //Unused
			.fFadeAfter = 6.0f,
			.fFadeDelta = 0.00f,        //Unused
		});


		this->RegisterExtraSettings(m_extraSettings);
		m_settingsHolder->SetCustomDefaults<WindowSettingsKillFeed_t>({
			.iFlags = 0,
			.fWidth = 400.f,
			.iMaxVisibleEntries = 8,
			.f3BGColor = {0.0f, 0.0f, 0.0f}
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

	void KillFeedWindow::DeathEvent(Actor* a_killer, Actor* a_victim) {

		static const WindowSettingsKillFeed_t& settings = GetExtraSettings<WindowSettingsKillFeed_t>();

		// Check if we should show any vanilla kills at all
		if (!settings.bShowGameKills) {
			return;
		}
		
		// Check if we should show kills that have no killer (world kills)
		if (!a_killer && !settings.bShowWorldKills) {
			return;
		}

		AddKillEntry(a_killer, a_victim, DeathType::kVanillaHit);

	}

	void KillFeedWindow::GameDeathEvent(Actor* a_killer, Actor* a_victim, bool a_dead) {

		static const WindowSettingsKillFeed_t& settings = GetExtraSettings<WindowSettingsKillFeed_t>();

		// Check if we should show any vanilla kills at all
		if (!settings.bShowGameKills) {
			return;
		}

		// Check if we should show kills that have no killer (world kills)
		if (!a_killer && !settings.bShowWorldKills) {
			return;
		}

		if (!a_dead) {
			// I think the dead bool indicates whether the actor is in a critical stage
			// We don't really need this event the normal death event which fires slightly earlier is more than enough for this use case.
			AddKillEntry(a_killer, a_victim, DeathType::kVanillaEvt);
		}

	}

	void KillFeedWindow::AddKillEntry(Actor* a_attacker, Actor* a_victim, DeathType a_type) {

		std::lock_guard lock(_Lock);
		if (!a_victim) return;
		if (a_victim->IsDeleted()) return;
		//if (a_victim->GetWorldspace() != PlayerCharacter::GetSingleton()->GetWorldspace()) return;
		//if (a_victim->GetParentCell() != PlayerCharacter::GetSingleton()->GetParentCell()) return;
		//if (a_victim->GetPosition().GetDistance(PlayerCharacter::GetSingleton()->GetPosition()) > 8192.f) return; //More than 2 cells away

		auto Exists = [a_victim](const auto& entry) {
			return entry && entry->victimHandle.get().get() && entry->victimHandle.get().get()->formID == a_victim->formID;
		};

		// Find existing entry in KillEntries or PendingKillEntries
		std::unique_ptr<ImGuiEx::KillEntry>* targetEntry = nullptr;
		bool isNewEntry = false;

		if (auto it = std::ranges::find_if(KillEntries, Exists); it != KillEntries.end()) {
			targetEntry = &(*it);
		}
		else if (auto it2 = std::ranges::find_if(PendingKillEntries, Exists); it2 != PendingKillEntries.end()) {
			targetEntry = &(*it2);
		}

		if (!targetEntry) {
			PendingKillEntries.emplace_back(std::make_unique<ImGuiEx::KillEntry>());
			targetEntry = &PendingKillEntries.back();
			isNewEntry = true;
		}

		ImGuiEx::KillEntry* entry = targetEntry->get();
		ImGuiEx::KillPriority NewPriority = GetAttackerPriority(a_attacker);

		// Lower priority attacker can't override an already recorded higher priority kill.
		if (!isNewEntry && NewPriority < entry->priority) return;

		bool SamePriority = (!isNewEntry && NewPriority == entry->priority);

		entry->priority = NewPriority;
		entry->victim = a_victim->GetName();
		entry->victimHandle = a_victim->CreateRefHandle();
		entry->attacker.clear();

		if (a_attacker) {
			entry->attacker = a_attacker->GetName();
		}

		std::string DeathTypeStr = "[World]";

		if (a_attacker) {

			switch (a_type) {

				case DeathType::kShrunkToNothing: 
				{
					DeathTypeStr = "Shrunk To Nothing";
					break;
				}
				case DeathType::kVoreAbsorbed:
				case DeathType::kAbsorbed:
				case DeathType::kBreastAbsorbed: 
				{
					DeathTypeStr = "Absorbed";
					break;
				}
				case DeathType::kThighSuffocated:
				case DeathType::kBreastSuffocated: 
				{
					DeathTypeStr = "Suffocated";
					break;
				}
				case DeathType::kButtCrushed: 
				{
					DeathTypeStr = "Butt Crushed";
					break;
				}
				case DeathType::kThighCrushed: 
				{
					DeathTypeStr = "Thigh Crushed";
					break;
				}
				case DeathType::kCrushed:
				case DeathType::kBreastCrushed:
				case DeathType::kGrabCrushed:
				case DeathType::kFingerCrushed: 
				{
					DeathTypeStr = "Crushed";
					break;
				}
				case DeathType::kHugCrushed: 
				{
					DeathTypeStr = "Hug Crushed";
					break;
				}
				case DeathType::kThighSandwiched: 
				{
					DeathTypeStr = "Sandwiched";
					break;
				}
				case DeathType::kGrinded:
				case DeathType::kThighGrinded: 
				{
					DeathTypeStr = "Pulverized";
					break;
				}
				case DeathType::kErasedFromExistence: 
				{
					DeathTypeStr = "Erased";
					break;
				}
				case DeathType::kKicked: 
				{
					DeathTypeStr = "Kicked";
					break;
				}
				case DeathType::kEaten: 
				{
					DeathTypeStr = "Vored";
					break;
				}
				//Generic "Killed"
				default:
				case DeathType::kVanillaHit:
				case DeathType::kVanillaEvt:
				case DeathType::kOtherSources: 
				{
					DeathTypeStr = "Killed";
					break;
				}
			}
		}

		// Higher priority always takes the new type. Same priority still prefers a specific
		// death message over the generic fallbacks so quirky callers don't clobber good data.
		bool IsGenericFallback = (a_type == DeathType::kVanillaHit || a_type == DeathType::kVanillaEvt || a_type == DeathType::kOtherSources);

		if (entry->type.empty() || !SamePriority || !IsGenericFallback) {
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

		
		static ImGuiEx::KillFeedStyle Style = {};
		{
			Style.visDuration  = BaseSettings.fFadeAfter;
			Style.neverFade    = Configuring ? true : !BaseSettings.bEnableFade;
			Style.bgColor      = ImUtil::Colors::fRGBToU32(ExtraSettings.f3BGColor);
			Style.attackerCol  = ImUtil::Colors::fRGBToU32(ExtraSettings.f3AttackerColor);
			Style.victimCol    = ImUtil::Colors::fRGBToU32(ExtraSettings.f3VictimColor);
			Style.deathTypeCol = ImUtil::Colors::fRGBToU32(ExtraSettings.f3DeathTypeColor);
			Style.bgAlpha      = BaseSettings.fBGAlphaMult;
			Style.fontScale    = ExtraSettings.fFontScaleMult;
			Style.flags        = static_cast<ImGuiEx::KillFeedEntryFlags>(ExtraSettings.iFlags);
		}


		// Demo entries if configuring
		if (Configuring) {
			static std::unique_ptr<ImGuiEx::KillEntry> DemoEntry = std::make_unique<ImGuiEx::KillEntry>(
				"Attacker", "Victim", PlayerCharacter::GetSingleton()->CreateRefHandle(), "Killed", ImGuiEx::KillPriority::kNPC
			);

			for (uint8_t i = 0; i < ExtraSettings.iMaxVisibleEntries; i++) {
				ImGuiEx::DrawKillfeedEntry(DemoEntry, Style);
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
				if (auto victim = Entry.get()->victimHandle.get().get()) {
					if (victim->IsDead()) { //Only draw if actually dead, masks the fact that hitdata will sometimes give us actors that are still alive.
						ImGuiEx::DrawKillfeedEntry(Entry, Style);
					}
				}
				
			}

			// Remove expired entries
			std::erase_if(KillEntries, [BaseSettings](const auto& entry) {
				return entry->lifetime >= BaseSettings.fFadeAfter;
			});

		}

		ImGui::Dummy({});
	}

}
