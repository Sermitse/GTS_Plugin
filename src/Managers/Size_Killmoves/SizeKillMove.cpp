#include "Managers/Size_Killmoves/SizeKillMove.hpp"
#include "Managers/Size_Killmoves/KillMoveHelper.hpp"
#include "Managers/Damage/Utils/SizeDamageUtils.hpp"
#include "Managers/Damage/CollisionDamage.hpp"
#include "Systems/Rays/Camera/CameraCollision.hpp"
#include "Managers/Cameras/CamUtil.hpp"
#include "Managers/GTSSizeManager.hpp"
#include "Utils/DifficultyUtils.hpp"
#include "Managers/HighHeel.hpp"
#include "Config/Config.hpp"

using namespace GTS;
namespace {

    // ---------------------------------------------------------------------
    // actor/node lookups - specific to this sequence (everything generic
    // - stage timers, blends, impact hit-stop - now lives in KillMoveHelper)
    // ---------------------------------------------------------------------

    float Scale() { return _enemy ? get_visual_scale(_enemy) : 1.0f; }

    RE::NiPoint3 HeadPos() {
        if (_enemy) {
            float bb = GetSizeFromBoundingBox(_enemy);
            RE::NiPoint3 pos = _enemy->GetPosition();
            if (auto head = find_node(_enemy, "NPC Head [Head]")) {
                RE::NiPoint3 pos = head->world.translate;
                return pos;
            }
            pos.z += _settings.MoveUpwards * bb;
            return pos;
        }
        return _cam.cameraPos;
    }

    RE::NiPoint3 Forward3D() {
        if (_enemy) {
            if (auto root = _enemy->Get3D(false)) {
                RE::NiPoint3 forward = root->world.rotate * RE::NiPoint3(0.f, 1.f, 0.f);
                forward.Unitize();
                return forward;
            }
        }

        return RE::NiPoint3(0.f, 1.f, 0.f);
    }
    RE::NiPoint3 CameraAnchor() {
        float scale = GetSizeFromBoundingBox(_enemy);
        float moveFrom = _settings.MoveFromEnemyOffset * std::clamp(get_visual_scale(_enemy), 0.4f, 1.0f);
        float distance = (_settings.MoveToEnemyDistance + moveFrom) * scale;
        float rise = _settings.RiseHeight * _settings.MoveToEnemyRiseFrac * scale;

        return HeadPos() + Forward3D() * distance + RE::NiPoint3(0.f, 0.f, 1.f) * rise;
    }
    // Position of the giant (the one doing the killing) used for the node
    // proximity check and for scaling world-space distances, mirroring how
    // DeathHoldDistance already scales off the player's visual scale below.
    RE::NiPoint3 GiantPos() {
        auto player = RE::PlayerCharacter::GetSingleton();
        return player ? player->GetPosition() : _cam.cameraPos;
    }

    float GiantScale() {
        auto player = RE::PlayerCharacter::GetSingleton();
        return player ? get_visual_scale(player) : 1.0f;
    }

    RE::NiAVObject* ResolveLookNode() {
        if (_node) {
            return _node;
        }
        return _enemy ? find_node(_enemy, _settings.DefaultLookNodeName) : nullptr;
    }

    // Kicks off the LookAtFace -> LookAtNode hand-off. lookDown selects the
    // proximity-triggered framing (camera lifts above the node and looks
    // down at it) vs. the plain timed cut (camera holds still, only the
    // look direction blends).
    void TransitionToLookAtNode(const RE::NiPoint3& node, bool lookDown, float blendTime) {
        _nodeLookDown = lookDown;
        _cam.rotBlendFrom = _cam.cameraRot;

        if (lookDown) {
            float giantScale = GiantScale();
            RE::NiPoint3 lateral = _cam.cameraPos - node;
            lateral.z = 0.f;
            if (lateral.Length() < 1e-3f) {
                lateral = RE::NiPoint3(0.f, -1.f, 0.f); // fallback if we're already dead-on above the node
            }
            // node is already heel-corrected (see NodeOrHeadPos), so this only needs
            // the configured look-down lift - no separate heel math here.
            _nodeEyeTarget = node + lateral + RE::NiPoint3(0.f, 0.f, 1.f) * (_settings.NodeLookDownHeight * giantScale);
        } else {
            _nodeEyeTarget = _cam.cameraPos; // hold position, only the look direction blends
        }

        BeginBlend(_cam, blendTime);
        EnterStage(_cam, _state, FakeKillmoveState::LookAtNode);
    }

    // ---------------------------------------------------------------------
    // stage updates
    // ---------------------------------------------------------------------

    void UpdateMoveToEnemy(float dt) {
        float t = AdvanceStageTimer(_cam, dt, _settings.MoveToEnemyTime);
        float eased = Ease(t);

        _cam.stageToPos = CameraAnchor();
        _cam.cameraPos = Lerp(_cam.stageFromPos, _cam.stageToPos, eased);

        // Bow the path upward at the midpoint so the camera arcs above the
        // enemy instead of cutting a straight line through their model.
        float arc = std::sin(Clamp01(t) * 3.14159265f) * (_settings.MoveToEnemyArcHeight * Scale());
        _cam.cameraPos.z += arc;
        
        _cam.cameraRot = BuildLookAt(_cam.cameraPos, HeadPos());

        Time::SGTM(1.0f - (1.0f - _settings.SlowMoTarget) * t);

        if (t >= 1.0f) {
            // Finish rising the remaining portion of RiseHeight (stage 1 already
            // banked MoveToEnemyRiseFrac of it in the approach point).
            float remainingRise = _settings.RiseHeight * (1.0f - _settings.MoveToEnemyRiseFrac) * Scale();
            _cam.stageToPos = _cam.cameraPos + RE::NiPoint3(0.f, 0.f, 1.f) * remainingRise;
            EnterStage(_cam, _state, FakeKillmoveState::RiseAboveEnemy);
        }
    }

    void UpdateRiseAboveEnemy(float dt) {
        float t = AdvanceStageTimer(_cam, dt, _settings.RiseTime);
        _cam.cameraPos = Lerp(_cam.stageFromPos, _cam.stageToPos, Ease(t));
        _cam.cameraRot = BuildLookAt(_cam.cameraPos, HeadPos());

        if (t >= 1.0f) {
            EnterStage(_cam, _state, FakeKillmoveState::LookAtFace);
        }
    }

    void UpdateLookAtFace(float dt) {
        float t = AdvanceStageTimer(_cam, dt, _settings.LookAtFaceTime);
        _cam.cameraRot = BuildLookAt(_cam.cameraPos, HeadPos());
        // Early-out: if the tracked node has already closed in on the giant,
        // cut over immediately instead of waiting out the fixed timer - this
        // is what stops the node from "swallowing" the enemy while the
        // camera is still staring at their old (pre-collision) position.
        RE::NiPoint3 node = NodeOrHeadPos();
        float proximityRadius = _settings.NodeProximityRadius * GiantScale();
        bool nodeIsClose = (node - GiantPos()).Length() <= proximityRadius;

        if (nodeIsClose) {
            TransitionToLookAtNode(node, true, _settings.ProximityBlendTime);
            return;
        }

        if (t >= 1.0f) {
            TransitionToLookAtNode(node, false, _settings.FaceToNodeBlendTime);
        }
    }

    void UpdateLookAtNode(float dt) {
        AdvanceStageTimer(_cam, dt, _settings.LookAtNodeMaxWait);  // duration only matters for the timeout check below

        RE::NiPoint3 node = NodeOrHeadPos();
        float bt = AdvanceBlend(_cam, dt);

        // Cross-fade both position and look direction from where LookAtFace
        // left off onto the live node target, instead of snapping onto it.
        _cam.cameraPos = Lerp(_cam.stageFromPos, _nodeEyeTarget, bt);

        // Keep the camera at a configurable distance from the target node.
        RE::NiPoint3 toCamera = _cam.cameraPos - node;
        if (toCamera.Length() < 1e-3f) {
            toCamera = RE::NiPoint3(1.f, 0.f, 0.f);
        }
        toCamera.Unitize();
        _cam.cameraPos = node + toCamera * (_settings.LookAtNodeDistance * get_visual_scale(PlayerCharacter::GetSingleton()));

        RE::NiMatrix3 liveTarget = BuildLookAt(_cam.cameraPos, node);
        _cam.cameraRot = SlerpMatrix(_cam.rotBlendFrom, liveTarget, bt);

        bool dead = _enemy && _enemy->IsDead();
        bool timedOut = _cam.timer >= _settings.LookAtNodeMaxWait;

        if (dead || timedOut) {
            if (dead) {
                TriggerImpactSlowMo(_cam);
            }

            RE::NiPoint3 outward = _cam.cameraPos - node;
            if (outward.Length() < 1e-3f) {
                outward = RE::NiPoint3(1.f, 0.f, 0.f);
            }
            outward.Unitize();

            RE::NiPoint3 side = outward.Cross(RE::NiPoint3(0.f, 0.f, 1.f));
            if (side.Length() < 1e-3f) {
                side = RE::NiPoint3(1.f, 0.f, 0.f);
            }
            side.Unitize();

            _cam.stageToPos = node + side * (_settings.DeathHoldDistance * get_visual_scale(PlayerCharacter::GetSingleton()));
            EnterStage(_cam, _state, FakeKillmoveState::DeathFlyOff);
        }
    }

    void UpdateDeathFlyOff(float dt) {
        float ggtm = Time::GGTM();
        _cam.timer += ggtm > 1e-4f ? dt / ggtm : dt;

        RE::NiPoint3 node = NodeOrHeadPos();
        float flyT = Clamp01(_cam.timer / std::max(_settings.DeathFlyOffTime, 0.01f));

        if (flyT < 1.0f || !_settings.OrbitEnabled) {
            _cam.cameraPos = Lerp(_cam.stageFromPos, _cam.stageToPos, Ease(flyT));
        } else {
            float orbitT = Clamp01((_cam.timer - _settings.DeathFlyOffTime) / std::max(_settings.OrbitTime, 0.01f));
            float angle = _settings.OrbitAngle * (3.14159265f / 180.0f) * Ease(orbitT);
            RE::NiPoint3 baseOffset = _cam.stageToPos - node;
            _cam.cameraPos = node + RotateAroundAxis(baseOffset, RE::NiPoint3(0.f, 0.f, 1.f), angle);
        }

        _cam.cameraRot = BuildLookAt(_cam.cameraPos, node);

        // Note: whether the player is still "busy" (i.e. the killmove animation
        // is still playing) is checked centrally in UpdateFakeKillmove(), which
        // will force us straight into ReturnCamera the moment it isn't - so
        // this stage only needs its own safety timeout.
        bool timedOut = _cam.timer >= (_settings.DeathFlyOffTime + _settings.OrbitTime + _settings.PostDeathMaxWait) * 4.0f;

        if (timedOut) {
            EnterStage(_cam, _state, FakeKillmoveState::ReturnCamera);
        }
    }

    void UpdateReturnCamera(float dt) {
        float t = AdvanceStageTimer(_cam, dt, _settings.ReturnTime);
        float eased = Ease(t);
        _cam.cameraPos = Lerp(_cam.stageFromPos, _cam.startPos, eased);

        // Slerp straight toward the exact captured start rotation, rather than
        // continuously re-aiming at the player's current look direction and
        // then hard-snapping to startRot once t hits 1 - that mismatch (a
        // live look-at target vs. a frozen final rotation) is what caused the
        // visible jerk right as the camera finished returning.
        _cam.cameraRot = SlerpMatrix(_cam.stageFromRot, _cam.startRot, eased);

        Time::SGTM(_settings.SlowMoTarget + (1.0f - _settings.SlowMoTarget) * t);

        if (t >= 1.0f) {
            _cam.cameraPos = _cam.startPos;
            _cam.cameraRot = _cam.startRot;
            Time::SGTM(1.0f);

            _enemy = nullptr;
            _node = nullptr;
            _cam.active = false;
            _state = FakeKillmoveState::None;
        }
    }
}

namespace GTS {

    void StartKillmove(RE::Actor* giant, RE::Actor* enemy, RE::NiAVObject* lookNode, DamageSource Cause, float base_damage, float crush_mult, bool isFoot, bool TinyCalamity) {
        if (!giant) {
            return;
        }
        if (!giant->IsPlayerRef()) {
            return;
        }
        if (!enemy) {
            return;
        }
        if (!lookNode) {
            return;
        }
        if (!ShouldTrigger(PlayerCharacter::GetSingleton(), enemy, Cause, base_damage, crush_mult)) {
            logger::info("Can't start killmove");
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

        _enemy = enemy;
        _node = lookNode;
        _nodeLookDown = false;
        _isFoot = isFoot;

        float scale = Scale();
        RE::NiPoint3 headPos = HeadPos();

        // The approach point already banks part of RiseHeight so stage 1
        // ends above the enemy rather than level with (or inside) their face;
        // stage 2 (RiseAboveEnemy) then finishes the climb.
        _cam.stageToPos = CameraAnchor();

        EnterStage(_cam, _state, FakeKillmoveState::MoveToEnemy);
    }

    void UpdateFakeKillmove() {
        if (!_cam.active) {
            return;
        }

        // Safety net: if the player's killmove animation stops being "busy"
        // for any reason (interrupted, cancelled, ended early) while we're
        // still tracking the enemy, don't keep the camera glued to them -
        // ease back out immediately instead of waiting for a stage-specific
        // timeout to eventually notice.
        if (_state != FakeKillmoveState::None && _state != FakeKillmoveState::ReturnCamera && !IsPlayerAnimBusy()) {
            EnterStage(_cam, _state, FakeKillmoveState::ReturnCamera);
        }

        float dt = Time::WorldTimeDelta() * GetAnimationSlowdown(PlayerCharacter::GetSingleton());

        switch (_state) {
            case FakeKillmoveState::MoveToEnemy:    UpdateMoveToEnemy(dt);    break;
            case FakeKillmoveState::RiseAboveEnemy: UpdateRiseAboveEnemy(dt); break;
            case FakeKillmoveState::LookAtFace:     UpdateLookAtFace(dt);     break;
            case FakeKillmoveState::LookAtNode:     UpdateLookAtNode(dt);     break;
            case FakeKillmoveState::DeathFlyOff:    UpdateDeathFlyOff(dt);    break;
            case FakeKillmoveState::ReturnCamera:   UpdateReturnCamera(dt);   break;
            default: break;
        }

        // Applied last so the impact hit-stop overrides whatever SGTM value
        // the stage above just set, for as long as it's still running.
        ApplyImpactSlowMo(_cam, dt, _settings.SlowMoTarget, _settings.ImpactSlowMoExtra, _settings.ImpactSlowMoInTime, _settings.ImpactSlowMoTime);
    }

    RE::NiPoint3 NodeOrHeadPos() {
        if (auto node = ResolveLookNode()) {
            RE::NiPoint3 pos = node->world.translate;
            // The skeleton bone itself doesn't move when heels go on - HighHeelManager's
            // offset is a purely visual effect the game doesn't bake into the node
            // transform. Correct for it here, once, so every caller downstream (proximity
            // checks, look-down framing, orbit anchor, death-hold distance) automatically
            // works off the true heel-adjusted height instead of each re-deriving it.
            pos.z -= ApplyHeelOffset(_isFoot);
            return pos;
        }
        return HeadPos();
    }

    bool UpdateKillMove() {
        if (_state == FakeKillmoveState::None) {
            return false;
        }
        if (!DriveCameraWithCollision(_cam, NodeOrHeadPos())) {
            return false;
        }
        UpdateFakeKillmove();
        return true;
    }

    void RecordStartingPosition() {
        RecordStartingPosition(_cam, _state, FakeKillmoveState::None);
    }
}