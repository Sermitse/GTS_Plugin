#include "Magic/Effects/Common.hpp"

#include "Config/Config.hpp"

#include "Managers/ShrinkToNothingManager.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Managers/Audio/MoansLaughs.hpp"

#include "Utils/DeathReport.hpp"

namespace GTS {

	std::string GetAllyEssentialText(const bool Teammate) {
		return Teammate ? "teammate" : "essential";
	}

	const char* GetIconPath(bool Teammate) {
		return Teammate ? "GTS/UI/Icon_Teammate.nif" : "GTS/UI/Icon_Essential.nif";
	}

	float TimeScale() {
		constexpr float BASE_FPS = 60.0f; // Parameters were optimised on this fps
		return Time::WorldTimeDelta() * BASE_FPS;
	}

	bool CanBendLifeless(Actor* giant) {
		bool allow = Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkBendTheLifeless);
		return allow;
	}

	void RecordPotionMagnitude(ActiveEffect* effect, float& power, float IfFalse) {
		if (effect) {
			power = effect->magnitude > 0 ? effect->magnitude / 100.0f : IfFalse;
		}
	}

	// Pretty much the same Essential check but with visualization in terms of icons/messages
	bool IsEssential_WithIcons(Actor* giant, Actor* tiny) {

		if (tiny->IsPlayerRef()) { // always allow with player
			return false;
		}

		if (IsEssential(giant, tiny)) {
			const bool Teammate = !IsHostile(giant, tiny) && IsTeammate(tiny) && Config::General.bProtectFollowers;
			const bool OnCooldown = IsActionOnCooldown(tiny, CooldownSource::Misc_ShrinkParticle_Animation);
			const bool icons_enabled = Config::General.bShowIcons;

			if (giant->IsPlayerRef() && !OnCooldown) { // player exclusive
				if (icons_enabled) { 
					auto node = find_node(tiny, "NPC Root [Root]");
					if (node) {
						float size = get_visual_scale(tiny);
						NiPoint3 pos = node->world.translate;
						float bounding_z = get_bounding_box_z(tiny);

						pos.z += (bounding_z * size * 2.35f); // 2.35 to be slightly above the head
						float iconScale = std::clamp(size, 1.0f, 9999.0f) * 2.4f;

						SpawnParticle(tiny, 3.00f, GetIconPath(Teammate), NiMatrix3(), pos, iconScale, 7, node);
					}
				} 
				else {
					std::string message_1 = std::format("{} is {}", tiny->GetDisplayFullName(), GetAllyEssentialText(Teammate));
					std::string message_2 = "Immune to size magic and effects";
					Notify(message_1);
					Notify(message_2);
				}
				ApplyActionCooldown(tiny, CooldownSource::Misc_ShrinkParticle_Animation);
			}
			return true;
		}
		return false;
	}

	void AdvanceSkill(Actor* giant, ActorValue Attribute, float points, float multiplier) {
		// Native Equivalent of Papyrus' AdvanceSkill
		if (giant->IsPlayerRef()) {
			//log::info("Advancing skill, points: {}, Mult: {}, TimeScale: {}, Result: {}, * 60: {}", points, multiplier, TimeScale(), points * multiplier * TimeScale(), points * 60 * multiplier * TimeScale());
			//float Level = GetAV(giant, Attribute) + 1.0f;
			//log::info("Level: {}", Level);
			giant->UseSkill(Attribute, points * 20 * multiplier * TimeScale(), nullptr);
		}
	}

	// Normal NPC's just die if they drink them
	void Potion_Penalty(Actor* giant) { 
		if (!giant->IsPlayerRef() && !IsTeammate(giant)) {
			float currentscale = get_visual_scale(giant);
			update_target_scale(giant, -currentscale * 0.5f, SizeEffectType::kNeutral);
			giant->KillImmediate();
		}
	}

	void AdjustSizeReserve(Actor* giant, float value) {
		if (!Runtime::HasPerk(giant, Runtime::PERK.GTSPerkSizeReserve)) {
			return;
		}
		auto Cache = Persistent::GetActorData(giant);
		if (Cache) {
			Cache->fSizeReserve += value * 1.5f;
		}
	}

	// for shrinking another
	float Shrink_GetPower(Actor* giant, Actor* tiny) {
		float reduction = 1.0f / GetSizeFromBoundingBox(tiny);
		//log::info("Default Shrink power for {} is {}", tiny->GetDisplayFullName(), reduction);
		if (IsUndead(tiny, false) && !IsLiving(tiny)) {
			if (CanBendLifeless(giant)) {
				reduction *= 0.31f;
			}
			else {
				reduction *= 0.22f;
			}
		}
		else if (IsGiant(tiny)) {
			reduction *= 0.75f;
		}
		else if (IsMechanical(tiny)) {
			if (CanBendLifeless(giant)) {
				reduction *= 0.12f;
			}
			else {
				reduction *= 0.0f;
			}
		}
		//log::info("Total Shrink power for {} is {}", tiny->GetDisplayFullName(), reduction);
		return reduction;
	}

	// For gaining size
	float SizeSteal_GetPower(Actor* giant, Actor* tiny) { 
		float increase = GetSizeFromBoundingBox(tiny);
		if (IsUndead(tiny, false) && !IsLiving(tiny)) {
			if (CanBendLifeless(giant)) {
				increase *= 0.31f;
			} else {
				increase *= 0.22f;
			}
		} else if (IsMechanical(tiny)) {
			increase *= 0.0f;
		}
		return increase;
	}

	void ModSizeExperience(Actor* Caster, float value) { 
		if (value > 0) {
			auto progressionQuest = Runtime::GetQuest(Runtime::QUST.GTSQuestProgression);
			if (progressionQuest) {
				auto queststage = progressionQuest->GetCurrentStageID();
				if (queststage >= 10) {

					if (Caster->IsPlayerRef()) {

						const auto& GtsSkillLevel = Runtime::GetGlobal(Runtime::GLOB.GTSSkillLevel);
						const auto& GtsSkillProgress = Runtime::GetGlobal(Runtime::GLOB.GTSSkillProgress);
						const auto& GtsSkillRatio = Runtime::GetGlobal(Runtime::GLOB.GTSSkillRatio);
						
						if (GtsSkillLevel && GtsSkillProgress && GtsSkillRatio) {
							if (GtsSkillLevel->value >= 100.0f) {
								GtsSkillLevel->value = 100.0f;
								GtsSkillRatio->value = 0.0f;
								return;
							}

							const float skill_level = GtsSkillLevel->value;
							const float ValueEffectiveness = std::clamp(1.0f - GtsSkillLevel->value / 100, 0.10f, 1.0f);
							const float oldvaluecalc = 1.0f - GtsSkillRatio->value; //Attempt to keep progress on the next level
							const float Total = value * ValueEffectiveness;
							GtsSkillRatio->value += Total * GetXpBonus();

							if (GtsSkillRatio->value >= 1.0f) {
								float transfer = std::clamp(Total - oldvaluecalc, 0.0f, 1.0f);
								GtsSkillRatio->value = transfer;
								GtsSkillLevel->value = skill_level + 1.0f;
								EventDispatcher::DoGTSLevelUpEvent(Caster);
								GtsSkillProgress->value = GtsSkillLevel->value;

								AddPerkPoints(GtsSkillLevel->value);
							}
						}
					}
					else if (IsTeammate(Caster)) {
						if (auto data = Persistent::GetActorData(Caster)) {
							if (data->fGTSSkillLevel >= 100.0f) {
								data->fGTSSkillLevel = 100.0f;
								data->fGTSSkillRatio = 0.0f;
								return;
							}

							const float skill_level = data->fGTSSkillLevel;
							const float ValueEffectiveness = std::clamp(1.0f - data->fGTSSkillLevel / 100, 0.10f, 1.0f);
							const float oldvaluecalc = 1.0f - data->fGTSSkillRatio; //Attempt to keep progress on the next level
							const float Total = value * ValueEffectiveness;
							data->fGTSSkillRatio += Total * GetXpBonus();

							if (data->fGTSSkillRatio >= 1.0f) {
								float transfer = std::clamp(Total - oldvaluecalc, 0.0f, 1.0f);
								data->fGTSSkillRatio = transfer;
								data->fGTSSkillLevel = skill_level + 1.0f;
								EventDispatcher::DoGTSLevelUpEvent(Caster);
								data->fGTSSkillExp = data->fGTSSkillLevel;
							}
						}
					}
				}
			}
		}
	}

	void ModSizeExperience_Crush(Actor* giant, Actor* tiny, bool check) {
		float size = get_visual_scale(tiny);
		float xp = 0.20f + (size * 0.02f);
		if (tiny->IsDead() && check) {
			//Cprint("Crush Tiny is ded");
			xp *= 0.20f;
		}
		ModSizeExperience(giant, xp); // Adjust Size Matter skill
	}

	void AdjustMassLimit(float value, Actor* caster) { // Adjust Size Limit for Mass Based Size Mode
		const auto selectedFormula = Config::Balance.sSizeMode;
		float progressionMultiplier = Config::Balance.fSpellEfficiency;

		if (selectedFormula == "kMassBased") {

			SoftPotential mod {
				.k = 0.070f,
				.n = 3.0f,
				.s = 0.54f,
			};

			if (auto data = Persistent::GetActorData(caster); data) {

				float modifier = soft_core(data->fMassBasedLimit, mod);
				modifier = std::max(modifier, 0.10f);
				value *= 10.0f * modifier;

				float new_value = data->fMassBasedLimit + value * progressionMultiplier * TimeScale();
				data->fMassBasedLimit = std::clamp(new_value, 0.0f, data->fSizeLimit);
			}
		}
	}

	float CalcEffeciency(Actor* caster, Actor* target) {

		const float level_caster = caster->GetLevel();
		const float level_target = target->GetLevel();
		const float casterlevel = std::clamp(level_caster, 1.0f, 500.0f);
		const float targetlevel = std::clamp(level_target, 1.0f, 500.0f);
		const float SizeHunger = 1.0f + Ench_Hunger_GetPower(caster);
		const float Gigantism_Caster = 1.0f + (Ench_Aspect_GetPower(caster) * 0.25f); // get GTS Aspect Of Giantess
		const float Gigantism_Target = 1.0f + Ench_Aspect_GetPower(target);  // get Tiny Aspect Of Giantess
		const float Scale_Resistance = std::clamp(get_visual_scale(target), 1.0f, 9999.0f); // Calf_power makes shrink effects stronger based on scale, this fixes that.
		
		float efficiency = std::clamp(casterlevel/targetlevel, 0.50f, 1.0f);

		efficiency *= Potion_GetShrinkResistance(target);
		efficiency *= Perk_GetSprintShrinkReduction(target); // Up to 20% resistance when sprinting
		efficiency *= Gigantism_Caster * SizeHunger; // amplity it by Aspect Of Giantess (on gts) and size hunger potion bonus
		efficiency /= Gigantism_Target; // resistance from Aspect Of Giantess (on Tiny)
		efficiency /= Scale_Resistance;
		efficiency *= Shrink_GetPower(caster, target);// take bounding box of actor into account

		//log::info("efficiency between {} and {} is {}", caster->GetDisplayFullName(), target->GetDisplayFullName(), efficiency);

		return efficiency;
	}

	float CalcPower(Actor* actor, float scale_factor, float bonus, bool shrink) {
		constexpr float MASTER_POWER = 2.0f;
		float progress_mult = Config::Balance.fSpellEfficiency;
		float size_cap = 0.5f;
		// y = mx +c
		// power = scale_factor * scale + bonus
		if (shrink) { // allow for more size weakness when we need it
			size_cap = 0.02f; // up to 98% shrink weakness
		}
		float scale = std::clamp(get_visual_scale(actor), size_cap, 1000000.0f);
		return (scale * scale_factor + bonus) * progress_mult * MASTER_POWER * TimeScale();
	}

	void Grow(Actor* actor, float scale_factor, float bonus) {
		// amount = scale * a + b
		update_target_scale(actor, CalcPower(actor, scale_factor, bonus, false), SizeEffectType::kGrow);
	}

	void ShrinkActor(Actor* actor, float scale_factor, float bonus) {
		// amount = scale * a + b
		update_target_scale(actor, -CalcPower(actor, scale_factor, bonus, true), SizeEffectType::kShrink);
	}

	bool Revert(Actor* actor, float scale_factor, float bonus) {
		// amount = scale * a + b
		float amount = CalcPower(actor, scale_factor, bonus, false);
		float target_scale = get_target_scale(actor);
		float natural_scale = get_natural_scale(actor, true); // get_neutral_scale(actor) 

		if (Runtime::HasPerk(PlayerCharacter::GetSingleton(), Runtime::PERK.GTSPerkColossalGrowth)) {

			if (actor->IsPlayerRef()) {
				const auto Mode = StringToEnum<LActiveGamemode_t>(Config::Gameplay.GamemodePlayer.sGameMode);
				if (Mode == LActiveGamemode_t::kSizeLocked ||
					Mode == LActiveGamemode_t::kCurseOfDiminishing ||
					Mode == LActiveGamemode_t::kCurseOfTheGiantess) {
					natural_scale = Config::Gameplay.GamemodePlayer.fCurseTargetScale;
				}
			}
			else if (IsTeammate(actor)) {
				const auto Mode = StringToEnum<LActiveGamemode_t>(Config::Gameplay.GamemodeFollower.sGameMode);
				if (Mode == LActiveGamemode_t::kSizeLocked ||
					Mode == LActiveGamemode_t::kCurseOfDiminishing ||
					Mode == LActiveGamemode_t::kCurseOfTheGiantess) {
					natural_scale = Config::Gameplay.GamemodeFollower.fCurseTargetScale;
				}
			}
		}

		if (target_scale < natural_scale) {
			set_target_scale(actor, natural_scale); // Without GetScale multiplier
			return false;
		}

		update_target_scale(actor, -amount, SizeEffectType::kNeutral);
		
		return true;
	}

	void Grow_Ally(Actor* from, Actor* to, float receiver, float caster) {
		float receive = CalcPower(from, receiver, 0, false);
		float lose = CalcPower(from, receiver, 0, false);
		float CasterScale = get_target_scale(from);
		if (CasterScale > get_natural_scale(from, true)) { // We don't want to scale the caster below this limit!
			update_target_scale(from, -lose, SizeEffectType::kShrink);
		}
		update_target_scale(to, receive, SizeEffectType::kGrow);
	}

	void Steal(Actor* from, Actor* to, float scale_factor, float bonus, float effeciency, ShrinkSource source) {
		effeciency = std::clamp(effeciency, 0.0f, 1.0f);
		float visual_scale = get_visual_scale(from);

		float amount = CalcPower(from, scale_factor, bonus, false);
		float amount_shrink = CalcPower(from, scale_factor, bonus, false);

		float shrink_amount = (amount*0.22f);
		float growth_amount = (amount_shrink*0.33f*effeciency) * SizeSteal_GetPower(to, from);

		ModSizeExperience(to, 0.14f * scale_factor * visual_scale * SizeSteal_GetPower(to, from));

		update_target_scale(from, -shrink_amount, SizeEffectType::kShrink);
		update_target_scale(to, growth_amount, SizeEffectType::kShrink); //kShrink to buff size steal with On The Edge perk

		float XpMult = 1.0f;

		if (from->IsDead()) {
			XpMult = 0.25f;
		}

		if (source == ShrinkSource::Hugs) { // For hugs: quest: shrink by 2 and 5 meters worth of size in total (stage 1 / 2) 
			AdvanceQuestProgression(to, nullptr, QuestStage::HugSteal, shrink_amount, false); // Stage 1: steal 2 meters worth of size (hugs)
			AdvanceQuestProgression(to, nullptr, QuestStage::HugSpellSteal, shrink_amount, false); // Stage 2: steal 5 meters worth of size (spells/hugs)
		} 
		else { // For spell shrink part of the quest
			if (source != ShrinkSource::Enchantment) {
				AdvanceSkill(to, ActorValue::kAlteration, shrink_amount, XpMult); // Gain vanilla Alteration xp
			}
			AdvanceQuestProgression(to, nullptr, QuestStage::HugSpellSteal, shrink_amount, false);
		}

		AddStolenAttributes(to, amount*effeciency);
	}

	void TransferSize(Actor* caster, Actor* target, bool dual_casting, float power, float transfer_effeciency, bool smt,
		ShrinkSource source) {
		constexpr float BASE_POWER = 0.0005f;
		constexpr float DUAL_CAST_BONUS = 2.0f;
		constexpr float SMT_BONUS = 1.25f;
		float PERK_BONUS = 1.0f;

		if (IsEssential(caster, target)) {
			return;
		}

		float target_scale = get_visual_scale(target); // used for xp only

		transfer_effeciency = std::clamp(transfer_effeciency, 0.0f, 1.0f); // Ensure we cannot grow more than they shrink

		power *= BASE_POWER * CalcEffeciency(caster, target);

		if (dual_casting) {
			power *= DUAL_CAST_BONUS;
		}

		if (smt) {
			power *= SMT_BONUS;
		}

		if (Runtime::HasPerkTeam(caster, Runtime::PERK.GTSPerkShrinkAdept)) {
			PERK_BONUS += 0.15f;
		}
		if (Runtime::HasPerkTeam(caster, Runtime::PERK.GTSPerkShrinkExpert)) {
			PERK_BONUS += 0.35f;
		}

		power *= PERK_BONUS; // multiply power by perk bonuses

		AdjustMassLimit(0.0002f * target_scale * power, caster);

		auto GtsSkillLevel = GetGtsSkillLevel(caster);

		float alteration_level_bonus = 0.0380f + (GtsSkillLevel * 0.000360f); // + 100% bonus at level 100
		Steal(target, caster, power, power * alteration_level_bonus, transfer_effeciency, source);
	}

	bool BlockShrinkToNothing(Actor* giant, Actor* tiny, float time_mult) {
		auto transient = Transient::GetActorData(tiny);
		if (transient) {
			float& tick = transient->ShrinkTicks;
			tick += 0.0166f * TimeScale();

			if (tick > Shrink_To_Nothing_After * time_mult) {
				tick = 0.0f;
				return false;
			} else {
				bool BlockParticle = IsActionOnCooldown(tiny, CooldownSource::Misc_ShrinkParticle);
				if (!BlockParticle) {
					float scale = get_visual_scale(tiny) * 6;
					float ticks = std::clamp(tick, 1.0f, 3.0f);

					SpawnCustomParticle(tiny, ParticleType::Red, NiPoint3(), "NPC Root [Root]", scale * ticks);
					ApplyActionCooldown(tiny, CooldownSource::Misc_ShrinkParticle);
				}
				return true;
			}
		}
		return true;
	}

	bool ShrinkToNothing(Actor* caster, Actor* target, bool check_ticks, float time_mult, float mass_mult,
		bool Calamity_PlayLaugh, bool ShrinkOutburst_Absorb) {
		if (!caster) {
			return false;
		}
		if (!target) {
			return false;
		}

		float bbscale = GetSizeFromBoundingBox(target);
		float target_scale = get_target_scale(target);

		if (target_scale <= SHRINK_TO_NOTHING_SCALE / bbscale && !Runtime::HasMagicEffect(target, Runtime::MGEF.GTSEffectShrinkToNothing)) {

			set_target_scale(target, SHRINK_TO_NOTHING_SCALE / bbscale);

			if (!ShrinkToNothingManager::CanShrink(caster, target)) {
				return false;
			}

			if (check_ticks && BlockShrinkToNothing(caster, target, time_mult)) {
				return true;
			}

			if (!target->IsDead()) {
				if (IsGiant(target)) {
					AdvanceQuestProgression(caster, target, QuestStage::Giant, 1, false);
				} else {
					AdvanceQuestProgression(caster, target, QuestStage::ShrinkToNothing, 1, false);
				}
			} 
			else {
				AdvanceQuestProgression(caster, target, QuestStage::ShrinkToNothing, 0.25f, false);
			}

			AdjustMassLimit(0.0060f * mass_mult, caster);

			AdjustSizeReserve(caster, target_scale * bbscale/25);
			ReportDeath(caster, target, DamageSource::ShrinkToNothing);
			ShrinkToNothingManager::Shrink(caster, target);

			ApplyKillEmotions(caster, Calamity_PlayLaugh, ShrinkOutburst_Absorb);

			return true;
		}
		return false;
	}

	void CrushBonuses(Actor* caster, Actor* target) {
		float target_scale = get_visual_scale(target) * GetSizeFromBoundingBox(target);
		if (caster->IsPlayerRef()) {
			AdjustSizeReserve(caster, target_scale/25);
			AdjustMassLimit(0.0066f * target_scale, caster);
		}
	}
}
