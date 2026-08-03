#pragma once

#include "Managers/Size_Killmoves/KillMoveHelper.hpp"

namespace GTS {

    // Stages of the fake killmove camera sequence, played in this order.
    enum class SizeKillMoveState {
        None,
        MoveToEnemy,     // 1) camera glides to a spot in front of (and partway above) the enemy
        RiseAboveEnemy,  // 2) camera finishes rising above the enemy
        LookAtFace,      // 3) camera tracks the giantess' face
        LookAtNode,      // 4) camera blends onto a custom node (usually a foot) and waits for the kill
        DeathFlyOff,     // 5) camera flies off to the side of the node, then holds / orbits
        ReturnCamera     // 6) camera eases back to where it started
    };

    // Every tunable number for the sequence lives here so the pacing/feel can
    // be adjusted without touching the update logic in the .cpp.
    struct SizeKillMoveSettings {
        // 1) Move to enemy
        float MoveToEnemyTime      = 0.15f; // seconds
        float MoveToEnemyDistance  = 10.0f;  // units in front of the enemy's head, scaled by their visual scale
        float MoveToEnemyRiseFrac  = 0.5f;  // fraction (0-1) of RiseHeight already gained by the end of this stage,
                                             // so the approach point sits above the enemy instead of level with their face
        float MoveToEnemyArcHeight = 10.0f;  // units, scaled by enemy visual scale - extra mid-flight upward bow so the
                                             // travel path bows over the enemy instead of cutting straight through them
        float MoveFromEnemyOffset = 200.0f;  // Move away from enemy
        float MoveUpwards         = 30.0f;   // If no target node found (in Animals case)
        // 2) Rise above enemy (covers the remaining 1 - MoveToEnemyRiseFrac of RiseHeight)
        float RiseTime   = 0.15f;  // seconds
        float RiseHeight = 40.0f;  // units (world up) above the head-level approach point, scaled by the enemy's visual scale

        // 3) Look at the giantess' face
        float LookAtFaceTime = 0.4f; // seconds - fallback/maximum; ends early if the tracked node closes in (see NodeProximityRadius)

        // Face -> node hand-off (shared by both the timed and the proximity-triggered transition)
        float FaceToNodeBlendTime = 0.3f;  // seconds - rotation/position blend when the fixed timer is what triggers the switch
        float NodeProximityRadius = 12.0f;  // units, scaled by the giant's visual scale - spherical radius; once the tracked
                                            // node is this close to the giant we cut over early instead of waiting out the timer
        float ProximityBlendTime  = 0.3f;  // seconds - blend duration used for the early/proximity-triggered switch
        float NodeLookDownHeight  = 15.0f; // units, scaled by the giant's visual scale - how far above the node the camera
                                            // lifts itself when the proximity trigger fires, for a slight top-down reveal angle

        // 4) Look at a custom node (defaults to a foot if none is passed to StartKillmove)
        const char* DefaultLookNodeName = "NPC Head [Head]";
        float LookAtNodeMaxWait = 12.0f;  // safety timeout in case IsDead() never triggers

        // 5) Fly off from the node once the enemy is dead
        float DeathFlyOffTime   = 1.5f; // seconds to reach the hold position
        float DeathHoldDistance = 40.0f; // units from the node, scaled by the giant's visual scale, applied when actor is dead
        float LookAtNodeDistance = 30.0f; // Units from the node, scaled by the giant's visual scale, applied while actor is still alive
        bool  OrbitEnabled = true;
        float OrbitAngle   = 360.0f;      // degrees swept around the node, recommended 50-90
        float OrbitTime    = 9.0f;       // seconds to complete the sweep
        float PostDeathMaxWait = 15.0f;   // extra safety timeout on top of fly+orbit before forcing the return

        // 6) Return camera to its original position
        float ReturnTime = 0.60f; // seconds

        // Extra "hit-stop" dip triggered the instant the enemy actually dies (Actor::IsDead()
        // turning true while we're in LookAtNode). Stacks a further multiplicative slowdown on
        // top of SlowMoTarget, then eases back up to SlowMoTarget over ImpactSlowMoTime. Runs
        // independently of the stage state machine so it survives the LookAtNode -> DeathFlyOff
        // (and, if it's still running, -> ReturnCamera) transitions unaffected.
        float SlowMoTarget = 0.25f; // global time multiplier reached at the end of stage 1, held through 2-5
        float ImpactSlowMoExtra = 0.96f; // fraction (0-1) SlowMoTarget is further cut by at the moment of death, e.g. 0.5 = half as fast as the normal slow-mo speed
        float ImpactSlowMoInTime = 0.1f;
        float ImpactSlowMoTime  = 0.6f; // seconds to recover from the impact dip back to the normal SlowMoTarget speed
    };

    inline SizeKillMoveSettings _settings{};
    inline SizeKillMoveState _state = SizeKillMoveState::None;

    // Shared camera-sequence state (timer, camera/start pose, stage blend,
    // impact hit-stop) - see CameraSequenceState in KillMoveHelper.hpp.
    inline CameraSequenceState _cam{};

    inline RE::Actor* _enemy = nullptr;
    inline std::vector<RE::NiAVObject*> _nodes = {};
    inline bool _isFoot = false; // Determines if Heel offset should be applied

    // Face -> node hand-off extras (not part of the generic sequence state)
    inline RE::NiPoint3 _nodeEyeTarget;        // camera position LookAtNode is blending towards
    inline bool         _nodeLookDown = false; // true if this LookAtNode entry was proximity-triggered (top-down framing)

    void UpdateSizeKillmove();
    void StartKillmove(RE::Actor* giant, RE::Actor* enemy, RE::NiAVObject* lookNode, DamageSource Cause, KillMoveParameters params);
    void StartKillmove(RE::Actor* giant, RE::Actor* enemy, std::vector<RE::NiAVObject*> lookNodes, KillMoveParameters params);
    RE::NiPoint3 NodeOrHeadPos();
    bool UpdateKillMove();
    void RecordStartingPosition();
}