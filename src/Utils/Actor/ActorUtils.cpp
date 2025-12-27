#include "Utils/Actor/ActorUtils.hpp"

#include "Config/Config.hpp"
#include "Debug/DebugDraw.hpp"

#include "Managers/AI/AIFunctions.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Managers/Damage/LaunchActor.hpp"

#include "Systems/Colliders/ActorCollisionData.hpp"


/* ACTOR UTILS
 * Contains general helper functions
 * related to manipulating skyrim actors.
 */

namespace {
	
	const std::vector<const char*> disarm_nodes = {
	"WeaponDagger",
	"WeaponAxe",
	"WeaponSword",
	"WeaponMace",
	"WEAPON",
	"SHIELD",
	"QUIVER",
	"WeaponBow",
	"WeaponBack",
	"AnimObjectL",
	"AnimObjectR",
	"NPC L MagicNode [LMag]",
	"NPC R MagicNode [RMag]",
	/*"NPC R Item01",
	"NPC R Item02",
	"NPC R Item03",
	"NPC L Item01",
	"NPC L Item02",
	"NPC L Item03",*/
	};

}

namespace GTS {

	Actor* GetActorPtr(Actor* actor) {
		return actor;
	}

	Actor* GetActorPtr(Actor& actor) {
		return &actor;
	}

	Actor* GetActorPtr(ActorHandle& actor) {
		if (!actor) {
			return nullptr;
		}
		return actor.get().get();
	}

	Actor* GetActorPtr(const ActorHandle& actor) {
		if (!actor) {
			return nullptr;
		}
		return actor.get().get();
	}

	Actor* GetActorPtr(FormID formId) {
		Actor* actor = TESForm::LookupByID<Actor>(formId);
		if (!actor) {
			return nullptr;
		}
		return actor;
	}

	Actor* GetCharContActor(bhkCharacterController* charCont) {
		for (auto actor : find_actors()) {
			if (charCont == actor->GetCharController()) {
				return actor;
			}
		}
		return nullptr;
	}

	//Function that creates a fake throw vector using Actor3D instead of bones
	bool CalculateThrowDirection(RE::Actor* a_ActorGiant, RE::Actor* a_ActorTiny, RE::NiPoint3& a_From, RE::NiPoint3& a_To, float a_HorizontalAngleOffset, float a_VerticalAngleOffset) {

		constexpr float DegToRadConst = 0.017453292f;
		NiPoint3& t = a_ActorTiny->GetCurrent3D()->world.translate;
		NiMatrix3& m = a_ActorGiant->GetCurrent3D()->world.rotate;
		a_From = { t.x, t.y, t.z };

		// Get the original forward direction
		a_To = {
			m.entry[0][1],
			m.entry[1][1],
			m.entry[2][1]
		};

		// Store the original z component before normalizing the 2D vector
		float originalZ = a_To.z;

		// Normalize the horizontal (XY) component
		a_To.z = 0.0f;
		const float len2d = std::sqrt(a_To.x * a_To.x + a_To.y * a_To.y);
		if (len2d > 0.0f) {
			a_To.x /= len2d;
			a_To.y /= len2d;
		}

		// Apply horizontal angle offset (rotate around Z axis)
		if (a_HorizontalAngleOffset != 0.0f) {

			float radians = a_HorizontalAngleOffset * DegToRadConst; // Convert degrees to radians
			float cosTheta = std::cos(radians);
			float sinTheta = std::sin(radians);

			float originalX = a_To.x;
			float originalY = a_To.y;

			// Rotate the XY vector
			a_To.x = originalX * cosTheta - originalY * sinTheta;
			a_To.y = originalX * sinTheta + originalY * cosTheta;
		}

		// Apply vertical angle offset (adjust Z component)
		if (a_VerticalAngleOffset != 0.0f) {
			float radians = a_VerticalAngleOffset * DegToRadConst; // Convert degrees to radians
			float verticalComponent = std::sin(radians);

			// The length of the horizontal component needs to be reduced to maintain a unit vector
			float horizontalScale = std::cos(radians);
			a_To.x *= horizontalScale;
			a_To.y *= horizontalScale;

			// Set the Z component based on the vertical angle
			a_To.z = verticalComponent;
		}
		else {
			// If no vertical offset is specified, restore the original Z component
			// This preserves any natural vertical aiming that might have been in the original direction
			a_To.z = originalZ;
		}

		// Debug visualization
		if (Config::Advanced.bShowOverlay) {
			glm::vec3 fromGLM{ a_From.x, a_From.y, a_From.z };
			glm::vec3 directionGLM{ a_To.x, a_To.y, a_To.z };
			glm::vec3 toGLM = fromGLM + directionGLM * 500.f;
			constexpr int lifetimeMS = 5000;
			glm::vec4 color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
			float lineThickness = 2.0f;
			DebugDraw::DrawLineForMS(fromGLM, toGLM, lifetimeMS, color, lineThickness);
		}

		return true;
	}

	RE::NiPoint3 RotateAngleAxis(const RE::NiPoint3& vec, const float angle, const RE::NiPoint3& axis) {
		float S = sin(angle);
		float C = cos(angle);

		const float XX = axis.x * axis.x;
		const float YY = axis.y * axis.y;
		const float ZZ = axis.z * axis.z;

		const float XY = axis.x * axis.y;
		const float YZ = axis.y * axis.z;
		const float ZX = axis.z * axis.x;

		const float XS = axis.x * S;
		const float YS = axis.y * S;
		const float ZS = axis.z * S;

		const float OMC = 1.f - C;

		return {
			(OMC * XX + C) * vec.x + (OMC * XY - ZS) * vec.y + (OMC * ZX + YS) * vec.z,
			(OMC * XY + ZS) * vec.x + (OMC * YY + C) * vec.y + (OMC * YZ - XS) * vec.z,
			(OMC * ZX - YS) * vec.x + (OMC * YZ + XS) * vec.y + (OMC * ZZ + C) * vec.z
		};
	}

	void SetSneaking(Actor* actor, bool override_sneak, int enable) {
		if (override_sneak) {
			actor->AsActorState()->actorState1.sneaking = enable;
		}
		else {
			auto transient = Transient::GetActorData(actor);
			if (transient) {
				actor->AsActorState()->actorState1.sneaking = transient->WasSneaking;
			}
		}
	}

	void SetReanimatedState(Actor* actor) {
		auto transient = Transient::GetActorData(actor);
		if (!WasReanimated(actor)) { // disallow to override it again if it returned true a frame before
			bool reanimated = actor->AsActorState()->GetLifeState() == ACTOR_LIFE_STATE::kReanimate;
			if (transient) {
				transient->WasReanimated = reanimated;
			}
		}
	}

	//Disables idle greetings by not allowing the timer to expire
	void ShutUp(Actor* actor) {
		if (!actor) {
			return;
		}
		if (actor->formID != 0x14) {
			auto ai = actor->GetActorRuntimeData().currentProcess;
			if (ai) {
				if (ai->high) {
					ai->high->greetingTimer = 5;
				}
			}
		}
	}

	void DecreaseShoutCooldown(Actor* giant) {
		auto process = giant->GetActorRuntimeData().currentProcess;
		if (giant->formID == 0x14 && process && Runtime::HasPerk(giant, Runtime::PERK.GTSPerkTinyCalamityRefresh)) {
			auto high = process->high;
			float by = 0.90f;
			if (high) {
				float& rec_time = high->voiceRecoveryTime;
				//log::info("Recovery Time: {}", rec_time);
				if (rec_time > 0.0f) {
					HasSMT(giant) ? by = 0.85f : by = 0.90f;

					rec_time < 0.0f ? rec_time == 0.0f : rec_time *= by;

					//log::info("New Recovery Time: {}", rec_time);
				}
			}
		}
	}

	void Disintegrate(Actor* actor) {
		std::string taskname = std::format("Disintegrate_{}", actor->formID);
		auto tinyref = actor->CreateRefHandle();
		TaskManager::RunOnce(taskname, [=](auto& update) {
			if (!tinyref) {
				return;
			}
			auto tiny = tinyref.get().get();

			SetCriticalStage(tiny, ACTOR_CRITICAL_STAGE::kDisintegrateEnd);
		});
	}

	void SetRestrained(Actor* actor) {
		CallVMFunctionOn(actor, "Actor", "SetRestrained", true);
	}

	void SetUnRestrained(Actor* actor) {
		CallVMFunctionOn(actor, "Actor", "SetRestrained", false);
	}

	void SetDontMove(Actor* actor) {
		CallVMFunctionOn(actor, "Actor", "SetDontMove", true);
	}

	void SetMove(Actor* actor) {
		CallVMFunctionOn(actor, "Actor", "SetDontMove", true);
	}

	void KnockAreaEffect(TESObjectREFR* source, float afMagnitude, float afRadius) {
		CallVMFunctionOn(source, "ObjectReference", "KnockAreaEffect", afMagnitude, afRadius);
	}

	void ForceRagdoll(Actor* actor, bool forceOn) {
		if (!actor) {
			return;
		}
		auto charCont = actor->GetCharController();
		if (!charCont) {
			return;
		}
		BSAnimationGraphManagerPtr animGraphManager;
		if (actor->GetAnimationGraphManager(animGraphManager)) {
			for (auto& graph : animGraphManager->graphs) {
				if (graph) {
					if (graph->HasRagdoll()) {
						if (forceOn) {
							graph->AddRagdollToWorld();
							charCont->flags.set(CHARACTER_FLAGS::kFollowRagdoll);
						}
						else {
							graph->RemoveRagdollFromWorld();
							charCont->flags.reset(CHARACTER_FLAGS::kFollowRagdoll);
						}
					}
				}
			}
		}
	}

	std::vector<hkpRigidBody*> GetActorRB(Actor* actor) {
		std::vector<hkpRigidBody*> results = {};
		auto charCont = actor->GetCharController();
		if (!charCont) {
			return results;
		}

		bhkCharProxyController* charProxyController = skyrim_cast<bhkCharProxyController*>(charCont);
		bhkCharRigidBodyController* charRigidBodyController = skyrim_cast<bhkCharRigidBodyController*>(charCont);
		if (charProxyController) {
			// Player controller is a proxy one
			auto& proxy = charProxyController->proxy;
			hkReferencedObject* refObject = proxy.referencedObject.get();
			if (refObject) {
				hkpCharacterProxy* hkpObject = skyrim_cast<hkpCharacterProxy*>(refObject);

				if (hkpObject) {
					// Not sure what bodies is doing
					for (auto body : hkpObject->bodies) {
						results.push_back(body);
					}
					// // This one appears to be active during combat.
					// // Maybe used for sword swing collision detection
					// for (auto phantom: hkpObject->phantoms) {
					// 	results.push_back(phantom);
					// }
					//
					// // This is the actual shape
					// if (hkpObject->shapePhantom) {
					// 	results.push_back(hkpObject->shapePhantom);
					// }
				}
			}
		}
		else if (charRigidBodyController) {
			// NPCs seem to use rigid body ones
			auto& characterRigidBody = charRigidBodyController->characterRigidBody;
			hkReferencedObject* refObject = characterRigidBody.referencedObject.get();
			if (refObject) {
				hkpCharacterRigidBody* hkpObject = skyrim_cast<hkpCharacterRigidBody*>(refObject);
				if (hkpObject) {
					if (hkpObject->m_character) {
						results.push_back(hkpObject->m_character);
					}
				}
			}
		}

		return results;
	}

	void PushActorAway(Actor* source_act, Actor* receiver, float force) {
		// Force < 1.0 can introduce weird sliding issues to Actors, not recommended to pass force < 1.0
		if (receiver->IsDead() || IsBeingKilledWithMagic(receiver)) {
			return;
		}

		if (force <= 1.0f) {
			force = 1.0f;
		}

		if (source_act) {
			auto ai = source_act->GetActorRuntimeData().currentProcess;
			if (ai) {
				if (receiver->Is3DLoaded()) {
					if (source_act->Is3DLoaded()) {
						NiPoint3 direction = receiver->GetPosition() - source_act->GetPosition();
						direction = direction / direction.Length();
						typedef void (*DefPushActorAway)(AIProcess* ai, Actor* actor, NiPoint3& direction, float force);
						REL::Relocation<DefPushActorAway> RealPushActorAway{ RELOCATION_ID(38858, 39895) };
						RealPushActorAway(ai, receiver, direction, force);
					}
				}
			}
		}
	}

	void ApplyManualHavokImpulse(Actor* target, float afX, float afY, float afZ, float Multiplier) {
		// For this function to work, actor must be pushed away first. 
		// It may be a good idea to wait about 0.05 sec before callind it after actor has been pushed, else it may not work
		hkVector4 impulse = hkVector4(afX * Multiplier, afY * Multiplier, afZ * Multiplier, 1.0f);

		auto collision = target->Get3D(false)->GetCollisionObject();
		if (collision) {
			auto rigidbody = collision->GetRigidBody();
			if (rigidbody) {
				auto body = rigidbody->AsBhkRigidBody();
				if (body) {
					SetLinearImpulse(body, impulse);
					//log::info("Bdy found, Applying impulse {} to {}", Vector2Str(impulse), target->GetDisplayFullName());
				}
			}
		}
	}

	float get_distance_to_actor(Actor* receiver, Actor* target) {
		GTS_PROFILE_SCOPE("ActorUtils: GetDistanceToActor");
		if (target) {
			auto point_a = receiver->GetPosition();
			auto point_b = target->GetPosition();
			auto delta = point_a - point_b;
			return delta.Length();
		}
		return std::numeric_limits<float>::max();
		//return 3.4028237E38; // Max float
	}

	void DisableCollisions(Actor* actor, TESObjectREFR* otherActor) {
		if (actor) {
			auto trans = Transient::GetActorData(actor);
			if (trans) {
				trans->DisableColissionWith = otherActor;
				auto colliders = ActorCollisionData(actor);
				colliders.UpdateCollisionFilter();
				if (otherActor) {
					Actor* asOtherActor = skyrim_cast<Actor*>(otherActor);
					auto otherColliders = ActorCollisionData(asOtherActor);
					otherColliders.UpdateCollisionFilter();
				}
			}
		}
	}

	void EnableCollisions(Actor* actor) {
		if (actor) {
			auto trans = Transient::GetActorData(actor);
			if (trans) {
				auto otherActor = trans->DisableColissionWith;
				trans->DisableColissionWith = nullptr;
				auto colliders = ActorCollisionData(actor);
				colliders.UpdateCollisionFilter();
				if (otherActor) {
					Actor* asOtherActor = skyrim_cast<Actor*>(otherActor);
					auto otherColliders = ActorCollisionData(asOtherActor);
					otherColliders.UpdateCollisionFilter();
				}
			}
		}
	}


	hkaRagdollInstance* GetRagdoll(Actor* actor) {
		BSAnimationGraphManagerPtr animGraphManager;
		if (actor->GetAnimationGraphManager(animGraphManager)) {
			for (auto& graph : animGraphManager->graphs) {
				if (graph) {
					auto& character = graph->characterInstance;
					auto ragdollDriver = character.ragdollDriver.get();
					if (ragdollDriver) {
						auto ragdoll = ragdollDriver->ragdoll;
						if (ragdoll) {
							return ragdoll;
						}
					}
				}
			}
		}
		return nullptr;
	}

	void DisarmActor(Actor* tiny, bool drop) {
		if (!drop) {
			for (auto& object_find : disarm_nodes) {
				auto object = find_node(tiny, object_find);
				if (object) {
					//log::info("Found Object: {}", object->name.c_str());
					object->local.scale = 0.01f;
					//object->SetAppCulled(true);

					update_node(object);

					std::string objectname = object->name.c_str();
					std::string name = std::format("ScaleWeapons_{}_{}", tiny->formID, objectname);
					ActorHandle tinyHandle = tiny->CreateRefHandle();

					double Start = Time::WorldTimeElapsed();

					TaskManager::Run(name, [=](auto& progressData) {
						if (!tinyHandle) {
							return false;
						}
						Actor* Tiny = tinyHandle.get().get();
						double Finish = Time::WorldTimeElapsed();

						object->local.scale = 0.01f;
						update_node(object);

						if (Finish - Start > 0.25 && !IsGtsBusy(Tiny)) {
							object->local.scale = 1.0f;
							update_node(object);
							return false;
						}

						return true;
						});
				}
			}
		}
		else {
			NiPoint3 dropPos = tiny->GetPosition();
			for (auto object : { tiny->GetEquippedObject(true), tiny->GetEquippedObject(false) }) {
				dropPos.x += 25 * get_visual_scale(tiny);
				dropPos.y += 25 * get_visual_scale(tiny);
				if (object) {
					log::info("Object found");

					TESBoundObject* as_object = skyrim_cast<TESBoundObject*>(object);
					if (as_object) {
						log::info("As object exists, {}", as_object->GetName());
						dropPos.z += 40 * get_visual_scale(tiny);
						tiny->RemoveItem(as_object, 1, ITEM_REMOVE_REASON::kDropping, nullptr, nullptr, &dropPos, nullptr);
					}
				}
			}
		}
	}

	void ManageRagdoll(Actor* tiny, float deltaLength, NiPoint3 deltaLocation, NiPoint3 targetLocation) {
		if (deltaLength >= 70.0f) {
			// WARP if > 1m
			auto ragDoll = GetRagdoll(tiny);
			hkVector4 delta = hkVector4(deltaLocation.x / 70.0f, deltaLocation.y / 70.0f, deltaLocation.z / 70, 1.0f);
			for (auto rb : ragDoll->rigidBodies) {
				if (rb) {
					auto ms = rb->GetMotionState();
					if (ms) {
						hkVector4 currentPos = ms->transform.translation;
						hkVector4 newPos = currentPos + delta;
						rb->motion.SetPosition(newPos);
						rb->motion.SetLinearVelocity(hkVector4(0.0f, 0.0f, -10.0f, 0.0f));
					}
				}
			}
		}
		else {
			// Just move the hand if <1m
			std::string_view handNodeName = "NPC HAND L [L Hand]";
			auto handBone = find_node(tiny, handNodeName);
			if (handBone) {
				auto collisionHand = handBone->GetCollisionObject();
				if (collisionHand) {
					auto handRbBhk = collisionHand->GetRigidBody();
					if (handRbBhk) {
						auto handRb = static_cast<hkpRigidBody*>(handRbBhk->referencedObject.get());
						if (handRb) {
							auto ms = handRb->GetMotionState();
							if (ms) {
								hkVector4 targetLocationHavok = hkVector4(targetLocation.x / 70.0f, targetLocation.y / 70.0f, targetLocation.z / 70, 1.0f);
								handRb->motion.SetPosition(targetLocationHavok);
								handRb->motion.SetLinearVelocity(hkVector4(0.0f, 0.0f, -10.0f, 0.0f));
							}
						}
					}
				}
			}
		}
	}

	void ChanceToScare(Actor* giant, Actor* tiny, float duration, int random, bool apply_sd) {
		if (tiny->formID == 0x14 || IsTeammate(tiny)) {
			return;
		}
		float sizedifference = get_scale_difference(giant, tiny, SizeType::VisualScale, true, true);
		if (sizedifference > 1.15f && !tiny->IsDead()) {
			int rng = RandomInt(1, random);
			if (apply_sd) {
				rng = static_cast<int>(static_cast<float>(rng) / sizedifference);
			}
			if (rng <= 2.0f * sizedifference) {
				bool IsScared = IsActionOnCooldown(tiny, CooldownSource::Action_ScareOther);
				if (!IsScared && GetAV(tiny, ActorValue::kConfidence) > 0) {
					ApplyActionCooldown(tiny, CooldownSource::Action_ScareOther);
					ForceFlee(giant, tiny, duration, true); // always scare
				}
			}
		}
	}

	void StaggerActor(Actor* receiver, float power) {
		if (receiver->IsDead() || IsRagdolled(receiver) || GetAV(receiver, ActorValue::kHealth) <= 0.0f) {
			return;
		}
		receiver->SetGraphVariableFloat("staggerMagnitude", power);
		receiver->NotifyAnimationGraph("staggerStart");
	}

	void StaggerActor(Actor* giant, Actor* tiny, float power) {
		if (tiny->IsDead() || IsRagdolled(tiny) || IsBeingHugged(tiny) || GetAV(tiny, ActorValue::kHealth) <= 0.0f) {
			return;
		}
		if (!IsBeingKilledWithMagic(tiny)) {
			StaggerActor_Directional(giant, power, tiny);
		}
	}

	void StaggerActor_Around(Actor* giant, const float radius, bool launch) {
		if (!giant) {
			return;
		}
		auto node = find_node(giant, "NPC Root [Root]");
		if (!node) {
			return;
		}
		NiPoint3 NodePosition = node->world.translate;

		float giantScale = get_visual_scale(giant) * GetSizeFromBoundingBox(giant);
		float CheckDistance = radius * giantScale;

		if (IsDebugEnabled() && (giant->formID == 0x14 || IsTeammate(giant))) {
			DebugDraw::DrawSphere(glm::vec3(NodePosition.x, NodePosition.y, NodePosition.z), CheckDistance, 600, { 0.0f, 1.0f, 0.0f, 1.0f });
		}

		NiPoint3 giantLocation = giant->GetPosition();
		for (auto otherActor : find_actors()) {
			if (otherActor != giant) {
				NiPoint3 actorLocation = otherActor->GetPosition();
				if ((actorLocation - giantLocation).Length() < CheckDistance * 3) {
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
						if (!launch) {
							StaggerActor(giant, otherActor, 0.50f);
						}
						else {
							if (get_scale_difference(giant, otherActor, SizeType::VisualScale, true, false) < Push_Jump_Launch_Threshold) {
								StaggerActor(giant, otherActor, 0.50f);
							}
							else {
								float launch_power = 0.33f;
								if (HasSMT(giant)) {
									launch_power *= 6.0f;
								}
								LaunchActor::ApplyLaunchTo(giant, otherActor, 1.0f, launch_power);
							}
						}
					}
				}
			}
		}
	}

	void PushForward(Actor* giantref, Actor* tinyref, float power) {
		double startTime = Time::WorldTimeElapsed();
		ActorHandle tinyHandle = tinyref->CreateRefHandle();
		ActorHandle gianthandle = giantref->CreateRefHandle();
		std::string taskname = std::format("PushOther_{}", tinyref->formID);
		PushActorAway(giantref, tinyref, 1.0f);
		TaskManager::Run(taskname, [=](auto& update) {
			if (!gianthandle) {
				return false;
			}
			if (!tinyHandle) {
				return false;
			}
			Actor* giant = gianthandle.get().get();
			Actor* tiny = tinyHandle.get().get();

			auto playerRotation = giant->GetCurrent3D()->world.rotate;
			RE::NiPoint3 localForwardVector{ 0.f, 1.f, 0.f };
			RE::NiPoint3 globalForwardVector = playerRotation * localForwardVector;

			RE::NiPoint3 direction = globalForwardVector;
			double endTime = Time::WorldTimeElapsed();

			if ((endTime - startTime) > 0.05) {
				ApplyManualHavokImpulse(tiny, direction.x, direction.y, direction.z, power);
				return false;
			}
			else {
				return true;
			}
			});
	}

	void PushTowards(Actor* giantref, Actor* tinyref, std::string_view bone, float power, bool sizecheck) {
		if (!AllowStagger(giantref, tinyref)) {
			return;
		}
		NiAVObject* node = find_node(giantref, bone);
		if (node) {
			PushTowards(giantref, tinyref, node, power, sizecheck);
		}
	}

	void PushTowards_Task(const ActorHandle& giantHandle, const ActorHandle& tinyHandle, const NiPoint3& startCoords, const NiPoint3& endCoords, std::string_view TaskName, float power, bool sizecheck) {

		double startTime = Time::WorldTimeElapsed();

		TaskManager::RunFor(TaskName, 2, [=](auto& update) {
			if (!giantHandle) {
				return false;
			}
			if (!tinyHandle) {
				return false;
			}
			Actor* giant = giantHandle.get().get();
			Actor* tiny = tinyHandle.get().get();

			double endTime = Time::WorldTimeElapsed();
			if (!tiny) {
				return false;
			}
			if (!tiny->Is3DLoaded()) {
				return true;
			}
			if (!tiny->GetCurrent3D()) {
				return true;
			}
			if ((endTime - startTime) > 0.05) {
				// Enough time has elapsed

				NiPoint3 vector = endCoords - startCoords;
				float distanceTravelled = vector.Length();
				float timeTaken = static_cast<float>(endTime - startTime);
				float speed = (distanceTravelled / timeTaken) / GetAnimationSlowdown(giant);
				NiPoint3 direction = vector / vector.Length();

				if (sizecheck) {
					float giantscale = get_visual_scale(giant);
					float tinyscale = get_visual_scale(tiny) * GetSizeFromBoundingBox(tiny);

					if (tiny->IsDead()) {
						tinyscale *= 0.4f;
					}

					if (HasSMT(giant)) {
						giantscale *= 6.0f;
					}
					float sizedifference = giantscale / tinyscale;

					if (sizedifference < 1.2f) {
						return false; // terminate task
					}
					else if (sizedifference > 1.2f && sizedifference < 3.0f) {
						StaggerActor(giant, tiny, 0.25f * sizedifference);
						return false; //Only Stagger
					}
				}

				float Time = (1.0f / Time::GGTM());
				ApplyManualHavokImpulse(tiny, direction.x, direction.y, direction.z, speed * 2.0f * power * Time);

				return false;
			}
			return true;
			});
	}

	void PushTowards(Actor* giantref, Actor* tinyref, NiAVObject* bone, float power, bool sizecheck) {
		NiPoint3 startCoords = bone->world.translate;

		ActorHandle tinyHandle = tinyref->CreateRefHandle();
		ActorHandle giantHandle = giantref->CreateRefHandle();

		PushActorAway(giantref, tinyref, 1.0f);

		double startTime = Time::WorldTimeElapsed();

		std::string name = std::format("PushTowards_{}_{}", giantref->formID, tinyref->formID);
		std::string TaskName = std::format("PushTowards_Job_{}_{}", giantref->formID, tinyref->formID);
		// Do this next frame (or rather until some world time has elapsed)
		TaskManager::Run(name, [=](auto& update) {
			if (!giantHandle) {
				return false;
			}
			if (!tinyHandle) {
				return false;
			}
			Actor* giant = giantHandle.get().get();

			if (!giant->Is3DLoaded()) {
				return true;
			}
			if (!giant->GetCurrent3D()) {
				return true;
			}

			double endTime = Time::WorldTimeElapsed();

			if ((endTime - startTime) > 1e-4) {

				NiPoint3 endCoords = bone->world.translate;

				//log::info("Passing coords: Start: {}, End: {}", Vector2Str(startCoords), Vector2Str(endCoords));
				// Because of delayed nature (and because coordinates become constant once we pass them to TaskManager)
				// i don't have any better idea than to do it through task + task, don't kill me
				PushTowards_Task(giantHandle, tinyHandle, startCoords, endCoords, TaskName, power, sizecheck);
				return false;
			}
			return true;
		});
	}

	void StaggerOr(Actor* giant, Actor* tiny, float afX, float afY, float afZ, float afMagnitude) {
		if (tiny->IsDead()) {
			return;
		}
		if (InBleedout(tiny)) {
			return;
		}
		if (IsBeingHeld(giant, tiny)) {
			return;
		}
		if (!AllowStagger(giant, tiny)) {
			return;
		}

		float giantSize = get_visual_scale(giant);
		float tinySize = get_visual_scale(tiny);

		if (HasSMT(giant)) {
			giantSize += 1.0f;
		} if (tiny->formID == 0x14 && HasSMT(tiny)) {
			tinySize += 1.25f;
		}

		float sizedifference = giantSize / tinySize;
		float sizedifference_tinypov = tinySize / giantSize;

		int ragdollchance = RandomInt(0, 30);
		if ((giantSize >= 2.0f || IsBeingGrinded(tiny)) && !IsRagdolled(tiny) && sizedifference > 2.8f && ragdollchance < 4.0f * sizedifference) { // Chance for ragdoll. Becomes 100% at high scales
			PushActorAway(giant, tiny, 1.0f); // Ragdoll
		}
		else if (sizedifference > 1.25f) { // Always Stagger
			tiny->SetGraphVariableFloat("GiantessScale", sizedifference_tinypov); // enable stagger just in case

			float push = std::clamp(0.25f * (sizedifference - 0.25f), 0.25f, 1.0f);
			StaggerActor(giant, tiny, push);
		}
	}

	void Utils_PushCheck(Actor* giant, Actor* tiny, float force) {
		auto model = tiny->GetCurrent3D();

		if (model) {
			bool isdamaging = IsActionOnCooldown(tiny, CooldownSource::Push_Basic);
			if (!isdamaging && (force >= 0.12f || IsFootGrinding(giant))) {
				//log::info("Check passed, pushing {}, force: {}", tiny->GetDisplayFullName(), force);
				StaggerOr(giant, tiny, 0, 0, 0, 0);
				ApplyActionCooldown(tiny, CooldownSource::Push_Basic);
			}
		}
	}

}
