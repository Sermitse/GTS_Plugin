#include "UI/Categories/Camera.hpp"
#include "UI/DearImGui/imgui.h"
#include "UI/ImGui/ImUtil.hpp"

namespace GTS {

	static void DrawCameraOffsets(const char* a_title, const char* a_toolip, std::array<float, 3>* a_offsets) {
	    ImGui::Spacing();

	    ImGui::Text(a_title);
	    ImGui::BeginGroup();

	    const char* T0 = "Move the camera left or right.";
	    const char* T1 = "Move the camera up or down.";
	    const char* T2 = "Move the camera forward or backward.";

	    ImUtil::SliderF("Left - Right", &a_offsets->at(0), -75.f, 75.f, T0, "%.1f");
	    ImUtil::SliderF("Up - Down", &a_offsets->at(2), -75.f, 75.f, T1, "%.1f");
	    ImUtil::SliderF("Forward - Back", &a_offsets->at(1), -300.f, 100.f, T2, "%.1f");

	    ImGui::EndGroup();

	    // Applies to the whole group
	    if (ImGui::IsItemHovered()) {
	        ImGui::SetTooltip(a_toolip);
	    }
	}

	static void DrawCameraSettings(CameraOffsets* a_set, const char* a_title) {

	    const char* T0 = "Select which biped skeleton bone the camera should track.";

	    if (ImGui::CollapsingHeader(a_title, ImGuiTreeNodeFlags_None)) {
	        ImUtil::ComboEx<CameraTrackingSettings>("Center On Bone", a_set->sCenterOnBone, T0);

	        ImUtil_Unique {
	            DrawCameraOffsets(
	                "Offsets | Standing",
	                "Adjust camera offsets when standing.",
	                &a_set->f3NormalStand
	            );
	        }

	        ImUtil_Unique {
	            DrawCameraOffsets(
	                "Offsets | Standing Combat",
	                "Adjust camera offsets when standing and in combat.",
	                &a_set->f3CombatStand
	            );
	        }

	        ImUtil_Unique {
	            DrawCameraOffsets(
	                "Offsets | Crawling",
	                "Adjust camera offsets while sneaking, crawling, or prone.",
	                &a_set->f3NormalCrawl
	            );
	        }

	        ImUtil_Unique {
	            DrawCameraOffsets(
	                "Offsets | Crawling Combat",
	                "Adjust camera offsets while sneaking, crawling, or prone and in combat.",
	                &a_set->f3CombatCrawl
	            );
	        }
	        ImGui::Spacing();
	        ImGui::Spacing();
	    }
	}

	void CategoryCamera::DrawLeft() {
	    ImUtil_Unique {

	        const char* T0 = "Change the intensity of camera shakes when performing actions as a player.";
			const char* T1 = "Change the intensity of first-person camera shakes caused by your own size or size actions";
			const char* T2 = "Change the intensity of camera shakes for NPCs.";

	        if (ImGui::CollapsingHeader("Camera Shake", ImUtil::HeaderFlagsDefaultOpen)) {
	            ImUtil::SliderF("Player Total Shake Power", &Settings.fCameraShakePlayer, 0.1f, 3.0f, T0, "%.2fx");
				ImUtil::SliderF("Player FP Shake Power", &Settings.fCameraShakePlayerFP, 0.0f, 1.0f, T1, "%.2fx");
	            ImUtil::SliderF("NPC Total Shake Power", &Settings.fCameraShakeOther, 0.1f, 3.0f, T2, "%.2fx");

	            ImGui::Spacing();
	        }
	    }

	    ImUtil_Unique {

	        const char* T0 = "Change the height multiplier of the camera while crawling in first person.";
	        const char* T1 = "Change the height multiplier of the camera while crawling in third person.\n"
	    					 "Note: This wont't work correctly if you are using smoothcam.";

	        if (ImGui::CollapsingHeader("Crawl Height", ImUtil::HeaderFlagsDefaultOpen)) {
	            ImUtil::SliderF("1st P. Crawl Height", &Settings.fFPCrawlHeightMult, 0.01f, 1.0f, T0, "%.1fx");
	            ImUtil::SliderF("3rd P. Crawl Height", &Settings.fTPCrawlHeightMult, 0.01f, 1.0f, T1, "%.1fx");

	            ImGui::Spacing();
	        }
	    }

	    ImUtil_Unique {

	        const char* T0 = "Enable camera collision with actors.";
	        const char* T1 = "Enable camera collision with trees.";
	        const char* T2 = "Enable camera collision with debris (physics-enabled objects).";
	        const char* T3 = "Enable camera collision with terrain.";
	        const char* T4 = "Enable camera collision with statics (basically any solid, non-movable object).";
	        const char* T5 = "Change the scale at which the above collision settings should apply.";
			const char* T6 = "Dynamically change the camera near distance value to fix clipping issues when small.\n"
							 "May introduce visual issues such as moving/disappearing shadows.\n\n"
	    				     "Starts applyng when smaller than 1.0x scale.\n"
	    					 "Disables itself when past 1.0x scale\n\n"
	    					 "Note: Can conflict with other mods that also change this value";

	        if (ImGui::CollapsingHeader("Camera Collision", ImGuiTreeNodeFlags_None)) {
	            ImUtil::CheckBox("Collide With Actors", &Settings.bCamCollideActor, T0);
	            ImGui::SameLine();
	            ImUtil::CheckBox("Collide With Trees", &Settings.bCamCollideTree, T1);
	            ImUtil::CheckBox("Collide With Debris", &Settings.bCamCollideDebris, T2);
	            ImGui::SameLine();
	            ImUtil::CheckBox("Collide With Terrain", &Settings.bCamCollideTerrain, T3);
	            ImUtil::CheckBox("Collide With Statics", &Settings.bCamCollideStatics, T4);

	            ImUtil::SliderF("Apply at Scale", &Settings.fModifyCamCollideAt, 0.0f, 50.0f, T5, "%.1fx");

	        	ImGui::Spacing();

	        	ImUtil::CheckBox("Dynamic Near Distance", &Settings.bEnableAutoFNearDist, T6);

	            ImGui::Spacing();
	        }
	    }

	    ImUtil_Unique {

	        const char* T0 = "Offsets the 3rd person camera's minimum zoom distance.\n"
							 "Combined with the maximum distance this affects the distance from the player\n"
							 "where the camera perspective switches to 1st person.";

	        const char* T1 = "Offsets the 3rd person camera's maximum zoom distance.\n"
							 "Higher values will zoom out the camera more.\n"
							 "Combined with the minimum distance this affects the distance from the player\n"
	    					 "where the camera perspective switches to 1st person.";

			const char* T2 = "Changes the transition speed between camera steps.";

	        const char* T3 = "Changes the camera's zoom step divisor.\n"
							 "Lower values increase the ammount of zoom steps,\n"
	    					 "whilst higher values decrease them.\n";

			const char* T4 = "Toggle wether this mod should override skyrim's camera settings.\n"
							 "Note: Requires a game restart after disabling for the original values to be reapplied.\n\n"
							 "Its reccomended to leave this enabled.";

	        const char* THelp = 
				             "These are the same settings as can be found in skyrim.ini.\n"
				             "They're added here for convenience.\n\n"
				             "Note 1: The settings here will continiously override the game's settings,\n"
				             "so no matter what values you set in any ini file or if another mod changes them, they will be overridden by the values set here.\n\n"
							 "Note 2: Its heavily recommended that you do not change the distance settings. This mod's camera system works best if these are left at their default values.\n\n"
	    					 "Defaults:\n"
	    					 " - Min: 150.0\n"
							 " - Max: 600.0\n"
							 " - Zoom: 0.8\n"
							 " - Step: 0.075\n";

	        if (ImGui::CollapsingHeader("Skyrim Camera Settings", ImGuiTreeNodeFlags_None)) {
	            ImGui::TextColored(ImUtil::ColorSubscript, "What is this (?)");

	            if (ImGui::IsItemHovered()) {
	                ImGui::SetTooltip(THelp);
	            }

				ImUtil::CheckBox("Enable Adjustments", &Settings.bEnableSkyrimCameraAdjustments, T4);

				ImGui::BeginDisabled(!&Settings.bEnableSkyrimCameraAdjustments);

	            ImUtil::SliderF("Minimum Distance", &Settings.fCameraDistMin, -100.0f, 300.0f, T0, "%.1f");
	            ImUtil::SliderF("Maximum Distance", &Settings.fCameraDistMax, 50.0f, 1200.0f, T1, "%.1f");
	            ImUtil::SliderF("Zoom Speed", &Settings.fCameraZoomSpeed, 0.1f, 10.0f, T2, "%.1f");
	            ImUtil::SliderF("Zoom Step", &Settings.fCameraIncrement, 0.01f, 0.25f, T3, "%.3f");

				ImGui::EndDisabled();


	            ImGui::Spacing();
	        }
	    }

	}

	void CategoryCamera::DrawRight() {
	    ImUtil_Unique {
	        const char* T0 = "Enable automatic camera.";

	        const char* T1 = "Change the third-person camera mode.\n"
	    					 "Note: This setting is save file specific.";

			const char* T2 = "Change how the camera's linerar interpolation behaves.\n"
	    					 "Lower values smooth harder while higher values smooth less";

			//Hack
            auto CamState = std::bit_cast<int*>(&Persistent::GetSingleton().TrackedCameraState.value);

	        if (ImGui::CollapsingHeader("Automatic Camera", ImUtil::HeaderFlagsDefaultOpen)) {
	            ImUtil::CheckBox("Enable Automatic Camera", &Settings.bAutomaticCamera, T0);
	            ImUtil::IComboEx<CameraModeTP>("Camera Mode", CamState, T1, !Settings.bAutomaticCamera);
				ImUtil::SliderF("Interpolation Factor", &Settings.fCameraInterpolationFactor, 0.01f, 1.0f, T2, "%.2fx", !Settings.bAutomaticCamera);
	        	ImGui::Spacing();
	        }
	    }

	    ImGui::BeginDisabled(!Settings.bAutomaticCamera);

	    ImUtil_Unique {
	        DrawCameraSettings(&Settings.OffsetsNormal, "Normal Camera");
	    }

	    ImUtil_Unique {
	        DrawCameraSettings(&Settings.OffsetsAlt, "Alternative Camera");
	    }

	    ImGui::EndDisabled();

	}
}