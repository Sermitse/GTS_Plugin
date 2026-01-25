#include "UI/Windows/Settings/Categories/Collision.hpp"

#include "UI/Controls/Slider.hpp"
#include "UI/Core/ImUtil.hpp"
#include "Config/Config.hpp"
#include "UI/Controls/CheckBox.hpp"
#include "UI/Controls/ComboBox.hpp"
#include "UI/Controls/Text.hpp"

namespace GTS {


	CategoryCollision::CategoryCollision() {
		m_name = "Collision";
	}

	void CategoryCollision::DrawLeft() {

		ImUtil_Unique
		{

			PSString T0 = "Visualize collision shapes for debugging purposes.";
			PSString T1 = "Show bumper collision shapes.";

			if (ImGui::CollapsingHeader("Collider Debug", ImUtil::HeaderFlagsDefaultOpen)) {

				ImGuiEx::CheckBox("Draw Debug Shapes", &Config::Collision.bDrawDebugShapes, T0);
				ImGuiEx::CheckBox("Draw Bumper Shapes", &Config::Collision.bDrawBumpers, T1);
				ImGui::Spacing();
			}

		}

		ImUtil_Unique
		{

			PSString T0 = "Enable bone-driven collision updates for followers.\n"
						  "Note: It's reccomended that you don't enable this. (See the help tooltip for an explanation as to why.)";

		    PSString THelp = "Bone-driven collision dynamically adjusts the collision shape by tracking specific bones (such as the head, arms, and legs).\n"
						     "This results in more accurate collision but comes at a higher performance cost, so it is only enabled for the player by default.\n\n"
						     "The simple collider uses uniform scaling: the original collision shape from the base game is scaled up or down as a whole.\n"
						     "It does not change shape or follow individual bones, and only updates when the character's scale or state changes "
						     "(for example when switching between walking, sneaking, or swimming).";

			PSString TWidth = "Adjusts the horizontal width of the collision shape relative to the base size.";

			if (ImGui::CollapsingHeader("Bone-Driven Collider", ImUtil::HeaderFlagsDefaultOpen)) {

				ImGuiEx::HelpText("What it this", THelp);

				ImGuiEx::CheckBox("Enable Bone-Driven Collision Updates For Followers", &Config::Collision.bEnableBoneDrivenCollisionUpdatesFollowers, T0);

				ImGuiEx::SliderF("Base Width", &Config::Collision.fBoneDrivenWidthMultBase, 0.5f, 3.0f, TWidth, "%.2fx");
				ImGuiEx::SliderF("Sneaking Width", &Config::Collision.fBoneDrivenWidthMultSneaking, 0.5f, 3.0f, TWidth, "%.2fx");
				ImGuiEx::SliderF("Crawling Width", &Config::Collision.fBoneDrivenWidthMultCrawling, 0.5f, 3.0f, TWidth, "%.2fx");
				ImGuiEx::SliderF("Proning Width", &Config::Collision.fBoneDrivenWidthMultProning, 0.5f, 3.0f, TWidth, "%.2fx");
				ImGuiEx::SliderF("Swimming Width", &Config::Collision.fBoneDrivenWidthMultSwimming, 0.5f, 3.0f, TWidth, "%.2fx");

				ImGui::Spacing();
			}

		}

	}

	void CategoryCollision::DrawRight() {

		ImUtil_Unique
		{

			PSString TWidth = "Adjusts the character's collision width (left/right) relative to the base size.\n"
							  "1.00x = default width. Higher values make the collider wider; lower values make it narrower.";

			PSString THeight = "Adjusts the character's collision height multiplier (up/down) relative to the base size.\n"
				               "1.00x = Standing height.";

			PSString TMax = "Maximum scale allowed for the collision shape.\n\n"
							"Note: NPC movement is navmesh-based, not true physics navigation. Very large colliders can increase the chance of getting stuck,\n"
							"or trigger physics instability (which can lead to lag).\n\n"
							"Its recomended that you leave this at 1.0x";

			PSString TMin = "Minimum scale allowed for the simple collision shape.\n"
				            "Acts as a safety floor to prevent the collider from becoming too small and causing clipping or unstable behavior.";

			if (ImGui::CollapsingHeader("Simple Collider", ImUtil::HeaderFlagsDefaultOpen)) {

				ImGuiEx::SliderF("Base Width", &Config::Collision.fSimpleDrivenWidthMultBase, 0.5f, 3.0f, TWidth, "%.2fx");
				ImGuiEx::SliderF("Sneaking Width", &Config::Collision.fSimpleDrivenWidthMultSneaking, 0.5f, 3.0f, TWidth, "%.2fx");
				ImGuiEx::SliderF("Crawling Width", &Config::Collision.fSimpleDrivenWidthMultCrawling, 0.5f, 3.0f, TWidth, "%.2fx");
				ImGuiEx::SliderF("Proning Width", &Config::Collision.fBoneDrivenWidthMultCrawling, 0.5f, 3.0f, TWidth, "%.2fx");
				ImGuiEx::SliderF("Swimming Width", &Config::Collision.fBoneDrivenWidthMultSwimming, 0.5f, 3.0f, TWidth, "%.2fx");

				ImGuiEx::SliderF("Max Scale", &Config::Collision.fMSimpleDrivenColliderMaxScale, Config::Collision.fMSimpleDrivenColliderMinScale, 10.0f, TMax, "%.2fx");
				ImGuiEx::SliderF("Min Scale", &Config::Collision.fMSimpleDrivenColliderMinScale, 0.05f, Config::Collision.fMSimpleDrivenColliderMaxScale, TMin, "%.2fx");

				ImGuiEx::SliderF("Swimming Height", &Config::Collision.fSimpleDrivenHeightMultSwimming, 0.1f, 1.0f, THeight, "%.2fx");
				ImGuiEx::SliderF("Sneaking Height", &Config::Collision.fSimpleDrivenHeightMultSneaking, 0.1f, 1.0f, THeight, "%.2fx");
				ImGuiEx::SliderF("Crawling Height", &Config::Collision.fSimpleDrivenHeightMultCrawling, 0.1f, 1.0f, THeight, "%.2fx");

				ImGui::Spacing();
			}
		}
	}
}
