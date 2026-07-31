#include "Managers/Size_Killmoves/KillMoveHelper.hpp"
#pragma once

using namespace GTS;
namespace Calamity {

    // Stages of the "true POV" fake killmove camera sequence, played in this order.
    //
    // Unlike the regular FakeKillmoveState sequence (which flies an external camera
    // around the ENEMY from the giant's point of view), this sequence pins the camera
    // to the shrinking victim's own eyes and looks *up* at the giant towering over them.
    // The camera position basically never leaves the victim's head after stage 1 - only
    // the look direction changes as the sequence progresses.
    enum class TinyPOVState {
        None,
        DiveToEye,       // 1) cut from the regular third-person camera down into the victim's eye position
        LookUpAtGiant,   // 2) camera, anchored at the eye, tracks the giant's face towering overhead
        LookAtGiantNode, // 3) camera blends its look direction onto a specific giant node (closing hand/foot/etc) and waits for the kill
        ImpactShake,     // 4) the instant the victim actually dies: freeze the anchor and shake/dip, instead of flying off
        ReturnCamera     // 5) camera eases back out to where the regular camera started
    };

    // Every tunable number for the sequence lives here so the pacing/feel can
    // be adjusted without touching the update logic in the .cpp.
    struct TinyPOVSettings {
        // 1) Dive to eye - blend from the regular camera position into the victim's eye anchor.
        float DiveToEyeTime = 0.15f; // seconds

        // Where exactly "the eyes" sit relative to the victim's head node, scaled by the
        // VICTIM's own visual scale (so the anchor shrinks together with them).
        float EyeForwardOffset = 3.0f; // units, along head-forward, to clear the front of the face
        float EyeHeightOffset  = 2.0f; // units, world-up, head node -> eye height

        // 2) Look up at giant (fallback/maximum; ends early if a tracked giant node closes in, see below)
        float LookUpAtGiantTime = 1.2f; // seconds

        // Giant-face -> giant-node hand-off (shared by both the timed and the proximity-triggered transition)
        float FaceToNodeBlendTime      = 0.3f;  // seconds - blend used when the fixed timer triggers the switch
        float GiantNodeProximityRadius = 30.0f; // units, scaled by the GIANT's visual scale - once the tracked
                                                 // giant node (e.g. an incoming hand) is this close to the victim's
                                                 // eye we cut over early instead of waiting out the timer
        float ProximityBlendTime = 0.3f; // seconds - blend duration used for the early/proximity-triggered switch

        float LookAtGiantNodeMaxWait = 12.0f; // safety timeout in case IsDead() never triggers on the victim
        bool  GiantNodeIsFoot        = false; // set true when giantLookNode is a foot, to apply the heel height correction

        // 4) Impact shake - fired the instant the victim actually dies. The eye anchor freezes at its
        // last position/rotation (continuing to follow a ragdolling body would look broken) and the
        // camera just kicks/wobbles briefly before we cut back out.
        float ImpactShakeTime      = 0.35f; // seconds
        float ImpactShakeMagnitude = 4.0f;  // units, scaled by the VICTIM's visual scale
        float ImpactShakeFrequency = 40.0f; // radians/sec fed into the shake sine wave

        // 5) Return camera to its original (pre-killmove) position
        float ReturnTime = 0.5f; // seconds

        // Bullet-time effect while the camera dives in / holds / pulls back out.
        float SlowMoTarget = 0.8f; // global time multiplier reached at the end of stage 1, held through 2-4

        // Extra "hit-stop" dip triggered the instant the victim actually dies (Actor::IsDead()
        // turning true while we're in LookAtGiantNode). Stacks a further multiplicative slowdown
        // on top of SlowMoTarget, then eases back up to SlowMoTarget over ImpactSlowMoTime.
        float ImpactSlowMoExtra = 0.9f; // fraction (0-1) SlowMoTarget is further cut by at the moment of death
        float ImpactSlowMoInTime = 0.3f;
        float ImpactSlowMoTime  = 1.0f; // seconds to recover from the impact dip back to the normal SlowMoTarget speed
    };

    inline TinyPOVSettings _settings{};
    inline TinyPOVState _state = TinyPOVState::None;

    // Shared camera-sequence state (timer, camera/start pose, stage blend,
    // impact hit-stop) - see CameraSequenceState in KillMoveHelper.hpp.
    inline CameraSequenceState _cam{};

    inline RE::Actor* _victim = nullptr; // the shrinking actor whose eyes the camera sits in (the "enemy" of the regular sequence)
    inline RE::Actor* _giant  = nullptr; // who the camera looks up at (must be the player - see StartKillmove)
    inline RE::NiAVObject* _giantNode = nullptr; // optional node on the giant to focus on (hand/foot/etc); nullptr -> giant's face

    

    RE::NiPoint3 VictimEyePos();     // live eye-anchor position (follows the victim's head node every frame)
    RE::NiPoint3 GiantNodeOrHeadPos(); // live look target on the giant (resolved node, or their head as a fallback)
}

namespace GTS {
    void UpdateFakeCalamityKillmove();
    void StartCalamityKillmove(RE::Actor* giant, RE::Actor* victim, RE::NiAVObject* giantLookNode, DamageSource Cause, float base_damage, float crush_mult, bool isFootNode = false, bool TinyCalamity = true);
    bool UpdateCalamityKillMove();
    void RecordCalamityStartingPosition();
}