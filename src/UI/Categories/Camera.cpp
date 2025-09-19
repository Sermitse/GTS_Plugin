#include "UI/Categories/Camera.hpp"
#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/ImUtil.hpp"

namespace GTS {

	static void DrawCameraOffsets(const char* a_title, const char* a_toolip, std::array<float, 3>* a_offsets) {

	    ImGui::BeginGroup();

		ImUtil::SliderF3(a_title, &a_offsets->at(0), -175.f, 175.f, nullptr, "%.1f");

	    ImGui::EndGroup();

	    // Applies to the whole group
	    if (ImGui::IsItemHovered()) {
	        ImGui::SetTooltip(a_toolip);
	    }
	}

	static void DrawCameraSettings(CameraOffsets_t* a_set, const char* a_title) {

	    PSString T0 = "Select which biped skeleton bone the camera should track.";

	    if (ImGui::CollapsingHeader(a_title, ImUtil::HeaderFlagsDefaultOpen)) {
	        ImUtil::ComboEx<LCameraTrackBone_t>("Center On Bone", a_set->sCenterOnBone, T0);

	        ImUtil_Unique {
	            DrawCameraOffsets(
	                "Offsets | Standing",
	                "Adjust camera offsets when standing.\n"
					"Left/Right | Forward/Back | Up/Down",
	                &a_set->f3NormalStand
	            );
	        }

	        ImUtil_Unique {
	            DrawCameraOffsets(
	                "Offsets | Standing Combat",
	                "Adjust camera offsets when standing and in combat.\n"
					"Left/Right | Forward/Back | Up/Down",
	                &a_set->f3CombatStand
	            );
	        }

	        ImUtil_Unique {
	            DrawCameraOffsets(
	                "Offsets | Crawling",
	                "Adjust camera offsets while sneaking, crawling, or prone.\n"
					"Left/Right | Forward/Back | Up/Down",
	                &a_set->f3NormalCrawl
	            );
	        }

	        ImUtil_Unique {
	            DrawCameraOffsets(
	                "Offsets | Crawling Combat",
	                "Adjust camera offsets while sneaking, crawling, or prone and in combat.\n"
					"Left/Right | Forward/Back | Up/Down",
	                &a_set->f3CombatCrawl
	            );
	        }

	        ImGui::Spacing();
	    }
	}

	void CategoryCamera::DrawLeft() {

	    ImUtil_Unique {

	        PSString T0 = "Change the intensity of camera shakes when performing actions as a player.";
			PSString T1 = "Change the intensity of first-person camera shakes caused by your own size or size actions";
			PSString T2 = "Change the intensity of camera shakes for NPCs.";

	        if (ImGui::CollapsingHeader("Camera Shake", ImUtil::HeaderFlagsDefaultOpen)) {
	            ImUtil::SliderF("Player Total Shake Power", &Settings.fCameraShakePlayer, 0.1f, 3.0f, T0, "%.2fx");
				ImUtil::SliderF("Player FP Shake Power", &Settings.fCameraShakePlayerFP, 0.0f, 1.0f, T1, "%.2fx");
	            ImUtil::SliderF("NPC Total Shake Power", &Settings.fCameraShakeOther, 0.1f, 3.0f, T2, "%.2fx");

	            ImGui::Spacing();
	        }
	    }

	    ImUtil_Unique {

	        PSString T0 = "Change the height multiplier of the camera while crawling.\n"
							 "1st Person | 3rd Person\n\n"
							 "Note: 3rd Person wont't work correctly if you are using smoothcam.";

			if (ImGui::CollapsingHeader("Crawl Height", ImUtil::HeaderFlagsDefaultOpen)) {
				//Temp Store
				static std::array const Temp = { &Settings.fFPCrawlHeightMult,  &Settings.fTPCrawlHeightMult };

				ImUtil::SliderF2("Crawl Height Mults.", Temp.at(0), 0.01f, 1.0f, T0, "%.1fx");
	            ImGui::Spacing();
	        }
	    }

	    ImUtil_Unique {

	        PSString T0 = "Enable camera collision with actors.";
	        PSString T1 = "Enable camera collision with trees.";
	        PSString T2 = "Enable camera collision with debris (physics-enabled objects).";
	        PSString T3 = "Enable camera collision with terrain.";
	        PSString T4 = "Enable camera collision with statics (basically any solid, non-movable object).";
	        PSString T5 = "Change the scale at which the above collision settings should apply.";
			PSString T6 = "Dynamically change the camera near distance value to fix clipping issues when small.\n"
							 "May introduce visual issues such as moving/disappearing shadows.\n\n"
	    				     "Starts applyng when smaller than 1.0x scale.\n"
	    					 "Disables itself when past 1.0x scale\n\n"
	    					 "Note: Can conflict with other mods that also change this value";

	        if (ImGui::CollapsingHeader("Camera Collision", ImUtil::HeaderFlagsDefaultOpen)) {
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

	        PSString T0 = "Offsets the 3rd person camera's minimum zoom distance.\n"
							 "Combined with the maximum distance this affects the distance from the player\n"
							 "where the camera perspective switches to 1st person.";

	        PSString T1 = "Offsets the 3rd person camera's maximum zoom distance.\n"
							 "Higher values will zoom out the camera more.\n"
							 "Combined with the minimum distance this affects the distance from the player\n"
	    					 "where the camera perspective switches to 1st person.";

			PSString T2 = "Changes the transition speed between camera steps.";

	        PSString T3 = "Changes the camera's zoom step divisor.\n"
							 "Lower values increase the ammount of zoom steps,\n"
	    					 "whilst higher values decrease them.\n";

			PSString T4 = "Toggle wether this mod should override skyrim's camera settings.\n"
							 "Note: Requires a game restart after disabling for the original values to be reapplied.\n\n"
							 "Its reccomended to leave this enabled.";

	        PSString THelp = 
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

	        PSString T0 = "Enable automatic camera.";

	        PSString T1 = "Change the third-person camera mode.\n"
	    					 "Note: This setting is save file specific.";

			//Hack
            auto CamState = std::bit_cast<int*>(&Persistent::GetSingleton().TrackedCameraState.value);

	        if (ImGui::CollapsingHeader("Automatic Camera", ImUtil::HeaderFlagsDefaultOpen)) {
	            ImUtil::CheckBox("Enable Automatic Camera", &Settings.bAutomaticCamera, T0);
	            ImUtil::IComboEx<LCameraMode_t>("Camera Mode", CamState, T1, !Settings.bAutomaticCamera);
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