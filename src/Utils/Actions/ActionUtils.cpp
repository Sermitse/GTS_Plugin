#include "Utils/Actions/ActionUtils.hpp"

#include "Managers/Animation/Grab.hpp"
#include "Managers/Animation/HugShrink.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"

#include "Magic/Effects/Common.hpp"

#include "Config/Config.hpp"

namespace GTS {

	bool IsBeingHeld(Actor* giant, Actor* tiny) {
		auto grabbed = Grab::GetHeldActor(giant);

		if (grabbed) {
			if (grabbed == tiny) {
				return true;
			}
		}

		auto transient = Transient::GetActorData(tiny);
		if (transient) {
			return transient->BeingHeld && !tiny->IsDead();
		}
		return false;
	}

	bool IsBetweenBreasts(Actor* actor) {
		auto transient = Transient::GetActorData(actor);
		if (transient) {
			return transient->BetweenBreasts;
		}
		return false;
	}

	void Attachment_SetTargetNode(Actor* giant, AttachToNode Node) {
		auto transient = Transient::GetActorData(giant);
		if (transient) {
			transient->AttachmentNode = Node;
		}
	}

	AttachToNode Attachment_GetTargetNode(Actor* giant) {
		auto transient = Transient::GetActorData(giant);
		if (transient) {
			return transient->AttachmentNode;
		}
		return AttachToNode::None;
	}

	void SetBusyFoot(Actor* giant, BusyFoot Foot) { // Purpose of this function is to prevent idle pushing/dealing damage during stomps
		auto transient = Transient::GetActorData(giant);
		if (transient) {
			transient->FootInUse = Foot;
		}
	}

	BusyFoot GetBusyFoot(Actor* giant) {
		auto transient = Transient::GetActorData(giant);
		if (transient) {
			return transient->FootInUse;
		}
		return BusyFoot::None;
	}

	void ControlAnother(Actor* target, bool reset) {
		Actor* player = PlayerCharacter::GetSingleton();
		auto transient = Transient::GetActorData(player);
		if (transient) {
			if (reset) {
				transient->IsInControl = nullptr;
				return;
			}
			transient->IsInControl = target;
		}
	}

	std::vector<Actor*> GetMaxActionableTinyCount(Actor* giant, const std::vector<Actor*>& actors) {
		float capacity = 1.0f;
		std::vector<Actor*> vories = {};
		if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkMassActions)) {
			capacity = 3.0f * get_visual_scale(giant);
			if (HasSMT(giant)) {
				capacity *= 3.0f;
			}
		}
		for (auto target : actors) {
			if (capacity <= 1.0f) {
				capacity = 1.0f;
				vories.push_back(target);
				//log::info("(Return) Max Vore for {} is {}", giant->GetDisplayFullName(), vories.size());
				return vories;
			}
			float decrease = get_target_scale(target) * 12.0f;
			capacity -= decrease;
			vories.push_back(target);
		}
		//log::info("Max Vore for {} is {}", giant->GetDisplayFullName(), vories.size());
		return vories;
	}

	float GetProneAdjustment() {
		auto player = PlayerCharacter::GetSingleton();
		float value = 1.0f;
		if (AnimationVars::Prone::IsProne(player)) {
			return 0.18f;
		}
		if (AnimationVars::Crawl::IsCrawling(player)) {
			value = Config::Camera.fFPCrawlHeightMult;
		}

		return value;
	}

	void SpawnActionIcon(Actor* giant) {
		if (!giant) {
			return;
		}
		bool enabled = Config::General.bShowIcons;

		if (!enabled) {
			return;
		}
		static Timer EffectTimer = Timer(3.0);
		if (giant->formID == 0x14 && EffectTimer.ShouldRunFrame()) {
			NiPoint3 NodePosition = giant->GetPosition();

			float giantScale = get_visual_scale(giant);
			auto huggedActor = HugShrink::GetHuggiesActor(giant);

			constexpr float BASE_DISTANCE = 124.0f;
			float CheckDistance = BASE_DISTANCE * giantScale;

			if (AnimationVars::Crawl::IsCrawling(giant)) {
				CheckDistance *= 1.5f;
			}

			if (DebugDraw::CanDraw()) {
				DebugDraw::DrawSphere(glm::vec3(NodePosition.x, NodePosition.y, NodePosition.z), CheckDistance, 60, { 0.5f, 1.0f, 0.0f, 0.5f });
			}

			for (auto otherActor : find_actors()) {
				if (otherActor != giant) {
					if (otherActor->Is3DLoaded() && !otherActor->IsDead()) {
						float tinyScale = get_visual_scale(otherActor) * GetSizeFromBoundingBox(otherActor);
						float difference = get_scale_difference(giant, otherActor, SizeType::VisualScale, true, false);
						if (difference > 5.8f || huggedActor) {
							NiPoint3 actorLocation = otherActor->GetPosition();
							if ((actorLocation - NodePosition).Length() < CheckDistance) {
								int nodeCollisions = 0;

								auto model = otherActor->GetCurrent3D();

								if (model) {
									VisitNodes(model, [&nodeCollisions, NodePosition, CheckDistance](NiAVObject& a_obj) {
										float distance = (NodePosition - a_obj.world.translate).Length();
										if (distance < CheckDistance) {
											nodeCollisions += 1;
											return false;
										}
										return true;
									});
								}
								if (nodeCollisions > 0) {
									auto node = find_node(otherActor, "NPC Root [Root]");
									if (node) {
										auto grabbedActor = Grab::GetHeldActor(giant);
										float correction = 0;
										if (tinyScale < 1.0f) {
											correction = std::clamp((18.0f / tinyScale) - 18.0f, 0.0f, 144.0f);
										}
										else {
											correction = (18.0f * tinyScale) - 18.0f;
										}

										float iconScale = std::clamp(tinyScale, 1.0f, 9999.0f) * 2.4f;
										bool Ally = !IsHostile(giant, otherActor) && IsTeammate(otherActor);
										bool HasLovingEmbrace = Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkHugsLovingEmbrace);
										bool Healing = AnimationVars::Hug::IsHugHealing(giant);

										NiPoint3 Position = node->world.translate;
										float bounding_z = get_bounding_box_z(otherActor);
										if (bounding_z > 0.0f) {
											if (AnimationVars::Crawl::IsCrawling(giant) && AnimationVars::Tiny::IsBeingHugged(otherActor)) {
												bounding_z *= 0.25f; // Move the icon down
											}
											Position.z += (bounding_z * get_visual_scale(otherActor) * 2.35f); // 2.25 to be slightly above the head
											//log::info("For Actor: {}", otherActor->GetDisplayFullName());
											//log::info("---	Position: {}", Vector2Str(Position));
											//log::info("---	Actor Position: {}", Vector2Str(otherActor->GetPosition()));
											//log::info("---	Bounding Z: {}, Bounding Z * Scale: {}", bounding_z, bounding_z * tinyScale);
										}
										else {
											Position.z -= correction;
										}

										if (grabbedActor && grabbedActor == otherActor) {
											//do nothing
										}
										else if (huggedActor && huggedActor == otherActor && Ally && HasLovingEmbrace && !Healing) {
											SpawnParticle(otherActor, 3.00f, "GTS/UI/Icon_LovingEmbrace.nif", NiMatrix3(), Position, iconScale, 7, node);
										}
										else if (huggedActor && huggedActor == otherActor && !AnimationVars::Hug::IsHugCrushing(giant) && !Healing) {
											bool LowHealth = (GetHealthPercentage(huggedActor) < GetHugCrushThreshold(giant, otherActor, true));
											bool ForceCrush = Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkHugMightyCuddles);
											float Stamina = GetStaminaPercentage(giant);
											if (HasSMT(giant) || LowHealth || (ForceCrush && Stamina > 0.75f)) {
												SpawnParticle(otherActor, 3.00f, "GTS/UI/Icon_Hug_Crush.nif", NiMatrix3(), Position, iconScale, 7, node); // Spawn 'can be hug crushed'
											}
										}
										else if (!AnimationVars::General::IsGTSBusy(giant) && IsEssential(giant, otherActor)) {
											SpawnParticle(otherActor, 3.00f, "GTS/UI/Icon_Essential.nif", NiMatrix3(), Position, iconScale, 7, node);
											// Spawn Essential icon
										}
										else if (!AnimationVars::General::IsGTSBusy(giant) && difference >= Action_Crush) {
											if (CanDoActionBasedOnQuestProgress(giant, QuestAnimationType::kVore)) {
												SpawnParticle(otherActor, 3.00f, "GTS/UI/Icon_Crush_All.nif", NiMatrix3(), Position, iconScale, 7, node);
												// Spawn 'can be crushed and any action can be done'
											}
											else {
												SpawnParticle(otherActor, 3.00f, "GTS/UI/Icon_Crush.nif", NiMatrix3(), Position, iconScale, 7, node);
												// just spawn can be crushed, can happen at any quest stage
											}
										}
										else if (!AnimationVars::General::IsGTSBusy(giant) && difference >= Action_Grab) {
											if (CanDoActionBasedOnQuestProgress(giant, QuestAnimationType::kVore)) {
												SpawnParticle(otherActor, 3.00f, "GTS/UI/Icon_Vore_Grab.nif", NiMatrix3(), Position, iconScale, 7, node);
												// Spawn 'Can be grabbed/vored'
											}
											else if (CanDoActionBasedOnQuestProgress(giant, QuestAnimationType::kGrabAndSandwich)) {
												SpawnParticle(otherActor, 3.00f, "GTS/UI/Icon_Grab.nif", NiMatrix3(), Position, iconScale, 7, node);
												// Spawn 'Can be grabbed'
											}
										}
										else if (!AnimationVars::General::IsGTSBusy(giant) && difference >= Action_Sandwich && CanDoActionBasedOnQuestProgress(giant, QuestAnimationType::kGrabAndSandwich)) {
											SpawnParticle(otherActor, 3.00f, "GTS/UI/Icon_Sandwich.nif", NiMatrix3(), Position, iconScale, 7, node); // Spawn 'Can be sandwiched'
										}
										// 1 = stomps and kicks
										// 2 = Grab and Sandwich
										// 3 = Vore
										// 5 = Others
									}
								}
							}
						}
					}
				}
			}
		}
	}

	void SetBeingHeld(Actor* tiny, bool enable) {
		auto transient = Transient::GetActorData(tiny);
		if (transient) {
			transient->BeingHeld = enable;
		}
	}

	void SetProneState(Actor* giant, bool enable) {
		if (giant->formID == 0x14) {
			auto transient = Transient::GetActorData(giant);
			if (transient) {
				transient->FPProning = enable;
			}
		}
	}

	void SetBetweenBreasts(Actor* actor, bool enable) {
		auto transient = Transient::GetActorData(actor);
		if (transient) {
			transient->BetweenBreasts = enable;
		}
	}

	void SetBeingEaten(Actor* tiny, bool enable) {
		auto transient = Transient::GetActorData(tiny);
		if (transient) {
			transient->AboutToBeEaten = enable;
		}
	}

	void SetBeingGrinded(Actor* tiny, bool enable) {
		auto transient = Transient::GetActorData(tiny);
		if (transient) {
			transient->BeingFootGrinded = enable;
		}
	}

	void ResetGrab(Actor* giant) {
		if (giant->formID == 0x14 || IsTeammate(giant)) {
			Grab::ExitGrabState(giant);

			AnimationVars::Grab::SetHasGrabbedTiny(giant, false); // Tell behaviors 'we have nothing in our hands'. A must.
			AnimationVars::Grab::SetGrabState(giant, false);
			AnimationVars::Action::SetIsStoringTiny(giant, false);
		}
	}

	void UpdateCrawlState(Actor* actor) {

		if (!actor) {
			return;
		}

		bool CrawlState = (actor->formID == 0x14) ? Persistent::EnableCrawlPlayer.value : Persistent::EnableCrawlFollower.value;

		//SetCrawlAnimation Returns true if the state has changed
		if (SetCrawlAnimation(actor, CrawlState)) {
			UpdateCrawlAnimations(actor, CrawlState);
		}
	}

	void UpdateFootStompType(RE::Actor* a_actor) {
		if (!a_actor) {
			return;
		}

		auto& ActionS = Config::Gameplay.ActionSettings;
		bool StompState = (a_actor->formID == 0x14) ? ActionS.bStompAlternative : ActionS.bStomAlternativeOther;
		SetAltFootStompAnimation(a_actor, StompState);
	}

	void UpdateSneakTransition(RE::Actor* a_actor) {
		if (!a_actor) {
			return;
		}

		auto& ActionS = Config::Gameplay.ActionSettings;
		bool SneaktState = (a_actor->formID == 0x14) ? ActionS.bSneakTransitions : ActionS.bSneakTransitionsOther;
		SetEnableSneakTransition(a_actor, !SneaktState);
	}

	float GetButtCrushCost(Actor* actor, bool DoomOnly) {
		float cost = 1.0f;
		if (!DoomOnly && Runtime::HasPerkTeam(actor, Runtime::PERK.GTSPerkButtCrush)) {
			cost -= 0.15f;
		}
		if (Runtime::HasPerkTeam(actor, Runtime::PERK.GTSPerkButtCrushAug4)) {
			cost -= 0.25f;
		}
		cost *= Perk_GetCostReduction(actor);
		return cost;
	}

	bool IsGrowthSpurtActive(Actor* actor) {
		if (!Runtime::HasPerkTeam(actor, Runtime::PERK.GTSPerkGrowthAug1)) {
			return false;
		}
		if (HasGrowthSpurt(actor)) {
			return true;
		}
		return false;
	}

	bool HasGrowthSpurt(Actor* actor) {
		bool Growth1 = Runtime::HasMagicEffect(actor, Runtime::MGEF.GTSEffectGrowthSpurt1);
		bool Growth2 = Runtime::HasMagicEffect(actor, Runtime::MGEF.GTSEffectGrowthSpurt2);
		bool Growth3 = Runtime::HasMagicEffect(actor, Runtime::MGEF.GTSEffectGrowthSpurt3);
		if (Growth1 || Growth2 || Growth3) {
			return true;
		}
		return false;
	}

	void RecordSneaking(Actor* actor) {
		auto transient = Transient::GetActorData(actor);
		bool sneaking = actor->IsSneaking();
		if (transient) {
			transient->WasSneaking = sneaking;
		}
	}

	Actor* GetPlayerOrControlled() {
		Actor* controlled = PlayerCharacter::GetSingleton();
		auto transient = Transient::GetActorData(controlled);
		if (transient) {
			if (transient->IsInControl != nullptr) {
				return transient->IsInControl;
			}
		}
		return controlled;
	}

	//RenameTo CanPerformActionOn
	bool CanPerformActionOn(Actor* giant, Actor* tiny, bool HugCheck) {

		bool Busy = AnimationVars::Tiny::IsBeingGrinded(tiny) || AnimationVars::Tiny::IsBeingHugged(tiny) || AnimationVars::Hug::IsHugging(giant);
		// If any of these is true = we disallow animation

		bool Teammate = IsTeammate(tiny);
		bool hostile = IsHostile(giant, tiny);
		bool essential = IsEssential_WithIcons(giant, tiny); // Teammate check is also done here, spawns icons
		bool no_protection = Config::AI.bAllowFollowers;
		bool Ignore_Protection = (HugCheck && giant->formID == 0x14 && Runtime::HasPerk(giant, Runtime::PERK.GTSPerkHugsLovingEmbrace));
		bool allow_teammate = (giant->formID != 0x14 && no_protection && IsTeammate(tiny) && IsTeammate(giant));

		if (IsFlying(tiny)) {
			return false; // Disallow to do stuff with flying dragons
		}
		if (Busy) {
			return false;
		}
		if (Ignore_Protection) {
			return true;
		}
		if (allow_teammate) { // allow if type is (teammate - teammate), and if bool is true
			return true;
		}
		if (essential) { // disallow to perform on essentials
			return false;
		}
		if (hostile) { // always allow for non-essential enemies. Will return true if Teammate is hostile towards someone (even player)
			return true;
		}
		if (!Teammate) { // always allow for non-teammates
			return true;
		}
		return true; // else allow
	}

}
