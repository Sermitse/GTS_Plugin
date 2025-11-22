#include "Managers/Animation/Grab_Attack.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Grab.hpp"

#include "Managers/Damage/SizeHitEffects.hpp"
#include "Managers/Damage/TinyCalamity.hpp"

#include "Managers/GTSSizeManager.hpp"
#include "Managers/Rumble.hpp"

#include "Magic/Effects/Common.hpp"

using namespace GTS;

namespace {

    void GTSGrab_Attack_MoveStart(AnimationEventData& data) {
		auto giant = &data.giant;
		DrainStamina(giant, "GrabAttack", Runtime::PERK.GTSPerkDestructionBasics, true, 0.75f);
		ManageCamera(giant, true, CameraTracking::Grab_Left);
		StartLHandRumble("GrabMoveL", data.giant, 0.5f, 0.10f);
	}

	void GTSGrab_Attack_Damage(AnimationEventData& data) {
		auto& sizemanager = SizeManager::GetSingleton();
		float bonus = 1.0f;
		auto giant = &data.giant;
		auto grabbedActor = Grab::GetHeldActor(giant);

		if (grabbedActor) {
			Attacked(grabbedActor, giant); // force combat

			float tiny_scale = get_visual_scale(grabbedActor) * GetSizeFromBoundingBox(grabbedActor);
			float gts_scale = get_visual_scale(giant) * GetSizeFromBoundingBox(giant);

			float sizeDiff = gts_scale/tiny_scale;
			float power = std::clamp(sizemanager.GetSizeAttribute(giant, SizeAttribute::Normal), 1.0f, 1000000.0f);
			float additionaldamage = 1.0f + sizemanager.GetSizeVulnerability(grabbedActor);
			float damage = (Damage_Grab_Attack * sizeDiff) * power * additionaldamage * additionaldamage;
			float experience = std::clamp(damage/1600, 0.0f, 0.06f);
			if (HasSMT(giant)) {
				bonus = 1.65f;
			}

            if (CanDoDamage(giant, grabbedActor, false)) {
                if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkGrowingPressure)) {
                    auto& mgr = SizeManager::GetSingleton();
                    mgr.ModSizeVulnerability(grabbedActor, damage * 0.0010f);
                }

                TinyCalamity_ShrinkActor(giant, grabbedActor, damage * 0.10f * GetDamageSetting());

                SizeHitEffects::PerformInjuryDebuff(giant, grabbedActor, damage*0.15f, 6);
                InflictSizeDamage(giant, grabbedActor, damage);
            }
			
			Rumbling::Once("GrabAttack", giant, Rumble_Grab_Hand_Attack * bonus, 0.05f, "NPC L Hand [LHnd]", 0.0f);

			ModSizeExperience(giant, experience);
			AddSMTDuration(giant, 1.0f);

            Utils_CrushTask(giant, grabbedActor, bonus, true, true, DamageSource::HandCrushed, QuestStage::HandCrush);
		}
	}

	void GTSGrab_Attack_MoveStop(AnimationEventData& data) {
		auto giant = &data.giant;
		auto& sizemanager = SizeManager::GetSingleton();
		auto grabbedActor = Grab::GetHeldActor(giant);
		ManageCamera(giant, false, CameraTracking::Grab_Left);
		DrainStamina(giant, "GrabAttack", Runtime::PERK.GTSPerkDestructionBasics, false, 0.75f);
		StopLHandRumble("GrabMoveL", data.giant);
		if (!grabbedActor) {
			giant->SetGraphVariableInt("GTS_GrabbedTiny", 0);
			giant->SetGraphVariableInt("GTS_Grab_State", 0);
			Grab::ExitGrabState(giant);
			Grab::DetachActorTask(giant);
			Grab::Release(giant);
		}
	}
}

namespace GTS {
    void Animation_GrabAttack::RegisterEvents() {
        AnimationManager::RegisterEvent("GTSGrab_Attack_MoveStart", "Grabbing", GTSGrab_Attack_MoveStart);
		AnimationManager::RegisterEvent("GTSGrab_Attack_Damage", "Grabbing", GTSGrab_Attack_Damage);
		AnimationManager::RegisterEvent("GTSGrab_Attack_MoveStop", "Grabbing", GTSGrab_Attack_MoveStop);
    }
}
