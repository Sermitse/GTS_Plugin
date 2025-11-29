#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CrawlUtils.hpp"
#include "Managers/Damage/LaunchActor.hpp"
#include "Managers/FurnitureManager.hpp"

#include "Config/Config.hpp"

#include "Utils/Actions/ButtCrushUtils.hpp"
#include "Managers/Rumble.hpp"

using namespace GTS;


namespace GTS_Markers {

    void GetFurnitureMarkerAnimations(RE::TESObjectREFR* ref) {

        if (!ref) {
            return;
        }

        auto root = ref->Get3D();
        if (!root) {
            return;
        }

        static constexpr std::array<const char*, 3> names = { "FURN", "FRN", "FurnitureMarker" };

        for (auto name : names) {
            VisitExtraData<RE::BSFurnitureMarkerNode>(root, name, [](NiAVObject& node, RE::BSFurnitureMarkerNode& data) {
                for (auto& marker : data.markers) {
                    auto animType = marker.animationType.get();
                    switch (animType) {
                    case RE::BSFurnitureMarker::AnimationType::kSit:
                        log::info("Marker: Sit");
                        break;
                    case RE::BSFurnitureMarker::AnimationType::kSleep:
                        log::info("Marker: Sleep");
                        break;
                    case RE::BSFurnitureMarker::AnimationType::kLean:
                        log::info("Marker: Lean");
                        break;
                    default:
                        log::info("Marker: Unknown ({})", static_cast<int>(animType));
                        break;
                    }
                }
                return true;
            });
        }
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
        bool SMT = HasSMT(giant);

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
        if (activator && object) {
            //GTS_Markers::GetFurnitureMarkerAnimations(object);
            RecordAndHandleFurnState(activator, object, enter);
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
            std::string name = object->GetName();
			if (name.empty()) return;

			name = str_tolower(name);
            if (name.contains("marker")) return;

			//Swiched to persistent, so on load we can restore the scale if an npc was saved when in furniture.
            auto data = Persistent::GetActorData(activator);
            if (data) {
                data->fRecordedFurnScale = object->GetScale() / get_natural_scale(activator, true);
                data->bIsUsingFurniture = enter;
            }

            if (enter) {
                activator->SetRotationX(object->GetAngleX());
                activator->SetPosition(object->GetPosition(), true);
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
}