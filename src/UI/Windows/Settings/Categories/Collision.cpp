#include "UI/Windows/Settings/Categories/Collision.hpp"

#include "UI/Controls/Slider.hpp"
#include "UI/Core/ImUtil.hpp"
#include "Config/Config.hpp"
#include "UI/Controls/CheckBox.hpp"
#include "UI/Controls/Text.hpp"

namespace GTS {


	CategoryCollision::CategoryCollision() {
		m_name = "Collision";
	}

	void CategoryCollision::DrawLeft() {

		ImUtil_Unique
		{

			PSString T0 = "Visualize collision shapes for debugging purposes.\n"
						  "- Yellow: Capsule colliders\n"
						  "- Magenta: Simple convex vertices colliders.\n"
						  "- Cyan: Bone-driven convex vertices colliders.";

			PSString T1 = "Show bumper collision shapes.\n"
				          "These are purely visual. Character bumper colission is disabled.";

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
						     "It does not change shape or follow individual bones, and only updates when the character's scale or state changes\n"
						     "(for example when switching between walking, sneaking, or swimming).";

			PSString TDMax = "Maximum scale allowed for bone driven collision shape.\n\n"
							 "Note: A cap for this exists for performance reasons.\n"
							 "It's recomended that you leave this at 50.0x";

			PSString TWidth = "Adjusts the horizontal width of the collision shape relative to the base size.";

			if (ImGui::CollapsingHeader("Bone-Driven Collider", ImUtil::HeaderFlagsDefaultOpen)) {

				ImGuiEx::HelpText("What it this", THelp);

				ImGuiEx::CheckBox("Enable Bone-Driven Collision Updates For Followers", &Config::Collision.bEnableBoneDrivenCollisionUpdatesFollowers, T0);
				ImGuiEx::SliderF("Max Scale", &Config::Collision.fDynamicColliderMaxUpdateScale, 10.0f, 250.0f, TDMax, "%.2fx");
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

			PSString TMax = "Maximum scale allowed for the simple (non bone driven) collision shape.\n\n"
							"Note: NPC movement is navmesh-based, not true physics navigation. Very large colliders can increase the chance of getting stuck,\n"
							"or trigger physics instability (which can lead to lag).\n\n"
							"Note 2: This shape also affects projectile collision (ie. arrows and fireballs for example) and melee collision (if precision is not installed).\n"
						    "If you don't plan on having GTS NPC's its best to leave this at 1.0 otherwise a max scale of around 50x is recommended.";

			PSString TMin = "Minimum scale allowed for the simple collision shape.\n"
				            "Acts as a safety floor to prevent the collider from becoming too small and causing clipping or unstable behavior.";

			if (ImGui::CollapsingHeader("Simple Collider", ImUtil::HeaderFlagsDefaultOpen)) {

				ImGuiEx::SliderF("Base Width", &Config::Collision.fSimpleDrivenWidthMultBase, 0.5f, 3.0f, TWidth, "%.2fx");
				ImGuiEx::SliderF("Sneaking Width", &Config::Collision.fSimpleDrivenWidthMultSneaking, 0.5f, 3.0f, TWidth, "%.2fx");
				ImGuiEx::SliderF("Crawling Width", &Config::Collision.fSimpleDrivenWidthMultCrawling, 0.5f, 3.0f, TWidth, "%.2fx");
				ImGuiEx::SliderF("Proning Width", &Config::Collision.fBoneDrivenWidthMultCrawling, 0.5f, 3.0f, TWidth, "%.2fx");
				ImGuiEx::SliderF("Swimming Width", &Config::Collision.fBoneDrivenWidthMultSwimming, 0.5f, 3.0f, TWidth, "%.2fx");

				ImGuiEx::SliderF("Max Scale", &Config::Collision.fMSimpleDrivenColliderMaxScale, Config::Collision.fMSimpleDrivenColliderMinScale, 250.0f, TMax, "%.2fx");
				ImGuiEx::SliderF("Min Scale", &Config::Collision.fMSimpleDrivenColliderMinScale, 0.05f, Config::Collision.fMSimpleDrivenColliderMaxScale, TMin, "%.2fx");

				ImGuiEx::SliderF("Swimming Height", &Config::Collision.fSimpleDrivenHeightMultSwimming, 0.1f, 1.0f, THeight, "%.2fx");
				ImGuiEx::SliderF("Sneaking Height", &Config::Collision.fSimpleDrivenHeightMultSneaking, 0.1f, 1.0f, THeight, "%.2fx");
				ImGuiEx::SliderF("Crawling Height", &Config::Collision.fSimpleDrivenHeightMultCrawling, 0.1f, 1.0f, THeight, "%.2fx");

				ImGui::Spacing();
			}

		}

		ImUtil_Unique
		{

			PSString THelp = "The game gives every actor the same step height and the same maximum walkable slope regardless of size.\n"
							 "A large character is therefore stopped by the same 31 unit rock that stops a normal one, which is what makes\n"
							 "giants catch on small terrain. These settings scale both with the character.";

			PSString T0 = "Scale step height and walkable slope with character size. Applies to the player and NPCs alike.";

			PSString TStep = "How much step height follows the character's scale.\n"
							 "0.00x keeps the vanilla step height, 1.00x makes it fully proportional (a 10x character steps over 310 units instead of 31).";

			PSString TSlope = "Steepest surface a fully scaled character will treat as walkable.\n\n"
							  "Note: Values near 90 degrees stop the game from blocking near vertical surfaces at all, which lets the\n"
							  "physics solver fling the character. Around 70 is the highest generally safe value.";

			PSString TRamp = "Scale at which the maximum slope is reached. Below this the angle is interpolated from the vanilla value.";

			PSString TMax = "Scale at which step height and slope stop increasing.";

			PSString TRadius = "How much the collision shape's edge rounding follows the character's scale.\n\n"
							   "The game rounds every character's collider by a fixed amount regardless of size, so a large character\n"
							   "ends up with a proportionally sharp bottom rim that snags on terrain a small one rides over.\n"
							   "0.00x is the vanilla fixed rounding, 1.00x keeps it proportional. The shape is shrunk to compensate,\n"
							   "so raising this does not make the collider wider.";

			if (ImGui::CollapsingHeader("Terrain Traversal", ImUtil::HeaderFlagsDefaultOpen)) {

				ImGuiEx::HelpText("What is this", THelp);

				ImGuiEx::CheckBox("Scale Terrain Traversal", &Config::Collision.bScaleTraversal, T0);
				ImGuiEx::SliderF("Step Height Scaling", &Config::Collision.fTraversalStepScaling, 0.0f, 1.0f, TStep, "%.2fx");
				ImGuiEx::SliderF("Max Slope", &Config::Collision.fTraversalMaxSlopeDegrees, 45.0f, 85.0f, TSlope, "%.0f°");
				ImGuiEx::SliderF("Slope Ramp Scale", &Config::Collision.fTraversalSlopeRampScale, 1.0f, 20.0f, TRamp, "%.2fx");
				ImGuiEx::SliderF("Max Scale", &Config::Collision.fTraversalMaxScale, 1.0f, 100.0f, TMax, "%.2fx");
				ImGuiEx::SliderF("Collider Edge Rounding", &Config::Collision.fConvexRadiusScaling, 0.0f, 1.0f, TRadius, "%.2fx");

				ImGui::Spacing();
			}
		}
	}
}
