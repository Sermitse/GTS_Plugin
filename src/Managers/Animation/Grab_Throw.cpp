
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Grab_Throw.hpp"
#include "Managers/OverkillManager.hpp"
#include "Managers/Animation/Grab.hpp"

#include "Managers/Rumble.hpp"

using namespace GTS;

namespace {

	constexpr std::string_view RNode = "NPC R Foot [Rft ]";
	constexpr std::string_view LNode = "NPC L Foot [Lft ]";

	void Throw_DoCollisionDamage(TESObjectREFR* victim_ref, TESObjectREFR* aggressor_ref, float speed) {
		float damage = speed * Damage_Throw_Collision;

		Actor* victim = skyrim_cast<Actor*>(victim_ref);
		Actor* aggressor = skyrim_cast<Actor*>(aggressor_ref);

		if (victim && aggressor) {
			InflictSizeDamage(aggressor, victim, damage);

			std::string task = std::format("ThrowTiny {}", victim->formID);
			ActorHandle giantHandle = aggressor->CreateRefHandle();
			ActorHandle tinyHandle = victim->CreateRefHandle();

			log::info("Inflicting throw damage for {}: {}", victim->GetDisplayFullName(), damage);

			TaskManager::RunOnce(task, [=](auto& update){
				if (!giantHandle) {
					return;
				}
				if (!tinyHandle) {
					return;
				}
				
				auto giant = giantHandle.get().get();
				auto tiny = tinyHandle.get().get();
				float health = GetAV(tiny, ActorValue::kHealth);
				if (health <= 1.0f || tiny->IsDead()) {
					OverkillManager::Overkill(giant, tiny);
				}
			});
		}
	}

	void Throw_RayCastTask(Actor* giant, Actor* tiny, float speed) {
		// currently does nothing
		// Throw_DoCollisionDamage(victim_ref, aggressor_ref, speed);
		// Idea is to apply damage when actor collides with the ground/rock and inflict damage based on how long it took to hit something
	}

	void Throw_RegisterForThrowDamage(Actor* giant, Actor* tiny, float speed) {
		auto transient = Transient::GetActorData(tiny);
		if (transient) {
			//Throw_RayCastTask(giant, tiny, speed);
			transient->ThrowWasThrown = true;
			transient->ThrowOffender = giant;
			transient->ThrowSpeed = speed;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////
	// E V E N T S
	/////////////////////////////////////////////////////////////////////////////////

    void GTSGrab_Throw_MoveStart(AnimationEventData& data) {
		auto giant = &data.giant;
		DrainStamina(giant, "GrabThrow", Runtime::PERK.GTSPerkDestructionBasics, true, 1.25f);
		ManageCamera(giant, true, CameraTracking::Grab_Left);
		StartLHandRumble("GrabThrowL", data.giant, 0.5f, 0.10f);
	}

	void GTSGrab_Throw_FS_R(AnimationEventData& data) {

		if (AnimationVars::Action::IsSitting(&data.giant) || AnimationVars::Crawl::IsCrawling(&data.giant)) {
			return; // Needed to not apply it during animation blending for thigh/crawling animations
		}
		float smt = 1.0f;
		float launch = 1.0f;
		float dust = 0.9f;
		float perk = GetPerkBonus_Basics(&data.giant);
		if (HasSMT(&data.giant)) {
			smt = 1.5f;
			launch = 1.5f;
			dust = 1.25f;
		}

		float shake_power = Rumble_Grab_Throw_Footstep * smt * GetHighHeelsBonusDamage(&data.giant, true);

		Rumbling::Once("Stomp", &data.giant, shake_power, 0.05f, RNode, 0.0f);
		DoDamageEffect(&data.giant, 1.1f * launch * data.animSpeed * perk, 1.0f * launch * data.animSpeed, 10, 0.20f, FootEvent::Right, 1.0f, DamageSource::CrushedRight);
		DoFootstepSound(&data.giant, 1.0f, FootEvent::Right, RNode);
		DoDustExplosion(&data.giant, dust, FootEvent::Right, RNode);
		DoLaunch(&data.giant, 0.75f * perk, 1.55f, FootEvent::Right);
	}

	void GTSGrab_Throw_FS_L(AnimationEventData& data) {
		if (AnimationVars::Action::IsSitting(&data.giant) || AnimationVars::Crawl::IsCrawling(&data.giant)) {
			return; // Needed to not apply it during animation blending for thigh/crawling animations
		}
		float smt = 1.0f;
		float launch = 1.0f;
		float dust = 0.9f;
		float perk = GetPerkBonus_Basics(&data.giant);
		if (HasSMT(&data.giant)) {
			smt = 1.5f;
			launch = 1.5f;
			dust = 1.25f;
		}

		float shake_power = Rumble_Grab_Throw_Footstep * smt * GetHighHeelsBonusDamage(&data.giant, true);

		Rumbling::Once("Stomp", &data.giant, shake_power, 0.05f, LNode, 0.0f);
		DoDamageEffect(&data.giant, 1.1f * launch * data.animSpeed * perk, 1.0f * launch * data.animSpeed, 10, 0.20f, FootEvent::Left, 1.0f, DamageSource::CrushedLeft);
		DoFootstepSound(&data.giant, 1.0f, FootEvent::Left, LNode);
		DoDustExplosion(&data.giant, dust, FootEvent::Left, LNode);
		DoLaunch(&data.giant, 0.75f * perk, 1.55f, FootEvent::Left);
	}

	void GTSGrab_Throw_Throw_Pre(AnimationEventData& data) {// Throw frame 0
		auto giant = &data.giant;
		auto otherActor = Grab::GetHeldActor(&data.giant);

		Grab::DetachActorTask(giant);
		Grab::Release(giant);

		AnimationVars::Grab::SetHasGrabbedTiny(giant, false);
		AnimationVars::Grab::SetGrabState(giant, false);

		if (otherActor) {

			auto charcont = otherActor->GetCharController();
			if (charcont) {
				charcont->SetLinearVelocityImpl((0.0f, 0.0f, 0.0f, 0.0f)); // Needed so Actors won't fall down.
			}

			auto bone = find_node(giant, "NPC L Hand [LHnd]"); 
			if (bone) {
				NiPoint3 startCoords = bone->world.translate;

				ActorHandle gianthandle = giant->CreateRefHandle();
				ActorHandle tinyhandle = otherActor->CreateRefHandle();

				std::string name = std::format("Throw_{}_{}", giant->formID, otherActor->formID);
				std::string pass_name = std::format("ThrowOther_{}_{}", giant->formID, otherActor->formID);
				// Run task that will actually launch the Tiny
				TaskManager::Run(name, [=](auto& update){
				if (!gianthandle) {
					return false;
				}
				if (!tinyhandle) {
					return false;
				}
				Actor* giant = gianthandle.get().get();
				Actor* tiny = tinyhandle.get().get();
				
				// Wait for 3D to be ready
				if (!giant->Is3DLoaded()) {
					return true;
				}
				if (!giant->GetCurrent3D()) {
					return true;
				}
				if (!tiny->Is3DLoaded()) {
					return true;
				}
				if (!tiny->GetCurrent3D()) {
					return true;
				}

				NiPoint3 endCoords = bone->world.translate;

				Anims_FixAnimationDesync(giant, tiny, true);

				SetBeingHeld(tiny, false);
				EnableCollisions(tiny);

				PushActorAway(giant, tiny, 1.0f);

				auto charcont = tiny->GetCharController();
				if (charcont) {
					charcont->SetLinearVelocityImpl((0.0f, 0.0f, 0.0f, 0.0f)); // Stop actor moving in space, just in case
				}
				float throw_mult = HasSMT(giant) ? 5.0f : 2.0f;
				float Z = 35.0f;
				if (giant->IsSneaking()) {
					throw_mult *= 0.2f; // Else it is too strong, literally throws 70+ meters at normal size
					if (AnimationVars::Crawl::IsCrawling(giant)) {
						Z = 25.0f;
					}
				}
				
				Animation_GrabThrow::Throw_Actor(gianthandle, tinyhandle, startCoords, endCoords, pass_name, throw_mult, Z);
				
				return false;
				});
			}
		}
	}
	

	void GTSGrab_Throw_ThrowActor(AnimationEventData& data) {
		// Throw frame 1
		auto giant = &data.giant;

		AnimationVars::Grab::SetHasGrabbedTiny(giant, false);
		AnimationVars::Grab::SetGrabState(giant, false);
		ManageCamera(giant, false, CameraTracking::Grab_Left);
		Rumbling::Once("ThrowFoe", &data.giant, 2.50f, 0.10f, "NPC L Hand [LHnd]", 0.0f);
		AnimationManager::StartAnim("TinyDied", giant);

		Grab::DetachActorTask(giant);
		Grab::Release(giant);
	}

	void GTSGrab_Throw_Throw_Post(AnimationEventData& data) {
		// Throw frame 2
	}

	void GTSGrab_Throw_MoveStop(AnimationEventData& data) {
		// Throw Frame 3
		auto giant = &data.giant;
		DrainStamina(giant, "GrabThrow", Runtime::PERK.GTSPerkDestructionBasics, false, 1.25f);
		StopLHandRumble("GrabThrowL", data.giant);
	}

	NiMatrix3 CreatePitchMatrix(float angleDeg) {
		float a = angleDeg * (std::numbers::pi_v<float> / 180.0f);
		float c = std::cos(a);
		float s = std::sin(a);
		NiMatrix3 m;

		m.entry[0][0] = 1.f;   m.entry[0][1] = 0.f;  m.entry[0][2] = 0.f;
		m.entry[1][0] = 0.f;   m.entry[1][1] = c;    m.entry[1][2] = s;
		m.entry[2][0] = 0.f;   m.entry[2][1] = -s;   m.entry[2][2] = c;

		return m;
	}

	NiMatrix3 CreateRotationMatrixY(float angleDegrees) {
		float angleRadians = angleDegrees * (std::numbers::pi_v<float> / 180.0f);
		NiMatrix3 m;
		float c = std::cos(angleRadians);
		float s = std::sin(angleRadians);
		m.entry[0][0] = c;         m.entry[0][1] = 0.f;      m.entry[0][2] = s;
		m.entry[1][0] = 0.f;       m.entry[1][1] = 1.f;      m.entry[1][2] = 0.f;
		m.entry[2][0] = -s;        m.entry[2][1] = 0.f;      m.entry[2][2] = c;
		return m;
	}

}

namespace GTS {

	void Animation_GrabThrow::Throw_Actor(const ActorHandle& giantHandle, const ActorHandle& tinyHandle, NiPoint3 startCoords, NiPoint3 endCoords, std::string_view TaskName, float speedmult, float upDownAngle, float leftRightAngle) {
		const double startTime = Time::WorldTimeElapsed();
		TaskManager::Run(TaskName, [=](auto& update) {
			// Check if actors exist
			if (!giantHandle || !tinyHandle) {
				return false;
			}

			Actor* giant = giantHandle.get().get();
			Actor* tiny = tinyHandle.get().get();
			// Make sure 3D models are loaded
			if (!giant->Is3DLoaded() || !giant->GetCurrent3D() ||
				!tiny->Is3DLoaded() || !tiny->GetCurrent3D()) {
				return true;
			}

			// Wait for minimum time to pass
			double endTime = Time::WorldTimeElapsed();
			if ((endTime - startTime) <= 0.05) {
				return true;
			}

			// Calculate throw direction and power
			NiPoint3 vector = endCoords - startCoords;
			float distanceTravelled = vector.Length();
			double timeTaken = endTime - startTime;

			// Base speed calculation
			float speed = static_cast<float>((distanceTravelled / timeTaken) * 10);

			NiMatrix3 giantRot = giant->GetCurrent3D()->world.rotate;
			NiMatrix3 pitchRot = CreatePitchMatrix(-upDownAngle + 90.0f);  // Pitch (up/down)
			NiMatrix3 yawRot = CreateRotationMatrixY(leftRightAngle); // Yaw (left/right)

			// Apply rotations in sequence: first pitch, then yaw
			NiPoint3 forward = NiPoint3(0.0f, 0.0f, 1.0f);
			NiPoint3 localDir = yawRot * (pitchRot * forward);

			// Apply giant's orientation and normalize
			NiPoint3 direction = giantRot * localDir;
			float len = direction.Length();
			if (len > 0.0f) {
				direction.x /= len;
				direction.y /= len;
				direction.z /= len;
			}

			float timeMultiplier = 1.0f / Time::GGTM();

			// Apply physics impulse to tiny
			ApplyManualHavokImpulse(tiny, direction.x, direction.y, direction.z, speed * speedmult * timeMultiplier);
			return false;  // Task complete
		});
	}


    void Animation_GrabThrow::RegisterEvents() {
        AnimationManager::RegisterEvent("GTSGrab_Throw_MoveStart", "Grabbing", GTSGrab_Throw_MoveStart);
		AnimationManager::RegisterEvent("GTSGrab_Throw_FS_R", "Grabbing", GTSGrab_Throw_FS_R);
		AnimationManager::RegisterEvent("GTSGrab_Throw_FS_L", "Grabbing", GTSGrab_Throw_FS_L);
		AnimationManager::RegisterEvent("GTSGrab_Throw_Throw_Pre", "Grabbing", GTSGrab_Throw_Throw_Pre);
		AnimationManager::RegisterEvent("GTSGrab_Throw_ThrowActor", "Grabbing", GTSGrab_Throw_ThrowActor);
		AnimationManager::RegisterEvent("GTSGrab_Throw_Throw_Post", "Grabbing", GTSGrab_Throw_Throw_Post);
		AnimationManager::RegisterEvent("GTSGrab_Throw_MoveStop", "Grabbing", GTSGrab_Throw_MoveStop);
    }
}