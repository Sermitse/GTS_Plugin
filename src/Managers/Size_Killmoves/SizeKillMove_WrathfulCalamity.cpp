#include "Managers/Size_Killmoves/SizeKillMove_WrathfulCalamity.hpp"
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
using namespace WrathfulCalamity;
namespace WrathfulCalamity {

    // ---------------------------------------------------------------------
    // actor/node lookups - specific to this sequence (everything generic
    // - stage timers, blends - lives in KillMoveHelper)
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

    RE::NiAVObject* ResolveGiantNode() {
        return _giantNode; // nullptr -> fall back to GiantHeadPos() (see GiantNodeOrHeadPos)
    }

    // -----------------------------------------------------------------------
    // public helpers
    // -----------------------------------------------------------------------

    // The live camera anchor: sits just in front of and above the enemy's head node, scaled by
    // the ENEMY's own visual scale so it shrinks together with them - but that scale is floored
    // at MinAnchorScale first. Without the floor, a heavily shrunken enemy (e.g. ~0.10x) would
    // pull the offsets down to near-zero and the camera would end up clipping straight into them.
    RE::NiPoint3 EnemyAnchorPos() {
        RE::NiPoint3 headPos = VictimHeadPos();
        RE::NiMatrix3 headRot = VictimHeadRot();
        RE::NiPoint3 forward = headRot * RE::NiPoint3(0.f, 1.f, 0.f);
        RE::NiPoint3 up(0.f, 0.f, 1.f);

        float scale = std::max(VictimScale(), _settings.MinAnchorScale);
        return headPos
            + forward * (_settings.FocusForwardOffset * scale)
            + up * (_settings.FocusHeightOffset * scale);
    }

    RE::NiPoint3 GiantNodeOrHeadPos() {
        if (auto node = ResolveGiantNode()) {
            RE::NiPoint3 pos = node->world.translate;
            // Skeleton bones don't move when heels go on - HighHeelManager's offset
            // is purely visual, so correct for it here once.
            return pos;
        }
        return GiantHeadPos();
    }

    // The shared AdvanceStageTimer (KillMoveHelper) normalizes each frame's delta by dividing by
    // the CURRENT global time multiplier (GGTM), so a stage's real-world length stays constant no
    // matter how much Time::SGTM() has slowed the game down. That's stable for the moderate
    // slowdowns most stages use, but ImpactHold/ReturnCamera can be sitting at a near-total freeze
    // here (ImpactSlowMoCut can push the multiplier down to a few thousandths) - once GGTM is that
    // small, dividing by it blows the normalized delta up far past `duration` in a single frame, so
    // the stage ends almost immediately instead of actually lasting ImpactHoldTime/ReturnTime
    // seconds. This is what caused time to "snap back to normal instantly" instead of holding for
    // the configured second. Flooring the divisor keeps the correction bounded without changing
    // behavior at the moderate slowdowns the rest of the sequence uses.
    float AdvanceStageTimerSafe(CameraSequenceState& state, float dt, float duration) {
        float ggtm = std::max(Time::GGTM(), 0.05f);
        state.timer += dt / ggtm;
        return duration > 1e-4f ? Clamp01(state.timer / duration) : 1.0f;
    }

    // ---------------------------------------------------------------------
    // stage updates
    // ---------------------------------------------------------------------

    // 1-2) Dive from the regular camera into a position settled near/above the enemy, looking
    // at them. No slowdown yet - the ramp only kicks in once we're actually holding on the node.
    void UpdateFocusEnemy(float dt) {
        float t = AdvanceStageTimer(_cam, dt, _settings.FocusEnemyTime);
        float eased = Ease(t);

        RE::NiPoint3 anchorTarget = EnemyAnchorPos();
        _cam.cameraPos = Lerp(_cam.stageFromPos, anchorTarget, eased);
        _cam.cameraRot = BuildLookAt(_cam.cameraPos, VictimHeadPos());

        Time::SGTM(1.0f);

        if (t >= 1.0f) {
            EnterStage(_cam, _state, WrathfulPOVState::LookAtGiantNode);
            _cam.rotBlendFrom = _cam.cameraRot;
            BeginBlend(_cam, _settings.ToGiantNodeBlendTime);
        }
    }

    // 3-4) Camera stays glued to the enemy anchor; look direction blends onto the giant's node.
    // The longer this stage runs, the stronger the slowdown gets (ramps to SlowMoMin over
    // SlowMoRampTime seconds of real time, then holds there).
    void UpdateLookAtGiantNode(float dt) {
        // AdvanceStageTimer is GGTM-corrected, so this tracks real (unslowed) elapsed time in
        // this stage regardless of how much we've already slowed the game down - exactly what
        // the ramp below and the safety timeout need.
        float rampT = AdvanceStageTimer(_cam, dt, _settings.SlowMoRampTime);

        _cam.cameraPos = EnemyAnchorPos(); // camera stays glued to the same anchor near the enemy
        RE::NiPoint3 node = GiantNodeOrHeadPos();
        if (_cam.timer >= 1.5f) {
            if (_victim && _victim->GetAlpha() > 0.0f) {
                _victim->SetAlpha(0.0f); // Hide vicitm
            }
        }
        float bt = AdvanceBlend(_cam, dt);
        RE::NiMatrix3 liveTarget = BuildLookAt(_cam.cameraPos, node);
        _cam.cameraRot = SlerpMatrix(_cam.rotBlendFrom, liveTarget, bt);

        float slowMo = 1.0f - (1.0f - _settings.SlowMoMin) * Ease(rampT);
        Time::SGTM(slowMo);

        bool dead = _victim && _victim->IsDead();

        if (dead) {
            _cam.stageFromPos = _cam.cameraPos;
            _cam.stageFromRot = _cam.cameraRot;
            _returnFromSGTM = slowMo;
            EnterStage(_cam, _state, WrathfulPOVState::ImpactHold);
        }
    }

    // 5) The instant the enemy actually dies: freeze the anchor and hold a strong, near-freeze
    // slowdown for ImpactHoldTime before we start easing back out.
    void UpdateImpactHold(float dt) {
        float t = AdvanceStageTimerSafe(_cam, dt, _settings.ImpactHoldTime);

        // Freeze the anchor at the position/rotation captured the instant the enemy died - once
        // the body ragdolls, continuing to sample the head node could throw the camera around.
        _cam.cameraPos = _cam.stageFromPos;
        _cam.cameraRot = _cam.stageFromRot;

        float dip = _returnFromSGTM * _settings.ImpactSlowMoCut;
        Time::SGTM(dip);

        if (t >= 1.0f) {
            _returnFromSGTM = dip;
            EnterStage(_cam, _state, WrathfulPOVState::ReturnCamera);
        }
    }

    // 7) Ease the camera back to its pre-killmove position, and ease SGTM all the way back to
    // full normal speed (1.0) - not just back to a mid-sequence slowdown value, since the
    // sequence is ending here.
    void UpdateReturnCamera(float dt) {
        float t = AdvanceStageTimerSafe(_cam, dt, _settings.ReturnTime);
        float eased = Ease(t);
        _cam.cameraPos = Lerp(_cam.stageFromPos, _cam.startPos, eased);

        // Slerp straight toward the exact captured start rotation rather than continuously
        // re-aiming, to avoid a snap once t hits 1.
        _cam.cameraRot = SlerpMatrix(_cam.stageFromRot, _cam.startRot, eased);

        Time::SGTM(_returnFromSGTM + (1.0f - _returnFromSGTM) * eased);

        if (t >= 1.0f) {
            _cam.cameraPos = _cam.startPos;
            _cam.cameraRot = _cam.startRot;

            // Explicit final snap back to full speed. This sequence deliberately drives SGTM
            // directly from the stages above instead of relying on the shared
            // impactActive/impactTimer hit-stop helper: that timer runs independently of the
            // stage state machine, and if the sequence deactivates before it finishes recovering,
            // nothing ever calls Time::SGTM(1.0f) again and the game is left permanently slowed.
            // Snapping here, right before deactivating, avoids that failure mode entirely.
            Time::SGTM(1.0f);

            _victim = nullptr;
            _giant = nullptr;
            _giantNode = nullptr;
            _cam.active = false;
            _state = WrathfulPOVState::None;
        }
    }

    // Failsafe: the killmove animation stopped being "busy" for any reason (interrupted,
    // cancelled, ended early) while we're still mid-sequence. Snap the camera and time back
    // immediately rather than waiting out a stage-specific timeout or playing the smooth
    // ReturnCamera ease over an animation that's already gone.
    void ForceEndSequence() {
        Time::SGTM(1.0f);
        _cam.cameraPos = _cam.startPos;
        _cam.cameraRot = _cam.startRot;

        _victim = nullptr;
        _giant = nullptr;
        _giantNode = nullptr;
        _cam.active = false;
        _state = WrathfulPOVState::None;
    }
}
namespace GTS {
    void StartWrathfulCalamityKillmove(RE::Actor* giant, RE::Actor* victim, RE::NiAVObject* giantLookNode, DamageSource Cause, float base_damage, float crush_mult, bool isFootNode, bool TinyCalamity) {
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
            logger::info("Can't start Wrathful Calamity killmove");
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
        _returnFromSGTM = 1.0f;

        _cam.stageFromPos = _cam.startPos; // FocusEnemy blends from here
        EnterStage(_cam, _state, WrathfulPOVState::FocusEnemy);
        // EnterStage overwrote stageFromPos with cameraPos (== startPos here anyway),
        // so this is just being explicit about the intent for future edits.
    }

    void UpdateWrathfulKillmove() {
        if (!_cam.active) {
            return;
        }

        // Failsafe (see ForceEndSequence above) - never linger mid-sequence once the killmove
        // animation itself has stopped playing, for any reason. Skipped once we're already in
        // ReturnCamera, since that stage is already doing the same job smoothly.
        if (_state != WrathfulPOVState::None && _state != WrathfulPOVState::ReturnCamera && !IsPlayerAnimBusy()) {
            ForceEndSequence();
            return;
        }

        float dt = Time::WorldTimeDelta() * GetAnimationSlowdown(PlayerCharacter::GetSingleton());

        switch (_state) {
            case WrathfulPOVState::FocusEnemy:      UpdateFocusEnemy(dt);      break;
            case WrathfulPOVState::LookAtGiantNode: UpdateLookAtGiantNode(dt); break;
            case WrathfulPOVState::ImpactHold:      UpdateImpactHold(dt);      break;
            case WrathfulPOVState::ReturnCamera:    UpdateReturnCamera(dt);    break;
            default: break;
        }
    }

    bool UpdateWrathfulCalamityKillMove() {
        if (_state == WrathfulPOVState::None) {
            return false;
        }

        // _giantNode is optional (nullptr -> focus on the giant's face), so the collision ray
        // has to fall back to GiantNodeOrHeadPos() rather than dereferencing _giantNode directly.
        RE::NiPoint3 rayStart = GiantNodeOrHeadPos();
        if (!DriveCameraWithCollision(_cam, rayStart)) {
            return false;
        }
        UpdateWrathfulKillmove();
        return true;
    }

    void RecordWrathfulCalamityStartingPosition() {
        RecordStartingPosition(_cam, _state, WrathfulPOVState::None);
    }

    bool OverrideWrathfulCalamityHeadtracking(NiPoint3 &target) {
        if (!_cam.active) {
            return false;
        }
        NiPoint3 coords = _victim->GetPosition();
        if (auto head = find_node(_victim, "NPC Head [Head]")) {
            coords = head->world.translate;
        } else if (auto COM = find_node(_victim, "NPC COM [COM ]")) {
            coords = COM->world.translate;
        }
        target = coords;
        return false;
    }
}