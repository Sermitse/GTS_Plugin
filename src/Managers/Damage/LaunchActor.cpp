#include "Managers/Damage/LaunchActor.hpp"
#include "Managers/Damage/LaunchPower.hpp"
#include "Utils/DeathReport.hpp"

#include "Managers/CrushManager.hpp"

#include "Managers/HighHeel.hpp"
#include "Managers/Damage/LaunchObject.hpp"

#include "Managers/Animation//Utils/CooldownManager.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"

#include "Managers/Audio/MoansLaughs.hpp"

#include "UI/DebugAPI.hpp"



using namespace GTS;

namespace {

	constexpr float LAUNCH_DAMAGE = 2.4f;
	constexpr float LAUNCH_KNOCKBACK = 0.02f;
	constexpr float BASE_CHECK_DISTANCE = 20.0f;

	bool ObliterateCheck(Actor* giant, Actor* tiny, float sizeRatio, float damage) {
		if (sizeRatio >= Overkills_Obliteration_Diff_Requirement && GetAV(tiny, ActorValue::kHealth) < damage * 0.5f) {
			if (CrushManager::GetSingleton().CanCrush(giant, tiny)) {
				bool OnCooldown = IsActionOnCooldown(giant, CooldownSource::Emotion_Moan_Crush);
				if (!OnCooldown) {
					Task_FacialEmotionTask_Smile(giant, 1.25f, "ObliterateSmile", RandomFloat(0.0f, 0.7f));
					Sound_PlayLaughs(giant, 1.0f, 0.14f, EmotionTriggerSource::Overkill);
					ApplyActionCooldown(giant, CooldownSource::Emotion_Moan_Crush);
				}
				ReportDeath(giant, tiny, DamageSource::Shockwave, false);
				CrushManager::Crush(giant, tiny);
				return true;
			}
			return false;
		}
		return false;
	}

	float GetLaunchThreshold(Actor* giant) {
		float threshold = 8.0f;
		if (Runtime::HasPerkTeam(giant, "GTSPerkRumblingFeet")) {
			threshold *= 0.75f;
		}
		return threshold;
	}

	void LaunchAtThighs(Actor* giant, float radius, float power) {
		auto ThighL = find_node(giant, "NPC L Thigh [LThg]");
		auto ThighR = find_node(giant, "NPC R Thigh [RThg]");
		if (ThighL && ThighR) {
			LaunchActor::GetSingleton().LaunchAtNode(giant, radius, power, ThighL);
			LaunchActor::GetSingleton().LaunchAtNode(giant, radius, power, ThighR);
		}
	}
	void LaunchAtCleavage(Actor* giant, float radius, float power) {
		auto BreastL = find_node(giant, "NPC L Breast");
		auto BreastR = find_node(giant, "NPC R Breast");
		auto BreastL03 = find_node(giant, "L Breast03");
		auto BreastR03 = find_node(giant, "R Breast03");
		if (BreastL03 && BreastR03) {
			LaunchActor::GetSingleton().LaunchAtNode(giant, radius, power, BreastL03);
			LaunchActor::GetSingleton().LaunchAtNode(giant, radius, power, BreastR03);
		} else if (BreastL && BreastR) {
			LaunchActor::GetSingleton().LaunchAtNode(giant, radius, power, BreastL);
			LaunchActor::GetSingleton().LaunchAtNode(giant, radius, power, BreastR);
		}
	}

	void LaunchWithDistance(Actor* giant, Actor* otherActor, float min_radius, float distance, float maxDistance, float power) {
		// Manually used with Hand Attacks. Goal of this function is to prevent launching if actor is inside the hand
		if (min_radius > 0.0f && distance < min_radius) {
			return;
		}
		if (AllowStagger(giant, otherActor)) {
			float force = GetForceFromDistance(distance, maxDistance);
			LaunchActor::GetSingleton().ApplyLaunchTo(giant, otherActor, force, power);
		}
	}
}


namespace GTS {

	LaunchActor& LaunchActor::GetSingleton() noexcept {
		static LaunchActor instance;
		return instance;
	}

	std::string LaunchActor::DebugName() {
		return "::LaunchActor";
	}

	void LaunchActor::ApplyLaunchTo(Actor* giant, Actor* tiny, float force, float launch_power) {
		auto profiler = Profilers::Profile("LaunchActor: ApplyLaunchTo");
		if (IsBeingKilledWithMagic(tiny)) {
			return;
		}
		if (IsBeingHeld(giant, tiny)) {
			return;
		}
		if (IsBeingGrinded(tiny)) {
			return; // Disallow to launch if we're grinding an actor
		}

		float DamageMult = 0.5f * GetDamageSetting();
		float giantSize = get_visual_scale(giant);
		float tinySize = std::clamp(get_visual_scale(tiny), 0.5f, 1000000.0f); // clamp else they will fly into the sky
		float highheel = GetHighHeelsBonusDamage(tiny, true);

		float startpower = Push_Actor_Upwards * highheel * (1.0f + Potion_GetMightBonus(giant)); // determines default power of launching someone
		
		if (Runtime::HasPerkTeam(giant, "GTSPerkRumblingFeet")) {
			startpower *= 1.25f;
		}

		float threshold = 6.0f;
		float SMT = 1.0f;

		bool OwnsPerk = false;

		if (HasSMT(giant)) {
			threshold = 0.92f;
			force += 0.02f;
		}
		float Adjustment = GetSizeFromBoundingBox(tiny);
		
		float sizeRatio = giantSize/(tinySize * Adjustment);

		bool IsLaunching = IsActionOnCooldown(tiny, CooldownSource::Damage_Launch);
		if (!IsLaunching) {
			if (sizeRatio > threshold) {
				if (force >= 0.10f) {
					float power = (1.0f * launch_power) / Adjustment;
					if (Runtime::HasPerkTeam(giant, "GTSPerkDisastrousTremmor")) {
						float might = 1.0f + Potion_GetMightBonus(giant);
						DamageMult *= 2.0f * might;
						OwnsPerk = true;
						power *= 1.5f;
					}

					ApplyActionCooldown(tiny, CooldownSource::Damage_Launch);

					if (Runtime::HasPerkTeam(giant, "GTSPerkDeadlyRumble") && CanDoDamage(giant, tiny, true)) {
						float damage = LAUNCH_DAMAGE * sizeRatio * force * DamageMult * highheel;
						if (OwnsPerk) { // Apply only when we have DisastrousTremor perk
							update_target_scale(tiny, -(damage / 1500) * GetDamageSetting(), SizeEffectType::kShrink);

							if (get_target_scale(tiny) < 0.12f/Adjustment) {
								set_target_scale(tiny, 0.12f/Adjustment);
							}
						}

						if (Runtime::HasPerkTeam(giant, "GTSPerkRuinousStride")) {
							if (ObliterateCheck(giant, tiny, sizeRatio, damage * 1.5f)) {
								return;
							}
							damage *= 1.5f;
						}
						
						InflictSizeDamage(giant, tiny, damage);
					}

					NiPoint3 Push = NiPoint3(0, 0, startpower * GetLaunchPowerFor(giant, sizeRatio, LaunchType::Actor_Launch) * force * power);
					PushActorAway(giant, tiny, 1.0f);

					std::string name = std::format("LaunchOther_{}_{}", giant->formID, tiny->formID);
					ActorHandle tinyHandle = tiny->CreateRefHandle();
					double startTime = Time::WorldTimeElapsed();

					TaskManager::Run(name, [=](auto& update){
						if (tinyHandle) {
							double endTime = Time::WorldTimeElapsed();
							auto tinyref = tinyHandle.get().get();
							if ((endTime - startTime) > 0.05) {
								ApplyManualHavokImpulse(tinyref, Push.x, Push.y, Push.z, 1.0f);
								return false;
							}
							return true;
						}
						return true;
					});
				}
			} else if (sizeRatio > threshold * 0.86f && sizeRatio < threshold) {
				ApplyActionCooldown(tiny, CooldownSource::Damage_Launch);
				StaggerActor(tiny, 0.25f);
			}
		}
	}

	void LaunchActor::ApplyLaunch_At(Actor* giant, float radius, float power, FootEvent kind) {
		if (giant->formID == 0x14 || IsTeammate(giant) || EffectsForEveryone(giant)) {
			switch (kind) {
				case FootEvent::Left: 
					LaunchActor::GetSingleton().LaunchAtFoot(giant, radius, power, false);
				break;
				case FootEvent::Right:
					LaunchActor::GetSingleton().LaunchAtFoot(giant, radius, power, true);
				break;
				case FootEvent::Butt: 
					LaunchAtThighs(giant, radius, power);
				break;
				case FootEvent::Breasts:
					LaunchAtCleavage(giant, radius, power);
				break;
			}
		}
	}


	void LaunchActor::LaunchAtNode(Actor* giant, float radius, float power, std::string_view node) {
		auto bone = find_node(giant, node);
		if (bone) {
			LaunchActor::LaunchAtCustomNode(giant, radius, 0.0f, power, bone);
		}
	}

	void LaunchActor::LaunchAtNode(Actor* giant, float radius, float power, NiAVObject* node) {
		LaunchActor::LaunchAtCustomNode(giant, radius, 0.0f, power, node);
	}

	void LaunchActor::LaunchAtCustomNode(Actor* giant, float radius, float min_radius, float power, NiAVObject* node) {
		auto profiler = Profilers::Profile("LaunchActor: LaunchAtCustomNode");
		if (giant->formID == 0x14 || IsTeammate(giant) || EffectsForEveryone(giant)) {
			if (!node) {
				return;
			}
			if (!giant) {
				return;
			}
			float giantScale = get_visual_scale(giant);

			float SCALE_RATIO = GetLaunchThreshold(giant)/GetMovementModifier(giant);
			if (HasSMT(giant)) {
				SCALE_RATIO = 1.0f/GetMovementModifier(giant);
				giantScale *= 1.5f;
			}

			NiPoint3 point = node->world.translate;

			float maxDistance = BASE_CHECK_DISTANCE * radius * giantScale;
			
			if (IsDebugEnabled() && (giant->formID == 0x14 || IsTeammate(giant) || EffectsForEveryone(giant))) {
				DebugAPI::DrawSphere(glm::vec3(point.x, point.y, point.z), maxDistance, 600, {0.0f, 0.0f, 1.0f, 1.0f});
			}
			
			std::vector<NiPoint3> LaunchObjectPoints = {point};

			NiPoint3 giantLocation = giant->GetPosition();

			bool IsFoot = (node->name == "NPC R Foot [Rft ]" || node->name == "NPC L Foot [Lft ]");

			PushObjectsUpwards(giant, LaunchObjectPoints, maxDistance, power, IsFoot);

			for (auto otherActor: find_actors()) {
				if (otherActor != giant) {
					float tinyScale = get_visual_scale(otherActor);
					if (giantScale / tinyScale > SCALE_RATIO) {
						NiPoint3 actorLocation = otherActor->GetPosition();
						float distance = (point - actorLocation).Length();
						if (distance <= maxDistance) {
							LaunchWithDistance(giant, otherActor, min_radius, distance, maxDistance, power);
						}
					}
				}
			}
		}
	}

	void LaunchActor::LaunchAtFoot(Actor* giant, float radius, float power, bool right_foot) {
		auto profiler = Profilers::Profile("LaunchActor: LaunchAtFoot");
		if (!giant) {
			return;
		}
		float giantScale = get_visual_scale(giant);
		float SCALE_RATIO = GetLaunchThreshold(giant)/GetMovementModifier(giant);
		if (HasSMT(giant)) {
			SCALE_RATIO = 1.0f / GetMovementModifier(giant);
			giantScale *= 1.5f;
		}

		radius *= GetHighHeelsBonusDamage(giant, true);

		float maxFootDistance = BASE_CHECK_DISTANCE * radius * giantScale;

		std::vector<NiPoint3> CoordsToCheck = GetFootCoordinates(giant, right_foot, false);
		float HH = HighHeelManager::GetHHOffset(giant).Length();

		if (!CoordsToCheck.empty()) {
			if (IsDebugEnabled() && (giant->formID == 0x14 || IsTeammate(giant) || EffectsForEveryone(giant))) {
				for (auto footPoints: CoordsToCheck) {
					footPoints.z -= HH;
					DebugAPI::DrawSphere(glm::vec3(footPoints.x, footPoints.y, footPoints.z), maxFootDistance, 600, {0.0f, 0.0f, 1.0f, 1.0f});
				}
			}

			NiPoint3 giantLocation = giant->GetPosition();
			PushObjectsUpwards(giant, CoordsToCheck, maxFootDistance, power, true);

			for (auto otherActor: find_actors()) {
				if (otherActor != giant) {
					float tinyScale = get_visual_scale(otherActor);
					if (giantScale / tinyScale > SCALE_RATIO) {
						NiPoint3 actorLocation = otherActor->GetPosition();
						for (auto point: CoordsToCheck) {
							point.z -= HH;
							float distance = (point - actorLocation).Length();
							if (distance <= maxFootDistance) {
								if (AllowStagger(giant, otherActor)) {
									float force = GetForceFromDistance(distance, maxFootDistance);
									ApplyLaunchTo(giant, otherActor, force, power);
								}
							}
						}
					}
				}
			}
		}
	}
}
