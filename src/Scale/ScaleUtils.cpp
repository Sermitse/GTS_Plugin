
#include "Config/Config.hpp"

#include "Managers/GTSSizeManager.hpp"
#include "Managers/HighHeel.hpp"
#include "Managers/Morphs/MorphManager.hpp"

namespace {

	constexpr float EPS = 1e-4f;

	float GetGrowthReduction(float size) {
		// https://www.desmos.com/calculator/pqgliwxzi2

		if (GTS::SizeManager::BalancedMode()) {
			GTS::SoftPotential cut{
				.k = 1.08f,
				.n = 0.90f,
				.s = 3.00f,
				.a = 0.0f,
			};
			const float balance = GTS::Config::Balance.fBMSizeGainPenaltyMult;
			const float power = GTS::soft_power(size, cut) * balance;
			return std::clamp(power, 1.0f, 99999.0f);
		}
		return 1.0f;
		// So it never reports values below 1.0. Just to make sure.
	}
}

namespace GTS {

	void override_actor_scale(Actor* giant, float amt, SizeEffectType type) { // This function overrides gts manager values. 
		// It ignores half-life, allowing more than 1 growth/shrink sources to stack nicely
		auto Persistent = Persistent::GetActorData(giant);
		if (Persistent) {
			float OnTheEdge = 1.0f;
			float scale = get_visual_scale(giant);

			if (amt > 0 && (giant->formID == 0x14 || IsTeammate(giant))) {
				if (scale >= 1.0f) {
					amt /= GetGrowthReduction(scale); // Enabled if BalanceMode is True. Decreases Grow Efficiency.
				}
			}
			else if (giant->formID == 0x14 && amt - EPS < 0.0f) {
				// If negative change: add stolen attributes
				DistributeStolenAttributes(giant, -amt * GetGrowthReduction(scale)); // Adjust max attributes
			}

			if (giant->formID == 0x14 && type == SizeEffectType::kShrink) {
				OnTheEdge = GetPerkBonus_OnTheEdge(giant, amt); // Player Exclusive
			}

			float target = get_target_scale(giant);
			float max_scale = get_max_scale(giant);
			if (target < max_scale || amt < 0) {
				amt /= game_getactorscale(giant);
				Persistent->fTargetScale += amt;
				Persistent->fVisualScale += amt;
			}
		}
	}

	void update_target_scale(Actor* giant, float amt, SizeEffectType type) { // used to mod scale with perk bonuses taken into account
		float OnTheEdge = 1.0f;
		
		if (amt > 0 && (giant->formID == 0x14 || IsTeammate(giant))) {
			float scale = get_visual_scale(giant);
			if (scale >= 1.0f) {
				amt /= GetGrowthReduction(scale); // Enabled if BalanceMode is True. Decreases Grow Efficiency.
			}
		}
		else if (giant->formID == 0x14 && amt - EPS < 0.0f) {
			// If negative change: add stolen attributes
			float scale = get_visual_scale(giant);
			DistributeStolenAttributes(giant, -amt * GetGrowthReduction(scale)); // Adjust max attributes
		}

		if (giant->formID == 0x14 && type == SizeEffectType::kShrink) {
			OnTheEdge = GetPerkBonus_OnTheEdge(giant, amt); // Player Exclusive
		}

		mod_target_scale(giant, amt * OnTheEdge); // set target scale value
	}

	float get_update_target_scale(Actor* giant, float amt, SizeEffectType type) { // Used for growth spurt
		float OnTheEdge = 1.0f;

		if (amt > 0 && (giant->formID == 0x14 || IsTeammate(giant))) {
			float scale = get_visual_scale(giant);
			if (scale >= 1.0f) {
				amt /= GetGrowthReduction(scale); // Enabled if BalanceMode is True. Decreases Grow Efficiency.
			}
		}
		else if (giant->formID == 0x14 && amt - EPS < 0.0f) {
			// If negative change: add stolen attributes
			float scale = get_visual_scale(giant);
			DistributeStolenAttributes(giant, -amt * GetGrowthReduction(scale)); // Adjust max attributes
		}

		if (giant->formID == 0x14 && type == SizeEffectType::kShrink) {
			OnTheEdge = GetPerkBonus_OnTheEdge(giant, amt); // Player Exclusive
		}

		mod_target_scale(giant, amt * OnTheEdge); // set target scale value

		return amt * OnTheEdge;
	}

	float get_scale_difference(Actor* giant, Actor* tiny, SizeType Type, bool Check_SMT, bool HH) {
		float hh_gts = 0.0f;
		float hh_tiny = 0.0f;

		float GiantScale = 1.0f;
		float TinyScale = 1.0f;

		if (HH) { // Apply HH only in cases when we need it, such as damage and hugs
			hh_gts = HighHeelManager::GetHHOffset(giant)[2] * 0.01f;
			hh_tiny = HighHeelManager::GetHHOffset(tiny)[2] * 0.01f;
		}

		switch (Type) {
		case SizeType::GiantessScale:
			GiantScale = get_giantess_scale(giant) + hh_gts;
			TinyScale = get_giantess_scale(tiny) + hh_tiny;
			break;
		case SizeType::VisualScale:
			GiantScale = (get_visual_scale(giant) + hh_gts) * GetSizeFromBoundingBox(giant);
			TinyScale = (get_visual_scale(tiny) + hh_tiny) * GetSizeFromBoundingBox(tiny);
			break;
		case SizeType::TargetScale:
			GiantScale = get_target_scale(giant) + hh_gts;
			TinyScale = get_target_scale(tiny) + hh_tiny;
			break;
		}

		if (Check_SMT) {
			if (HasSMT(giant)) {
				GiantScale += 10.2f;
			}
		}

		if (tiny->formID == 0x14 && HasSMT(tiny)) {
			TinyScale += 1.50f;
		}

		float Difference = GiantScale / TinyScale;
		/*if (giant->formID == 0x14 && !tiny->IsDead()) {
			log::info("Size Difference between {} and {} is {}", giant->GetDisplayFullName(), tiny->GetDisplayFullName(), Difference);
			log::info("Tiny Data: TS: {} ; HH: {} ; BB: {}, target: {}", TinyScale, hh_tiny, GetSizeFromBoundingBox(tiny), get_target_scale(tiny));
			log::info("GTS Data: TS {} ; HH: {} BB :{}", GiantScale, hh_gts, GetSizeFromBoundingBox(giant));
		}*/

		return Difference;
	}

	float GetSizeFromBoundingBox(Actor* tiny) {
		GTS_PROFILE_SCOPE("ActorUtils: GetSizeFromBoundingBox");
		float sc = get_bounding_box_to_mult(tiny);
		return sc;
	}

	float GetRoomStateScale(Actor* giant) {
		// Goal is to make us effectively smaller during these checks, so RayCast won't adjust our height unless we're truly too big
		float Normal = 1.0f;
		float Reduction = 1.0f;

		if (AnimationVars::Prone::IsProne(giant)) {
			return 0.30f;
		}
		else if (AnimationVars::Crawl::IsCrawling(giant)) {
			return 0.46f;
		}
		else if (giant->IsSneaking()) {
			Reduction = 0.70f;
		}
		else {
			Reduction = 1.0f;
		}
		float HH = (HighHeelManager::GetBaseHHOffset(giant).Length() / 100) / Characters_AssumedCharSize; // Get HH value and convert it to meters
		return (Normal + HH) * Reduction;
	}

}
