#include "UI/GTSMenu.hpp"
#include "UI/ScaleformLogger.hpp"

#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/Lib/imgui_impl_dx11.h"
#include "UI/ImGui/Lib/imgui_impl_win32.h"


namespace GTS {

	//-------------------
	// INIT
	//-------------------

	void GTSMenu::InitScaleform() {

		if (m_scaleformInitialized.load()) {
			return;
		}

		BSScaleformManager* Scaleform = RE::BSScaleformManager::GetSingleton();
		if (!Scaleform) {
			log::error("Failed to initialize GTSMenu - ScaleformManager not found");
			ReportAndExit("Failed to initialize GTSMenu - ScaleformManager not found");
			return;
		}

		// ** Set base flags and IMenu vars ** //
		
		depthPriority = 0;               //Default priority.
		inputContext = Context::kNone;   //Don't need special input context

		menuFlags.set(UI_MENU_FLAGS::kAlwaysOpen);     //If not added, The game will destroy this menu object on menu change.
		menuFlags.set(UI_MENU_FLAGS::kRequiresUpdate);
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
			log::error("Failed to initialize GTSMenu - RenderManager not found");
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
			ReportAndExit("Could not get the swapchain, HRESULT: {}");
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

		ImGui_ImplWin32_Init(SwapChainDesc.OutputWindow);
		ImGui_ImplDX11_Init(D3DDevice, D3DContext);

		logger::info("ImGraphics Init");
		Graphics = new ImGraphics(D3DDevice, D3DContext);
		Graphics->Load();

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

		auto sourceIdx = ranges::find(m_hideSources, source);
		if (sourceIdx != m_hideSources.end()) {
			m_hideSources.erase(sourceIdx);
		}

		if (m_hideSources.empty()) {
			SetVisibility(true);
		}
	}

	void GTSMenu::Hide(const std::string& source) {
		auto sourceIdx = ranges::find(m_hideSources, source);
		if (sourceIdx == m_hideSources.end()) {
			m_hideSources.push_back(source);
		}

		if (!m_hideSources.empty()) {
			SetVisibility(false);
		}
	}

	void GTSMenu::SetVisibility(bool a_state) {
		if (static UIMessageQueue* const msgQ = UIMessageQueue::GetSingleton()) {
			msgQ->AddMessage(MENU_NAME, a_state ? UI_MESSAGE_TYPE::kShow : UI_MESSAGE_TYPE::kHide, nullptr);
			m_visible.store(a_state, std::memory_order_relaxed);
		}
	}

	//Called by InputEventDispatcher Hook
	void GTSMenu::ProcessAndFilterEvents(RE::InputEvent** a_events) const {

		if (!m_imGuiInitialized.load(std::memory_order_relaxed) || !m_scaleformInitialized.load(std::memory_order_relaxed)) {
			return;
		}

		if (WindowManager->HasInputConsumers() && m_visible.load()) {
			Input->ProcessInputEvents(a_events);
			Input->UpdateMousePos();
			ImInput::RemoveAllKeyEvents(a_events); // Remove all button events so they don't reach the game
		}
	}

	void GTSMenu::SetInputFlags(bool a_enable) {

		if (m_cursorEnabled.exchange(a_enable) == a_enable) {
			return;
		}

		if (a_enable) {
			menuFlags.set(UI_MENU_FLAGS::kUpdateUsesCursor);
			menuFlags.set(UI_MENU_FLAGS::kUsesCursor);
			ImIO->ConfigFlags &= ~(ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoKeyboard);

		}
		else {
			menuFlags.reset(UI_MENU_FLAGS::kUpdateUsesCursor);
			menuFlags.reset(UI_MENU_FLAGS::kUsesCursor);
			ImIO->ConfigFlags |= ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoKeyboard;
		}

		logger::trace("GTSMenu Cursor: {}", a_enable);
	}

	std::string GTSMenu::DebugName() {
		return "::GTSHUDMenu";
	}

	void GTSMenu::DataReady() {

		log::info("Registering GTSMenu...");

		if (static UI* const ui = UI::GetSingleton()) {
			ui->Register(MENU_NAME, Creator);
			SetVisibility(true);

			log::info("Successfully registered GTSMenu");
			return;
		}

		log::error("Failed to register GTSMenu");
		ReportAndExit("Failed to register GTSMenu");

	}

	void GTSMenu::MenuChange(const MenuOpenCloseEvent* a_event) {

		auto mName = std::string(a_event->menuName);

		if (
			mName == RE::JournalMenu::MENU_NAME ||
			mName == RE::InventoryMenu::MENU_NAME ||
			mName == RE::MapMenu::MENU_NAME ||
			mName == RE::BookMenu::MENU_NAME ||
			mName == RE::LockpickingMenu::MENU_NAME ||
			mName == RE::MagicMenu::MENU_NAME ||
			mName == RE::RaceSexMenu::MENU_NAME ||
			mName == RE::CraftingMenu::MENU_NAME ||
			mName == RE::SleepWaitMenu::MENU_NAME ||
			mName == RE::TrainingMenu::MENU_NAME ||
			mName == RE::BarterMenu::MENU_NAME ||
			mName == RE::FavoritesMenu::MENU_NAME ||
			mName == RE::GiftMenu::MENU_NAME ||
			mName == RE::StatsMenu::MENU_NAME ||
			mName == RE::ContainerMenu::MENU_NAME ||
			mName == RE::DialogueMenu::MENU_NAME ||
			mName == RE::MessageBoxMenu::MENU_NAME ||
			mName == RE::TweenMenu::MENU_NAME ||
			//mName == RE::MainMenu::MENU_NAME ||
			mName == "CustomMenu") { // papyrus custom menus go here

			a_event->opening ? Hide(mName) : Show(mName);

		}
		// for some reason, after each cell change, the menu is hidden and needs to be shown again
		else if (mName == RE::LoadingMenu::MENU_NAME) {
			if (!a_event->opening) {
				SetVisibility(true);
			}
		}
	}

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

		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX11_NewFrame();
		ImGui::NewFrame();

		Input->ProcessInputEventQueue();
		WindowManager->Update();

		depthPriority = WindowManager->GetDesiredPriority();
		SetInputFlags(WindowManager->GetDesiredCursorState());

		m_frameReady.store(true, std::memory_order_release);
	}

	void GTSMenu::PostDisplay() {

		IMenu::PostDisplay();

		if (m_frameReady.exchange(false, std::memory_order_acquire)) {
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
		
	}
}
