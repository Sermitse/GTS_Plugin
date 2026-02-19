#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CrawlUtils.hpp"
#include "Managers/Damage/LaunchActor.hpp"
#include "Managers/FurnitureManager.hpp"

#include "Config/Config.hpp"

#include "Utils/Actions/ButtCrushUtils.hpp"
#include "Managers/Rumble.hpp"

using namespace GTS;


namespace GTS_Markers {

    static NiPoint3 Mul(const RE::NiMatrix3& R, const NiPoint3& v) {
        return 
    	{
            R.entry[0][0] * v.x + R.entry[0][1] * v.y + R.entry[0][2] * v.z,
            R.entry[1][0] * v.x + R.entry[1][1] * v.y + R.entry[1][2] * v.z,
            R.entry[2][0] * v.x + R.entry[2][1] * v.y + R.entry[2][2] * v.z
        };
    }

    static float YawRad_FromMatrix_FwdY_ZUp(const RE::NiMatrix3& R){
        // forward = R * (0,1,0) => (R01, R11, R21)
        return std::atan2(R.entry[0][1], R.entry[1][1]);
    }


    std::tuple<NiPoint3, float> GetClosestMarkerWorld(RE::TESObjectREFR* a_user, RE::TESObjectREFR* a_furn) {

        if (!a_user || !a_furn) {
            return { {}, 0.0f };
        }

        auto root = a_furn->Get3D();
        if (!root) {
            return { {}, 0.0f };
        }

        static constexpr std::array<const char* const, 3> names = { "FURN", "FRN", "FurnitureMarker" };

        const NiPoint3 userPos = a_user->GetPosition();

        float bestDist = std::numeric_limits<float>::max();
        NiPoint3 bestWorldPos{};
        float bestWorldYaw = 0.0f;

        for (auto name : names) {
            VisitExtraData<RE::BSFurnitureMarkerNode>(root, name, [&](NiAVObject& ownerNode, RE::BSFurnitureMarkerNode& data) {
                for (auto& marker : data.markers) {

                    const NiPoint3 worldPos = ownerNode.world.translate + Mul(ownerNode.world.rotate, (marker.offset * a_furn->GetScale())); // start with no scale

                    const float d = userPos.GetDistance(worldPos);
                    if (d < bestDist) {
                        bestDist = d;
                        bestWorldPos = worldPos;
                        const float nodeYaw = YawRad_FromMatrix_FwdY_ZUp(ownerNode.world.rotate);
                        bestWorldYaw = nodeYaw + marker.heading; // radians
                    }
                }
                return true;
            });
        }

        if (bestDist == std::numeric_limits<float>::max()) {
            return { {}, 0.0f };
        }

        return { bestWorldPos, bestWorldYaw };
    }
}

namespace GTS_Hitboxes {

    void ApplySitDamage_Loop(Actor* giant) {
        float damage = GetButtCrushDamage(giant);
        for (auto Nodes: Butt_Zones) {
            auto Butt = find_node(giant, Nodes);
            if (Butt) {
                DoDamageAtPoint(giant, Radius_ButtCrush_Sit, Damage_ButtCrush_Idle * damage, Butt, 1000, 0.05f, 3.0f, DamageSource::Booty);
            }
        }
    } 

    void ApplySitDamage_Once(Actor* giant) {
        float damage = GetButtCrushDamage(giant);
        float perk = GetPerkBonus_Basics(giant);
        bool SMT = TinyCalamityActive(giant);

		float dust = SMT ? 1.25f : 1.0f;
		float smt = SMT ? 1.5f : 1.0f;

        float shake_power = Rumble_ButtCrush_Sit/2.0f * dust * damage;
        for (auto Nodes: Butt_Zones) {
            auto Butt = find_node(giant, Nodes);
            if (Butt) {
                DoDamageAtPoint(giant, Radius_ButtCrush_Sit, Damage_ButtCrush_Sit * damage, Butt, 4, 0.70f, 0.8f, DamageSource::Booty);
                Rumbling::Once("Butt_L", giant, shake_power * smt, 0.075f, "NPC R Butt", 0.0f);
                LaunchActor::LaunchAtNode(giant, 1.3f * perk, 2.0f, Butt);
            }
        }
    } 

    void StartLoopDamage(Actor* actor) {

        std::string taskname = std::format("SitLoop_{}", actor->formID);
        ActorHandle giantHandle = actor->CreateRefHandle();

        TaskManager::Run(taskname, [=](auto& progressData){
            if (!giantHandle) {
                return false;
            }

            auto giant = giantHandle.get().get();

            if (!giant) {
                return false;
            }

            if (!giant->Is3DLoaded() || giant->IsDead() || GetAV(giant, ActorValue::kHealth) <= 0.0f) {
                return false;
            }

            ApplySitDamage_Loop(giant);

            return true;
        });
    }
}

namespace GTS {

	std::string FurnitureManager::DebugName() {
		return "::FurnitureManager";
	}



    void FurnitureManager::FurnitureEvent(RE::Actor* activator, TESObjectREFR* object, bool enter) {
        if (activator) {
            if (ValidActor(activator)) {
                RecordAndHandleFurnState(activator, object, enter);
            }
            else if (!enter) {
                ResetTrackedFurniture(activator);
            }
        }
    }


    void FurnitureManager::ActorLoaded(RE::Actor* actor) {

		//Check if the actor is in furniture when loaded.
		//else reset the tracked furn state.
        //Its sometimes possible for the exit event to not fire.
        //This corrects it on actor load.

        if (actor) {
            if (AIProcess* aiProcess = actor->GetActorRuntimeData().currentProcess) {
                if (ObjectRefHandle handle = aiProcess->GetOccupiedFurniture()) {
                   if (!handle) ResetTrackedFurniture(actor);
                   else{
                       FurnitureEvent(actor, handle.get().get(), true);
                   }
                }
            }
        }
    }

    //Fallback check
    void FurnitureManager::ActorUpdate(RE::Actor* actor) {
        if (actor) {
            if (ValidActor(actor)) {
                if (AIProcess* aiProcess = actor->GetActorRuntimeData().currentProcess) {
                    ObjectRefHandle handle = aiProcess->GetOccupiedFurniture();
                    if (!handle) ResetTrackedFurniture(actor);
                }
            }
        }
    }


    // If the scale keywords is removed from the default object, through an esp/patch, the game will calculate where
    // the actor should be placed however it does this before this event fires.
    // So if this system is used as is, the actor will be offset incorrectly.
    // To counter this we can simply reposition the actor to the furns pos and rotation.
	// This "fix" happens only after the "enter" anim completes but its better than nothing.
    void FurnitureManager::RecordAndHandleFurnState(RE::Actor* activator, TESObjectREFR* object, bool enter) {

        if (object) {
			//Almost all "invisible" furns have "Marker" in their name, skip these.
			//easiest way to avoid scaling invisible furnitures.
			//it would be better if the actual model was checked to see if it contains visible geometry.
			//but i don't know how to do that.

            if (!ValidFurn(object)) return;

            if (enter) {

                auto data = Transient::GetActorData(activator);

                auto [markerWorldPos, headingDeg] = GTS_Markers::GetClosestMarkerWorld(activator, object);
				//if not (0,0,0)
                if (markerWorldPos.x || markerWorldPos.y || markerWorldPos.z) {

                    if (!activator->Is3DLoaded() || activator->IsDead()) {
                        return;
                    }

                    //Works but appears to not be the cause.
                    //if (auto x = skyrim_cast<hkpRigidBody*>(object->Get3D()->GetCollisionObject()->GetRigidBody()->referencedObject.get())) {
                    //    x->collidable.broadPhaseHandle.collisionFilterInfo |= 1 << 14;
                    //    x->world->UpdateCollisionFilterOnEntity(x, hkpUpdateCollisionFilterOnEntityMode::kFullCheck, hkpUpdateCollectionFilterMode::kIncludeCollections);
                    //}

                    activator->SetRotationZ(headingDeg);
                    NiPoint3 pos = markerWorldPos;

                    if (DebugDraw::CanDraw()) {
                        DebugDraw::DrawSphere({ pos.x, pos.y, pos.z }, 5.0f, 10000);
                    }

                	pos.z = activator->GetPosition().z;
                    activator->SetPosition(pos, true);

                    if (data) {
                        data->fRecordedFurnScale = object->GetScale() / get_natural_scale(activator, true);
                        data->bIsUsingFurniture = true;
                        //Bad fix. We rely on a timer + the movement delta hook to outright prevent the actor from moving.
                        //The move is probably animation related.
                        data->BlockMovementTimer.UpdateDelta(0.5f);
                        data->BlockMovementTimer.ResetGate();
                    }
                }
            }
        }
    }

    void FurnitureManager::Furniture_EnableButtHitboxes(RE::Actor* activator, FurnitureDamageSwitch type) {
        GTS_Hitboxes::ApplySitDamage_Once(activator);
        GTS_Hitboxes::StartLoopDamage(activator);

        if (type == FurnitureDamageSwitch::DisableDamage) {
            std::string taskname = std::format("SitLoop_{}", activator->formID);
            TaskManager::Cancel(taskname);
        }
    }

    void FurnitureManager::ResetTrackedFurniture(RE::Actor* actor) {
        auto data = Transient::GetActorData(actor);
        if (data) {
            data->fRecordedFurnScale = 1.0f;
            data->bIsUsingFurniture = false;
            data->BlockMovementTimer.StopGate();
        }

	}

    bool FurnitureManager::ValidActor(Actor* a_actor) {

        if (a_actor) {
            if ((a_actor->IsPlayerRef() && Config::General.bDynamicFurnSizePlayer) || (IsTeammate(a_actor) && Config::General.bDynamicFurnSizeFollowers)) {
                if (IsHuman(a_actor)) {
                    return true;
                }
            }
        }
        return false;
	}

    bool FurnitureManager::ValidFurn(TESObjectREFR* a_obj) {

        if (a_obj){            
        	std::string name = a_obj->GetName();
			if (!name.empty()) {
                name = str_tolower(name);
                if (!name.contains("marker")) {
                    return true;
                }
			}
		}
        return false;
	}
}
