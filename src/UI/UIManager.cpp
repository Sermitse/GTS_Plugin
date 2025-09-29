#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/Lib/imgui_impl_dx11.h"
#include "UI/ImGui/Lib/imgui_impl_win32.h"

#include "UI/UIManager.hpp"

namespace GTS {

    void UIManager::ShowInfos() {
        /*if (auto Window = dynamic_cast<WindowStatus*>(WindowManager->GetWindowByName("Status"))) {
            if (Window->ShouldDraw()) {
                Window->Show();
            }
        }

        if (auto Window = dynamic_cast<WindowUnderstomp*>(WindowManager->GetWindowByName("Understomp"))) {
            if (Window->ShouldDraw()) {
                Window->Show();
            }
        }*/
    }

    void UIManager::CloseSettings() {
   //     if (auto Window = dynamic_cast<WindowSettings*>(WindowManager->GetWindowByName("Settings"))) {

   //     	Window->AsyncSave();

   //         //Show Settings Window
   //         ShouldDrawOverTop = false;
   //         Window->m_show = false;

   //         //If we incremented the pause counter ourselves
   //         //decrement it again
   //         //if we didnt and some random message box did, it prevents underflowing the counter
   //         if (GTS::UIManager::GamePaused){
   //             if (RE::UI::GetSingleton()->numPausesGame > 0)
   //                 RE::UI::GetSingleton()->numPausesGame--;
   //             GTS::UIManager::GamePaused = false;
			//}

   //         RE::UIBlurManager::GetSingleton()->DecrementBlurCount();
   //         //Restore Modified Game time.
   //         Hooks::Time::SGTM(GTS::UIManager::UnPausedGameTime);

   //         GTS::ImInput::ShowGameCursor(false);
   //     }
    }

    void UIManager::Init() {

        if (m_initialized.load()) {
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
        io.IniFilename = m_ImGuiINI.data();

        io.DisplaySize = {
            static_cast<float>(SwapChainDesc.BufferDesc.Width),
            static_cast<float>(SwapChainDesc.BufferDesc.Height)
        };

        io.ConfigNavCaptureKeyboard = false;

        // Cursor configuration:
        // - MouseDrawCursor = false: We don't want ImGui to draw its own cursor for HUD/Cursor layers
        // - ConfigFlags: Don't change cursor because we use game's cursor for HUD/Cursor layers
        // - For kPresent layer, ImGui Win32 backend will handle cursor automatically
        io.MouseDrawCursor = false;
        io.ConfigNavEscapeClearFocusWindow = false;
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

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

        m_initialized.store(true);

        logger::info("UIManager Init OK");

    }

    void UIManager::Update() {

        if (!m_initialized.load(std::memory_order_relaxed)) {
            return;
        }

        if (m_frameReady.load(std::memory_order_relaxed)) {
            ImGui::EndFrame();
            return;
        }

        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX11_NewFrame();
        ImGui::NewFrame();

        Input->ProcessInputEventQueue();
        Input->UpdateCursorState();

        // Update ImGui cursor visibility based on draw level
        // kPresent layer uses ImGui's Win32 cursor
        // HUD/Cursor layers use game cursor (don't show ImGui cursor)
        ImGuiIO& io = ImGui::GetIO();
        if (WindowManager->GetHighestDrawLevel() == ImWindow::DrawLevel::kPresent) {
            // Allow ImGui to manage cursor for kPresent layer
            io.MouseDrawCursor = true; 
            io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
            Input->GetOverlayWantsCursor();
        }
        else {
            // Use game cursor for HUD/Cursor layers
            io.MouseDrawCursor = false;
            io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
            Input->UpdateMousePos();
        }

        WindowManager->Update();

        m_frameReady.store(true, std::memory_order_release);

    }

    void UIManager::Present(ImWindow::DrawLevel a_level) {

        if (WindowManager->GetHighestDrawLevel() != a_level) {
            return;
        }

        {
            ReCallGuard guard(g_alreadyPresenting);

            if (!guard.acquired()) {
                return;
            }

            if (m_frameReady.exchange(false, std::memory_order_acquire)) {
                ImGui::Render();
                ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            }
        }

    }

    bool UIManager::Ready() {
        return GetSingleton().m_initialized.load();
    }

    void UIManager::ProcessAndFilterEvents(RE::InputEvent** a_events) {

        Input->ProcessInputEvents(a_events);

        //A menu that needs input is open, remove all keypresses from the linked list.
        if (WindowManager->HasInputConsumers()) {
            //ImInput::RemoveAllKeyEvents(a_events);
            return;
        }

        Input->ResetImGuiInputState();

    }
}


