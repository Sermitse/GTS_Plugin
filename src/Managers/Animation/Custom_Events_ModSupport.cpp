#include "Managers/Animation/Controllers/ThighSandwichController.hpp"
#include "Managers/Animation/Custom_Events_ModSupport.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Damage/CollisionDamage.hpp"
#include "Managers/CrushManager.hpp"
#include "Utils/DeathReport.hpp"

using namespace GTS;

// Animation: Compatibility
// Notes: Made avaliable for other generic anim mods
//  - Stages
//    - "GTScrush_caster",          //[0] The gainer.
//    - "GTScrush_victim",          //[1] The one to crush
// Notes: Modern Combat Overhaul compatibility
// - Stages
//   - "MCO_SecondDodge",           // enables GTS sounds and footstep effects
//   - "SoundPlay.MCO_DodgeSound",


/*

GTS_CustomDamage_Butt_ON
GTS_CustomDamage_Butt_OFF

GTS_CustomDamage_Legs_ON
GTS_CustomDamage_Legs_OFF

GTS_CustomDamage_FullBody_ON
GTS_CustomDamage_FullBody_OFF

GTS_CustomDamage_Cleavage_ON
GTS_CustomDamage_Cleavage_OFF

 ^ List of custom anims for modders to utilize

*/

namespace {

	constexpr std::string_view RNode = "NPC R Foot [Rft ]";
	constexpr std::string_view LNode = "NPC L Foot [Lft ]";

	void RunThighCollisionTask(Actor* giant, bool right, bool CooldownCheck, float radius, float damage, float bbmult, float crush_threshold, int random, std::string_view name) {
		auto gianthandle = giant->CreateRefHandle();
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto giantref = gianthandle.get().get();

			CollisionDamage::DoFootCollision(giantref, damage, radius, random, bbmult, crush_threshold, DamageSource::Crushed, right, CooldownCheck, false, false); // Foot damage
			ApplyThighDamage(giantref, right, CooldownCheck, radius, damage, bbmult, crush_threshold, random, DamageSource::ThighCrushed); // Thigh Damage
		    
			return true; // Do not cancel it, let it repeat
		});
	}

	void TriggerKillZone(Actor* giant) {
		if (!giant) {
			return;
		}
		float BASE_CHECK_DISTANCE = 90.0f;
		float SCALE_RATIO = 3.0f;
		if (TinyCalamityActive(giant)) {
			SCALE_RATIO = 0.8f;
		}
		float giantScale = get_visual_scale(giant);
		NiPoint3 giantLocation = giant->GetPosition();
		for (auto otherActor: find_actors()) {
			if (otherActor != giant) {
				if (otherActor->IsInKillMove()) {
					float tinyScale = get_visual_scale(otherActor);
					if (giantScale / tinyScale > SCALE_RATIO) {
						NiPoint3 actorLocation = otherActor->GetPosition();
						if ((actorLocation-giantLocation).Length() < BASE_CHECK_DISTANCE*giantScale * 3) {
							ReportDeath(giant, otherActor, DamageSource::Booty);
							CrushManager::Crush(giant, otherActor);
						}
					}
				}
			}
		}
	}

	void GTScrush_caster(AnimationEventData& data) { 
		// Compatibility with Thick Thighs Take Lives mod, this compatibility probably needs a revision.
		// Mainly just need to call damage similar to how we do it with DoDamageAtPoint() function
		// 21.01.2024
		//data.stage = 0;
		TriggerKillZone(&data.giant);
	}

	void GTScrush_victim(AnimationEventData& data) { // Compatibility with Thick Thighs Take Lives mod
		//data.stage = 0;
		if (!data.giant.IsPlayerRef()) {
			TriggerKillZone(PlayerCharacter::GetSingleton());
		}
	}

	void GTS_CustomDamage_Butt_On(AnimationEventData& data) {
		//PrintMessageBox("GTS_CustomDamage_Butt_ON");
	}

	void GTS_CustomDamage_Butt_Off(AnimationEventData& data) {
		//PrintMessageBox("GTS_CustomDamage_Butt_OFF");
	}

	void GTS_CustomDamage_Legs_On(AnimationEventData& data) {
		//PrintMessageBox("GTS_CustomDamage_Legs_ON");
	}
	void GTS_CustomDamage_Legs_Off(AnimationEventData& data) {
		//PrintMessageBox("GTS_CustomDamage_Legs_OFF");
	}

	void GTS_CustomDamage_FullBody_On(AnimationEventData& data) {
		//PrintMessageBox("GTS_CustomDamage_FullBody_ON");
	}
	void GTS_CustomDamage_FullBody_Off(AnimationEventData& data) {
		//PrintMessageBox("GTS_CustomDamage_FullBody_OFF");
	}

	void GTS_CustomDamage_Cleavage_On(AnimationEventData& data) {
		//PrintMessageBox("GTS_CustomDamage_Cleavage_ON");
	}
	void GTS_CustomDamage_Cleavage_Off(AnimationEventData& data) {
		//PrintMessageBox("GTS_CustomDamage_Cleavage_OFF");
	}

	void GTS_FootSwipe_L_ON(AnimationEventData& data) { // Compatibility with NickNack's upcoming Hand 2 Hand mod
		auto giant = &data.giant;
		std::string name = std::format("FootSwipeL_{}", giant->formID);
		RunThighCollisionTask(&data.giant, false, true, Radius_ThighCrush_Idle, Damage_ThighCrush_CrossLegs_Out, 0.1f, 0.95f, 10, name);
	}

	void GTS_FootSwipe_R_ON(AnimationEventData& data) { // Compatibility with NickNack's upcoming Hand 2 Hand mod
	auto giant = &data.giant;
		std::string name = std::format("FootSwipeR_{}", giant->formID);
		RunThighCollisionTask(&data.giant, true, true, Radius_ThighCrush_Idle, Damage_ThighCrush_CrossLegs_Out, 0.1f, 0.95f, 10, name);
	}

	void GTS_FootSwipe_L_OFF(AnimationEventData& data) { // Compatibility with NickNack's upcoming Hand 2 Hand mod
		auto giant = &data.giant;
		std::string name = std::format("FootSwipeL_{}", giant->formID);
		TaskManager::Cancel(name);
	}

	void GTS_FootSwipe_R_OFF(AnimationEventData& data) { // Compatibility with NickNack's upcoming Hand 2 Hand mod
		auto giant = &data.giant;
		std::string name = std::format("FootSwipeR_{}", giant->formID);
		TaskManager::Cancel(name);
	}

	void MCO_SecondDodge(AnimationEventData& data) {
		data.stage = 0;
		float scale = get_visual_scale(&data.giant);
		float volume = scale * 0.20f;
		DoDamageEffect(&data.giant, Damage_Stomp, Radius_Stomp, 10, 0.20f, FootEvent::Right, 1.0f, DamageSource::CrushedRight);
		DoDamageEffect(&data.giant, Damage_Stomp, Radius_Stomp, 10, 0.20f, FootEvent::Left, 1.0f, DamageSource::CrushedLeft);
		DoFootstepSound(&data.giant, 1.0f, FootEvent::Right, RNode);
		DoFootstepSound(&data.giant, 1.0f, FootEvent::Left, LNode);
		DoDustExplosion(&data.giant, 1.0f, FootEvent::Right, RNode);
		DoDustExplosion(&data.giant, 1.0f, FootEvent::Left, LNode);
		DoLaunch(&data.giant, 0.90f, 1.35f, FootEvent::Right);
		DoLaunch(&data.giant, 0.90f, 1.35f, FootEvent::Left);
	}
	void MCO_DodgeSound(AnimationEventData& data) {
		data.stage = 0;
		float scale = get_visual_scale(&data.giant);
		float volume = scale * 0.20f;
		DoDamageEffect(&data.giant, Damage_Stomp, Radius_Stomp, 10, 0.20f, FootEvent::Right, 1.0f, DamageSource::CrushedRight);
		DoDamageEffect(&data.giant, Damage_Stomp, Radius_Stomp, 10, 0.20f, FootEvent::Left, 1.0f, DamageSource::CrushedLeft);
		DoFootstepSound(&data.giant, 1.0f, FootEvent::Right, RNode);
		DoFootstepSound(&data.giant, 1.0f, FootEvent::Left, LNode);
		DoDustExplosion(&data.giant, 1.0f, FootEvent::Right, RNode);
		DoDustExplosion(&data.giant, 1.0f, FootEvent::Left, LNode);
		DoLaunch(&data.giant, 0.90f, 1.35f, FootEvent::Right);
		DoLaunch(&data.giant, 0.90f, 1.35f, FootEvent::Left);
	}

	void GTS_ToLeft(AnimationEventData& data) {Attachment_SetTargetNode(&data.giant, AttachToNode::ObjectL);}
	void GTS_ToRight(AnimationEventData& data) {
		if (AnimationVars::Action::IsThighGrinding(&data.giant) || AnimationVars::Action::IsThighSandwiching(&data.giant)) {
			auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(&data.giant);
			sandwichdata.EnableSuffocate(false);
			sandwichdata.SetSuffocateMult(1.0f);
		} 
		// Yay more hacks
		Attachment_SetTargetNode(&data.giant, AttachToNode::ObjectR);
	}

	void GTS_ToAnimB(AnimationEventData& data) {Attachment_SetTargetNode(&data.giant, AttachToNode::ObjectB);}
	void GTS_ToAnimA(AnimationEventData& data) {Attachment_SetTargetNode(&data.giant, AttachToNode::ObjectA);}
}

namespace GTS
{
	void Animation_ModSupport::RegisterEvents() {
		AnimationManager::RegisterEvent("GTScrush_caster", "Compat", GTScrush_caster);
		AnimationManager::RegisterEvent("GTScrush_victim", "Compat", GTScrush_victim);
		AnimationManager::RegisterEvent("MCO_SecondDodge", "MCOCompat1", MCO_SecondDodge);
		AnimationManager::RegisterEvent("SoundPlay.MCO_DodgeSound", "Compat", MCO_DodgeSound);

		AnimationManager::RegisterEvent("GTS_CustomDamage_Butt_On", "Compat", GTS_CustomDamage_Butt_On);
		AnimationManager::RegisterEvent("GTS_CustomDamage_Butt_Off", "Compat", GTS_CustomDamage_Butt_Off);

		AnimationManager::RegisterEvent("GTS_CustomDamage_Legs_On", "Compat", GTS_CustomDamage_Legs_On);
		AnimationManager::RegisterEvent("GTS_CustomDamage_Legs_Off", "Compat", GTS_CustomDamage_Legs_Off);

		AnimationManager::RegisterEvent("GTS_CustomDamage_FullBody_On", "Compat", GTS_CustomDamage_FullBody_On);
		AnimationManager::RegisterEvent("GTS_CustomDamage_FullBody_Off", "Compat", GTS_CustomDamage_FullBody_Off);

		AnimationManager::RegisterEvent("GTS_CustomDamage_Cleavage_On", "Compat", GTS_CustomDamage_Cleavage_On);
		AnimationManager::RegisterEvent("GTS_CustomDamage_Cleavage_Off", "Compat", GTS_CustomDamage_Cleavage_Off);

		AnimationManager::RegisterEvent("GTS_FootSwipe_L_ON", "Compat", GTS_FootSwipe_L_ON);
		AnimationManager::RegisterEvent("GTS_FootSwipe_L_OFF", "Compat", GTS_FootSwipe_L_OFF);
		AnimationManager::RegisterEvent("GTS_FootSwipe_R_ON", "Compat", GTS_FootSwipe_R_ON);
		AnimationManager::RegisterEvent("GTS_FootSwipe_R_OFF", "Compat", GTS_FootSwipe_R_OFF);

		AnimationManager::RegisterEvent("GTS_ToLeft", "Compat", GTS_ToLeft);
		AnimationManager::RegisterEvent("GTS_ToRight", "Compat", GTS_ToRight);
		AnimationManager::RegisterEvent("GTS_ToAnimA", "Compat", GTS_ToAnimA);
		AnimationManager::RegisterEvent("GTS_ToAnimB", "Compat", GTS_ToAnimB);
	}

	void Animation_ModSupport::RegisterTriggers() {
		AnimationManager::RegisterTrigger("Tiny_ExitAnims", "Compat3", "GTSBEH_Tiny_Abort");
	}
}
