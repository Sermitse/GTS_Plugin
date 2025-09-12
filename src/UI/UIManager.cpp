#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/Lib/imgui_impl_dx11.h"
#include "UI/ImGui/Lib/imgui_impl_win32.h"
#include "UI/UIManager.hpp"
#include "Managers/Console/ConsoleManager.hpp"
#include "UI/ImGui/ImUtil.hpp"
#include "UI/ImGui/ImWindowManager.hpp"
#include "UI/Windows/WindowSettings.hpp"
#include "UI/Windows/WindowStatus.hpp"
#include "Managers/Input/InputManager.hpp"
#include "Windows/WindowUnderstomp.hpp"
#include "ImGui/ImGraphics.hpp"

namespace {

    void OpenSettingsImpl(bool a_IsConsole) {

        auto UI = RE::UI::GetSingleton();
        
        if (!GTS::Plugin::Ready() || GTS::Plugin::AnyMenuOpen() || UI->IsMenuOpen(RE::FaderMenu::MENU_NAME)) {

            if (a_IsConsole) {
                GTS::Cprint("Can not open the settings menu at this time. Try again in game.");
            }

            return;
        }

        if (auto Window = GTS::ImWindowManager::GetSingleton().GetWindowByName("Settings")) {

            if (UI->IsMenuOpen(RE::Console::MENU_NAME)) {
                const auto msgQueue = RE::UIMessageQueue::GetSingleton();
                msgQueue->AddMessage(RE::Console::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
            }

            if (UI->IsMenuOpen(RE::DialogueMenu::MENU_NAME)) {
                const auto msgQueue = RE::UIMessageQueue::GetSingleton();
                msgQueue->AddMessage(RE::DialogueMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
            }

            GTS::UIManager::UnPausedGameTime = GTS::Time::GGTM();

            if (GTS::Config::Advanced.bPauseGame) {
                //Pause the game
                RE::UI::GetSingleton()->numPausesGame++;
                GTS::UIManager::GamePaused = true;
                //Old method Only stops world update, Its litterally only used for the TFC 1 command in the whole exe.
                //WorldUpdate Checks this value and it only ever gets set by the TFC 1 console command....
                //RE::Main::GetSingleton()->freezeTime = true;
            }
            else {
                GTS::Time::SGTM(GTS::Config::Advanced.fSGTMMult);
            }

            RE::UIBlurManager::GetSingleton()->IncrementBlurCount();

            //Show Settings Window
            GTS::UIManager::ShouldDrawOverTop = true;
            Window->Show = true;


        }
    }

    void OpenSettingsK(const GTS::ManagedInputEvent& data) {
        OpenSettingsImpl(false);
    }

    void OpenSettingsC() {
        OpenSettingsImpl(true);
    }

}

namespace GTS {


    void UIManager::ShowInfos() {
        if (auto Window = dynamic_cast<WindowStatus*>(GTS::ImWindowManager::GetSingleton().GetWindowByName("Status"))) {
            if (Window->ShouldDraw()) {
                Window->Show();
            }
        }

        if (auto Window = dynamic_cast<WindowUnderstomp*>(GTS::ImWindowManager::GetSingleton().GetWindowByName("UnderstompAngle"))) {
            if (Window->ShouldDraw()) {
                Window->Show();
            }
        }
    }

    void UIManager::CloseSettings() {
        if (auto Window = dynamic_cast<WindowSettings*>(GTS::ImWindowManager::GetSingleton().GetWindowByName("Settings"))) {

        	Window->AsyncSave();

            //Show Settings Window
            ShouldDrawOverTop = false;
            Window->Show = false;

            //If we incremented the pause counter ourselves
            //decrement it again
            //if we didnt and some random message box did, it prevents underflowing the counter
            if (GTS::UIManager::GamePaused){
                if (RE::UI::GetSingleton()->numPausesGame > 0)
                    RE::UI::GetSingleton()->numPausesGame--;
                GTS::UIManager::GamePaused = false;
			}

            RE::UIBlurManager::GetSingleton()->DecrementBlurCount();
            //Restore Modified Game time.
            Hooks::Time::SGTM(GTS::UIManager::UnPausedGameTime);
        }
    }

    void UIManager::Init() {

        if (Initialized.load()) {
            return;
        }

        const auto RenderManager = RE::BSRenderManager::GetSingleton();

        const auto D3DDevice = reinterpret_cast<ID3D11Device*>(RenderManager->GetRuntimeData().forwarder);
        const auto D3DContext = reinterpret_cast<ID3D11DeviceContext*>(RenderManager->GetRuntimeData().context);
        const auto DXGISwapChain = reinterpret_cast<IDXGISwapChain*>(RenderManager->GetRuntimeData().swapChain);

        DXGI_SWAP_CHAIN_DESC SwapChainDesc{};

		HRESULT hr = DXGISwapChain->GetDesc(&SwapChainDesc);

		if (FAILED(hr)) {
		    logger::error("Could not get the swapchain, HRESULT: {}", hr);
            ReportAndExit("Could not get the swapchain, HRESULT: {}");
		}

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = ImGuiINI.data();

        io.DisplaySize = {
        	static_cast<float>(SwapChainDesc.BufferDesc.Width),
        	static_cast<float>(SwapChainDesc.BufferDesc.Height)
        };

        io.ConfigNavCaptureKeyboard = false;
        io.ConfigNavEscapeClearFocusWindow = false;
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

        ImGui_ImplWin32_Init(SwapChainDesc.OutputWindow);
        ImGui_ImplDX11_Init(D3DDevice, D3DContext);

        FontMgr.Init();
    	StyleMgr.LoadStyle();

        WinMgr.AddWindow(std::make_unique<WindowSettings>());
        WinMgr.AddWindow(std::make_unique<WindowStatus>());
        WinMgr.AddWindow(std::make_unique<WindowUnderstomp>());

        Graphics = new ImGraphics(D3DDevice, D3DContext);
        Graphics->Load();

        Initialized.store(true);

        logger::info("ImGui Init OK");

        InputManager::RegisterInputEvent("OpenModSettings", OpenSettingsK);
        ConsoleManager::RegisterCommand("menu", OpenSettingsC, "Open the settings menu");
    }

    void UIManager::Draw() {

        if (!Initialized.load()) {
            return;
        }

        if (!Plugin::Ready()) {
            return;
        }

        ProcessInputEventQueue();

        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX11_NewFrame();

        ImGui::NewFrame();

        ImWindowManager::GetSingleton().Update();

#ifdef GTS_PROFILER_ENABLED
		if (Profilers::DrawProfiler) {
            ShouldDrawOverTop = true;
        }
        GTS_PROFILER_DISPLAY_REPORT();
#endif

        ImGui::Render();

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    }
}


