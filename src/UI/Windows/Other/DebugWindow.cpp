#include "UI/Windows/Other/DebugWindow.hpp"
#include "UI/Controls/Button.hpp"
#include "UI/Controls/CheckBox.hpp"
#include "UI/GTSMenu.hpp"

#include "Config/Config.hpp"

#include "Managers/HighHeel.hpp"
#include "Managers/Input/InputManager.hpp"
#include "Managers/Input/ManagedInputEvent.hpp"

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

		// ------------------------------ CLOSE BUTTON
		if (ImGuiEx::Button("Close")) {
			HandleOpenClose(false);
		}

		// ------------------------------ GTS
		if (ImGui::CollapsingHeader("GTS")) {

			ImGui::Indent();
			{
				ImGuiEx::CheckBox("Show Overlay", &Config::Advanced.bShowOverlay);

				#ifdef GTS_PROFILER_ENABLED
				ImGuiEx::CheckBox("Show Profiler", &Profilers::DrawProfiler);
				#endif

				if (ImGui::CollapsingHeader("Scale Information (Size Debug)")) {
					for (const auto& actor : find_actors()) {
						DrawScaleDebugInfo(actor);
					}
				}
			}
			ImGui::Unindent();
		}

		// ------------------------------ UI
		if (ImGui::CollapsingHeader("UI")) {
			

			ImGui::Indent();
			{
				if (ImGui::CollapsingHeader("ImGui Debug")) {
					ImGuiEx::CheckBox("Show Stack", &m_showStackWindow);
					ImGuiEx::CheckBox("Show Demo", &m_showDemoWindow);
					ImGuiEx::CheckBox("Show Metrics", &m_showMetricsWindow);
				}

				// ------------------------------ ImGraphics Debug
				if (ImGui::CollapsingHeader("ImGraphics - Images")) {
					ImGraphics::DebugDrawTest();
					ImGui::Spacing();
				}

				// ------------------------------ ImGraphics Transforms
				if (ImGui::CollapsingHeader("ImGraphics - Transforms")) {

					static ImGraphics::ImageTransform transform, transform2, transform3, transform4, transform5;
					static float time = 0.0f;
					time += ImGui::GetIO().DeltaTime;

					auto dirFromIndex = [](int idx) {
						switch (idx) {
							case 0:  return ImGraphics::Direction::LeftToRight;
							case 1:  return ImGraphics::Direction::RightToLeft;
							case 2:  return ImGraphics::Direction::TopToBottom;
							default: return ImGraphics::Direction::BottomToTop;
						}
					};

					// Oscillating recolor through RGB spectrum
					{
						float hue = std::fmod(time * 0.3f, 1.0f);
						float r = std::abs(std::sin(hue * std::numbers::pi * 2.0f));
						float g = std::abs(std::sin((hue + 0.333f) * std::numbers::pi * 2.0f));
						float b = std::abs(std::sin((hue + 0.666f) * std::numbers::pi * 2.0f));
						transform.recolorEnabled = true;
						transform.targetColor = { r, g, b, 1.0f };
						float scaleValue = 1.3f + 0.5f * std::sin(time * 2.0f);
						transform.affine.scale = { scaleValue, scaleValue };

						ImGui::Text("Time: %.2f", time);
						ImGui::Text("Color: R:%.2f G:%.2f B:%.2f", r, g, b);
						ImGui::Text("Scale: %.2f", scaleValue);
					}

					// Continuous rotation
					{
						transform2.affine.rotation = time * (std::numbers::pi / 2.0f);
						ImGui::Text("Rotation: %.2f rad (%.1f deg)", transform2.affine.rotation, transform2.affine.rotation * 180.0f / std::numbers::pi);
					}

					// Oscillating translation + flip
					{
						transform3.affine.translation = { 10.0f * std::sin(time * 1.5f), 10.0f * std::cos(time * 1.5f) };
						transform3.affine.flipHorizontal = static_cast<int>(time / 2.0f) % 2 == 0;
						transform3.affine.flipVertical = static_cast<int>(time / 3.0f) % 2 == 0;
					}

					// Cycling cutoff direction + gradient
					{
						int dir = static_cast<int>(time / 5.0f) % 4;
						transform4.transformDirection = dirFromIndex(dir);
						transform4.cutoffPercent = 0.5f + 0.5f * std::sin(time);

						transform5.gradientFadeEnabled = true;
						transform5.transformDirection = dirFromIndex(dir);
						transform5.gradientStartPercent = std::sin(time) * 0.5f + 0.5f;
						transform5.gradientTargetAlpha = 0.0f;

						const char* dirLabel = dir == 0 ? "L->R" : dir == 1 ? "R->L" : dir == 2 ? "T->B" : "B->T";
						ImGui::Text("Cutoff: %s %.1f%%", dirLabel, transform4.cutoffPercent * 100.0f);
					}

					{
						ImGraphics::RenderTransformed(ImageList::PlaceHolder, transform, { 96, 96 });
						ImGui::SameLine();
						ImGraphics::RenderTransformed(ImageList::PlaceHolder, transform2, { 96, 96 });
						ImGui::SameLine();
						ImGraphics::RenderTransformed(ImageList::PlaceHolder, transform3, { 96, 96 });
						ImGui::SameLine();
						ImGraphics::RenderTransformed(ImageList::PlaceHolder, transform4, { 96, 96 });
						ImGui::SameLine();
						ImGraphics::RenderTransformed(ImageList::PlaceHolder, transform5, { 96, 96 });
					}

					ImGui::Spacing();
				}

				// ------------------------------ ImGui Font2 Tests
				if (ImGui::CollapsingHeader("Font2 Test")) {
					ImGui::Text("This îs à fónt tèst - façade, naïve, jalapeño, groß, déjà vu, fiancée, coöperate, élève");
					ImGui::Text("Αυτή είναι μια δοκιμή για το σύστημα γραμματοσειράς");
					ImGui::Text("Это тест загрузчика шрифтов");
					ImGui::Text("これはフォントローダーのテストです");
					ImGui::Text("이것은 폰트로더 테스트입니다");
					ImGui::Text("这是一个字体加载器测试");
					ImGui::Spacing();
				}
			}
			ImGui::Unindent();
		}

		// ------------------------------ Physics
		if (ImGui::CollapsingHeader("Havok")) {
			
			ImGui::Indent();
			{
				// ------------------------------ Char Controller
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
			}
			ImGui::Unindent();

		}

		// ------------------------------ Test
		if (ImGui::CollapsingHeader("Scratch/Test Values")) {

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
