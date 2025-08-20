#include "Managers/Damage/LaunchObject.hpp"
#include "Managers/Damage/LaunchPower.hpp"

#include "Config/Config.hpp"

#include "Managers/HighHeel.hpp"

#include "RE/T/TES.hpp"

#include "UI/DebugAPI.hpp"

using namespace GTS;

namespace {

	void Break_Object(TESObjectREFR* ref, float damage, float giant_size, bool smt) {
		if (giant_size > 1.5f || smt) {
			if (smt) {
				damage *= 4.0f;
			}
			ref->DamageObject(damage * 40, true);
		}
	}

	float Multiply_By_Perk(Actor* giant) {
		float multiply = 1.0f;
		if (Runtime::HasPerkTeam(giant, "GTSPerkRumblingFeet")) {
			multiply *= 1.25f;
		}

		if (Runtime::HasPerkTeam(giant, "GTSPerkDisastrousTremmor")) {
			multiply *= 1.5f;
		}
		return multiply;
	}

	float Multiply_By_Mass(bhkRigidBody* body) {
		float mass_total = 1.0f;
		if (body->referencedObject) {
			if (const auto havokRigidBody = static_cast<hkpRigidBody*>(body->referencedObject.get())) {
				hkVector4 mass_get = havokRigidBody->motion.inertiaAndMassInv;
				float mass = reinterpret_cast<float*>(&mass_get.quad)[3];
				
				if (mass > 0) {
					//log::info("Basic Mass is {}", mass);
					
					mass_total /= mass;

					//log::info("Mass of object is {}", mass_total);
					mass_total *= 0.5f; // Just to have old push force on objects
				}
			}
		}
		return mass_total;
	}

    void ApplyPhysicsToObject_Towards(Actor* giant, TESObjectREFR* object, NiPoint3 push, float force, float scale) {
		force *= GetLaunchPowerFor(giant, scale, LaunchType::Object_Towards); 

		NiAVObject* Node = object->Get3D1(false);
		if (Node) {
			auto collision = Node->GetCollisionObject();
			if (collision) {
				bhkRigidBody* body = collision->GetRigidBody();
				if (body) {
					push *= Multiply_By_Mass(body);
					//log::info("Applying force to object, Push: {}, Force: {}, Result: {}", Vector2Str(push), force, Vector2Str(push * force));
					SetLinearImpulse(body, hkVector4(push.x * force, push.y * force, push.z * force, 1.0f));
				}
			}
		}
	}
}



namespace GTS {
    void PushObjectsUpwards(Actor* giant, const std::vector<NiPoint3>& footPoints, float maxFootDistance, float power, bool IsFoot) {
		GTS_PROFILE_SCOPE("LaunchObject: PushObjectsUpwards");

		const bool AllowLaunch = Config::GetGameplay().bLaunchObjects;

    	if (!AllowLaunch) {
			return;
		}

		float giantScale = get_visual_scale(giant);
		bool smt = false;

		power *= Multiply_By_Perk(giant);
		power *= GetHighHeelsBonusDamage(giant, true);
		float HH = HighHeelManager::GetHHOffset(giant).Length();

		if (HasSMT(giant)) {
			smt = true;
			power *= 1.25f;
		}

		if (giantScale < 2.5f) {  // slowly gain power of pushing
			float reduction = (giantScale - 1.5f);
			if (smt) {
				reduction = 0.6f;
			}
			if (reduction < 0.0f) {
				reduction = 0.0f;
			}
			power *= reduction;
		}

		float start_power = Push_Object_Upwards * (1.0f + Potion_GetMightBonus(giant));

		std::vector<ObjectRefHandle> Refs = GetNearbyObjects(giant);

		if (IsDebugEnabled() && (giant->formID == 0x14 || IsTeammate(giant) || EffectsForEveryone(giant))) {
			for (auto point: footPoints) {
				if (IsFoot) {
					point.z -= HH;
				}
				DebugAPI::DrawSphere(glm::vec3(point.x, point.y, point.z), maxFootDistance, 600, {0.0f, 1.0f, 0.0f, 1.0f});
			}
		}

        for (auto object: Refs) {
			if (object) {
				auto objectref = object.get().get();
				if (objectref) {
					if (objectref && objectref->Is3DLoaded()) {
						NiPoint3 objectlocation = objectref->GetPosition();
						for (auto point: footPoints) {
							if (IsFoot) {
								point.z -= HH;
							}
							float distance = (point - objectlocation).Length();
							if (distance <= maxFootDistance) {
								float force = GetForceFromDistance(distance, maxFootDistance);
								float push = GetLaunchPowerFor(giant, giantScale, LaunchType::Object_Launch, start_power * force * power) ;

								auto Object1 = objectref->Get3D1(false);

								if (distance <= maxFootDistance / 3.0f) { // Apply only if too close
									Break_Object(objectref, push, giantScale, smt);
								}

								if (Object1) {
									auto collision = Object1->GetCollisionObject();
									if (collision) {
										auto body = collision->GetRigidBody();
										if (body) {
											push *= Multiply_By_Mass(body);
											SetLinearImpulse(body, hkVector4(0, 0, push, push));
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
    
            
    void PushObjectsTowards(Actor* giant, TESObjectREFR* object, NiAVObject* Bone, float power, float radius, bool Kick) {

    	GTS_PROFILE_SCOPE("LaunchObject: PushObjectsTowards");
		const bool AllowLaunch = Config::GetGameplay().bLaunchObjects;

    	if (!AllowLaunch) {
			return;
		}

		if (!giant || !Bone || !object) {
			return;
		}

        if (!object->Is3DLoaded() || !object->GetCurrent3D()) {
			return;
        }

		if (!giant->Is3DLoaded() || !giant->GetCurrent3D()) {
			return;
		}

		bool smt = HasSMT(giant);
		float giantScale = get_visual_scale(giant);

		float start_power = Push_Object_Forward * (1.0f + Potion_GetMightBonus(giant));

		NiPoint3 point = Bone->world.translate;
		float maxDistance = radius * giantScale;

        if (Kick) { // Offset pos down
            float HH = HighHeelManager::GetHHOffset(giant).Length();
			point.z -= HH * 0.75f;
		}

		if (IsDebugEnabled() && (giant->formID == 0x14 || IsTeammate(giant) || EffectsForEveryone(giant))) {
			DebugAPI::DrawSphere(glm::vec3(point.x, point.y, point.z), maxDistance, 20, {0.5f, 0.0f, 0.5f, 1.0f});
		}

		int nodeCollisions = 0;

		auto model = object->Get3D1(false);
		if (model) {
			VisitNodes(model, [&nodeCollisions, point, maxDistance](NiAVObject& a_obj) {
				float distance = (point - a_obj.world.translate).Length();
				if (distance < maxDistance) {
					nodeCollisions += 1;
					return false;
				}
				return true;
			});
		}

		if (nodeCollisions > 0) {

			double Start = Time::WorldTimeElapsed();
			ActorHandle gianthandle = giant->CreateRefHandle();
			ObjectRefHandle objectref = object->CreateRefHandle();
			std::string name = std::format("PushObject_{}_{}", giant->formID, object->formID);

			NiPoint3 StartPos = Bone->world.translate;

			TaskManager::Run(name, [=](auto& progressData) {

				if (!gianthandle) {
					return false;
				} if (!objectref) {
					return false;
				}

				Actor* giantref = gianthandle.get().get();
				TESObjectREFR* ref = objectref.get().get();
				double Finish = Time::WorldTimeElapsed();
				double timepassed = Finish - Start;

				if (timepassed > 1e-4) {

					if (!Bone) {
						return false;
					}

					NiPoint3 EndPos = Bone->world.translate;

					ApplyPhysicsToObject_Towards(giantref, ref, EndPos - StartPos, start_power, giantScale);
					Break_Object(ref, power * 12 * giantScale * start_power, giantScale, smt);
					return false; // end it
				}
				return true;
			});
		}
	}

	void PushObjects(const std::vector<ObjectRefHandle>& refs, Actor* giant, NiAVObject* bone, float power, float radius, bool Kick) {
		if (!refs.empty()) {
			for (const auto& object: refs) {
				if (object) {
					TESObjectREFR* objectref = object.get().get();
					PushObjectsTowards(giant, objectref, bone, power, radius, Kick);
				}
			}
		}
	}

	std::vector<ObjectRefHandle> GetNearbyObjects(Actor* giant) {
		bool AllowLaunch = Config::GetGameplay().bLaunchObjects;
		if (!AllowLaunch) {
			return {};
		}
		float giantScale = get_visual_scale(giant);

		float maxDistance = 220 * giantScale;

		std::vector<ObjectRefHandle> Objects = {};
		NiPoint3 point = giant->GetPosition();

		const bool PreciseScan = Config::GetGameplay().bLaunchAllCells;

		if (!PreciseScan) { // Scan single cell only
			TESObjectCELL* cell = giant->GetParentCell();
			if (cell) {
				const auto data = cell->GetRuntimeData();
				for (const auto& object: data.references) {
					if (object) {
						const auto objectref = object.get();
						if (objectref) {
							const bool IsActor = objectref->Is(FormType::ActorCharacter);
							if (!IsActor) { // we don't want to apply it to actors
								const NiPoint3 objectlocation = objectref->GetPosition();
								const float distance = (point - objectlocation).Length();
								if (distance <= maxDistance) {
									ObjectRefHandle refhandle = objectref->CreateRefHandle(); 
									Objects.push_back(refhandle);
								}
							}
						}
					}
				}
			}
		}
    	else { // Else scan Entire world
			TESObjectREFR* GiantRef = skyrim_cast<TESObjectREFR*>(giant);
			if (GiantRef) {
				TESEx::ForEachReferenceInRangeEx(GiantRef, maxDistance, [&](RE::TESObjectREFR* a_ref){
					const bool IsActor = a_ref->Is(FormType::ActorCharacter);
					if (!IsActor) { // we don't want to apply it to actors
						const NiPoint3 objectlocation = a_ref->GetPosition();
						const float distance = (point - objectlocation).Length();
						if (distance <= maxDistance) {
							const ObjectRefHandle handle = a_ref->CreateRefHandle();
							if (handle) {
								Objects.push_back(handle);
							}
						}
					}
					return RE::BSContainer::ForEachResult::kContinue;    
				});
			}
		}
		
		return Objects;
	}
}