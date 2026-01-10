#include "Managers/Animation/GrabUtils.hpp"
#include "Managers/Animation/Grab.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/TurnTowards.hpp"
#include "Managers/Animation/AnimationManager.hpp"

#include "Magic/Effects/Common.hpp"

#include "Utils/AttachPoint.hpp"

using namespace GTS;
using namespace std;


namespace {
    bool ShouldAbortGrab(Actor* giantref, Actor* tinyref, bool CanCancel, bool Dead, bool small_size) {
        if (CanCancel) {
            if (Dead || GetAV(tinyref, ActorValue::kHealth) <= 0.0f 
                || small_size
                || (!IsBetweenBreasts(tinyref) 
                && GetAV(giantref, ActorValue::kStamina) < 2.0f)) {

                log::info("grab task cancelled");
                // For debugging

                PushActorAway(giantref, tinyref, 1.0f);
                Grab::CancelGrab(giantref, tinyref);

                return true;
            }
        }
        return false;
    }

    bool ManageGrabPlayAttachment(Actor* giantref, Actor* tinyref) {
        auto TargetBone = Attachment_GetTargetNode(giantref);
        std::string_view node_lookup;

        

        switch (TargetBone) {
            case AttachToNode::ObjectL: {
                node_lookup = "AnimObjectL"; break;
            }
            case AttachToNode::ObjectR: {
                node_lookup = "AnimObjectR"; break;
            }
            case AttachToNode::ObjectA: {
                node_lookup = "AnimObjectA"; break;
            }
            case AttachToNode::ObjectB: {
                node_lookup = "AnimObjectB"; break;
            }
            case AttachToNode::None: {
                node_lookup = "AnimObjectL"; break;
            }
            default: return false;
        }

        NiAVObject* Object = find_node(giantref, node_lookup);

        if (Object) {
            NiPoint3 coords = Object->world.translate;
            
            if (tinyref->formID != 0x14) {
                FaceSame(giantref, tinyref);
            }

            if (TransientActorData* Transient = Transient::GetActorData(giantref)) {
                if (Transient->KissVoring) {
                    const auto Offset = Config::Gameplay.ActionSettings.fGrabPlayVoreOffset_Z;
                    float offset = (0.6f + Offset) * get_visual_scale(giantref);
                    coords.z += offset;
                }
            }

            if (!AttachTo(giantref, tinyref, coords)) {
                Grab::CancelGrab(giantref, tinyref);
                //AnimationManager::StartAnim("GTS_HS_Exit_NoTiny", giantref); Doesn;t work
                return false;
            }
            return true;
        }

        return false;
    }

    void SetReattachingState(Actor* giant, bool Reattach) {
        auto data = Transient::GetActorData(giant);
        if (data) {
            data->ReattachingTiny = Reattach;
        }
    }

    void ReattachTinyTask(Actor* giant, Actor* tiny, bool Dead) {
        if (!Dead) {
            // Sometimes Tiny still exists and just unloaded, so we move Tiny to us in that case as well
            log::info("Moving tiny to giant");
            SetReattachingState(giant, true);
            tiny->MoveTo(giant);

            double Start = Time::WorldTimeElapsed();
            std::string name = std::format("Reattach_{}", tiny->formID);
            ActorHandle gianthandle = giant->CreateRefHandle();
            ActorHandle tinyhandle = tiny->CreateRefHandle();
            TaskManager::Run(name, [=](auto& progressData) {
                if (!gianthandle) {
                    return false;
                }
                if (!tinyhandle) {
                    SetReattachingState(gianthandle.get().get(), false);
                    return false;
                }
                auto giantref = gianthandle.get().get();
                auto tinyref = tinyhandle.get().get();

                double Finish = Time::WorldTimeElapsed();
                double timepassed = Finish - Start;
                if (timepassed > 0.25) {
                    tinyref->MoveTo(giantref);
                    DisableCollisions(tinyref, giantref);
                    if (IsBetweenBreasts(tinyref) && !IsGtsBusy(tinyref)) {
                        if (IsHostile(giantref, tinyref)) {
                            AnimationManager::StartAnim("Breasts_Idle_Unwilling", tinyref);
                        } else {
                            AnimationManager::StartAnim("Breasts_Idle_Willing", tinyref);
                        }
                    }
                    if (!tinyref->Is3DLoaded()) {

                    }
                    SetReattachingState(giantref, false);
                    if (timepassed > 1.25) {
                        return false;
                    }
                }
                return true;
            });
        }
    }
}

namespace GTS {
    bool IsCurrentlyReattaching(Actor* giant) { // Sometimes Tiny is still grabbed and we need to update Tiny pos so Tiny becomes visible
        // Works in such cases like Changing locations/going between loading screens with Tiny grabbed
        bool Attaching = false;
        auto data = Transient::GetActorData(giant);
        if (data) {
            Attaching = data->ReattachingTiny;
        }
        return Attaching;
    }

    bool HandleGrabLogic(Actor* giantref, Actor* tinyref, ActorHandle gianthandle, ActorHandle tinyhandle) {
        float sizedifference = get_scale_difference(giantref, tinyref, SizeType::VisualScale, true, false);

        ForceRagdoll(tinyref, false); 

        for (auto muted: {giantref, tinyref}) { // Try to stfu actors
            ShutUp(muted);
        }

        bool Attacking = AnimationVars::Grab::GetIsGrabAttacking(giantref);

        bool Dead = (giantref->IsDead() || tinyref->IsDead() || GetAV(tinyref, ActorValue::kHealth) <= 0.0f);
        bool CanCancel = (Dead || !IsVoring(giantref)) && (!Attacking || IsBeingEaten(tinyref));
        bool small_size = sizedifference < Action_Grab;

        if (ShouldAbortGrab(giantref, tinyref, CanCancel, Dead, small_size)) {
            return false;
        }
        // Switch to always using Grab Play logic in that case, we need it since it doesn't cancel properly without it
        if (AnimationVars::Action::GetIsInGrabPlayState(giantref)) {
            return ManageGrabPlayAttachment(giantref, tinyref);
        }

        if (IsBeingEaten(tinyref) && !IsBetweenBreasts(tinyref) && !IsInCleavageState(giantref)) {
            if (!AttachToObjectA(gianthandle, tinyhandle)) {
                // Unable to attach
                log::info("Can't attach to ObjectA");
                Grab::CancelGrab(giantref, tinyref);
                return false;
            }
        } else if (IsBetweenBreasts(tinyref)) {
            bool hostile = IsHostile(giantref, tinyref);
            float restore = 0.04f * TimeScale();
            if (!hostile) {
                tinyref->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kHealth, restore);
                tinyref->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kStamina, restore);
            }

            RestoreBreastAttachmentState(giantref, tinyref); // If someone suddenly ragdolls us during breast anims
            Anims_FixAnimationDesync(giantref, tinyref, false);

            float AnimSpeed_GTS = AnimationManager::GetAnimSpeed(giantref);
            float AnimSpeed_Tiny = AnimationManager::GetAnimSpeed(tinyref);

            if (Attachment_GetTargetNode(giantref) == AttachToNode::ObjectL) {
                auto ObjectL = find_node(giantref, "AnimObjectL");
                if (ObjectL) {
                    NiPoint3 coords = ObjectL->world.translate;

                    if (!AttachTo(giantref, tinyref, coords)) {
                        Grab::CancelGrab(giantref, tinyref);
                        return false;
                    }
                }
                return true;
            } else if (Attachment_GetTargetNode(giantref) == AttachToNode::ObjectB) { // Used in Cleavage state
                if (DebugDraw::CanDraw()) {
                    auto node = find_node(tinyref, "NPC Root [Root]");
                    if (node) {
                        NiPoint3 point = node->world.translate;
                        
                        DebugDraw::DrawSphere(glm::vec3(point.x, point.y, point.z), 6.0f, 40, {0.0f, 1.0f, 0.0f, 1.0f});
                    }
                }

                if (AnimationVars::Cleavage::GetIsBoobsDoting(giantref) && 
                    (small_size || Dead) && 
                    !AnimationVars::Cleavage::GetIsExitingStrangle(giantref)) {// If size is too small 
                    AnimationManager::StartAnim("Cleavage_DOT_Stop", giantref);
                }

                if (!AttachToObjectB(gianthandle, tinyhandle)) { // Attach to ObjectB non stop
                    Grab::CancelGrab(giantref, tinyref);
                    return false;
                }
                return true;
            }
            
            if (hostile) {
                DamageAV(tinyref, ActorValue::kStamina, restore * 2);
            }
            if (!AttachToCleavage(gianthandle, tinyhandle)) {
                // Unable to attach
                Grab::CancelGrab(giantref, tinyref);
                log::info("Can't attach to Cleavage");
                return false;
            }
        } else if (AttachToHand(gianthandle, tinyhandle)) {
            GrabStaminaDrain(giantref, tinyref, sizedifference);
            return true;
        } else {
            if (!AttachToHand(gianthandle, tinyhandle)) {
                // Unable to attach
                Grab::CancelGrab(giantref, tinyref);
                log::info("Can't attach to hand");
                return false;
            }
        }
        return true;
    }

    void ReattachTiny(Actor* giant, Actor* tiny) {
		auto HandNode = find_node(giant, "NPC L Hand [LHnd]");
		if (HandNode) {
			NiPoint3 GiantDist = HandNode->world.translate;
			NiPoint3 TinyDist = tiny->GetPosition();
			float distance = (GiantDist - TinyDist).Length();
			float reattach_dist = std::clamp(512.0f * get_visual_scale(giant), 512.0f, 4096.0f);
            bool Dead = (tiny->IsDead() || GetAV(tiny, ActorValue::kHealth) <= 0.0f);

            if (distance > reattach_dist) {
                ReattachTinyTask(giant, tiny, Dead);
            } else {
                TESWorldSpace* gts_space = giant->GetWorldspace();
                TESWorldSpace* tiny_space = tiny->GetWorldspace();

                if (gts_space && tiny_space) {
                    if (gts_space->formID != tiny_space->formID) {
                        ReattachTinyTask(giant, tiny, Dead);
                    }
                }
            }
        }
    }

    bool FailSafeAbort(Actor* giantref, Actor* tinyref) {
        if (AnimationVars::Grab::GetIsGrabAttacking(giantref)) { // Breast state also counts as attacking, so we reset only when NOT grab attacking/in breast state
            if (AnimationVars::Cleavage::GetIsBoobsDoting(giantref) && 
				!AnimationVars::Cleavage::GetIsExitingStrangle(giantref)) { // These checks are important so we don't spam StartAnim
                AnimationManager::StartAnim("Cleavage_DOT_Stop", giantref);
                return true; // True = try again, do not abort
            }
            return true; // Try again
        }
        Grab::FailSafeReset(giantref); // Reset anims only on Giant side
        return false; // End grab.cpp task
    }
}