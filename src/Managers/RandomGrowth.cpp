#include "Managers/RandomGrowth.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Audio/MoansLaughs.hpp"
#include "Managers/GtsSizeManager.hpp"
#include "Managers/Rumble.hpp"

#include "Magic/Effects/Common.hpp"

using namespace GTS;

namespace {

	float Get_Breach_Threshold(Actor* actor) {
		float threshold = 1.65f;

		if (Runtime::HasPerkTeam(actor, "GTSPerkRandomGrowthTerror")) {
			threshold = 1.60f;
		}

		return threshold;
	}

	float Get_size_penalty(Actor* actor) {
		float scale = get_visual_scale(actor);
		float penalty = 1.0f;

		if (scale >= 1.5f) {
			penalty = std::clamp(scale - 0.5f, 1.0f, 4.0f);
		}

		return penalty;
	}

	bool ShouldGrow(Actor* actor) {

		const bool BalancedMode = SizeManager::BalancedMode();
		const int BalanceModeMult = BalancedMode ? 2 : 1;

		float MultiplySlider = Config::Gameplay.GamemodePlayer.fRandomGrowthDelay;
		if (IsTeammate(actor) || CountAsGiantess(actor)) {
			MultiplySlider = Config::Gameplay.GamemodeFollower.fRandomGrowthDelay;
		}

		if (BalancedMode && MultiplySlider > 0.0f) {
			MultiplySlider = 1.0f;
		}

		if (!Runtime::HasPerkTeam(actor, "GTSPerkRandomGrowth")) {
			return false;
		}

		if (MultiplySlider <= 0.0f) {
			return false;
		}

		if (!IsFemale(actor, true)) {
			return false;
		}

		if (HasSMT(actor)) {
			return false; // Disallow random groth during Tiny Calamity
		}

		const float Gigantism = 1.0f + Ench_Aspect_GetPower(actor);
		float Requirement = (300.0f * MultiplySlider) / Gigantism; // Doubles random in Balance Mode
		Requirement *= Get_size_penalty(actor);

		const int random = RandomInt(1, static_cast<int>(round(Requirement)) * BalanceModeMult);
		constexpr int chance = 1;
		if (random <= chance) {
			return true;
		}

		return false;
		
	}
}

namespace GTS {

	RandomGrowth& RandomGrowth::GetSingleton() noexcept {
		static RandomGrowth instance;
		return instance;
	}

	std::string RandomGrowth::DebugName() {
		return "::RandomGrowth";
	}

	void RandomGrowth::Update() {
		static Timer GrowthTimer = Timer(1.0);
		if (!Plugin::Live()) {
			return;
		}
		if (GrowthTimer.ShouldRunFrame()) {
			for (auto actor: find_actors()) {
				if (actor && actor->Is3DLoaded() && IsVisible(actor)) {
					if (actor->formID == 0x14 || IsTeammate(actor) || CountAsGiantess(actor)) {
						if (ShouldGrow(actor)) {
							if (get_target_scale(actor) < get_max_scale(actor)) {
								float scale = get_visual_scale(actor);
								const float SpellEfficiency = Config::Balance.fSpellEfficiency;
								int random = RandomInt(0, 80);
								float TotalPower = (100.0f + random)/100.0f;
								float base_power = ((0.00750f * TotalPower * 25) * SpellEfficiency);  // The power of it
								float Gigantism = 1.0f + Ench_Aspect_GetPower(actor);

								if (Runtime::HasPerkTeam(actor, "GTSPerkRandomGrowthAug") && TotalPower >= Get_Breach_Threshold(actor) && !IsGtsBusy(actor)) {
									AnimationManager::StartAnim("StartRandomGrowth", actor);
								} else {
									if (!IsGrowing(actor)) {
										ActorHandle gianthandle = actor->CreateRefHandle();
										std::string name = std::format("RandomGrowth_{}", actor->formID);
										// Sounds
										float Volume = std::clamp(scale/4, 0.20f, 1.0f);

										if (TotalPower >= 1.45f) {
											Sound_PlayMoans(actor, 1.0f, 0.14f, EmotionTriggerSource::Growth);
											Task_FacialEmotionTask_Moan(actor, 0.8f, "RandomGrow");
										}
										Runtime::PlaySoundAtNode("GTSSoundRumble", actor, base_power, "NPC COM [COM ]");
										Runtime::PlaySoundAtNode("GTSSoundGrowth", actor, Volume, "NPC Pelvis [Pelv]");

										double Start = Time::WorldTimeElapsed();
										TaskManager::Run(name, [=](auto& progressData) {
											if (!gianthandle) {
												return false;
											}
											if (!Plugin::Live()) {
												return true; // Pause task while game is paused
											}

											double LifeTime = 0.40f * TotalPower;
											double Finish = Time::WorldTimeElapsed();
											double timepassed = Finish - Start;

											if (timepassed > LifeTime) {
												return false;// remove task, lifetime is over
											}
											auto giantref = gianthandle.get().get();
											// Grow
											float delta_time = Time::WorldTimeDelta();
											update_target_scale(giantref, base_power * delta_time * Gigantism, SizeEffectType::kGrow);

											// Play sound
											Rumbling::Once("RandomGrowth", giantref, base_power, 0.10f);
											RandomGrowth::RestoreStats(giantref, 0.8f); // Regens Attributes if PC has perk
											return true;
										});
									}
								}
							}
						}
					}
				}
			}
		}
	}

	void RandomGrowth::RestoreStats(Actor* actor, float multiplier) { // Regenerate attributes
		float HP = GetMaxAV(actor, ActorValue::kHealth) * 0.00185f;
		float MP = GetMaxAV(actor, ActorValue::kMagicka) * 0.00095f;
		float SP = GetMaxAV(actor, ActorValue::kStamina) * 0.00125f;
		actor->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kHealth, HP * TimeScale() * multiplier);
		actor->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kMagicka, SP * TimeScale() * multiplier);
		actor->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kStamina, MP * TimeScale() * multiplier);
	}
}

