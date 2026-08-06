#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Damage/Utils/SizeDamageUtils.hpp"

#include "Hooks/Experiments/Experiments_FootColliders.hpp"
#include "Config/Config.hpp"

#include "Managers/Damage/SizeHitEffects.hpp"
#include "Managers/Damage/TinyCalamity.hpp"
#include "Managers/Audio/GoreAudio.hpp"
#include "Managers/CrushManager.hpp"
#include "Managers/GTSSizeManager.hpp"

#include "Magic/Effects/Common.hpp"
#include "Managers/Contact/ColliderUtils.hpp"

#include "Utils/DeathReport.hpp"
#include "Utils/MovementForce.hpp"



using namespace GTS;

namespace GTS {
	bool IsIdleDamage(DamageSource cause) {
		return cause == DamageSource::FootIdleL || cause == DamageSource::FootIdleR;
	}
	bool StrongGore(DamageSource cause) {
		bool Strong = false;
		switch (cause) {
			case DamageSource::FootGrindedRight_Impact:
			case DamageSource::FootGrindedLeft_Impact:
			case DamageSource::RightFinger_Impact:
			case DamageSource::LeftFinger_Impact:
			case DamageSource::HandCrawlRight:
			case DamageSource::HandCrawlLeft:
			case DamageSource::KneeDropRight:
			case DamageSource::KneeDropLeft:
			case DamageSource::KneeRight:
			case DamageSource::KneeLeft:
			case DamageSource::HandDropRight:
			case DamageSource::HandDropLeft:
			case DamageSource::HandSlamRight:
			case DamageSource::HandSlamLeft:
			case DamageSource::CrushedRight:
			case DamageSource::CrushedLeft:
			case DamageSource::WalkRight:
			case DamageSource::WalkLeft:
			case DamageSource::BodyCrush:
			case DamageSource::BreastImpact:
			case DamageSource::Booty:
				Strong = true;
			break;
		}
		return Strong;
	}
	bool Allow_Damage(Actor* giant, Actor* tiny, DamageSource cause, float difference) {
		float threshold = 3.0f;

		if (DisallowSizeDamage(giant, tiny)) {
			return false;
		}

		if (difference > threshold) {
			return true;
		}

		bool PreventDamage = false;

		switch (cause) {
			case DamageSource::WalkLeft:
			case DamageSource::WalkRight:
				PreventDamage = true;
			break;
			case DamageSource::KneeLeft:
			case DamageSource::KneeRight:
				PreventDamage = true;
			break;
			case DamageSource::HandCrawlLeft:
			case DamageSource::HandCrawlRight:
				PreventDamage = true;
			break;
		}
		if (PreventDamage) {
			// goal of this function is to deal heavily decreased damage on normal walk footsteps to actors
			// so it won't look silly by dealing 30 damage by briefly colliding with others
			if (difference > 1.4f) {
				InflictSizeDamage(giant, tiny, difference * 0.35f);
			} 
			return false;
		}
		return true;
	}

	void ModVulnerability(Actor* giant, Actor* tiny, float damage) {
		if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkGrowingPressure)) {
			auto& sizemanager = SizeManager::GetSingleton();

			if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkRavagingInjuries) && giant->AsActorState()->IsSprinting() && !AnimationVars::General::IsGTSBusy(giant)) {
				damage *= 3.0f; // x3 stronger during sprint
			}
			sizemanager.ModSizeVulnerability(tiny, damage * 0.0010f);
		}
	}
	bool DamageAllowed(Actor* giant, Actor* tiny, DamageSource Cause) {
		float size_difference = get_scale_difference(giant, tiny, SizeType::VisualScale, false, true);
		bool SMT = TinyCalamityActive(giant);
		float size_threshold = 1.25f;
		if (SMT) {
			size_threshold = 0.9f;
		}
		if (size_difference > size_threshold) {
			if (Allow_Damage(giant, tiny, Cause, size_difference)) { 
				return true;
			}
		}
		return false;
	}
	bool ApplyHighHeelBonus(Actor* giant, DamageSource cause) {
		bool HighHeel = false;
		switch (cause) {
			case DamageSource::CrushedRight:
				HighHeel = true;
			break;
			case DamageSource::CrushedLeft:
				HighHeel = true;
			break;
			case DamageSource::WalkRight:
				HighHeel = true;
			break;
			case DamageSource::WalkLeft:
				HighHeel = true;
			break;
		}
		return HighHeel;
	}

	float HighHeels_PerkDamage(Actor* giant, DamageSource Cause) {
		float value = 1.0f;
		bool matches = false;

		switch (Cause) {
			case DamageSource::CrushedRight:
				matches = true;
			break;
			case DamageSource::CrushedLeft:
				matches = true;
			break;
			case DamageSource::WalkRight:
				matches = true;
			break;
			case DamageSource::WalkLeft:
				matches = true;
			break;
		}
		if (matches) {
			bool rumbling_feet = Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkRumblingFeet);
			bool perk = Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkHighHeels);
			if (rumbling_feet) {
				value += 0.25f; // 25% bonus damage if we have lvl 65 perk
			} if (perk) {
				value += 0.15f; // 15% bonus damage if we have High Heels perk
			}
		}
		return value;
	}

	void CrushCheck(Actor* giant, Actor* tiny, float size_difference, float crush_threshold, DamageSource Cause) {
		bool CanBeCrushed = (GetAV(tiny, ActorValue::kHealth) <= 1.0f || tiny->IsDead());
		if (CanBeCrushed) {
			if (size_difference > Action_Crush * crush_threshold && CrushManager::CanCrush(giant, tiny)) {
				ModSizeExperience_Crush(giant, tiny, true);

				if (!tiny->IsDead()) {
					if (IsGiant(tiny)) {
						AdvanceQuestProgression(giant, tiny, QuestStage::Giant, 1, false);
					} else {
						AdvanceQuestProgression(giant, tiny, QuestStage::Crushing, 1, false);
					}
				} else {
					AdvanceQuestProgression(giant, tiny, QuestStage::Crushing, 0.25f, false);
				}
				SetReanimatedState(tiny);

				CrushBonuses(giant, tiny);
				ReportDeath(giant, tiny, Cause);
				if (!Config::General.bLessGore) {
					auto node = find_node(giant, GetDeathNodeName(Cause));
					if (!IsMechanical(tiny)) {
						PlayCrushSound(giant, node, StrongGore(Cause), get_corrected_scale(tiny)); // Run Crush Sound task that will determine which exact type of crushing audio to play
					} 
				}

				SetBetweenBreasts(tiny, false);
				SetBeingHeld(tiny, false);

				CrushManager::Crush(giant, tiny);
			}
		}
	}

	float CalculateSizeDamage(Actor* giant, Actor* tiny, DamageSource Cause, float damage) {
		float size_difference = get_scale_difference(giant, tiny, SizeType::VisualScale, false, false);
		auto& sizemanager = SizeManager::GetSingleton();
		bool SMT = TinyCalamityActive(giant);
		float size_threshold = SMT ? 0.9f : 1.25f;

		if (size_difference > size_threshold) {
			if (Allow_Damage(giant, tiny, Cause, size_difference)) {
				float damagebonus = HighHeels_PerkDamage(giant, Cause); // 15% bonus HH damage if we have perk

				float vulnerability = 1.0f + sizemanager.GetSizeVulnerability(tiny); // Get size damage debuff from enemy
				float normaldamage = std::clamp(SizeManager::GetSizeAttribute(giant, SizeAttribute::Normal) * 0.30f, 0.30f, 1000000.0f);

				float highheelsdamage = ApplyHighHeelBonus(giant, Cause) ? GetHighHeelsBonusDamage(giant, true) : 1.0f;

				float sprintdamage = 1.0f; // default Sprint damage of 1.0
				float weightdamage = 1.0f + (giant->GetWeight() * 0.01f);

				if (giant->AsActorState()->IsSprinting()) {
					sprintdamage = 1.5f * SizeManager::GetSizeAttribute(giant, SizeAttribute::Sprint);
					damage *= 1.5f;
				}

				float Might = 1.0f + Potion_GetMightBonus(giant);
				float damage_result = (damage * BalanceSizeDamage(size_difference) * damagebonus) * (normaldamage * sprintdamage) 
									* (highheelsdamage * weightdamage) * vulnerability;
				/*logger::info("Damage: {}, Size Difference: {}, Damage Bonus {}", damage, size_difference, damagebonus);
				logger::info("Normal Damage {}, Sprint Damage {}", normaldamage, sprintdamage);
				logger::info("Heels Damage: {}, Weight Damage {}", highheelsdamage, weightdamage);
				logger::info("Vulnerability: {}, Might: {}", vulnerability, Might);
				logger::info("Idle Damage: {}, TimeScale: {}, TimeDelta: {}", Damage_Default_Underfoot * TimeScale(), TimeScale(), Time::WorldTimeDelta());
				*/

				if (giant->IsSneaking()) {
					damage_result *= 0.85f;
				}
				damage_result *= Might;
				//logger::info("Total Damage: {}", damage_result);
				return damage_result;
			}
		}
		return 0.0f;
	}

	float BalanceSizeDamage(float sizeDifference) { // Reduces power of Size Difference Damage Bonus based on toggles
		if (!SizeManager::BalancedMode() && !Config::Balance.bReducedSizeDamage) {
			return sizeDifference;
		}
		return sizeDifference > 0.0f ? std::pow(sizeDifference, Damage_SizeDifferenceReductionMult) : sizeDifference;
	}
}
