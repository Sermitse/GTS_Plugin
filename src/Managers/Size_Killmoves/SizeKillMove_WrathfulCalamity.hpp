#include "Managers/Size_Killmoves/KillMoveHelper.hpp"
#pragma once

using namespace GTS;
namespace WrathfulCalamity {

    // Stages of the WrathfulCalamity fake killmove camera sequence, played in this order.
    //
    // Unlike the regular FakeKillmoveState sequence (external camera orbiting the enemy),
    // this sequence settles the camera close to (and slightly above) the shrunken enemy and
    // has it look toward the giantess - first at her generally, then at a specific node
    // (hand/foot/etc). The longer the camera holds on the giantess' node before the kill, the
    // stronger the time slowdown gets. The instant the enemy actually dies we punch in a brief,
    // much stronger slowdown, then smoothly hand control - and full-speed time - back.
    enum class WrathfulPOVState {
        None,
        FocusEnemy,       
        LookAtGiantNode,  
        ImpactHold,      
        ReturnCamera
    };

    // Every tunable number for the sequence lives here so the pacing/feel can
    // be adjusted without touching the update logic in the .cpp.
    struct WrathfulCalamitySettings {
        // 1-2) Focus enemy - blend from the regular camera into a position settled near the enemy.
        float FocusEnemyTime = 0.15f; // seconds

        // Where the camera sits relative to the enemy's head node, scaled by the ENEMY's own
        // visual scale - but that scale is floored at MinAnchorScale first, so the offsets don't
        // collapse toward zero (and the camera doesn't clip into the enemy) once they've shrunk
        // to very small scales (e.g. ~0.10x).
        float FocusForwardOffset = -15.0f;  // units, along head-forward
        float FocusHeightOffset  = 6.0f;  // units, world-up - raised above the head so the shot clears a tiny enemy
        float MinAnchorScale     = 0.35f; // floor applied to the enemy's visual scale for the offsets above

        // 3) Enemy anchor -> giant node hand-off blend.
        float ToGiantNodeBlendTime = 0.3f; // seconds

        // 4) The longer LookAtGiantNode holds, the stronger the slowdown: ramps from normal speed
        // down to SlowMoMin over SlowMoRampTime seconds of real (unslowed) time, then holds at
        // SlowMoMin until the kill (or the safety timeout below).
        float SlowMoRampTime = 12.0f; // seconds
        float SlowMoMin      = 0.25f; // global time multiplier at maximum ramp

        // 5) Impact hold - the instant the enemy actually dies, freeze the anchor and cut the
        // current slowdown down further for a brief, strong near-freeze beat before returning.
        float ImpactHoldTime  = 0.75f;  // seconds
        float ImpactSlowMoCut = 0.03f; // fraction (0-1) the slowdown in effect at death is further cut by, held for ImpactHoldTime

        // 7) Return camera to its original (pre-killmove) position, and ease SGTM back to 1.0
        float ReturnTime = 1.5f; // seconds
    };

    inline WrathfulCalamitySettings _settings{};
    inline WrathfulPOVState _state = WrathfulPOVState::None;

    // Shared camera-sequence state (timer, camera/start pose, stage blend) - see
    // CameraSequenceState in KillMoveHelper.hpp. Note: this sequence deliberately does NOT use
    // the shared impactActive/impactTimer hit-stop helper (ApplyImpactSlowMo) - see ImpactHold /
    // ReturnCamera in the .cpp for why (that independent timer could outlive the sequence and
    // leave time stuck slowed forever).
    inline CameraSequenceState _cam{};

    inline RE::Actor* _victim = nullptr; // the shrinking enemy the camera settles near
    inline RE::Actor* _giant  = nullptr; // who the camera looks toward (must be the player - see StartKillmove)
    inline RE::NiAVObject* _giantNode = nullptr; // optional node on the giant to focus on (hand/foot/etc); nullptr -> giant's face

    // The global time multiplier in effect the moment we left LookAtGiantNode (either into
    // ImpactHold on death, or straight into ReturnCamera on timeout) / left ImpactHold.
    // ReturnCamera eases from this value back up to 1.0, rather than assuming a fixed start point.
    inline float _returnFromSGTM = 1.0f;

    RE::NiPoint3 EnemyAnchorPos();      // live camera anchor near/above the enemy (follows their head node every frame)
    RE::NiPoint3 GiantNodeOrHeadPos();  // live look target on the giant (resolved node, or their head as a fallback)
}

namespace GTS {
    void UpdateWrathfulKillmove();
    void StartWrathfulCalamityKillmove(RE::Actor* giant, RE::Actor* victim, RE::NiAVObject* giantLookNode, DamageSource Cause, float base_damage, float crush_mult, bool isFootNode = false, bool TinyCalamity = true);
    bool UpdateWrathfulCalamityKillMove();
    void RecordWrathfulCalamityStartingPosition();
}