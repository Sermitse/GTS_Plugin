#include "Managers/Perks/ShrinkingGaze.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/AI/AIFunctions.hpp"
#include "Magic/Effects/Common.hpp"



#include "Managers/Audio/MoansLaughs.hpp"

using namespace GTS;

namespace {
    void ShrinkByOverTime(Actor* tiny, float over_time, float by) {
		ActorHandle tinyHandle = tiny->CreateRefHandle();
		std::string name = std::format("ShrinkToSize_{}", tiny->formID);

		if (tiny->IsDead()) {
			by *= 1.75f;
		}
		float bb = std::clamp(GetSizeFromBoundingBox(tiny), 0.6f, 100.f);
		double Start = Time::WorldTimeElapsed();

		TaskManager::Run(name, [=](auto& progressData) {
			Actor* actor = tinyHandle.get().get();
			if (!actor) {
				return false;
			}

			/*float Scale = get_visual_scale(actor);*/
			double End = Time::WorldTimeElapsed();

			if (End - Start < over_time) {
				override_actor_scale(actor, -by * (0.0166f/bb) * TimeScale(), SizeEffectType::kNeutral);
				if (get_target_scale(actor) < SHRINK_TO_NOTHING_SCALE / bb) {
					set_target_scale(actor, SHRINK_TO_NOTHING_SCALE / bb);
					return false;
				}
				return true;
			} else {
				return false;
			}
		});
	}

	void LaughOr(Actor* giant) {
		int MoanRNG = RandomInt(0, 3);
		if (MoanRNG >= 2 && IsActionOnCooldown(giant, CooldownSource::Emotion_Moan)) {
			ApplyActionCooldown(giant, CooldownSource::Emotion_Moan);
			Task_FacialEmotionTask_Smile(giant, 2.0f, "CalamityMoan",0.0f, 0.35f);
			Sound_PlayLaughs(giant, 1.0f, 0.14f, EmotionTriggerSource::Superiority);
		}
	}

	void ShrinkTheTargetOr(Actor* giant, Actor* otherActor, float stare_threshold, float tiny_size, float difference, TransientActorData* data) {
		SpawnParticle(otherActor, 6.00f, "GTS/Effects/TinyCalamity.nif", NiMatrix3(), otherActor->GetPosition(), tiny_size * 4.5f, 7, nullptr); 
		SpawnCustomParticle(otherActor, ParticleType::Red, otherActor->GetPosition(), "NPC Root [Root]", tiny_size);

		Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundMagicProctectTinies, otherActor, 0.5f, "NPC Root [Root]");

		InflictSizeDamage(giant, otherActor, GetAV(otherActor, ActorValue::kHealth) * 0.015f);
		AddSMTDuration(giant, stare_threshold * 0.75f, false);

		float shrink_bonus = std::clamp(tiny_size, 0.20f, 1.0f);
		ShrinkByOverTime(otherActor, stare_threshold * 0.1f, tiny_size * (0.45f / shrink_bonus));

		if (get_scale_difference(giant, otherActor, SizeType::VisualScale, false, false) >= 0.92f) {
			otherActor->StaggerDirectional(giant, difference);
		}

		if (tiny_size <= 0.50f) {
			ForceFlee(giant, otherActor, 3.0f, true);
		}
		DamageAV(giant, ActorValue::kMagicka, 75 * tiny_size * Perk_GetCostReduction(giant));

		data->ShrinkTicksCalamity = 0.0f;
	}

	void PerformShrinkOnActor(Actor* giant) {
		CrosshairPickData* data = CrosshairPickData::GetSingleton();
		if (data) {
			float maxDistance = 24.0f;
			float checkDistance = 280.0f;
		
			NiPoint3 NodePosition = data->collisionPoint;

			if (NodePosition.Length() > 0.0f) {

				if (DebugDraw::CanDraw(giant, DebugDraw::DrawTarget::kPlayerOnly)) {
					DebugDraw::DrawSphere(glm::vec3(NodePosition.x, NodePosition.y, NodePosition.z), maxDistance);
				}

				if (NodePosition.Length() > 0) {
					NiPoint3 giantLocation = giant->GetPosition();
					for (auto otherActor: find_actors()) {
						if (otherActor != giant && IsHostile(giant, otherActor) && !IsEssential(giant, otherActor)) {
							if (!IsBetweenBreasts(otherActor) && !IsBeingHeld(giant, otherActor)) {
								auto data = Transient::GetActorData(otherActor);
								if (data) {
									NiPoint3 actorLocation = otherActor->GetPosition();
									if ((actorLocation-giantLocation).Length() <= checkDistance) {
										int nodeCollisions = 0;

										float bb = std::clamp(GetSizeFromBoundingBox(otherActor), 1.0f, 100.0f);
										auto model = otherActor->GetCurrent3D();
										
										if (model) {
											VisitNodes(model, [&nodeCollisions, &data, NodePosition, maxDistance, bb](NiAVObject& a_obj) {
												float distance = (NodePosition - a_obj.world.translate).Length() - Collision_Distance_Override * 20 * bb;
					
												if (distance <= maxDistance) {
													data->ShrinkTicksCalamity += 0.0166f * TimeScale();
													if (data->MovementSlowdown > 0.33f) {
														data->MovementSlowdown -= 0.0032f * TimeScale();
													}
													nodeCollisions += 1;
													return false;
												} else {
													if (data->ShrinkTicksCalamity > 0) {
														data->ShrinkTicksCalamity -= 0.0166f * 0.20f * TimeScale();
													}
													data->MovementSlowdown = 1.0f; // Reset it
												}
												return true;
											});
										}
										if (nodeCollisions > 0) {
											float difference = get_scale_difference(giant, otherActor, SizeType::VisualScale, false, false);
											float stare_threshold_s = std::clamp(3.25f * get_visual_scale(otherActor), 0.25f, 6.0f);
											float tiny_size = get_visual_scale(otherActor);

											if (data->ShrinkTicksCalamity >= stare_threshold_s) {
												Laugh_Chance(giant, 1.25f, "CalamityShrink");
												if (!ShrinkToNothing(giant, otherActor, true, 1.50f, 1.20f, true)) {
													ShrinkTheTargetOr(giant, otherActor, stare_threshold_s, tiny_size, difference, data);
													AdjustMassLimit(tiny_size * 0.00025f, giant);
												} else {
													if (!IsActionOnCooldown(otherActor, CooldownSource::Misc_ShrinkParticle_Gaze)) {
														SpawnParticle(otherActor, 6.00f, "GTS/Effects/TinyCalamity.nif", NiMatrix3(), otherActor->GetPosition(), tiny_size * 4.5f, 7, nullptr); 
														Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundMagicProctectTinies, otherActor, 0.5f, "NPC Root [Root]");
														ApplyActionCooldown(otherActor, CooldownSource::Misc_ShrinkParticle_Gaze);
													}
													data->MovementSlowdown = 1.0f;
													LaughOr(giant);
												}
											}
											return;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	void Task_ShrinkingGazeTask(Actor* giant) {
		std::string name = std::format("ShrinkingGaze_{}", giant->formID);
		ActorHandle gianthandle = giant->CreateRefHandle();

		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto giantref = gianthandle.get().get();
			if (!HasSMT(giantref)) {
				return false;
			}

			if (!IsGtsBusy(giantref)) {	
				PerformShrinkOnActor(giantref);
			}

			return true;
		});
	}
}

namespace GTS {

	void StartShrinkingGaze(Actor* giant) {
		if (Runtime::HasPerk(giant, Runtime::PERK.GTSPerkShrinkingGaze) && giant->formID == 0x14) {
			Task_ShrinkingGazeTask(giant);
		}
	}
}