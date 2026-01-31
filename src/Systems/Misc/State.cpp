#include "Systems/Misc/State.hpp"
#include "UI/GTSMenu.hpp"

namespace GTS {

	bool State::IsInRaceMenu() {
		const auto ui = UI::GetSingleton();
		if (ui->GetMenu(RaceSexMenu::MENU_NAME)) {
			//Used for animation blocking in race menu.
			return true;
		}
		return false;
	}

	bool State::Enabled() {
		return m_enabled.load();
	}

	bool State::InGame() {
		const auto ui = UI::GetSingleton();
		if (!ui) {
			return false;
		}
		if (ui->IsMenuOpen(MainMenu::MENU_NAME)) {
			return false;
		}
		return m_inGame.load();
	}

	void State::SetInGame(bool value) {
		m_inGame.store(value);
	}

	bool State::Ready() {
		if (m_inGame.load()) { // We are not loading or in the mainmenu
			if (const auto player_char = PlayerCharacter::GetSingleton()) {
				if (player_char->Is3DLoaded()) { // Player is loaded
					return true;
				}
			}
		}
		return false;
	}

	bool State::Live() {

		if (!Ready()) {
			return false;
		}

		const auto ui = UI::GetSingleton();
		if (!ui) {
			return false;
		}

		if (ui->GameIsPaused()) {
			if (GTSMenu::WindowManager->HasInputConsumers() || !IsInRaceMenu()) {
				return false;
			}
		}

		return true;
	}

	//Check if a blocking menu is visible.
	//These menus normally pause gameplay.
	bool State::IsInBlockingMenu() {

		static const std::vector Menus = {
			RE::CraftingMenu::MENU_NAME,
			RE::BarterMenu::MENU_NAME,
			RE::BookMenu::MENU_NAME,
			RE::ContainerMenu::MENU_NAME,
			RE::FavoritesMenu::MENU_NAME,
			RE::GiftMenu::MENU_NAME,
			RE::InventoryMenu::MENU_NAME,
			RE::JournalMenu::MENU_NAME,
			RE::LevelUpMenu::MENU_NAME,
			RE::LockpickingMenu::MENU_NAME,
			RE::MagicMenu::MENU_NAME,
			RE::MapMenu::MENU_NAME,
			RE::MessageBoxMenu::MENU_NAME,
			RE::RaceSexMenu::MENU_NAME,
			RE::SleepWaitMenu::MENU_NAME,
			RE::StatsMenu::MENU_NAME,
			RE::TrainingMenu::MENU_NAME,
			RE::TutorialMenu::MENU_NAME,
			RE::TweenMenu::MENU_NAME,
			RE::FaderMenu::MENU_NAME
		};

		if (Ready()) {
			const auto ui = RE::UI::GetSingleton();
			for (const auto& Menu : Menus) {
				if (ui->IsMenuOpen(Menu)) {
					//log::debug("Menu is open: {}", Menu);
					return true;
				}
			}
		}
		return false;
	}

	bool State::OnMainThread() {
		return m_onMainThread.load();
	}

	void State::SetOnMainThread(bool value) {
		m_onMainThread.store(value);
	}

}
