#include "UI/Windows/Other/DebugWindow.hpp"
#include "UI/Controls/Button.hpp"
#include "UI/Controls/CheckBox.hpp"
#include "UI/GTSMenu.hpp"

#include "Config/Config.hpp"

#include "Managers/HighHeel.hpp"
#include "Managers/Input/InputManager.hpp"
#include "Managers/Input/ManagedInputEvent.hpp"

#include "UI/Core/ImUtil.hpp"



namespace {

	using namespace RE;
	using namespace GTS;

	void DrawScaleDebugInfo(Actor* actor) {
		if (!actor) {
			return;
		}

		float hh = HighHeelManager::GetBaseHHOffset(actor)[2] / 100.0f;
		float gigantism = Ench_Aspect_GetPower(actor) * 100.0f;
		float naturalscale = get_natural_scale(actor, true);
		float scale = get_visual_scale(actor);
		float maxscale = get_max_scale(actor);
		float bbSize = GetSizeFromBoundingBox(actor);
		
		Actor* player = PlayerCharacter::GetSingleton();
		std::string name = fmt::format("{} - 0x{:X}",actor->GetDisplayFullName(), actor->formID);

		ImGui::SeparatorText(name.c_str());

		ImGui::Text("Bounding Box -> Size: %.2f", bbSize);
		ImGui::Text("Game Scale: %.2f", game_getactorscale(actor));
		ImGui::Text("Size Diff vs Player (Visual): %.2f", get_scale_difference(player, actor, SizeType::VisualScale, false, true));
		ImGui::Text("Height: %s", GetFormatedHeight(actor).c_str());
		ImGui::Text("Weight: %s", GetFormatedWeight(actor).c_str());

		ImGui::Text(
			"Scale: %.2f (Natural: %.2f | BB: %.2f | Limit: %.2f | Aspect: %.1f%%)",
			scale,
			naturalscale,
			bbSize,
			maxscale,
			gigantism
		);

		ImGui::Text(
			"High Heels: %.2f (+%.2f cm / +%.2f ft)",
			hh,
			hh,
			hh * 3.28f
		);

		ImGui::Spacing();

	}
}

namespace GTS {

	void DebugWindow::Draw() {

		ImGui::PushFont(nullptr, 16);

		if (ImGuiEx::Button("Close")) {
			HandleOpenClose(false);
		}

		if (ImGui::CollapsingHeader("GTS Debug")) {
			ImGuiEx::CheckBox("Show Overlay", &Config::Advanced.bShowOverlay);

			#ifdef GTS_PROFILER_ENABLED
			ImGuiEx::CheckBox("Show Profiler", &Profilers::DrawProfiler);
			#endif
		}

		if (ImGui::CollapsingHeader("ImGui Debug Windows")) {
			ImGuiEx::CheckBox("Show Stack", &m_showStackWindow);
			ImGuiEx::CheckBox("Show Demo", &m_showDemoWindow);
			ImGuiEx::CheckBox("Show Metrics", &m_showMetricsWindow);
		}

		if (ImGui::CollapsingHeader("Graphics Test")) {
			ImGraphics::DebugDrawTest();
			ImGui::Spacing();
		}

		if (ImGui::CollapsingHeader("Graphics Transform Test")) {

			// Create a transformation with continuously changing values
			static ImGraphics::ImageTransform transform;
			static ImGraphics::ImageTransform transform2;
			static ImGraphics::ImageTransform transform3;
			static ImGraphics::ImageTransform transform4;
			static ImGraphics::ImageTransform transform5;

			static float time = 0.0f;

			// Increment time
			time += ImGui::GetIO().DeltaTime;

			// Oscillating recolor through RGB spectrum
			transform.recolorEnabled = true;
			float hue = std::fmod(time * 0.3f, 1.0f); // Cycle through hue every ~3.3 seconds
			float r = std::abs(std::sin(hue * std::numbers::pi * 2.0f));
			float g = std::abs(std::sin((hue + 0.333f) * std::numbers::pi * 2.0f));
			float b = std::abs(std::sin((hue + 0.666f) * std::numbers::pi * 2.0f));
			transform.targetColor = { r, g, b, 1.0f };

			// Continuous rotation (full rotation every 4 seconds)
			transform2.affine.rotation = time * (std::numbers::pi / 2.0f);

			// Pulsing scale (between 0.8x and 1.8x)
			float scaleValue = 1.3f + 0.5f * std::sin(time * 2.0f);
			transform.affine.scale = { scaleValue, scaleValue };

			// Oscillating translation
			transform3.affine.translation.x = 10.0f * std::sin(time * 1.5f);
			transform3.affine.translation.y = 10.0f * std::cos(time * 1.5f);

			// Flip every 2 seconds
			transform3.affine.flipHorizontal = static_cast<int>(time / 2.0f) % 2 == 0;
			transform3.affine.flipVertical = static_cast<int>(time / 3.0f) % 2 == 0;

			// Cycling cutoff direction every 5 seconds
			int dirIndex = static_cast<int>(time / 5.0f) % 4;
			switch (dirIndex) {
				case 0: transform4.transformDirection = ImGraphics::Direction::LeftToRight; break;
				case 1: transform4.transformDirection = ImGraphics::Direction::RightToLeft; break;
				case 2: transform4.transformDirection = ImGraphics::Direction::TopToBottom; break;
				case 3: transform4.transformDirection = ImGraphics::Direction::BottomToTop; break;
				default: break;
			}

			// Oscillating cutoff percentage (0% to 100% and back)
			transform4.cutoffPercent = 0.5f + 0.5f * std::sin(time);

			transform5.gradientFadeEnabled  = true;
			transform5.gradientStartPercent = std::sin(time) * 0.5f + 0.5f; // Varies between 0.0 and 1.0
			transform5.gradientTargetAlpha = 0.0f;

			switch (dirIndex) {
				case 0: transform5.transformDirection = ImGraphics::Direction::LeftToRight; break;
				case 1: transform5.transformDirection = ImGraphics::Direction::RightToLeft; break;
				case 2: transform5.transformDirection = ImGraphics::Direction::TopToBottom; break;
				case 3: transform5.transformDirection = ImGraphics::Direction::BottomToTop; break;
				default: break;
			}


			ImGui::Text("Time: %.2f", time);
			ImGui::Text("Color: R:%.2f G:%.2f B:%.2f", r, g, b);
			ImGui::Text("Rotation: %.2f rad (%.1f deg)", transform2.affine.rotation, transform2.affine.rotation * 180.0f / std::numbers::pi);
			ImGui::Text("Scale: %.2f", scaleValue);
			ImGui::Text("Cutoff: %s %.1f%%", dirIndex == 0 ? "L->R" : dirIndex == 1 ? "R->L" : dirIndex == 2 ? "T->B" : "B->T", transform4.cutoffPercent * 100.0f);

			ImGraphics::RenderTransformed(ImageList::PlaceHolder, transform,  { 96, 96 });
			ImGui::SameLine();
			ImGraphics::RenderTransformed(ImageList::PlaceHolder, transform2, { 96, 96 });
			ImGui::SameLine();
			ImGraphics::RenderTransformed(ImageList::PlaceHolder, transform3, { 96, 96 });
			ImGui::SameLine();
			ImGraphics::RenderTransformed(ImageList::PlaceHolder, transform4, { 96, 96 });
			ImGui::SameLine();
			ImGraphics::RenderTransformed(ImageList::PlaceHolder, transform5, { 96, 96 });

			ImGui::Spacing();
		}

		//Multi-Language Font Test
		if (ImGui::CollapsingHeader("Font Test")) {
			ImGui::Text("This îs à fónt tèst — façade, naïve, jalapeño, groß, déjà vu, fiancée, coöperate, élève");
			ImGui::Text("Αυτή είναι μια δοκιμή για το σύστημα γραμματοσειράς");
			ImGui::Text("Это тест загрузчика шрифтов");
			ImGui::Text("これはフォントローダーのテストです");
			ImGui::Text("이것은 폰트로더 테스트입니다");
			ImGui::Text("这是一个字体加载器测试");
			ImGui::Spacing();
		}

		if (ImGui::CollapsingHeader("Misc")) {
			if (ImGuiEx::Button("Quit", "This will immediatly close the game.", false, 1.0f)) {
				WinAPI::TerminateProcess(WinAPI::GetCurrentProcess(), EXIT_SUCCESS);
			}

			ImGui::SameLine();

			if (ImGuiEx::Button("Trigger Crash", "This will immediatly crash the game.", false, 1.0f)) {
				using FuncType = void(*)();
				FuncType func = nullptr;
				func();
			}
		}

		if (ImGui::CollapsingHeader("Experimental", ImUtil::HeaderFlagsDefaultOpen)) {

			if (ImGuiEx::Button("Clear World Decals", "Removes All Loaded World Decals. Needs cell reload to fully take effect.", false, 1.0f)) {

				TES::GetSingleton()->ForEachCell([&](TESObjectCELL* a_ref) {
					if (const NiPointer<NiNode>& cell = a_ref->GetRuntimeData().loadedData->cell3D) {
						if (BGSDecalNode* node = static_cast<BGSDecalNode*>(cell->GetObjectByName(FixedStrings::GetSingleton()->decalNode))) {
							for (NiPointer<BSTempEffect>& decal : node->GetRuntimeData().decals) {
								decal.get()->lifetime = 0.0f;
								decal->Detach();
								decal->Update(0.0f);
								decal->Get3D()->CullNode(true);
								decal->Get3D()->UpdateMaterialAlpha(0.0f, false);
							}
						}
					}
				});
			}
			static int32_t deletedcnt = -1;
			if (ImGuiEx::Button("Delete Dead Dynamic NPC's", "Disables and deletes dynamic form NPC's (Refid starting with FF) in a 4x4 cell radius.", false, 1.0f)) {
				deletedcnt = 0;

				TES::GetSingleton()->ForEachReferenceInRange(PlayerCharacter::GetSingleton(), 16384.0f, [&](TESObjectREFR* a_ref) {

					if (Actor* asActor = skyrim_cast<Actor*>(a_ref)) {

						if (asActor->IsDynamicForm() && asActor->IsDead()) {
							asActor->Disable();
							asActor->SetDelete(true);
							deletedcnt++;
						}

					}
					return BSContainer::ForEachResult::kContinue;
				});
			}

			if (deletedcnt > 0) {
				ImGui::Text("Deleted %d NPC's", deletedcnt);
			}
		}

		if (ImGui::CollapsingHeader("bhkCharacterController maxSlope")) {
			//Value is mislabeled in clib, its a float storing the inverse cosine of the max slope angle in radians.
			auto& maxSlopeRaw = PlayerCharacter::GetSingleton()->GetCharController()->maxSlope;
			float asFloat = std::bit_cast<float>(maxSlopeRaw);
			ImGui::Text("Raw uint32: %u", maxSlopeRaw);
			ImGui::Text("As float: %.6f", asFloat);
			ImGui::Text("Radians to degrees: %.2f°", asFloat * 180.0f / std::numbers::pi);
			ImGui::Text("As tan(angle): %.2f°", std::atan(asFloat) * 180.0f / std::numbers::pi);
			ImGui::Text("As cos(angle): %.2f°", std::acos(asFloat) * 180.0f / std::numbers::pi);
			ImGui::Text("As slope ratio (rise/run): %.2f%%", asFloat * 100.0f);
		}

		if (ImGui::CollapsingHeader("Size Debug")) {
			for (const auto& actor : find_actors()) {
				DrawScaleDebugInfo(actor);
			}
		}

		if (ImGui::CollapsingHeader("Test Values")) {

			ImGui::InputFloat("fTest1", &Config::Experiments.fTest1);
			ImGui::InputFloat("fTest2", &Config::Experiments.fTest2);
			ImGui::InputFloat("fTest3", &Config::Experiments.fTest3);
			ImGui::InputFloat("fTest4", &Config::Experiments.fTest4);
			ImGui::InputFloat("fTest5", &Config::Experiments.fTest5);
			ImGui::InputFloat("fTest6", &Config::Experiments.fTest6);
			ImGui::InputFloat("fTest7", &Config::Experiments.fTest7);
			ImGui::InputFloat("fTest8", &Config::Experiments.fTest8);

			ImGui::InputInt("iTest1", &Config::Experiments.iTest1);
			ImGui::InputInt("iTest2", &Config::Experiments.iTest1);
			ImGui::InputInt("iTest3", &Config::Experiments.iTest1);
			ImGui::InputInt("iTest4", &Config::Experiments.iTest1);
			ImGui::InputInt("iTest5", &Config::Experiments.iTest1);
			ImGui::InputInt("iTest6", &Config::Experiments.iTest1);
			ImGui::InputInt("iTest7", &Config::Experiments.iTest1);
			ImGui::InputInt("iTest8", &Config::Experiments.iTest1);

		}

		ImGui::PopFont();
		ImGui::Spacing();
	}

	bool DebugWindow::WantsToDraw() {
		return m_show;
	}

	void DebugWindow::HandleOpenClose(bool a_open) {

		if (!State::Ready() && !m_show) {
			logger::warn("Can't show menu: Not Ingame!");
			return;
		}

		if (a_open) {

			SKSE::GetTaskInterface()->AddUITask([] {
				if (const auto msgQueue = UIMessageQueue::GetSingleton()) {
					//The console draws above and since we disable input it becomes unclosable, we need to close it ourselves.
					msgQueue->AddMessage(Console::MENU_NAME, UI_MESSAGE_TYPE::kHide, nullptr);
				}
			});

			m_show = true;
		}

		else {
			m_show = false;
		}
	}

	void DebugWindow::OpenSettingsKeybindCallback(const ManagedInputEvent& a_event) {
		if (auto Window = dynamic_cast<DebugWindow*>(GTSMenu::WindowManager->wDebug)) {

			if (!Config::Hidden.IKnowWhatImDoing) {
				return;
			}

			Window->HandleOpenClose(true);
			return;
		}
		logger::error("Can't call handler window, pointer was invalid!");
	}

	void DebugWindow::Init() {

		m_windowType = kWidget;

		//m_flags;
		m_name = "Debug";
		m_title = "Debug";
		m_windowType = kDebug;
		m_anchorPos = WindowAnchor::kTopLeft;
		m_fadeSettings.enabled = false;

		InputManager::RegisterInputEvent("OpenDebugMenu", OpenSettingsKeybindCallback);

	}

	float DebugWindow::GetFullAlpha() {
		return 1.0f;
	}

	float DebugWindow::GetBackgroundAlpha() {
		return 1.0f;
	}

	std::string DebugWindow::GetWindowName() {
		return m_name;
	}

	void DebugWindow::RequestClose() {
		HandleOpenClose(false);
	}

	void DebugWindow::DebugDraw() {

		if (m_showDemoWindow) {
			ImGui::ShowDemoWindow(&m_showDemoWindow);
		}

		if (m_showMetricsWindow) {
			ImGui::ShowMetricsWindow(&m_showMetricsWindow);
		}

		if (m_showStackWindow) {
			ImGui::ShowIDStackToolWindow(&m_showStackWindow);
		}

		GTS_PROFILER_DISPLAY_REPORT();

	}

	bool DebugWindow::IsDebugging() {
		return m_showStackWindow       ||
			   m_showMetricsWindow     ||
            #ifdef GTS_PROFILER_ENABLED
			   Profilers::DrawProfiler ||
            #endif
			   m_showDemoWindow;
	}
}
