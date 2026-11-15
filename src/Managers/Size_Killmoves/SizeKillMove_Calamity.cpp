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

    RE::NiPoint3 VictimEyePos() {
        RE::NiPoint3 headPos = GTS::VictimHeadPos(_victim, _cam.cameraPos);
        RE::NiMatrix3 headRot = GTS::VictimHeadRot(_victim);
        RE::NiPoint3 forward = headRot * RE::NiPoint3(0.f, 1.f, 0.f);
        RE::NiPoint3 up(0.f, 0.f, 1.f);

        float scale = VictimScale(_victim);
        return headPos
            + forward * (_settings.EyeForwardOffset * scale)
            + up * (_settings.EyeHeightOffset * scale);
    }

    RE::NiPoint3 GiantNodeOrHeadPos() {
        return GTS::GiantNodeOrHeadPos(_giantNode, _giant, _cam.cameraPos, _settings.GiantNodeIsFoot);
    }

    void TransitionToLookAtGiantNode(float blendTime) {
        _cam.rotBlendFrom = _cam.cameraRot;
        BeginBlend(_cam, blendTime);
        EnterStage(_cam, _state, TinyPOVState::LookAtGiantNode);
    }

    void FinishKillmove() {
        _cam.cameraPos = _cam.startPos;
        _cam.cameraRot = _cam.startRot;

        _cam.impactActive = false;
        _cam.impactTimer = 0.0f;
        Time::SGTM(1.0f);

        _victim = nullptr;
        _giant = nullptr;
        _giantNode = nullptr;
        _cam.active = false;
        _state = TinyPOVState::None;
    }

    // ---------------------------------------------------------------------
    // stage updates
    // ---------------------------------------------------------------------

    void UpdateDiveToEye(float dt) {
        float t = AdvanceStageTimer(_cam, dt, _settings.DiveToEyeTime);
        float eased = Ease(t);

        RE::NiPoint3 eyeTarget = VictimEyePos();
        _cam.cameraPos = Lerp(_cam.stageFromPos, eyeTarget, eased);
        _cam.cameraRot = BuildLookAt(_cam.cameraPos, GTS::GiantHeadPos(_giant, _cam.cameraPos));

        Time::SGTM(1.0f - (1.0f - _settings.SlowMoTarget) * t);

        if (t >= 1.0f) {
            EnterStage(_cam, _state, TinyPOVState::LookUpAtGiant);
        }
    }

    void UpdateLookUpAtGiant(float dt) {
        float t = AdvanceStageTimer(_cam, dt, _settings.LookUpAtGiantTime);

        _cam.cameraPos = VictimEyePos(); 
        _cam.cameraRot = BuildLookAt(_cam.cameraPos, GTS::GiantHeadPos(_giant, _cam.cameraPos));

        RE::NiPoint3 node = GiantNodeOrHeadPos();
        float proximityRadius = _settings.GiantNodeProximityRadius * GiantScale(_giant);
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
        AdvanceStageTimer(_cam, dt, _settings.LookAtGiantNodeMaxWait);

        _cam.cameraPos = VictimEyePos(); 
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
            EnterStage(_cam, _state, TinyPOVState::ReturnCamera);
        }
    }

    void UpdateImpactShake(float dt) {
        float t = AdvanceStageTimer(_cam, dt, _settings.ImpactShakeTime);
        float decay = 1.0f - Ease(t);
        float scale = VictimScale(_victim);

        RE::NiPoint3 shakeOffset = GTS::ComputeImpactShakeOffset(_cam, _settings.ImpactShakeMagnitude, _settings.ImpactShakeFrequency, scale, decay);
        _cam.cameraPos = _cam.stageFromPos + shakeOffset;
        _cam.cameraRot = _cam.stageFromRot;

        if (t >= 1.0f) {
            EnterStage(_cam, _state, TinyPOVState::ReturnCamera);
        }
    }

    void UpdateReturnCamera(float dt) {
        float t = AdvanceStageTimer(_cam, dt, _settings.ReturnTime);
        float eased = Ease(t);
        GTS::EaseCameraToStart(_cam, eased);

        Time::SGTM(_settings.SlowMoTarget + (1.0f - _settings.SlowMoTarget) * t);

        if (t >= 1.0f) {
            FinishKillmove();
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
        if (!CanTriggerKillMove(PlayerCharacter::GetSingleton(), victim, Cause, base_damage, crush_mult)) {
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
    }
    void UpdateFakeCalamityKillmove() {
        if (!_cam.active) {
            return;
        }

        if (_state != TinyPOVState::None && _state != TinyPOVState::ReturnCamera && !IsPlayerAnimBusy()) {
            EnterStage(_cam, _state, TinyPOVState::ReturnCamera);
        }

        float dt = Time::WorldTimeDelta() * GetAnimationSlowdown(PlayerCharacter::GetSingleton());

        ApplyImpactSlowMo(_cam, dt, _settings.SlowMoTarget, _settings.ImpactSlowMoExtra, _settings.ImpactSlowMoInTime, _settings.ImpactSlowMoTime);

        switch (_state) {
            case TinyPOVState::DiveToEye:       UpdateDiveToEye(dt);       break;
            case TinyPOVState::LookUpAtGiant:   UpdateLookUpAtGiant(dt);   break;
            case TinyPOVState::LookAtGiantNode: UpdateLookAtGiantNode(dt); break;
            case TinyPOVState::ImpactShake:     UpdateImpactShake(dt);     break;
            case TinyPOVState::ReturnCamera:    UpdateReturnCamera(dt);    break;
            default: break;
        }
    }

    bool UpdateCalamityKillMove() {
        if (_state == TinyPOVState::None) {
            return false;
        }
        UpdateFakeCalamityKillmove();

        if (_state == TinyPOVState::None) {
            return false;
        }

        RE::NiPoint3 rayStart = GiantNodeOrHeadPos(_giantNode, _giant, _cam.cameraPos);
        if (!DriveCameraWithCollision(_cam, rayStart)) {
            return false;
        }
        return true;
    }

    void RecordCalamityStartingPosition() {
        RecordStartingPosition(_cam, _state, TinyPOVState::None);
    }

    bool OverrideHeadtracking_TinyCalamity(NiPoint3 &target) {
        if (!_cam.active) {
            return false;
        }
        target = _cam.cameraPos;
        return true;
    }
}