#include "Managers/Size_Killmoves/SizeKillMove_Calamity.hpp"
#include "Managers/Size_Killmoves/KillMoveHelper.hpp"
#include "Managers/Damage/Utils/SizeDamageUtils.hpp"
#include "Systems/Rays/Camera/CameraCollision.hpp"
#include "Managers/Damage/CollisionDamage.hpp"
#include "Managers/Cameras/CamUtil.hpp"
#include "Managers/GTSSizeManager.hpp"
#include "Utils/DifficultyUtils.hpp"
#include "Managers/HighHeel.hpp"
#include "Config/Config.hpp"

using namespace GTS;
using namespace Calamity;
namespace Calamity {

    // ---------------------------------------------------------------------
    // actor/node lookups - specific to this sequence (everything generic
    // - stage timers, blends, impact hit-stop - now lives in KillMoveHelper)
    // ---------------------------------------------------------------------

    float VictimScale() { return _victim ? get_visual_scale(_victim) : 1.0f; }
    float GiantScale()  { return _giant  ? get_visual_scale(_giant)  : 1.0f; }

    RE::NiPoint3 VictimHeadPos() {
        if (_victim) {
            if (auto head = find_node(_victim, "NPC Head [Head]")) {
                return head->world.translate;
            }
            return _victim->GetPosition();
        }
        return _cam.cameraPos;
    }

    RE::NiMatrix3 VictimHeadRot() {
        if (_victim) {
            if (auto head = find_node(_victim, "NPC Head [Head]")) {
                return head->world.rotate;
            }
        }
        return RE::NiMatrix3();
    }

    RE::NiPoint3 GiantHeadPos() {
        if (_giant) {
            if (auto head = find_node(_giant, "NPC Head [Head]")) {
                return head->world.translate;
            }
            return _giant->GetPosition();
        }
        return _cam.cameraPos;
    }

    float GiantHeelOffset() {
        return ApplyHeelOffset(_settings.GiantNodeIsFoot);
    }

    RE::NiAVObject* ResolveGiantNode() {
        return _giantNode; // nullptr -> fall back to GiantHeadPos() (see GiantNodeOrHeadPos)
    }

    // -----------------------------------------------------------------------
    // public helpers
    // -----------------------------------------------------------------------

    // The live eye anchor: sits just in front of and slightly above the victim's
    // head node, scaled by the VICTIM's own visual scale so it shrinks together
    // with them and stays glued to their face regardless of size or animation.
    RE::NiPoint3 VictimEyePos() {
        RE::NiPoint3 headPos = VictimHeadPos();
        RE::NiMatrix3 headRot = VictimHeadRot();
        RE::NiPoint3 forward = headRot * RE::NiPoint3(0.f, 1.f, 0.f);
        RE::NiPoint3 up(0.f, 0.f, 1.f);

        float scale = VictimScale();
        return headPos
            + forward * (_settings.EyeForwardOffset * scale)
            + up * (_settings.EyeHeightOffset * scale);
    }

    RE::NiPoint3 GiantNodeOrHeadPos() {
        if (auto node = ResolveGiantNode()) {
            RE::NiPoint3 pos = node->world.translate;
            // Skeleton bones don't move when heels go on - HighHeelManager's offset
            // is purely visual, so correct for it here once (same reasoning as the
            // regular sequence's NodeOrHeadPos).
            pos.z -= GiantHeelOffset();
            return pos;
        }
        return GiantHeadPos();
    }

    // Kicks off the LookUpAtGiant -> LookAtGiantNode hand-off. Position never
    // changes here (the camera stays pinned to the victim's eyes throughout);
    // only the look direction blends from the captured rotation onto a live
    // look-at built each frame in UpdateLookAtGiantNode.
    void TransitionToLookAtGiantNode(float blendTime) {
        _cam.rotBlendFrom = _cam.cameraRot;
        BeginBlend(_cam, blendTime);
        EnterStage(_cam, _state, TinyPOVState::LookAtGiantNode);
    }

    // ---------------------------------------------------------------------
    // stage updates
    // ---------------------------------------------------------------------

    void UpdateDiveToEye(float dt) {
        float t = AdvanceStageTimer(_cam, dt, _settings.DiveToEyeTime);
        float eased = Ease(t);

        // The eye anchor itself is re-sampled live (rather than frozen at the
        // t=0 position) since the victim may already be moving/being lifted
        // the instant the sequence starts.
        RE::NiPoint3 eyeTarget = VictimEyePos();
        _cam.cameraPos = Lerp(_cam.stageFromPos, eyeTarget, eased);
        _cam.cameraRot = BuildLookAt(_cam.cameraPos, GiantHeadPos());

        Time::SGTM(1.0f - (1.0f - _settings.SlowMoTarget) * t);

        if (t >= 1.0f) {
            EnterStage(_cam, _state, TinyPOVState::LookUpAtGiant);
        }
    }

    void UpdateLookUpAtGiant(float dt) {
        float t = AdvanceStageTimer(_cam, dt, _settings.LookUpAtGiantTime);

        _cam.cameraPos = VictimEyePos(); // camera stays glued to the victim's eyes
        _cam.cameraRot = BuildLookAt(_cam.cameraPos, GiantHeadPos());

        // Early-out: if a tracked giant node (e.g. a hand reaching down) has
        // already closed in on the victim, cut over immediately instead of
        // waiting out the fixed timer.
        RE::NiPoint3 node = GiantNodeOrHeadPos();
        float proximityRadius = _settings.GiantNodeProximityRadius * GiantScale();
        bool nodeIsClose = (node - _cam.cameraPos).Length() <= proximityRadius;

        if (nodeIsClose) {
            TransitionToLookAtGiantNode(_settings.ProximityBlendTime);
            return;
        }

        if (t >= 1.0f) {
            TransitionToLookAtGiantNode(_settings.FaceToNodeBlendTime);
        }
    }

    void UpdateLookAtGiantNode(float dt) {
        AdvanceStageTimer(_cam, dt, _settings.LookAtGiantNodeMaxWait); // duration only matters for the timeout check below

        _cam.cameraPos = VictimEyePos(); // still glued to the victim's eyes right up until death
        RE::NiPoint3 node = GiantNodeOrHeadPos();

        float bt = AdvanceBlend(_cam, dt);
        RE::NiMatrix3 liveTarget = BuildLookAt(_cam.cameraPos, node);
        _cam.cameraRot = SlerpMatrix(_cam.rotBlendFrom, liveTarget, bt);

        bool dead = _victim && _victim->IsDead();
        bool timedOut = _cam.timer >= _settings.LookAtGiantNodeMaxWait;

        if (dead) {
            TriggerImpactSlowMo(_cam);
            EnterStage(_cam, _state, TinyPOVState::ImpactShake);
        } else if (timedOut) {
            // Nothing actually happened (e.g. the victim broke free) - skip the
            // impact shake and just ease back out.
            EnterStage(_cam, _state, TinyPOVState::ReturnCamera);
        }
    }

    void UpdateImpactShake(float dt) {
        float t = AdvanceStageTimer(_cam, dt, _settings.ImpactShakeTime);

        // Freeze the anchor at the position/rotation captured the instant the
        // victim died, rather than continuing to sample VictimEyePos() - once
        // the body ragdolls the head node can fly off unpredictably, which
        // would otherwise throw the camera around with it.
        float decay = 1.0f - Ease(t);
        float scale = VictimScale();

        RE::NiPoint3 right = RE::NiPoint3(_cam.stageFromRot.entry[0][0], _cam.stageFromRot.entry[1][0], _cam.stageFromRot.entry[2][0]);
        RE::NiPoint3 up     = RE::NiPoint3(_cam.stageFromRot.entry[0][2], _cam.stageFromRot.entry[1][2], _cam.stageFromRot.entry[2][2]);

        float wobble = std::sin(_cam.timer * _settings.ImpactShakeFrequency);
        RE::NiPoint3 shakeOffset = (right * wobble + up * std::cos(_cam.timer * _settings.ImpactShakeFrequency * 0.7f))
            * (_settings.ImpactShakeMagnitude * scale * decay);

        _cam.cameraPos = _cam.stageFromPos + shakeOffset;
        _cam.cameraRot = _cam.stageFromRot;

        if (t >= 1.0f) {
            EnterStage(_cam, _state, TinyPOVState::ReturnCamera);
        }
    }

    void UpdateReturnCamera(float dt) {
        float t = AdvanceStageTimer(_cam, dt, _settings.ReturnTime);
        float eased = Ease(t);
        _cam.cameraPos = Lerp(_cam.stageFromPos, _cam.startPos, eased);

        // Slerp straight toward the exact captured start rotation rather than
        // continuously re-aiming, to avoid a snap once t hits 1 (same fix as
        // the regular sequence's ReturnCamera stage).
        _cam.cameraRot = SlerpMatrix(_cam.stageFromRot, _cam.startRot, eased);

        Time::SGTM(_settings.SlowMoTarget + (1.0f - _settings.SlowMoTarget) * t);

        if (t >= 1.0f) {
            _cam.cameraPos = _cam.startPos;
            _cam.cameraRot = _cam.startRot;
            Time::SGTM(1.0f);

            _victim = nullptr;
            _giant = nullptr;
            _giantNode = nullptr;
            _cam.active = false;
            _state = TinyPOVState::None;
        }
    }
}
namespace GTS {
    void StartCalamityKillmove(RE::Actor* giant, RE::Actor* victim, RE::NiAVObject* giantLookNode, DamageSource Cause, float base_damage, float crush_mult, bool isFootNode, bool TinyCalamity) {
        if (!giant) {
            return;
        }
        if (!giant->IsPlayerRef()) {
            return;
        }
        if (!victim) {
            return;
        }
        if (!ShouldTrigger(PlayerCharacter::GetSingleton(), victim, Cause, base_damage, crush_mult)) {
            logger::info("Can't start Calamity killmove");
            return;
        }

        auto camera = RE::PlayerCamera::GetSingleton();
        auto root = camera->cameraRoot.get();

        _cam.startPos = root->world.translate;
        _cam.startRot = root->world.rotate;
        _cam.cameraPos = _cam.startPos;
        _cam.cameraRot = _cam.startRot;
        _cam.active = true;
        _cam.blendTimer = 0.0f;
        _cam.blendDuration = 0.0f;
        _cam.impactActive = false;
        _cam.impactTimer = 0.0f;

        _giant = giant;
        _victim = victim;
        _giantNode = giantLookNode;
        _settings.GiantNodeIsFoot = isFootNode;

        _cam.stageFromPos = _cam.startPos; // DiveToEye blends from here
        EnterStage(_cam, _state, TinyPOVState::DiveToEye);
        // EnterStage overwrote stageFromPos with cameraPos (== startPos here anyway),
        // so this is just being explicit about the intent for future edits.
    }
    void UpdateFakeCalamityKillmove() {
        if (!_cam.active) {
            return;
        }

        // Safety net: if the player's killmove animation stops being "busy"
        // for any reason (interrupted, cancelled, ended early) while we're
        // still in the victim's POV, don't keep the camera pinned there -
        // ease back out immediately instead of waiting for a stage-specific
        // timeout to eventually notice.
        if (_state != TinyPOVState::None && _state != TinyPOVState::ReturnCamera && !IsPlayerAnimBusy()) {
            EnterStage(_cam, _state, TinyPOVState::ReturnCamera);
        }

        float dt = Time::WorldTimeDelta() * GetAnimationSlowdown(PlayerCharacter::GetSingleton());

        switch (_state) {
            case TinyPOVState::DiveToEye:       UpdateDiveToEye(dt);       break;
            case TinyPOVState::LookUpAtGiant:   UpdateLookUpAtGiant(dt);   break;
            case TinyPOVState::LookAtGiantNode: UpdateLookAtGiantNode(dt); break;
            case TinyPOVState::ImpactShake:     UpdateImpactShake(dt);     break;
            case TinyPOVState::ReturnCamera:    UpdateReturnCamera(dt);    break;
            default: break;
        }

        // Applied last so the impact hit-stop overrides whatever SGTM value the
        // stage above just set, for as long as it's still running.
        ApplyImpactSlowMo(_cam, dt, _settings.SlowMoTarget, _settings.ImpactSlowMoExtra, _settings.ImpactSlowMoInTime, _settings.ImpactSlowMoTime);
    }

    bool UpdateCalamityKillMove() {
        if (_state == TinyPOVState::None) {
            return false;
        }

        // _giantNode is optional (nullptr -> focus on the giant's face), so the
        // collision ray has to fall back to GiantNodeOrHeadPos() rather than
        // dereferencing _giantNode directly.
        RE::NiPoint3 rayStart = GiantNodeOrHeadPos();
        if (!DriveCameraWithCollision(_cam, rayStart)) {
            return false;
        }
        UpdateFakeCalamityKillmove();
        return true;
    }

    void RecordCalamityStartingPosition() {
        RecordStartingPosition(_cam, _state, TinyPOVState::None);
    }
}