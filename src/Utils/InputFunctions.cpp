#include "Utils/InputFunctions.hpp"
#include "Utils/InputConditions.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Input/InputManager.hpp"
#include "Managers/Animation/Controllers/VoreController.hpp"
#include "Managers/Rumble.hpp"
#include "Managers/HighHeel.hpp"

#include "Magic/Effects/Common.hpp"

#include "Managers/GtsSizeManager.hpp"


#include "Managers/Audio/MoansLaughs.hpp"


using namespace GTS;

namespace {

	constexpr float DURATION = 2.0f;

	void ReportScaleIntoConsole(Actor* actor, bool enemy) {

		float hh = HighHeelManager::GetBaseHHOffset(actor)[2]/100;
		float gigantism = Ench_Aspect_GetPower(actor) * 100;
		float naturalscale = get_natural_scale(actor, true);
		float scale = get_visual_scale(actor);
		float maxscale = get_max_scale(actor);

		Actor* player = PlayerCharacter::GetSingleton();

		float BB = GetSizeFromBoundingBox(actor);
		if (enemy) {

			Cprint("{} Bounding Box To Size: {:.2f}, GameScale: {:.2f}", 
				actor->GetDisplayFullName(), 
				BB, 
				game_getactorscale(actor)
			);

			Cprint("{} Size Difference With the Player: {:.2f}", 
				actor->GetDisplayFullName(), 
				GetSizeDifference(player, actor, SizeType::VisualScale, false, true)
			);
		}
		else {
			Cprint("{} Height: {} Weight: {}", 
				actor->GetDisplayFullName(),
				GetFormatedHeight(actor),
				GetFormatedWeight(actor)
			);
		}

		if (maxscale > 250.0f * naturalscale) {

			Cprint("{} Scale: {:.2f}  (Natural Scale: {:.2f}; Bounding Box: {}; Size Limit: Infinite; Aspect Of Giantess: {:.1f}%)", 
				actor->GetDisplayFullName(), 
				scale, 
				naturalscale, 
				BB, 
				gigantism
			);
		}
		else {
			Cprint("{} Scale: {:.2f}  (Natural Scale: {:.2f}; Bounding Box: {}; Size Limit: {:.2f}; Aspect Of Giantess: {:.1f}%)", 
				actor->GetDisplayFullName(), 
				scale, 
				naturalscale, 
				BB, 
				maxscale, 
				gigantism
			);
		}

		if (hh > 0.0f) { // if HH is > 0, print HH info
			Cprint("{} High Heels: {:.2f} (+{:.2f} cm / +{:.2f} ft)", 
				actor->GetDisplayFullName(), 
				hh, 
				hh, 
				hh*3.28f
			);
		}
	}

	void ReportScale(bool enemy) {
		for (auto actor: find_actors()) {
			if (actor->formID != 0x14) {
				if (enemy && !IsTeammate(actor)) {
					ReportScaleIntoConsole(actor, enemy);
				} else if (IsTeammate(actor)) {
					ReportScaleIntoConsole(actor, false);
				}
			} else {
				if (!enemy) {
					ReportScaleIntoConsole(actor, false);
				}
			}
		}
	}

	void regenerate_health(Actor* giant, float value) {
		if (Runtime::HasPerk(giant, "GTSPerkSizeReserveAug2")) {
			float maxhp = GetMaxAV(giant, ActorValue::kHealth);
			float regenerate = maxhp * 0.25f * value; // 25% of health

			giant->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kHealth, regenerate * TimeScale());
		}
	}

	//////////////////////////////////////////////////////////////////// Growths/Shrinks that have duration but no animation, we forgot to add them

	void TotalControlEnlargeTeammate_OverTime(const ManagedInputEvent& data) {
		auto casterRef = PlayerCharacter::GetSingleton();
		if (casterRef) {
			for (auto targetRef: FindTeammates()) {
				if (targetRef) {
					float falloff = 0.11f * get_visual_scale(targetRef);
					float Volume = std::clamp(get_visual_scale(targetRef)/8.0f, 0.20f, 1.0f);
					Runtime::PlaySoundAtNode_FallOff("GTSSoundGrowth", targetRef, Volume, "NPC Pelvis [Pelv]", falloff);

					// Thread safe handles
					ActorHandle casterHandle = casterRef->CreateRefHandle();
					ActorHandle targetHandle = targetRef->CreateRefHandle();

					std::string name = std::format("GrowFollower_{}", targetRef->formID);
					
					TaskManager::RunFor(name, DURATION, [=](auto& progressData){
						if (!casterHandle) {
							return false;
						}
						if (!targetHandle) {
							return false;
						}
						float timeDelta = static_cast<float>(progressData.delta * 60); // Was optimised as 60fps

						auto target = targetHandle.get().get();
						auto caster = casterHandle.get().get();

						float target_scale = get_target_scale(target);
						float magicka = std::clamp(GetMagikaPercentage(caster), 0.05f, 1.0f);

						float bonus = 1.0f;
						if (Runtime::HasMagicEffect(caster, "GTSPotionEffectSizeAmplify")) {
							bonus = target_scale * 0.25f + 0.75f;
						}

						DamageAV(caster, ActorValue::kMagicka, 0.45f * (target_scale * 0.25f + 0.75f) * magicka * bonus * timeDelta * 1.0f);
						Grow(target, 0.0030f * magicka * bonus, 0.0f);
						Rumbling::Once("GrowOtherButton", target, 1.0f, 0.05f);

						return true;
					});
				}
			}
		}
	}

	void TotalControlShrinkTeammate_OverTime(const ManagedInputEvent& data) {
		auto casterRef = PlayerCharacter::GetSingleton();
		if (casterRef) {
			for (auto targetRef: FindTeammates()) {
				if (targetRef) {
					float scale = get_visual_scale(targetRef);
					float Volume = std::clamp(scale * 0.10f, 0.10f, 1.0f);
					
					float falloff = 0.11f * scale;
					Runtime::PlaySoundAtNode_FallOff("GTSSoundShrink", targetRef, Volume, "NPC Pelvis [Pelv]", falloff);
					
					// Thread safe handles
					ActorHandle casterHandle = casterRef->CreateRefHandle();
					ActorHandle targetHandle = targetRef->CreateRefHandle();

					std::string name = std::format("ShrinkFollower_{}", targetRef->formID);

					TaskManager::RunFor(name, DURATION, [=](auto& progressData){
						if (!casterHandle) {
							return false;
						}
						if (!targetHandle) {
							return false;
						}

						auto target = targetHandle.get().get();
						auto caster = casterHandle.get().get();

						float target_scale = get_target_scale(target);
						float magicka = std::clamp(GetMagikaPercentage(caster), 0.05f, 1.0f);

						float bonus = 1.0f;
						if (Runtime::HasMagicEffect(caster, "GTSPotionEffectSizeAmplify")) {
							bonus = target_scale * 0.25f + 0.75f;
						}

						if (target_scale > get_natural_scale(target, true)) {
							DamageAV(caster, ActorValue::kMagicka, 0.25f * (target_scale * 0.25f + 0.75f) * magicka * bonus * TimeScale() * 1.0f);
							ShrinkActor(target, 0.0030f * magicka * bonus, 0.0f);
							Rumbling::Once("ShrinkOtherButton", target, 1.0f, 0.05f);
						}
						return true;
					});
				}
			}
		}
	}

	void TotalControlGrowPlayer_OverTime(const ManagedInputEvent& data) {
		auto casterRef = PlayerCharacter::GetSingleton();
		if (casterRef) {
			float scale = get_visual_scale(casterRef);
			float Volume = std::clamp(scale * 0.20f, 0.20f, 1.0f);

			float falloff = 0.11f * scale;

			Runtime::PlaySoundAtNode_FallOff("GTSSoundGrowth", casterRef, Volume, "NPC Pelvis [Pelv]", falloff);
		
			// Thread safe handles
			ActorHandle casterHandle = casterRef->CreateRefHandle();

			std::string name = std::format("GrowPlayer_{}", casterRef->formID);

			TaskManager::RunFor(name, DURATION, [=](auto& progressData){
				if (!casterHandle) {
					return false;
				}

				auto caster = casterHandle.get().get();

				float caster_scale = get_visual_scale(caster);
				float target_scale = get_target_scale(caster);

				float bonus = 1.0f;
				if (Runtime::HasMagicEffect(caster, "GTSPotionEffectSizeAmplify")) {
					bonus = target_scale * 0.25f + 0.75f;
				}

				float stamina = std::clamp(GetStaminaPercentage(caster), 0.05f, 1.0f);
				DamageAV(caster, ActorValue::kStamina, 0.45f * (caster_scale * 0.5f + 0.5f) * stamina * TimeScale());

				Grow(caster, 0.0030f * stamina, 0.0f);

				Rumbling::Once("GrowButton", caster, 1.0f, 0.05f);

				return true;
			});
		}
	}
	

	void TotalControlShrinkPlayer_OverTime(const ManagedInputEvent& data) {
		auto casterRef = PlayerCharacter::GetSingleton();
		if (casterRef) {
			float scale = get_visual_scale(casterRef);
			float Volume = std::clamp(scale * 0.10f, 0.10f, 1.0f);
			float falloff = 0.11f * scale;

			Runtime::PlaySoundAtNode_FallOff("GTSSoundShrink", casterRef, Volume, "NPC Pelvis [Pelv]", falloff);
		
			// Thread safe handles
			ActorHandle casterHandle = casterRef->CreateRefHandle();

			std::string name = std::format("ShrinkPlayer_{}", casterRef->formID);

			TaskManager::RunFor(name, DURATION, [=](auto& progressData){
				if (!casterHandle) {
					return false;
				}

				auto caster = casterHandle.get().get();

				float caster_scale = get_visual_scale(caster);
				float target_scale = get_target_scale(caster);

				float stamina = std::clamp(GetStaminaPercentage(caster), 0.05f, 1.0f);

				float bonus = 1.0f;
				if (Runtime::HasMagicEffect(caster, "GTSPotionEffectSizeAmplify")) {
					bonus = target_scale * 0.25f + 0.75f;
				}

				if (target_scale > Minimum_Actor_Scale * 2.0f) {
					DamageAV(caster, ActorValue::kStamina, 0.25f * (caster_scale * 0.5f + 0.5f) * stamina * TimeScale());
					ShrinkActor(caster, 0.0020f * stamina, 0.0f);
					Rumbling::Once("ShrinkButton", caster, 0.60f, 0.05f);
				} else {
					set_target_scale(caster, Minimum_Actor_Scale * 2.0f);
					return false;
				}
				return true;
			});
		}
	}

	////////////////////////////////////////////////////////////////////

	void TotalControlGrowEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		float scale = get_visual_scale(player);
		float stamina = std::clamp(GetStaminaPercentage(player), 0.05f, 1.0f);

		float perk = Perk_GetCostReduction(player);

		DamageAV(player, ActorValue::kStamina, 0.15f * perk * (scale * 0.5f + 0.5f) * stamina * TimeScale());
		Grow(player, 0.0010f * stamina, 0.0f);
		float Volume = std::clamp(get_visual_scale(player)/16.0f, 0.20f, 2.0f);
		Rumbling::Once("ColossalGrowth", player, 0.15f, 0.05f);
		static Timer timergrowth = Timer(2.00);
		if (timergrowth.ShouldRun()) {
			Runtime::PlaySoundAtNode("GTSSoundGrowth", player, Volume, "NPC Pelvis [Pelv]");
		}
	}

	void TotalControlShrinkEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		float scale = get_visual_scale(player);
		float stamina = std::clamp(GetStaminaPercentage(player), 0.05f, 1.0f);

		float perk = Perk_GetCostReduction(player);

		if (get_target_scale(player) > 0.12f) {
			DamageAV(player, ActorValue::kStamina, 0.07f * perk * (scale * 0.5f + 0.5f) * stamina * TimeScale());
			ShrinkActor(player, 0.0010f * stamina, 0.0f);
		} else {
			set_target_scale(player, 0.12f);
		}

		float Volume =std::clamp(get_visual_scale(player)*0.10f, 0.10f, 1.0f);
		Rumbling::Once("ColossalGrowth", player, 0.15f, 0.05f);
		static Timer timergrowth = Timer(2.00);
		if (timergrowth.ShouldRun()) {
			Runtime::PlaySound("GTSSoundShrink", player, Volume, 1.0f);
		}
	}

	void TotalControlGrowOtherEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		for (auto actor: find_actors()) {
			if (!actor) {
				continue;
			}
			if (actor->formID != 0x14 && (IsTeammate(actor))) {

				float perk = Perk_GetCostReduction(player);

				float npcscale = get_visual_scale(actor);
				float magicka = std::clamp(GetMagikaPercentage(player), 0.05f, 1.0f);
				DamageAV(player, ActorValue::kMagicka, 0.15f * perk * (npcscale * 0.5f + 0.5f) * magicka * TimeScale());
				Grow(actor, 0.0010f * magicka, 0.0f);
				float Volume = std::clamp(0.20f, 2.0f, get_visual_scale(actor)/16.0f);
				Rumbling::Once("TotalControlOther", actor, 0.15f, 0.05f);
				static Timer timergrowth = Timer(2.00);
				if (timergrowth.ShouldRun()) {
					Runtime::PlaySoundAtNode("GTSSoundGrowth", actor, Volume, "NPC Pelvis [Pelv]");
				}
			}
		}
	}

	void TotalControlShrinkOtherEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		for (auto actor: find_actors()) {
			if (!actor) {
				continue;
			}
			if (actor->formID != 0x14 && (IsTeammate(actor))) {
				
				float perk = Perk_GetCostReduction(player);

				float npcscale = get_visual_scale(actor);
				float magicka = std::clamp(GetMagikaPercentage(player), 0.05f, 1.0f);
				DamageAV(player, ActorValue::kMagicka, 0.07f * perk * (npcscale * 0.5f + 0.5f) * magicka * TimeScale());
				ShrinkActor(actor, 0.0010f * magicka, 0.0f);
				float Volume = std::clamp(get_visual_scale(actor) * 0.10f, 0.10f, 1.0f);
				Rumbling::Once("TotalControlOther", actor, 0.15f, 0.05f);
				static Timer timergrowth = Timer(2.00);
				if (timergrowth.ShouldRun()) {
					Runtime::PlaySound("GTSSoundShrink", actor, Volume, 1.0f);
				}
			} 
		}
	}

	void RapidGrowthEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
			float target = get_target_scale(player);
			float max_scale = get_max_scale(player);// * get_natural_scale(player);
			if (target >= max_scale) {
				NotifyWithSound(player, "You can't grow any further");
				Rumbling::Once("CantGrow", player, 0.25f, 0.05f);
				return;
			}
			AnimationManager::StartAnim("TriggerGrowth", player);
	}

	void RapidShrinkEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
			float target = get_target_scale(player);
			if (target <= Minimum_Actor_Scale) {
				NotifyWithSound(player, "You can't shrink any further");
				Rumbling::Once("CantGrow", player, 0.25f, 0.05f);
				return;
			}
			AnimationManager::StartAnim("TriggerShrink", player);
	}

	void SizeReserveEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		auto Cache = Persistent::GetActorData(player);
		if (Cache) {
			if (Cache->fSizeReserve > 0.0f) {
				bool Attacking = false;
				player->GetGraphVariableBool("GTS_IsGrabAttacking", Attacking);

				if (!Attacking) {
					float duration = data.Duration();
					
					if (duration >= 1.2f && Runtime::HasPerk(player, "GTSPerkSizeReserve")) {
						bool ShouldPrevent = get_target_scale(player) >= 1.49f && HasSMT(player); // So we don't waste it on Calamity that shrinks player back
						if (!ShouldPrevent) {
							bool HandsBusy = Grab::GetHeldActor(player);
							if (!HandsBusy) {
								float SizeCalculation = duration - 1.2f;
								float gigantism = 1.0f + Ench_Aspect_GetPower(player);
								float Volume = std::clamp(get_visual_scale(player) * Cache->fSizeReserve/10.0f, 0.10f, 2.0f);
								static Timer timergrowth = Timer(3.00);
								if (timergrowth.ShouldRunFrame()) {
									Runtime::PlaySoundAtNode("GTSSoundGrowth", player, Cache->fSizeReserve/50 * duration, "NPC Pelvis [Pelv]");
									Task_FacialEmotionTask_Moan(player, 2.0f, "SizeReserve");
									Sound_PlayMoans(player, 0.8f, 0.14f, EmotionTriggerSource::Growth);
								}

								float shake_power = std::clamp(Cache->fSizeReserve/15 * duration, 0.0f, 2.0f);
								Rumbling::Once("SizeReserve", player, shake_power, 0.05f);

								update_target_scale(player, (SizeCalculation/80) * gigantism, SizeEffectType::kNeutral);
								regenerate_health(player, (SizeCalculation/80) * gigantism);

								Cache->fSizeReserve -= SizeCalculation/80;
								if (Cache->fSizeReserve <= 0) {
									Cache->fSizeReserve = 0.0f; // Protect against negative values.
								}
							}
						}
					}
				}
			}
		}
	}

	void DisplaySizeReserveEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		auto Cache = Persistent::GetActorData(player);
		if (Cache) {
			if (Runtime::HasPerk(player, "GTSPerkSizeReserve")) {
				float gigantism = 1.0f + Ench_Aspect_GetPower(player);
				float Value = Cache->fSizeReserve * gigantism;
				Notify("Size Reserve: {:.2f}", Value);
			}
		}
	}

	void PartyReportEvent(const ManagedInputEvent& data) { // Report follower scale into console
		ReportScale(false);
	}

	void DebugReportEvent(const ManagedInputEvent& data) { // Report enemy scale into console
		ReportScale(true);
	}

	void ShrinkOutburstEvent(const ManagedInputEvent& data) {

		auto player = PlayerCharacter::GetSingleton();

		bool DarkArts2 = Runtime::HasPerk(player, "GTSPerkDarkArtsAug2");
		bool DarkArts3 = Runtime::HasPerk(player, "GTSPerkDarkArtsAug3");
		bool DarkArts_Legendary = Runtime::HasPerk(player, "GTSPerkDarkArtsLegendary");

		float gigantism = 1.0f + Ench_Aspect_GetPower(player);

		float multi = GetDamageResistance(player);

		float healthMax = GetMaxAV(player, ActorValue::kHealth);
		float healthCur = GetAV(player, ActorValue::kHealth);
		float damagehp = 80.0f;

		if (DarkArts2) {
			damagehp -= 10; // less hp drain
		}
		if (DarkArts3) {
			damagehp -= 10; // even less hp drain
		}
		if (DarkArts_Legendary) {
			damagehp -= 12; // even more hp decrease
		}

		damagehp *= multi;
		damagehp /= gigantism;

		if (healthCur < damagehp * 1.10f) {
			Notify("Your health is too low!");
			return; // don't allow us to die from own shrinking
		}

		static Timer NotifyTimer = Timer(2.0);
		bool OnCooldown = IsActionOnCooldown(player, CooldownSource::Misc_ShrinkOutburst);
		if (OnCooldown) {
			if (NotifyTimer.ShouldRunFrame()) {
				double cooldown = GetRemainingCooldown(player, CooldownSource::Misc_ShrinkOutburst);
				std::string message = std::format("Shrink Outburst is on a cooldown: {:.1f} sec", cooldown);
				shake_camera(player, 0.75f, 0.35f);
				NotifyWithSound(player, message);
			}
			return;
		}
		ApplyActionCooldown(player, CooldownSource::Misc_ShrinkOutburst);
		DamageAV(player, ActorValue::kHealth, damagehp);
		ShrinkOutburstExplosion(player, false);
	}

	void ProtectSmallOnesEvent(const ManagedInputEvent& data) {
		static Timer ProtectTimer = Timer(5.0);
		if (ProtectTimer.ShouldRunFrame()) {
			Utils_ProtectTinies(SizeManager::BalancedMode());
		}
	}

	void AnimSpeedUpEvent(const ManagedInputEvent& data) {
		AnimationManager::AdjustAnimSpeed(0.045f); // Increase speed and power
	}

	void AnimSpeedDownEvent(const ManagedInputEvent& data) {
		AnimationManager::AdjustAnimSpeed(-0.045f); // Decrease speed and power
	}

	void AnimMaxSpeedEvent(const ManagedInputEvent& data) {
		AnimationManager::AdjustAnimSpeed(0.090f); // Strongest attack speed buff
	}

	void VoreInputEvent(const ManagedInputEvent& data) {
		static Timer voreTimer = Timer(0.25);
		auto pred = PlayerCharacter::GetSingleton();
		if (IsGtsBusy(pred)) {
			return;
		}

		if (voreTimer.ShouldRunFrame()) {
			auto& VoreManager = VoreController::GetSingleton();

			std::vector<Actor*> preys = VoreManager.GetVoreTargetsInFront(pred, 1);
			for (auto prey: preys) {
				VoreManager.StartVore(pred, prey);
			}
		}
	}

	void VoreInputEvent_Follower(const ManagedInputEvent& data) {
		Actor* player = PlayerCharacter::GetSingleton();
		ForceFollowerAnimation(player, FollowerAnimType::Vore);
	}

	//True for player false for fol;
	void ToggleCrawlImpl(const bool a_IsPlayer) {

		if (a_IsPlayer) {
			/// XOR Bit flip to toggle
			Persistent::EnableCrawlPlayer.value ^= true;
			const std::string Msg = fmt::format("Player Crawl: {}", Persistent::EnableCrawlPlayer.value ? "Enabled" : "Disabled");
			RE::DebugNotification(Msg.c_str(),nullptr,false);
		}
		else {
			/// XOR Bit flip to toggle
			Persistent::EnableCrawlFollower.value ^= true;
			const std::string Msg = fmt::format("Follower Crawl: {}", Persistent::EnableCrawlPlayer.value ? "Enabled" : "Disabled");
			RE::DebugNotification(Msg.c_str(),nullptr, false);
		}


	}

	void ToggleCrawlImpl_Player(const ManagedInputEvent& data) {
		ToggleCrawlImpl(true);
	}

	void ToggleCrawlImpl_Follower(const ManagedInputEvent& data) {
		ToggleCrawlImpl(false);
	}

	void PrintQuickStats(Actor* a_Actor) {

		if (!a_Actor) return;

		//TODO: Re-implement this
		//UIManager::ShowInfos();

		const bool Mammoth = Config::UI.sDisplayUnits == "kMammoth";
		float HH = HighHeelManager::GetBaseHHOffset(a_Actor)[2] / 100;
		const std::string HHOffset = (HighHeelManager::IsWearingHH(a_Actor) && !Mammoth) ? fmt::format(" + {}", GetFormatedHeight(HH)) : "";

		Notify("{}: ({:.2f}x) {}{}",
			a_Actor->GetName(),
			get_visual_scale(a_Actor),
			GetFormatedHeight(a_Actor),
			HHOffset
		);
	}

	void ShowQuickStats(const ManagedInputEvent& data) {
		auto Player = PlayerCharacter::GetSingleton();
		PrintQuickStats(Player);

		for (auto TeamMate : FindTeammates()) {
			PrintQuickStats(TeamMate);
		}
	}

	void OpenSkillTree(const ManagedInputEvent& data) {

		auto UI = UI::GetSingleton();
		if (!UI) return;

		if (!State::IsInBlockingMenu() && !UI->IsMenuOpen(DialogueMenu::MENU_NAME) && UI->IsMenuOpen(Console::MENU_NAME)) {
			Notify("Close all menu's first before opening the skill tree");
			return;
		}

		Runtime::SetFloat("GTSSkillMenu", 1.0);
	}

}

namespace GTS {

	void InputFunctions::RegisterEvents() {

		InputManager::RegisterInputEvent("SizeReserve", SizeReserveEvent, SizeReserveCondition);
		InputManager::RegisterInputEvent("DisplaySizeReserve", DisplaySizeReserveEvent, SizeReserveCondition);
		InputManager::RegisterInputEvent("PartyReport", PartyReportEvent);
		InputManager::RegisterInputEvent("DebugReport", DebugReportEvent);
		InputManager::RegisterInputEvent("AnimSpeedUp", AnimSpeedUpEvent);
		InputManager::RegisterInputEvent("AnimSpeedDown", AnimSpeedDownEvent);
		InputManager::RegisterInputEvent("AnimMaxSpeed", AnimMaxSpeedEvent);
		InputManager::RegisterInputEvent("RapidGrowth", RapidGrowthEvent, RappidGrowShrinkCondition);
		InputManager::RegisterInputEvent("RapidShrink", RapidShrinkEvent, RappidGrowShrinkCondition);
		InputManager::RegisterInputEvent("ShrinkOutburst", ShrinkOutburstEvent, ShrinkOutburstCondition);
		InputManager::RegisterInputEvent("ProtectSmallOnes", ProtectSmallOnesEvent, ProtectSmallOnesCondition);

		InputManager::RegisterInputEvent("ManualGrow", TotalControlGrowEvent, TotalControlCondition);
		InputManager::RegisterInputEvent("ManualShrink", TotalControlShrinkEvent, TotalControlCondition);
		InputManager::RegisterInputEvent("ManualGrowOther", TotalControlGrowOtherEvent, TotalControlCondition);
		InputManager::RegisterInputEvent("ManualShrinkOther", TotalControlShrinkOtherEvent, TotalControlCondition);

		InputManager::RegisterInputEvent("ManualGrowOverTime", TotalControlGrowPlayer_OverTime, TotalControlCondition);
		InputManager::RegisterInputEvent("ManualShrinkOverTime", TotalControlShrinkPlayer_OverTime, TotalControlCondition);
		InputManager::RegisterInputEvent("ManualGrowOtherOverTime", TotalControlEnlargeTeammate_OverTime, TotalControlCondition);
		InputManager::RegisterInputEvent("ManualShrinkOtherOverTime", TotalControlShrinkTeammate_OverTime, TotalControlCondition);

		InputManager::RegisterInputEvent("Vore", VoreInputEvent, VoreCondition);
		InputManager::RegisterInputEvent("PlayerVore", VoreInputEvent_Follower, VoreCondition_Follower);

		//Ported from papyrus
		InputManager::RegisterInputEvent("TogglePlayerCrawl", ToggleCrawlImpl_Player);
		InputManager::RegisterInputEvent("ToggleFollowerCrawl", ToggleCrawlImpl_Follower);
		InputManager::RegisterInputEvent("ShowQuickStats", ShowQuickStats);
		InputManager::RegisterInputEvent("OpenSkillTree", OpenSkillTree);
	}
}