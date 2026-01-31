#include "Managers/Animation/Stomp_Under_FullBody.hpp"
#include "Managers/Animation/AnimationManager.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CrawlUtils.hpp"
#include "Managers/Input/InputManager.hpp"

#include "Managers/Rumble.hpp"

using namespace GTS;

namespace {

    const std::vector<std::string_view> BODY_NODES = {
		"NPC R Thigh [RThg]",
		"NPC L Thigh [LThg]",
		"NPC R Butt",
		"NPC L Butt",
		"NPC Spine [Spn0]",
		"NPC Spine1 [Spn1]",
		"NPC Spine2 [Spn2]",
	};

    constexpr std::string_view RNode = "NPC R Foot [Rft ]";
    constexpr std::string_view LNode = "NPC L Foot [Lft ]";

	std::string_view GetBreastNodeName(Actor* giant, bool Is3BB) {

	}

    void DoImpactRumble(Actor* giant, std::string_view node, std::string_view name, bool hh, float magnitude) {
		float shake_power = Rumble_Stomp_Under_Strong;
		float smt = HasSMT(giant) ? 1.5f : 1.0f;
        if (hh) {
		    smt *= GetHighHeelsBonusDamage(giant, true);
        }
		Rumbling::Once(name, giant, shake_power * smt * magnitude, 0.0f, node, 1.25f);
	}

    void InflictBodyDamage(Actor* giant) {
		float perk = GetPerkBonus_Basics(giant);
		float damage = 1.0f;

		for (auto Nodes: BODY_NODES) {
			auto Node = find_node(giant, Nodes);
			if (Node) {
				std::string rumbleName = std::format("Node: {}", Nodes);
				DoDamageAtPoint(giant, Radius_BreastCrush_BodyImpact, Damage_Stomp_Under_Breast_Body * damage, Node, 400, 0.10f, 0.8f, DamageSource::BodyCrush);
                Rumbling::Once(rumbleName, giant, 0.6f * damage, 0.035f, Nodes, 0.0f);
				DoLaunch(giant, 1.75f * perk, 3.8f, Node);
			}
		}

        for (auto LeftOrRight:{true, false}) {
            ApplyThighDamage(giant, LeftOrRight, false, Radius_ThighCrush_Spread_Out, Damage_Stomp_Under_Breast_Legs, 0.15f, 1.0f, 8, DamageSource::ThighCrushed);
        }
	}

	void PerformBreastDamage(Actor* giant, NiAVObject* BreastR, NiAVObject* BreastL, std::string_view breast_L, std::string_view breast_R) {
		float perk = GetPerkBonus_Basics(giant);
		float damage = 1.25f;
		float dust = 1.0f;
		float smt = 1.0f;

		if (HasSMT(giant)) {
			dust = 1.25f;
			smt = 1.5f;
		}

		float shake_power = Rumble_Cleavage_Impact/2.5f * dust * damage;

		DoDamageAtPoint(giant, Radius_BreastCrush_BreastImpact, Damage_BreastCrush_BreastImpact * damage, BreastL, 4, 0.70f, 0.8f, DamageSource::BreastImpact);
		DoDamageAtPoint(giant, Radius_BreastCrush_BreastImpact, Damage_BreastCrush_BreastImpact * damage, BreastR, 4, 0.70f, 0.8f, DamageSource::BreastImpact);
		DoDustExplosion(giant, 1.25f * dust + damage/10, FootEvent::Right, "NPC R Breast");
		DoDustExplosion(giant, 1.25f * dust + damage/10, FootEvent::Left, "NPC L Breast");
		Rumbling::Once("Breast_L", giant, shake_power * smt, 0.075f, "NPC L Breast", 0.0f);
		Rumbling::Once("Breast_R", giant, shake_power * smt, 0.075f, "NPC R Breast", 0.0f);
		DoFootstepSound(giant, 1.25f, FootEvent::Right, "NPC R Breast");
		DoFootstepSound(giant, 1.25f, FootEvent::Right, "NPC L Breast");
		DoLaunch(giant, 2.25f * perk, 5.0f, FootEvent::Breasts);
	}

	void ChooseBreastNodes(Actor* giant) {
		auto BreastL = find_node(giant, "NPC L Breast");
		auto BreastR = find_node(giant, "NPC R Breast");
		auto BreastL03 = find_node(giant, "L Breast03");
		auto BreastR03 = find_node(giant, "R Breast03");

		if (BreastL03 && BreastR03) { // 3BB
			PerformBreastDamage(giant, BreastR03, BreastL03, "L Breast03", "R Breast03");
		} else if (BreastL && BreastR) { // Non 3BB
			PerformBreastDamage(giant, BreastR, BreastL, "NPC L Breast", "NPC R Breast");
		} else { // Notify user about skill issue
			if (!BreastR) {
				Notify("Error: Missing Breast Nodes"); // Will help people to troubleshoot it. Not everyone has 3BB/XPMS32 body.
				Notify("Error: effects not inflicted");
				Notify("Suggestion: install Female body replacer");
			} else if (!BreastR03) {
				Notify("Error: Missing 3BB Breast Nodes"); // Will help people to troubleshoot it. Not everyone has 3BB/XPMS32 body.
				Notify("Error: effects not inflicted");
				Notify("Suggestion: install 3BB/SMP Body");
			}
		}
	}


    void GTS_UnderStomp_Crawl_BodyCamOn(AnimationEventData& data) {
		DrainStamina(&data.giant,"StaminaDrain_BreastAttack", Runtime::PERK.GTSPerkDestructionBasics, true, 20.0f);
		ManageCamera(&data.giant, true, CameraTracking::Breasts_02);
	}
    void GTS_UnderStomp_Crawl_BodyCamOff(AnimationEventData& data) {
		ManageCamera(&data.giant, false, CameraTracking::Breasts_02);
	}

    void GTS_UnderStomp_Crawl_BodyImpact(AnimationEventData& data) {
		DrainStamina(&data.giant,"StaminaDrain_BreastAttack", Runtime::PERK.GTSPerkDestructionBasics, false, 20.0f);
        ChooseBreastNodes(&data.giant);
        InflictBodyDamage(&data.giant);
    }
}
namespace GTS {
    void AnimationUnderStompFullBody::RegisterEvents() {
        AnimationManager::RegisterEvent("GTS_UnderStomp_Crawl_BodyCamOn", "UnderStompBody", GTS_UnderStomp_Crawl_BodyCamOn);
        AnimationManager::RegisterEvent("GTS_UnderStomp_Crawl_BodyCamOff", "UnderStompBody", GTS_UnderStomp_Crawl_BodyCamOff);

        AnimationManager::RegisterEvent("GTS_UnderStomp_Crawl_BodyImpact", "UnderStompBody", GTS_UnderStomp_Crawl_BodyImpact);
	}
}