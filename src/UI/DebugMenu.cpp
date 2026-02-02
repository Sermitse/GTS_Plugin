
#include "UI/DebugMenu.hpp"
#include "UI/Util/ScaleformLogger.hpp"

namespace GTS {

	void DebugMenu::InitScaleform() {


		std::filesystem::path hud = ScaleformLogger::_baseSwfPath / (std::string(MENU_PATH) + std::string(".swf"));
		if (!std::filesystem::exists(hud)) {
			ReportAndExit(fmt::format("Failed to initialize DebugMenu - Missing HUD file\n {}", hud.string()));
			return;
		}

		if (m_scaleformInitialized.load()) {
			return;
		}

		auto Scaleform = RE::BSScaleformManager::GetSingleton();
		if (!Scaleform) {
			logger::error("Failed to initialize DebugMenu - ScaleformManager not found");
			return;
		}

		depthPriority = 0;

		menuFlags.set(RE::UI_MENU_FLAGS::kRequiresUpdate);
		menuFlags.set(RE::UI_MENU_FLAGS::kAllowSaving);
		menuFlags.set(RE::UI_MENU_FLAGS::kAlwaysOpen);

		inputContext = Context::kNone;

		Scaleform->LoadMovieEx(this, MENU_PATH, [](RE::GFxMovieDef* a_def) -> void {
			a_def->SetState(RE::GFxState::StateType::kLog, RE::make_gptr<ScaleformLogger>().get());
		});

		m_scaleformInitialized.store(true);

		logger::info("Init Scaleform");
	}

	std::string DebugMenu::DebugName() {
		return "::DebugMenu";
	}

	void DebugMenu::DataReady() {
		logger::info("Registering DebugMenu...");

		auto ui = RE::UI::GetSingleton();
		if (ui) {
			ui->Register(MENU_NAME, Creator);
			DebugMenu::Start();

			logger::info("Successfully registered DebugMenu");
		}
		else {
			logger::error("Failed to register DebugMenu");
		}
	}

	void DebugMenu::Start() {

		auto msgQ = RE::UIMessageQueue::GetSingleton();
		if (msgQ) {
			msgQ->AddMessage(MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr);
		}
		else {
			logger::warn("Could not show DebugMenu");
		}

	}

	void DebugMenu::Update() {
		DebugDraw::Update();
	}

	void DebugMenu::Show(const std::string& source) {
		auto sourceIdx = std::ranges::find(m_hideSources, source);
		if (sourceIdx != m_hideSources.end()) {
			m_hideSources.erase(sourceIdx);
		}

		if (m_hideSources.empty()) {
			ToggleVisibility(true);
		}
	}

	void DebugMenu::Hide(const std::string& source) {
		auto sourceIdx = std::ranges::find(m_hideSources, source);
		if (sourceIdx == m_hideSources.end()) {
			m_hideSources.push_back(source);
		}

		if (!m_hideSources.empty()) {
			ToggleVisibility(false);
		}
	}

	void DebugMenu::MenuChange(const MenuOpenCloseEvent* a_event) {

		auto mName = a_event->menuName;

		if (
			mName == RE::JournalMenu::MENU_NAME ||
			mName == RE::InventoryMenu::MENU_NAME ||
			mName == RE::MapMenu::MENU_NAME ||
			mName == RE::BookMenu::MENU_NAME ||
			mName == RE::LockpickingMenu::MENU_NAME ||
			mName == RE::MagicMenu::MENU_NAME ||
			mName == RE::RaceSexMenu::MENU_NAME ||
			mName == RE::CraftingMenu::MENU_NAME ||
			//mName == RE::SleepWaitMenu::MENU_NAME ||
			//mName == RE::TrainingMenu::MENU_NAME ||
			mName == RE::BarterMenu::MENU_NAME ||
			//mName == RE::FavoritesMenu::MENU_NAME ||
			mName == RE::GiftMenu::MENU_NAME ||
			mName == RE::StatsMenu::MENU_NAME ||
			mName == RE::ContainerMenu::MENU_NAME ||
			//mName == RE::DialogueMenu::MENU_NAME ||
			//mName == RE::MessageBoxMenu::MENU_NAME ||
			mName == RE::TweenMenu::MENU_NAME || // tab menu
			mName == RE::MainMenu::MENU_NAME) {

			if (a_event->opening) {
				Hide(mName.c_str());
			}
			else {
				Show(mName.c_str());
			}

		}
		// for some reason, after each cell change, the menu is hidden and needs to be shown again
		// using a UI message kShow
		else if (mName == RE::LoadingMenu::MENU_NAME) {
			if (!a_event->opening) {
				DebugMenu::Start();
			}
		}
	}

	void DebugMenu::ToggleVisibility(bool mode) {
		auto ui = RE::UI::GetSingleton();
		if (!ui) {
			return;
		}

		auto overlayMenu = ui->GetMenu(MENU_NAME);
		if (!overlayMenu || !overlayMenu->uiMovie) {
			return;
		}

		overlayMenu->uiMovie->SetVisible(mode);
	}

	RE::stl::owner<RE::IMenu*> DebugMenu::Creator() {
		DebugMenu* Instance = &GetSingleton();
		Instance->InitScaleform();
		return Instance;
	}

	bool DebugMenu::Ready() {
		const DebugMenu* const menu = &GetSingleton();
		return menu && menu->m_scaleformInitialized.load();
	}

	void DebugMenu::AdvanceMovie(float a_interval, std::uint32_t a_currentTime) {
		IMenu::AdvanceMovie(a_interval, a_currentTime);
		DebugDraw::Update();
	}
}
