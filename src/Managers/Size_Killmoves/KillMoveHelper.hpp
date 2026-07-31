#pragma once

namespace GTS {
    float PredictDamage(Actor* giant, Actor* enemy, DamageSource Cause, float base);
    bool ShouldTrigger(Actor* giant, Actor* enemy, DamageSource Cause, float base, float crush_threshold, float increase = 0.0f);
    float ApplyHeelOffset(bool foot);

    // ---------------------------------------------------------------------
    // small math helpers
    // ---------------------------------------------------------------------

    float Clamp01(float x);
    float Ease(float x);
    RE::NiPoint3 Lerp(const RE::NiPoint3& a, const RE::NiPoint3& b, float t);
    RE::NiPoint3 RotateAroundAxis(const RE::NiPoint3& v, const RE::NiPoint3& axis, float angle);
    RE::NiMatrix3 BuildLookAt(const RE::NiPoint3& from, const RE::NiPoint3& to);

    // ---------------------------------------------------------------------
    // quaternion helpers, used only to slerp between two camera rotations
    // during the face -> node hand-off (NiMatrix3 has no built-in slerp)
    // ---------------------------------------------------------------------

    struct Quat { float w, x, y, z; };
    Quat MatrixToQuat(const RE::NiMatrix3& m);
    RE::NiMatrix3 QuatToMatrix(const Quat& q);
    Quat SlerpQuat(const Quat& a, Quat b, float t);
    RE::NiMatrix3 SlerpMatrix(const RE::NiMatrix3& a, const RE::NiMatrix3& b, float t);

    // ---------------------------------------------------------------------
    // shared camera-sequence state & helpers
    //
    // Both the regular (external, third-person-looking-at-the-enemy) and the
    // Calamity ("true POV", camera pinned to the victim's eyes) fake killmove
    // sequences are built from the same handful of moving parts: a staged
    // timer, a from/to blend, an impact hit-stop, and a return-to-start ease.
    // CameraSequenceState holds that shared state so the per-sequence .cpp
    // files only need to keep track of what's actually unique to them (which
    // actors/nodes are involved, the specific state enum, per-sequence
    // settings).
    // ---------------------------------------------------------------------

    struct CameraSequenceState {
        bool  active = false;
        float timer  = 0.0f;

        RE::NiMatrix3 cameraRot;
        RE::NiMatrix3 startRot;
        RE::NiPoint3  cameraPos;
        RE::NiPoint3  startPos;

        RE::NiPoint3  stageFromPos; // position the current stage is blending from
        RE::NiPoint3  stageToPos;   // position the current stage is blending to
        RE::NiMatrix3 stageFromRot; // rotation the current stage is blending from

        // Face/eye -> node hand-off blend (shared by both sequences' timed
        // and proximity-triggered transitions)
        RE::NiMatrix3 rotBlendFrom;
        float blendTimer    = 0.0f;
        float blendDuration = 0.0f;

        // Impact hit-stop, fired the instant the target actually dies. Tracked
        // independently of `timer`/the stage state machine so it survives
        // stage hand-offs unaffected.
        bool  impactActive = false;
        float impactTimer  = 0.0f;
    };

    // Advances state.timer by a GGTM-corrected delta - so slowing down game
    // time (which every stage does, via Time::SGTM) doesn't also drag out the
    // real-world length of the camera sequence - and returns the eased-free
    // [0,1] progress through `duration`.
    float AdvanceStageTimer(CameraSequenceState& state, float dt, float duration);

    // Moves to a new stage: resets the stage timer and captures the current
    // camera position/rotation as the "from" point for whatever the new
    // stage wants to blend from.
    template <typename StateEnum>
    void EnterStage(CameraSequenceState& state, StateEnum& stateVar, StateEnum newStage) {
        stateVar = newStage;
        state.timer = 0.0f;
        state.stageFromPos = state.cameraPos;
        state.stageFromRot = state.cameraRot;
    }

    // Starts a from/to blend (e.g. the face -> node hand-off).
    void BeginBlend(CameraSequenceState& state, float duration);

    // Advances the (GGTM-corrected) blend timer and returns an eased [0,1]
    // factor. Once the blend duration has elapsed this just returns 1.0
    // every frame without further accumulation.
    float AdvanceBlend(CameraSequenceState& state, float dt);

    // Arms the impact hit-stop.
    void TriggerImpactSlowMo(CameraSequenceState& state);

    // Advances the impact timer and, while active, overrides SGTM with a
    // value eased from the dip back up to slowMoTarget. Returns true if it
    // touched SGTM this frame (so the caller applies it *after* the per-stage
    // update, letting the impact win any conflict). impactExtra is the
    // fraction (0-1) slowMoTarget is further cut by at the moment of death;
    // impactTime is the seconds to recover back to slowMoTarget.
    bool ApplyImpactSlowMo(CameraSequenceState& state, float dt, float slowMoTarget, float impactExtra, float impactInTime, float impactOutTime);

    // True while the player's current animation (the killmove itself) is
    // still playing. Both sequences must bail out - ease the camera back to
    // its start instead of continuing to track the target - the moment this
    // goes false, since that means the killmove animation was interrupted,
    // cancelled, or otherwise ended without the state machine noticing.
    bool IsPlayerAnimBusy();

    // Applies state.cameraPos/cameraRot to the player camera, running the
    // desired position through the standard camera-collision correction
    // first (raycast from collisionRayStart, the node/head the camera is
    // currently focused on). Returns false if the camera/player singletons
    // aren't available (caller should treat that as "nothing to update").
    bool DriveCameraWithCollision(const CameraSequenceState& state, const RE::NiPoint3& collisionRayStart);

    // Captures the player camera's current position/rotation into `state`,
    // but only while the sequence is inactive (state == noneStage) - called
    // every frame from outside so that whenever a sequence *does* start, its
    // StartXxx() function has an up-to-date, non-stale camera pose to ease
    // out of.
    template <typename StateEnum>
    void RecordStartingPosition(CameraSequenceState& state, StateEnum currentStage, StateEnum noneStage) {
        if (currentStage == noneStage) {
            auto camera = RE::PlayerCamera::GetSingleton();
            auto root = camera->cameraRoot.get();
            state.cameraPos = root->world.translate;
            state.cameraRot = root->world.rotate;
        }
    }
}