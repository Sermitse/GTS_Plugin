namespace GTS {

   // void UIManager::ShowInfos() {
   //     /*if (auto Window = dynamic_cast<WindowStatus*>(WindowManager->GetWindowByName("Status"))) {
   //         if (Window->ShouldDraw()) {
   //             Window->Show();
   //         }
   //     }

   //     if (auto Window = dynamic_cast<WindowUnderstomp*>(WindowManager->GetWindowByName("Understomp"))) {
   //         if (Window->ShouldDraw()) {
   //             Window->Show();
   //         }
   //     }*/
   // }

   // void UIManager::CloseSettings() {
   ////     if (auto Window = dynamic_cast<WindowSettings*>(WindowManager->GetWindowByName("Settings"))) {

   ////     	Window->AsyncSave();

   ////         //Show Settings Window
   ////         ShouldDrawOverTop = false;
   ////         Window->m_show = false;

   ////         //If we incremented the pause counter ourselves
   ////         //decrement it again
   ////         //if we didnt and some random message box did, it prevents underflowing the counter
   ////         if (GTS::UIManager::GamePaused){
   ////             if (RE::UI::GetSingleton()->numPausesGame > 0)
   ////                 RE::UI::GetSingleton()->numPausesGame--;
   ////             GTS::UIManager::GamePaused = false;
			////}

   ////         RE::UIBlurManager::GetSingleton()->DecrementBlurCount();
   ////         //Restore Modified Game time.
   ////         Hooks::Time::SGTM(GTS::UIManager::UnPausedGameTime);

   ////         GTS::ImInput::ShowGameCursor(false);
   ////     }
   // }

}


