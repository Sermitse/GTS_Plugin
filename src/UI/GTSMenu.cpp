#include "UI/GTSMenu.hpp"
#include "Config/Config.hpp"
#include "Core/ImGraphics.hpp"
#include "UI/Util/ScaleformLogger.hpp"

namespace GTS {

	//-------------------
	// INIT
	//-------------------

	void GTSMenu::InitScaleform() {

		std::filesystem::path hud = ScaleformLogger::_baseSwfPath / (std::string(MENU_PATH) + std::string(".swf"));
		if (!std::filesystem::exists(hud)) {
			ReportAndExit(fmt::format("Failed to initialize GTSMenu - Missing HUD file\n {}", hud.string()));
			return;
		}

		if (m_scaleformInitialized.load()) {
			return;
		}

		BSScaleformManager* Scaleform = RE::BSScaleformManager::GetSingleton();
		if (!Scaleform) {
			logger::error("Failed to initialize GTSMenu - ScaleformManager not found");
			ReportAndExit("Failed to initialize GTSMenu - ScaleformManager not found");
			return;
		}

		// ** Set base flags and IMenu vars ** //
		
		depthPriority = 0;               //Default priority.
		inputContext = Context::kNone;   //Don't need special input context

		menuFlags.set(UI_MENU_FLAGS::kAlwaysOpen);     //If not added, The game will destroy this menu object on menu change.
		menuFlags.set(UI_MENU_FLAGS::kRequiresUpdate); //Enable The AdvanceMovie Callback.
		menuFlags.set(UI_MENU_FLAGS::kAllowSaving);

		//Route scaleform messages for this menu to our logger
		Scaleform->LoadMovieEx(this, MENU_PATH, [](RE::GFxMovieDef* a_def) -> void {
			a_def->SetState(RE::GFxState::StateType::kLog, RE::make_gptr<ScaleformLogger>().get());
		});

		m_scaleformInitialized.store(true);

	}

	void GTSMenu::InitImGui() {

		if (m_imGuiInitialized.load()) {
			return;
		}

		const auto RenderManager = RE::BSRenderManager::GetSingleton();
		if (!RenderManager) {
			logger::error("Failed to initialize GTSMenu - RenderManager not found");
			ReportAndExit("Failed to initialize GTSMenu - RenderManager not found");
			return;
		}

		const auto D3DDevice = RenderManager->GetRuntimeData().forwarder;
		const auto D3DContext = RenderManager->GetRuntimeData().context;
		const auto DXGISwapChain = RenderManager->GetRuntimeData().swapChain;

		DXGI_SWAP_CHAIN_DESC SwapChainDesc{};
		HRESULT hr = DXGISwapChain->GetDesc(&SwapChainDesc);

		if (FAILED(hr)) {
			logger::error("Could not get the swapchain, HRESULT: {}", hr);
			ReportAndExit("GTSMenu: Could not get the D3D Swapchain, HRESULT: {}");
		}

		ImGui::CreateContext();

		ImIO = &ImGui::GetIO();
		ImIO->IniFilename = m_ImGuiINI.data();

		ImIO->DisplaySize = {
			static_cast<float>(SwapChainDesc.BufferDesc.Width),
			static_cast<float>(SwapChainDesc.BufferDesc.Height)
		};

		ImIO->ConfigNavCaptureKeyboard = false;
		ImIO->MouseDrawCursor = false;
		ImIO->ConfigNavEscapeClearFocusWindow = false;

		//Disable mouse/keyboard input for imgui by default, will be enabled when a window requests it.
		ImIO->ConfigFlags |= 
			ImGuiConfigFlags_NoMouseCursorChange | 
			ImGuiConfigFlags_NoMouse |
			ImGuiConfigFlags_NoKeyboard;

		//ImGui_ImplWin32_Init(SwapChainDesc.OutputWindow);
		ImGui_ImplDX11_Init(D3DDevice, D3DContext);

		logger::info("ImGraphics Init");
		ImGraphics::Init(D3DDevice, D3DContext);
		ImGraphics::Load();

		logger::info("ImWindowManager Init");
		WindowManager = new ImWindowManager();
		WindowManager->Init();

		logger::info("ImInput Init");
		Input = new ImInput();

		m_imGuiInitialized.store(true);

		logger::info("GTSMenu ImGui Init OK");

	}

	bool GTSMenu::Ready() {
		const GTSMenu* const menu = &GetSingleton();
		return menu && menu->m_scaleformInitialized.load() && menu->m_imGuiInitialized.load();
	}

	RE::stl::owner<RE::IMenu*> GTSMenu::Creator() {
		GTSMenu* Instance = &GetSingleton();
		Instance->InitScaleform();
		Instance->InitImGui();
		return Instance;
	}

	void GTSMenu::Show(const std::string& source) {

		auto sourceIdx = std::ranges::find(m_hideSources, source);
		if (sourceIdx != m_hideSources.end()) {
			m_hideSources.erase(sourceIdx);
		}

		if (m_hideSources.empty()) {
			SetVisibility(true);
		}
	}

	void GTSMenu::Hide(const std::string& source) {
		auto sourceIdx = std::ranges::find(m_hideSources, source);
		if (sourceIdx == m_hideSources.end()) {
			m_hideSources.push_back(source);
		}

		if (!m_hideSources.empty()) {
			SetVisibility(false);
		}
	}

	void GTSMenu::SetVisibility(bool a_state) {
		if (UIMessageQueue* const msgQ = UIMessageQueue::GetSingleton()) {
			msgQ->AddMessage(MENU_NAME, a_state ? UI_MESSAGE_TYPE::kShow : UI_MESSAGE_TYPE::kHide, nullptr);
			m_isScaleformVisible.store(a_state, std::memory_order_relaxed);
		}
	}

	//Called by InputEventDispatcher Hook
	void GTSMenu::ProcessAndFilterEvents(RE::InputEvent** a_events) const {

		if (!m_imGuiInitialized.load(std::memory_order_relaxed) || !m_scaleformInitialized.load(std::memory_order_relaxed)) {
			return;
		}

		if (WindowManager->HasInputConsumers() && m_isScaleformVisible.load()) {
			Input->ProcessInputEvents(a_events);
			Input->UpdateMousePos();
			ImInput::RemoveAllKeyEvents(a_events); // Remove all button events so they don't reach the game
		}
	}

	void GTSMenu::SetInputFlags(bool a_enable) {

		if (const auto ui = RE::UI::GetSingleton()) {
			if (const auto msgQueue = UIMessageQueue::GetSingleton()) {

				if (m_cursorEnabled.exchange(a_enable) == a_enable) {
					return;
				}

				if (a_enable) {
					//The Flags don't behave as expected
					//So we're forced to just outright show the cursor instead of relying on the game to manage it for us.
					//You can sort of force a partial flag read by sending a kHide and kShow Ui message,
					//Essentially turning it off and on again. This appears to make the mouse flags work however it seems to have no
					//effect on other flags like pause game/blur backround.
					menuFlags.set(UI_MENU_FLAGS::kUpdateUsesCursor);
					menuFlags.set(UI_MENU_FLAGS::kUsesCursor);

					//Enable Imgui's win32 input update code.
					ImIO->ConfigFlags &= ~(ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoKeyboard);

					//Manually Show the cursor
					msgQueue->AddMessage(RE::CursorMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr);
				}
				else {

					menuFlags.reset(UI_MENU_FLAGS::kUpdateUsesCursor);
					menuFlags.reset(UI_MENU_FLAGS::kUsesCursor);
					ImIO->ConfigFlags |= ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoKeyboard;

					//Traverse the menu stack and check if another menu is currently being drawn that uses the cursor.
					//If it does don't force hide the cursor as that would break said other menu.
					for (const auto& menu : ui->menuStack) {

						//If the menu is us skip over.
						if (menu.get() == this) {
							continue;
						}

						if ((menu->menuFlags & UI_MENU_FLAGS::kUsesCursor) != UI_MENU_FLAGS::kNone ||
							(menu->menuFlags & UI_MENU_FLAGS::kUpdateUsesCursor) != UI_MENU_FLAGS::kNone) {
							return;
						}
					}

					msgQueue->AddMessage(RE::CursorMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
				}

				logger::trace("GTSMenu Cursor: {}", a_enable);
			}
		}
	}

	void GTSMenu::BlurBackground(bool a_enable) {
		auto* const blur = RE::UIBlurManager::GetSingleton();
		if (!blur) {
			return;
		}

		if (a_enable) {
			if (!Config::UI.bDoBGBlur) {
				return;
			}

			// Only 0->1 actually increments engine blur counter
			if (m_localBlurCount.fetch_add(1, std::memory_order_acq_rel) == 0) {
				blur->IncrementBlurCount();
			}
		}
		else {
			// Release a local blur reference if any are held
			uint32_t prev = m_localBlurCount.load(std::memory_order_acquire);
			while (prev > 0) {
				if (m_localBlurCount.compare_exchange_weak(prev, prev - 1, std::memory_order_acq_rel, std::memory_order_acquire)){
					// Only 1->0 decrements engine blur counter
					if (prev == 1 && blur->blurCount > 0) {
						blur->DecrementBlurCount();
					}
					break;
				}
			}
		}
	}


	void GTSMenu::PauseGame(bool a_enable) {

		if (static UI* const ui = UI::GetSingleton()) {

			int32_t* ipauses = reinterpret_cast<int32_t*>(&ui->numPausesGame);

			if (a_enable) {

				if (!Config::UI.bDoPause) {
					return;
				}

				// Increment local pause reference count; only the 0 to 1 transition
				// actually increments the ui pause counter
				if (m_localPauseCount.fetch_add(1, std::memory_order_acq_rel) == 0) {
					++(*ipauses);
				}
			}
			else {
				// Release a local pause reference if any are held
				uint32_t prev = m_localPauseCount.load(std::memory_order_acquire);
				while (prev > 0) {
					// Attempt to decrement the local pause count
					if (m_localPauseCount.compare_exchange_weak(prev, prev - 1, std::memory_order_acq_rel)) {
						// Only the 1 to 0 transition decrements the ui pause counter
						if (prev == 1 && *ipauses > 0) {
							--(*ipauses);
						}
						break;
					}
				}
			}
		}
	}

	void GTSMenu::AlterTimeScale(bool a_enable) {

		if (a_enable) {
			if (!Config::UI.bDoSlowdown) {
				return;
			}

			// Only 0->1 captures original + applies slowdown
			if (m_localSlowMoCount.fetch_add(1, std::memory_order_acq_rel) == 0) {
				m_originalGameTime.store(Time::GGTM(), std::memory_order_release);
				Time::SGTM(Config::UI.fSGTMMult);
			}
		}
		else {
			// Release a local slowdown reference if any are held
			uint32_t prev = m_localSlowMoCount.load(std::memory_order_acquire);
			while (prev > 0) {
				if (m_localSlowMoCount.compare_exchange_weak(prev, prev - 1, std::memory_order_acq_rel, std::memory_order_acquire)){
					// Only 1->0 restores original
					if (prev == 1) {
						const float original = m_originalGameTime.load(std::memory_order_acquire);
						Time::SGTM(original);
					}
					break;
				}
			}
		}
	}

	bool GTSMenu::CloseInputConsumers() {
		return WindowManager->CloseInputConsumers();
	}

	std::string GTSMenu::DebugName() {
		return "::GTSHUDMenu";
	}

	void GTSMenu::DataReady() {

		logger::info("Registering GTSMenu...");

		if (static UI* const ui = UI::GetSingleton()) {
			ui->Register(MENU_NAME, Creator);
			SetVisibility(true);

			logger::info("Successfully registered GTSMenu");
			return;
		}

		logger::error("Failed to register GTSMenu");
		ReportAndExit("GTSMenu: Scaleform registration failed.");

	}

	void GTSMenu::MenuChange(const MenuOpenCloseEvent* a_event) {

		auto mName = std::string(a_event->menuName);

		if (
			mName == RE::JournalMenu::MENU_NAME      ||
			mName == RE::InventoryMenu::MENU_NAME    ||
			mName == RE::MapMenu::MENU_NAME          ||
			mName == RE::BookMenu::MENU_NAME         ||
			mName == RE::LockpickingMenu::MENU_NAME  ||
			mName == RE::MagicMenu::MENU_NAME        ||
			mName == RE::RaceSexMenu::MENU_NAME      ||
			mName == RE::CraftingMenu::MENU_NAME     ||
			mName == RE::SleepWaitMenu::MENU_NAME    ||
			mName == RE::TrainingMenu::MENU_NAME     ||
			mName == RE::BarterMenu::MENU_NAME       ||
			mName == RE::FavoritesMenu::MENU_NAME    ||
			mName == RE::GiftMenu::MENU_NAME         ||
			mName == RE::StatsMenu::MENU_NAME        ||
			mName == RE::ContainerMenu::MENU_NAME    ||
			//mName == RE::DialogueMenu::MENU_NAME     ||
			//mName == RE::MessageBoxMenu::MENU_NAME   ||
			mName == RE::TweenMenu::MENU_NAME) {

			//If the game switches to any of these menus hide ours.
			a_event->opening ? Hide(mName) : Show(mName);

		}
		// For some reason, changing cells seems to hide the menu.
		else if (mName == RE::LoadingMenu::MENU_NAME) {
			if (!a_event->opening) {
				SetVisibility(true);
			}
		}
	}

	//Imgui Update
	void GTSMenu::AdvanceMovie(float a_interval, std::uint32_t a_currentTime) {

		IMenu::AdvanceMovie(a_interval, a_currentTime);

		if (!m_imGuiInitialized.load(std::memory_order_relaxed) || !m_scaleformInitialized.load(std::memory_order_relaxed)) {
			return;
		}

		bool expected = true;
		if (m_frameReady.compare_exchange_strong(expected, false, std::memory_order_relaxed)) {
			ImGui::EndFrame();
			return;
		}

		//ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX11_NewFrame();
		ImGui::NewFrame();
		ImGraphics::ClearTransformedTextureCache();

		Input->ProcessInputEventQueue();
		WindowManager->Update();

		depthPriority = WindowManager->GetDesiredPriority();
		SetInputFlags(WindowManager->GetDesiredCursorState());
		m_DrawOnPresent.store(WindowManager->GetWantDrawOnPresent());

		m_frameReady.store(true, std::memory_order_release);
	}

	//ImGui Present
	void GTSMenu::PostDisplay() {
		IMenu::PostDisplay();
		if (m_DrawOnPresent.load() == false) {
			Present();
		}
	}

	void GTSMenu::Present() {
		if (m_frameReady.exchange(false, std::memory_order_acquire)) {
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
	}
}
