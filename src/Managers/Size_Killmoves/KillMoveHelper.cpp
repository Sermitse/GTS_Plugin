#include "Managers/Size_Killmoves/SizeKillMove_WrathfulCalamity.hpp"
#include "Managers/Size_Killmoves/SizeKillMove_BreastSuffocate.hpp"
#include "Managers/Size_Killmoves/SizeKillMove_BreastAbsorb.hpp"
#include "Managers/Size_Killmoves/SizeKillMove_Calamity.hpp"
#include "Managers/Size_Killmoves/KillMoveParamObtainer.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Size_Killmoves/KillMoveHelper.hpp"
#include "Managers/Size_Killmoves/SizeKillMove.hpp"
#include "Managers/Damage/Utils/SizeDamageUtils.hpp"
#include "Systems/Rays/Camera/CameraCollision.hpp"
#include "Utils/Actions/AutoAim/AutoAimUtils.hpp"
#include "Managers/Damage/CollisionDamage.hpp"
#include "Utils/Actions/AutoAim/AimAssist.hpp"
#include "Managers/Cameras/CamUtil.hpp"
#include "Managers/GTSSizeManager.hpp"
#include "Utils/DifficultyUtils.hpp"
#include "Managers/HighHeel.hpp"
#include "Config/Config.hpp"



using namespace GTS;

namespace {
    constexpr float reducefinalDamage = 0.55f;
}

namespace GTS {
    float VictimScale(Actor* a_victim) { return a_victim ? get_visual_scale(a_victim) : 1.0f; }
    float GiantScale(Actor* a_giant)  { return a_giant  ? get_visual_scale(a_giant)  : 1.0f; }

    float PredictDamage(Actor* giant, Actor* enemy, DamageSource Cause, float baseDamage) {
        if (DamageAllowed(giant, enemy, Cause)) {
            float finalDamage = CalculateSizeDamage(giant, enemy, Cause, baseDamage);
            finalDamage /= GetDifficultyMultiplier(giant, enemy);

            float levelbonus = 1.0f + ((GetGtsSkillLevel(giant) * 0.01f) * 0.50f);
            finalDamage *= levelbonus * Config::Balance.fSizeDamageMult;
            finalDamage *= reducefinalDamage; // To be safe
            logger::info("Damage Result: {}, Health: {}, Can Start: {}", finalDamage, GetAV(enemy, ActorValue::kHealth), finalDamage > GetAV(enemy, ActorValue::kHealth));
            return finalDamage;
        }
        return 0.0f;
    }

    bool CanTriggerKillMove(Actor* giant, Actor* enemy, DamageSource Cause, float baseDamage, float crushModifier, float killMoveChance_Crush) {
        if (get_scale_difference(giant, enemy, SizeType::VisualScale, false, false) >= Action_Crush * crushModifier) {
            killMoveChance_Crush = Config::KillMove.fKillMoveChance_Crush;
        }
        const float combinedChance = std::clamp(Config::KillMove.fKillMoveChance_Death + killMoveChance_Crush, 0.0f, 100.0f);
        const float predictedDamage = PredictDamage(giant, enemy, Cause, baseDamage);

        
        const bool isEnemyLowHealth = predictedDamage >= GetAV(enemy, ActorValue::kHealth);
        const bool isEnabled = Config::KillMove.bEnableKillMoves;
        const bool isRandomTrue = RandomBool(combinedChance);
        const bool isEnemyAlive = !enemy->IsDead();

        const bool canStartKillMove = isEnabled && isEnemyAlive && isRandomTrue && isEnemyLowHealth;
        const bool isFirstPerson = IsFirstPerson() || HasFirstPersonBody(); 
        // ^ Looks weird in first person because it captures FP head pos as a start point
        // So we disallow that
        
        return !isFirstPerson && canStartKillMove;
    }

    float ApplyHeelOffset(bool foot) {
        if (foot) {
            return HighHeelManager::GetInitialHeelHeight(RE::PlayerCharacter::GetSingleton()) * 100.0f;
            // Returns offset * 0.01f by default, so we * by 100 to convert to game units
        }
        return 0.0f;
    }

    RE::NiPoint3 VictimHeadPos(Actor* victim, const RE::NiPoint3& fallback, float noBoneOffsetZ) {
        if (!victim) {
            return fallback;
        }
        if (auto head = find_node(victim, "NPC Head [Head]")) {
            return head->world.translate;
        }
        return victim->GetPosition() + RE::NiPoint3(0.0f, 0.0f, noBoneOffsetZ);
    }

    RE::NiMatrix3 VictimHeadRot(Actor* victim) {
        if (victim) {
            if (auto head = find_node(victim, "NPC Head [Head]")) {
                return head->world.rotate;
            }
        }
        return RE::NiMatrix3();
    }

    RE::NiPoint3 GiantHeadPos(Actor* giant, const RE::NiPoint3& fallback) {
        if (!giant) {
            return fallback;
        }
        if (auto head = find_node(giant, "NPC Head [Head]")) {
            return head->world.translate;
        }
        return giant->GetPosition();
    }

    RE::NiPoint3 GiantNodeOrHeadPos(RE::NiAVObject* node, Actor* giant, const RE::NiPoint3& fallback, bool applyHeels) {
        if (node) {
            RE::NiPoint3 pos = node->world.translate;
            if (applyHeels) {
                pos.z -= ApplyHeelOffset(true);
            }
            return pos;
        }
        return GiantHeadPos(giant, fallback);
    }

    // ---------------------------------------------------------------------
    // small math helpers
    // ---------------------------------------------------------------------

    float Clamp01(float x) { return std::clamp(x, 0.0f, 1.0f); }

    float Ease(float x) {
        x = Clamp01(x);
        return x < 0.5f ? 4.0f * x * x * x : 1.0f - std::pow(-2.0f * x + 2.0f, 3.0f) / 2.0f;
    }

    RE::NiPoint3 Lerp(const RE::NiPoint3& a, const RE::NiPoint3& b, float t) {
        return a + (b - a) * t;
    }

    RE::NiPoint3 RotateAroundAxis(const RE::NiPoint3& v, const RE::NiPoint3& axis, float angle) {
        float c = std::cos(angle);
        float s = std::sin(angle);
        RE::NiPoint3 kxv = axis.Cross(v);
        float kdotv = axis.Dot(v);
        return v * c + kxv * s + axis * (kdotv * (1.0f - c));
    }

    RE::NiMatrix3 BuildLookAt(const RE::NiPoint3& from, const RE::NiPoint3& to) {
        RE::NiPoint3 forward = to - from;
        float len = forward.Length();
        if (len < 1e-3f) {
            return RE::NiMatrix3();
        }
        forward /= len;

        RE::NiPoint3 worldUp(0.f, 0.f, 1.f);
        if (std::fabs(forward.Dot(worldUp)) > 0.999f) {
            worldUp = RE::NiPoint3(0.f, 1.f, 0.f);
        }

        RE::NiPoint3 right = forward.Cross(worldUp);
        right.Unitize();
        RE::NiPoint3 up = right.Cross(forward);
        up.Unitize();

        return RE::NiMatrix3(
            RE::NiPoint3(right.x, forward.x, up.x),
            RE::NiPoint3(right.y, forward.y, up.y),
            RE::NiPoint3(right.z, forward.z, up.z)
        );
    }

    Quat MatrixToQuat(const RE::NiMatrix3& m) {
        Quat q{};
        float trace = m.entry[0][0] + m.entry[1][1] + m.entry[2][2];

        if (trace > 0.0f) {
            float s = std::sqrt(trace + 1.0f) * 2.0f;
            q.w = 0.25f * s;
            q.x = (m.entry[2][1] - m.entry[1][2]) / s;
            q.y = (m.entry[0][2] - m.entry[2][0]) / s;
            q.z = (m.entry[1][0] - m.entry[0][1]) / s;
        } else if (m.entry[0][0] > m.entry[1][1] && m.entry[0][0] > m.entry[2][2]) {
            float s = std::sqrt(1.0f + m.entry[0][0] - m.entry[1][1] - m.entry[2][2]) * 2.0f;
            q.w = (m.entry[2][1] - m.entry[1][2]) / s;
            q.x = 0.25f * s;
            q.y = (m.entry[0][1] + m.entry[1][0]) / s;
            q.z = (m.entry[0][2] + m.entry[2][0]) / s;
        } else if (m.entry[1][1] > m.entry[2][2]) {
            float s = std::sqrt(1.0f + m.entry[1][1] - m.entry[0][0] - m.entry[2][2]) * 2.0f;
            q.w = (m.entry[0][2] - m.entry[2][0]) / s;
            q.x = (m.entry[0][1] + m.entry[1][0]) / s;
            q.y = 0.25f * s;
            q.z = (m.entry[1][2] + m.entry[2][1]) / s;
        } else {
            float s = std::sqrt(1.0f + m.entry[2][2] - m.entry[0][0] - m.entry[1][1]) * 2.0f;
            q.w = (m.entry[1][0] - m.entry[0][1]) / s;
            q.x = (m.entry[0][2] + m.entry[2][0]) / s;
            q.y = (m.entry[1][2] + m.entry[2][1]) / s;
            q.z = 0.25f * s;
        }
        return q;
    }

    RE::NiMatrix3 QuatToMatrix(const Quat& q) {
        RE::NiMatrix3 m;
        float xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z;
        float xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z;
        float wx = q.w * q.x, wy = q.w * q.y, wz = q.w * q.z;

        m.entry[0][0] = 1.0f - 2.0f * (yy + zz);
        m.entry[0][1] = 2.0f * (xy - wz);
        m.entry[0][2] = 2.0f * (xz + wy);

        m.entry[1][0] = 2.0f * (xy + wz);
        m.entry[1][1] = 1.0f - 2.0f * (xx + zz);
        m.entry[1][2] = 2.0f * (yz - wx);

        m.entry[2][0] = 2.0f * (xz - wy);
        m.entry[2][1] = 2.0f * (yz + wx);
        m.entry[2][2] = 1.0f - 2.0f * (xx + yy);

        return m;
    }

    Quat SlerpQuat(const Quat& a, Quat b, float t) {
        float dot = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;

        // take the shorter arc
        if (dot < 0.0f) {
            b.w = -b.w; b.x = -b.x; b.y = -b.y; b.z = -b.z;
            dot = -dot;
        }

        if (dot > 0.9995f) {
            Quat r{ a.w + (b.w - a.w) * t, a.x + (b.x - a.x) * t,
                    a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t };
            float len = std::sqrt(r.w * r.w + r.x * r.x + r.y * r.y + r.z * r.z);
            if (len > 1e-6f) {
                r.w /= len; r.x /= len; r.y /= len; r.z /= len;
            }
            return r;
        }

        float theta0 = std::acos(std::clamp(dot, -1.0f, 1.0f)); 
        float theta = theta0 * t;
        float sinTheta0 = std::sin(theta0);
        float sinTheta = std::sin(theta);

        float s0 = std::cos(theta) - dot * sinTheta / sinTheta0;
        float s1 = sinTheta / sinTheta0;

        return Quat{ s0 * a.w + s1 * b.w, s0 * a.x + s1 * b.x,
                     s0 * a.y + s1 * b.y, s0 * a.z + s1 * b.z };
    }

    RE::NiMatrix3 SlerpMatrix(const RE::NiMatrix3& a, const RE::NiMatrix3& b, float t) {
        if (t <= 0.0f) return a;
        if (t >= 1.0f) return b;
        return QuatToMatrix(SlerpQuat(MatrixToQuat(a), MatrixToQuat(b), t));
    }

    // ---------------------------------------------------------------------
    // shared camera-sequence state helpers
    // ---------------------------------------------------------------------

    float AdvanceStageTimer(CameraSequenceState& state, float dt, float duration) {
        float ggtm = Time::GGTM();
        state.timer += ggtm > 1e-4f ? dt / ggtm : dt;
        return duration > 1e-4f ? Clamp01(state.timer / duration) : 1.0f;
    }
    void AdvanceCustomTimer(float dt, float duration, float& timer) {
        const float ggtm = Time::GGTM();
        const float add = ggtm > 1e-4f ? dt / ggtm : dt;

        timer += add;

        if (duration > 1e-4f) {
            timer = std::min(timer, duration);
        }
    }

    float AdvanceStageTimerSafe(CameraSequenceState& state, float dt, float duration) {
        float ggtm = std::max(Time::GGTM(), 0.05f);
        state.timer += dt / ggtm;
        return duration > 1e-4f ? Clamp01(state.timer / duration) : 1.0f;
    }

    void BeginBlend(CameraSequenceState& state, float duration) {
        state.blendDuration = duration;
        state.blendTimer = 0.0f;
    }

    float AdvanceBlend(CameraSequenceState& state, float dt) {
        if (state.blendTimer >= state.blendDuration) {
            return 1.0f;
        }
        float ggtm = Time::GGTM();
        state.blendTimer += ggtm > 1e-4f ? dt / ggtm : dt;
        return Ease(state.blendDuration > 1e-4f ? Clamp01(state.blendTimer / state.blendDuration) : 1.0f);
    }

    void TriggerImpactSlowMo(CameraSequenceState& state) {
        state.impactActive = true;
        state.impactTimer = 0.0f;
    }

    bool ApplyImpactSlowMo(CameraSequenceState& state, float dt, float slowMoTarget, float impactExtra, float impactInTime, float impactOutTime) {
        if (!state.impactActive) {
            return false;
        }
        float ggtm = Time::GGTM();
        state.impactTimer += ggtm > 1e-4f ? dt / ggtm : dt;
 
        float dip = slowMoTarget * (1.0f - impactExtra);
 
        if (state.impactTimer < impactInTime) {
            float t = Clamp01(impactInTime > 1e-4f ? state.impactTimer / impactInTime : 1.0f);
            Time::SGTM(slowMoTarget - (slowMoTarget - dip) * Ease(t));
            return true;
        }
 
        float t = Clamp01((state.impactTimer - impactInTime) / std::max(impactOutTime, 0.01f));
        Time::SGTM(dip + (slowMoTarget - dip) * Ease(t));
 
        if (t >= 1.0f) {
            state.impactActive = false;
        }
        return true;
    }

    bool IsPlayerAnimBusy() {
        auto player = RE::PlayerCharacter::GetSingleton();
        return player && AnimationVars::General::IsBusy(player);
    }

    bool DriveCameraWithCollision(const CameraSequenceState& state, const RE::NiPoint3& collisionRayStart) {
        auto camera = RE::PlayerCamera::GetSingleton();
        auto player = RE::PlayerCharacter::GetSingleton();
        if (!camera || !camera->cameraRoot || !player) {
            return false;
        }

        float scale = get_visual_scale(player);
        RE::NiPoint3 finalPos = state.cameraPos;

        if (collisionRayStart != RE::NiPoint3()) {
            finalPos = CameraCol::ComputeCameraCollision(player, collisionRayStart, finalPos, -1.0f, scale);
        }

        // UpdatePlayerCamera/UpdateNiCamera only ever touch translation, so
        // set the rotation directly first.
        camera->cameraRoot->local.rotate = state.cameraRot;
        camera->cameraRoot->world.rotate = state.cameraRot;

        SetCameraNearFarPlanes(scale);
        UpdatePlayerCamera(finalPos);
        UpdateNiCamera(finalPos);

        return true;
    }

    RE::NiPoint3 SmoothTowards(const RE::NiPoint3& current, const RE::NiPoint3& target, float dt, float halflife) {
        if (isInConfiguringMode()) {
            halflife *= configureModeTimeSlowdown;
        }
        float t = 1.0f - std::exp(-dt / std::max(halflife, 1e-3f));
        return Lerp(current, target, t);
    }

    RE::NiPoint3 ComputeImpactShakeOffset(const CameraSequenceState& state, float magnitude, float frequency, float scale, float decay) {
        RE::NiPoint3 right = { state.stageFromRot.entry[0][0], state.stageFromRot.entry[1][0], state.stageFromRot.entry[2][0] };
        RE::NiPoint3 up     = { state.stageFromRot.entry[0][2], state.stageFromRot.entry[1][2], state.stageFromRot.entry[2][2] };

        float wobble = std::sin(state.timer * frequency);
        float sway   = std::cos(state.timer * frequency * 0.7f);

        return (right * wobble + up * sway) * (magnitude * scale * decay);
    }

    void EaseCameraToStart(CameraSequenceState& state, float easedT) {
        state.cameraPos = Lerp(state.stageFromPos, state.startPos, easedT);
        state.cameraRot = SlerpMatrix(state.stageFromRot, state.startRot, easedT);
    }

    void TryKillMove(Actor* giant, const AimOutcome& aim, KillMoveParameters params) {
        if (!aim.victim) {
            logger::info("No victim");
            return;
        }
        std::vector<NiAVObject*> nodes;
        nodes.reserve(params.nodeLookups.size());

        for (auto name : params.nodeLookups) {
            if (auto node = find_node(giant, name)) {
                logger::info("Found node {}, pushing back", name);
                nodes.push_back(node);
            }
        }
        
        StartKillmove(
            giant, 
            aim.victim, 
            nodes, 
            params
        );
    }

    void RecordKillMoveCameraPositions() {
        RecordWrathfulCalamityStartingPosition();
		RecordCalamityStartingPosition();
		RecordBreastSuffocateStartingPosition();
		RecordBreastAbsorbStartingPosition();
		RecordStartingPosition();
    }

     void ResetKillMoveCameraTracking() {// At the moment camera snaps back at the end of size actions instead of returning to original pos smoothly
        // It happens because bone tracking adds offset to the camera, but im unsure how to fix it at the moment. 
        // This function doesn't help no matter where i put it.
        const auto PC = PlayerCharacter::GetSingleton();
        if (SizeManager::GetSingleton().GetTrackedBone(PC) != CameraTracking::None) {
		    SizeManager::GetSingleton().SetTrackedBone(PC, false, CameraTracking::None);
        }
    }
    
    bool UpdatingAnyKillMove() {
        bool KillMove = UpdateKillMove();
        bool Calamity = UpdateCalamityKillMove();
        bool WrathfulCalamity = UpdateWrathfulCalamityKillMove();
        bool BreastSuffocate = UpdateBreastSuffocateKillMove();
        bool BreastAbsorb = UpdateBreastAbsorbKillMove();
        bool InKillMove = KillMove || Calamity || WrathfulCalamity || BreastSuffocate || BreastAbsorb;
        return InKillMove;
    }
    bool IsInAnyGTSKillMove() {
        const bool BreastAbsorb     = BreastAbsorbKillMove::_state != BreastAbsorbKillMove::BreastAbsorbPOVState::None;
        const bool BreastSuffocate  = BreastSuffocateKillMove::_state != BreastSuffocateKillMove::BreastSuffocatePOVState::None;
		const bool WrathfulCalamity = WrathfulCalamity::_state != WrathfulCalamity::WrathfulPOVState::None;
		const bool Calamity 		= Calamity::_state != Calamity::TinyPOVState::None;		
		const bool SizeKillMove		= _state != SizeKillMoveState::None;

		const bool InKillMove		= Calamity || WrathfulCalamity || SizeKillMove || BreastAbsorb || BreastSuffocate;
		return InKillMove;
	}
    bool isInConfiguringMode() {
        return Config::KillMove.bConfigureMode;
    }
}