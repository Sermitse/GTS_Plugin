#include "Managers/Animation/Sneak_Slam_FingerGrind.hpp"

#include "Config/Config.hpp"

#include "Managers/Animation/AnimationManager.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CrawlUtils.hpp"
#include "Managers/Input/InputManager.hpp"
#include "Managers/Rumble.hpp"

#include "Rays/Raycast.hpp"

using namespace GTS;

namespace {

	constexpr std::string_view Rfinger = "NPC R Finger12 [RF12]";
	constexpr std::string_view Lfinger = "NPC L Finger12 [LF12]";

    void Finger_DoSounds(Actor* giant, std::string_view node_name, float mult) {
		NiAVObject* node = find_node(giant, node_name);
		if (node) {
			float scale = get_visual_scale(giant) * 0.72f;
			DoCrawlingSounds(giant, scale * mult, node, FootEvent::Left);
		}
	}

	void Finger_ApplyVisuals(Actor* giant, std::string_view node_name, float threshold, float multiplier) {
		NiAVObject* node = find_node(giant, node_name);
		if (node) {
			float min_scale = 3.5f * threshold;
			float scale = get_visual_scale(giant);
			if (HasSMT(giant)) {
				scale += 2.6f;
				multiplier *= 0.33f;
			}
			if (scale >= threshold && !giant->AsActorState()->IsSwimming()) {
				NiPoint3 node_location = node->world.translate;

				NiPoint3 ray_start = node_location + NiPoint3(0.0f, 0.0f, meter_to_unit(-0.05f*scale)); // Shift up a little
				NiPoint3 ray_direction(0.0f, 0.0f, -1.0f);
				bool success = false;
				float ray_length = meter_to_unit(std::max(1.05f*scale, 1.05f));
				NiPoint3 explosion_pos = CastRay(giant, ray_start, ray_direction, ray_length, success);

				if (!success) {
					explosion_pos = node_location;
					explosion_pos.z = giant->GetPosition().z;
				}
				if (giant->formID == 0x14 && Config::Gameplay.bPlayerAnimEffects) {
					SpawnParticle(giant, 4.60f, "GTS/Effects/Footstep.nif", NiMatrix3(), explosion_pos, (scale * multiplier) * 1.8f, 7, nullptr);
				}
				if (giant->formID != 0x14 && Config::Gameplay.bNPCAnimEffects) {
					SpawnParticle(giant, 4.60f, "GTS/Effects/Footstep.nif", NiMatrix3(), explosion_pos, (scale * multiplier) * 1.8f, 7, nullptr);
				}
			}
		}
	}
    void Finger_DoDamage(Actor* giant, DamageSource source, bool Right, float Radius, float Damage, float CrushMult, float ShrinkMult) {
		std::string_view NodeLookup = Rfinger;
		if (!Right) {
			source = DamageSource::LeftFinger;
			NodeLookup = Lfinger;
		}

		NiAVObject* node = find_node(giant, NodeLookup);

		ApplyFingerDamage(giant, Radius, Damage, node, 50, 0.10f, CrushMult, -0.038f * ShrinkMult, source);
	}

    ////////////////////////////////////////////////////////////////////
    /////// Events
    ///////////////////////////////////////////////////////////////////

    void GTS_Sneak_FingerGrind_CameraOn_R(AnimationEventData& data) {
		ManageCamera(&data.giant, true, CameraTracking::Finger_Right);
	};  

    void GTS_Sneak_FingerGrind_CameraOn_L(AnimationEventData& data) {
		ManageCamera(&data.giant, true, CameraTracking::Finger_Left);
	};  

	void GTS_Sneak_FingerGrind_Impact_R(AnimationEventData& data) {
		Finger_DoDamage(&data.giant, DamageSource::RightFinger_Impact, true, Radius_Sneak_FingerGrind_Impact, Damage_Sneak_FingerGrind_Impact, 2.8f, 1.2f);
		Finger_DoSounds(&data.giant, Rfinger, 1.0f);
		Finger_ApplyVisuals(&data.giant, Rfinger, 2.6f, 1.0f);

		Rumbling::Once("Finger", &data.giant, Rumble_FingerGrind_Impact, 0.025f, Rfinger, 0.0f);

		DrainStamina(&data.giant, "StaminaDrain_FingerGrind", "GTSPerkDestructionBasics", true, 0.8f);
	};
    void GTS_Sneak_FingerGrind_Impact_L(AnimationEventData& data) {
		Finger_DoDamage(&data.giant, DamageSource::LeftFinger_Impact, false, Radius_Sneak_FingerGrind_Impact, Damage_Sneak_FingerGrind_Impact, 2.8f, 1.2f);
		Finger_DoSounds(&data.giant, Lfinger, 1.0f);
		Finger_ApplyVisuals(&data.giant, Lfinger, 2.6f, 1.0f);

		Rumbling::Once("Finger", &data.giant, Rumble_FingerGrind_Impact, 0.025f, Lfinger, 0.0f);

		DrainStamina(&data.giant, "StaminaDrain_FingerGrind", "GTSPerkDestructionBasics", true, 0.8f);
	};

	void GTS_Sneak_FingerGrind_Rotation_R(AnimationEventData& data) {
		Finger_DoDamage(&data.giant, DamageSource::RightFinger, true, Radius_Sneak_FingerGrind_DOT, Damage_Sneak_FingerGrind_DOT, 3.2f, 0.8f);
		Rumbling::Once("FingerROT", &data.giant, Rumble_FingerGrind_Rotate, 0.025f, Rfinger, 0.0f);
		Finger_ApplyVisuals(&data.giant, Rfinger, 2.6f, 0.85f);
	};   
    void GTS_Sneak_FingerGrind_Rotation_L(AnimationEventData& data) {
		Finger_DoDamage(&data.giant, DamageSource::LeftFinger, false, Radius_Sneak_FingerGrind_DOT, Damage_Sneak_FingerGrind_DOT, 3.2f, 0.8f);
		Rumbling::Once("FingerROT", &data.giant, Rumble_FingerGrind_Rotate, 0.025f, Lfinger, 0.0f);
		Finger_ApplyVisuals(&data.giant, Lfinger, 2.6f, 0.85f);
	};   

	void GTS_Sneak_FingerGrind_Finisher_R(AnimationEventData& data) {
		Finger_DoDamage(&data.giant, DamageSource::RightFinger_Impact, true, Radius_Sneak_FingerGrind_Finisher, Damage_Sneak_FingerGrind_Finisher, 2.4f, 4.0f);
		Rumbling::Once("FingerFIN", &data.giant, Rumble_FingerGrind_Finisher, 0.045f, Rfinger, 0.0f);
        Finger_ApplyVisuals(&data.giant, Rfinger, 2.6f, 1.25f);
		Finger_DoSounds(&data.giant, Rfinger, 1.5f);
        StopStaminaDrain(&data.giant);	
	};
    void GTS_Sneak_FingerGrind_Finisher_L(AnimationEventData& data) {
		Finger_DoDamage(&data.giant, DamageSource::LeftFinger, false, Radius_Sneak_FingerGrind_Finisher, Damage_Sneak_FingerGrind_Finisher, 2.4f, 4.0f);
		Rumbling::Once("FingerFIN", &data.giant, Rumble_FingerGrind_Finisher, 0.045f, Lfinger, 0.0f);
        Finger_ApplyVisuals(&data.giant, Lfinger, 2.6f, 1.25f);
		Finger_DoSounds(&data.giant, Lfinger, 1.5f);
        StopStaminaDrain(&data.giant);
	};

	void GTS_Sneak_FingerGrind_CameraOff_R(AnimationEventData& data) {TrackMatchingHand(&data.giant, CrawlEvent::RightHand, false);}
    void GTS_Sneak_FingerGrind_CameraOff_L(AnimationEventData& data) {TrackMatchingHand(&data.giant, CrawlEvent::LeftHand, false);}

}

namespace GTS {
    void Animation_SneakSlam_FingerGrind::RegisterEvents() {
        AnimationManager::RegisterEvent("GTS_Sneak_FingerGrind_CameraOn_R", "Sneak", GTS_Sneak_FingerGrind_CameraOn_R);
        AnimationManager::RegisterEvent("GTS_Sneak_FingerGrind_CameraOn_L", "Sneak", GTS_Sneak_FingerGrind_CameraOn_L);

		AnimationManager::RegisterEvent("GTS_Sneak_FingerGrind_Impact_R", "Sneak", GTS_Sneak_FingerGrind_Impact_R);
        AnimationManager::RegisterEvent("GTS_Sneak_FingerGrind_Impact_L", "Sneak", GTS_Sneak_FingerGrind_Impact_L);

		AnimationManager::RegisterEvent("GTS_Sneak_FingerGrind_Rotation_R", "Sneak", GTS_Sneak_FingerGrind_Rotation_R);
        AnimationManager::RegisterEvent("GTS_Sneak_FingerGrind_Rotation_L", "Sneak", GTS_Sneak_FingerGrind_Rotation_L);

		AnimationManager::RegisterEvent("GTS_Sneak_FingerGrind_Finisher_R", "Sneak", GTS_Sneak_FingerGrind_Finisher_R);
        AnimationManager::RegisterEvent("GTS_Sneak_FingerGrind_Finisher_L", "Sneak", GTS_Sneak_FingerGrind_Finisher_L);

		AnimationManager::RegisterEvent("GTS_Sneak_FingerGrind_CameraOff_R", "Sneak", GTS_Sneak_FingerGrind_CameraOff_R);
        AnimationManager::RegisterEvent("GTS_Sneak_FingerGrind_CameraOff_L", "Sneak", GTS_Sneak_FingerGrind_CameraOff_L);
    }

	void Animation_SneakSlam_FingerGrind::RegisterTriggers() {
		AnimationManager::RegisterTrigger("Tiny_Finger_Impact_S", "Sneak", "GTSBEH_T_Slam_Start");
	}

    void TrackMatchingHand(Actor* giant, CrawlEvent kind, bool enable) {
        if (kind == CrawlEvent::RightHand) {
            ManageCamera(giant, enable, CameraTracking::Hand_Right);
        } else if (kind == CrawlEvent::LeftHand) {
            ManageCamera(giant, enable, CameraTracking::Hand_Left);
        }
    }

    void StopStaminaDrain(Actor* giant) {
		DrainStamina(giant, "StaminaDrain_StrongSneakSlam", "GTSPerkDestructionBasics", false, 2.2f);
		DrainStamina(giant, "StaminaDrain_FingerGrind", "GTSPerkDestructionBasics", false, 0.8f);
		DrainStamina(giant, "StaminaDrain_SneakSlam", "GTSPerkDestructionBasics", false, 1.4f);
	}
}