#include "Hooks/Animation/PreventAnimations.hpp"

#include "Config/Config.hpp"

#include "Managers/Animation/Grab.hpp"
#include "Hooks/Util/HookUtil.hpp"

using namespace GTS;

namespace {

	constexpr float KillMove_Threshold_High = 2.00f;       // If GTS/Tiny size ratio is > than 2 times = disallow killmove on Tiny
	constexpr float KillMove_Threshold_Low = 0.75f;        // If Tiny/GTS size ratio is < than 0.75 = disallow killmove on GTS

	// Actions that we want to prevent
	constexpr auto DefaultSheathe = 			0x46BB2;
	constexpr auto JumpRoot =					0x88302;
	constexpr auto NonMountedDraw = 			0x1000992;
	constexpr auto NonMountedForceEquip = 		0x1000993;
	constexpr auto JumpStandingStart =        	0x884A2;   // 558242
	constexpr auto JumpDirectionalStart =       0x884A3;   // 558243
	constexpr auto JumpFall =                   0xA791D;   // 686365 
	constexpr auto FallRoot =                   0xA790E;   // 686350 // The "falling down" anim

	// Killmoves that we want to prevent
	constexpr auto KillMoveFrontSideRoot =      0x24CD4;
	constexpr auto KillMoveDragonToNPC =        0xC1F20;
	constexpr auto KillMoveRootDragonFlight =   0xC9A1B;
	constexpr auto KillMoveBackSideRoot =       0xE8458;
	constexpr auto KillMoveFrontSideRoot00 =    0x100e8B;
	constexpr auto KillMoveBackSideRoot00 =     0x100F16;

	// Attacks that we want to prevent
	constexpr auto MountedDraw =                16779659;
	constexpr auto DrawMagic =                  561168;
	constexpr auto DefaultDrawWeapon =          561169;
	constexpr auto DrawSheathRoot =             78512;

	// Magic
	constexpr auto CastDualMagic =              220049;
	constexpr auto AttackMagicLeftRoot =        116570;
	constexpr auto AttackMagicRightRoot =       116321;

	// Left Hand
	constexpr auto BowLeftAttack =              619835;
	constexpr auto LeftHandAttack =             765123;
	constexpr auto AttackLeftRoot =             78358;
	constexpr auto LeftAttackRelease =          16779651;
	constexpr auto ReleaseLeftRoot =            78940;

	// Normal attack
	constexpr auto NormalAttack =               78357;

	// Right Hand
	constexpr auto AttackRightRoot =            78353;
	constexpr auto RightAttackRelease =         83292;
	constexpr auto ReleaseRightRoot =           78943;

	// Power attacks
	constexpr auto PowerAttack =                951382;
	constexpr auto PowerBash =                  951378;
	constexpr auto PowerAttackRoot =            78724;

	// Sprint
	constexpr auto SprintStart =                1069299;
	constexpr auto SprintRootStart =            0x03B4A8;
	constexpr auto SprintRootStop =             78362;

	// Turn
	constexpr auto NPCTurnToWalk =  	        0x02EDE6;
	constexpr auto NPCTurnInPlace = 	        0x02EDBC;
	constexpr auto NPCPathStartRoot = 	        0x02EDBA;

	// Blocking
	constexpr auto BlockHit =                   80631;
	constexpr auto BlockHitRoot =               80630;

	bool ShouldBlockFalling(Actor* actor) {
		bool block = false;
		auto charCont = actor->GetCharController();
		if (charCont) {
			float scale = get_visual_scale(actor);
			float falltime = charCont->fallTime;
			float threshold = 0.04f * scale;
			//log::info("Fall time of {} is {}", actor->GetDisplayFullName(), falltime);
			if (falltime < threshold) {
				//log::info("Blocking anim");
				block = true;
			}
		}
		return block;
	}

	bool PreventJumpFall(FormID idle, Actor* performer) {
		if (idle == FallRoot) {
			//log::info("Checking fall root");
			return ShouldBlockFalling(performer);
		}
		return false;
	}

	bool PreventKillMove(FormID idle, ConditionCheckParams* params, Actor* performer, TESObjectREFR* victim) {
		// KillMoves
		
		bool KillMove = false;
		bool Block = false;

		switch (idle) {
			case KillMoveFrontSideRoot:
			case KillMoveDragonToNPC:
			case KillMoveRootDragonFlight:
			case KillMoveBackSideRoot:
			case KillMoveFrontSideRoot00:
			case KillMoveBackSideRoot00:
			{
				KillMove = true;
			} break;
			default: break;
		}

		if (KillMove) {
			if (victim) {
				Actor* victimref = skyrim_cast<Actor*>(victim);
				if (victimref) {
					float size_difference = get_scale_difference(performer, victimref, SizeType::GiantessScale, true, false);
					if (size_difference > KillMove_Threshold_High || size_difference < KillMove_Threshold_Low) {
						Block = true;
					}
				}
			}
		}

		return Block;
	}

	bool PreventSprinting(FormID idle, Actor* performer) {
		if (idle == SprintRootStart) {
			if (IsTeammate(performer) && get_visual_scale(performer) > 2.0f) {
				return true;
			}
			return false;
		}
		return false;
	}

	bool IsDisallowed(FormID idle) {
		switch (idle) {
			case DefaultSheathe:
			case JumpRoot:
			case NonMountedDraw:
			case NonMountedForceEquip:
			case JumpStandingStart:
			case JumpDirectionalStart: 
			{
				return true;
			}
			default: break;
		}
		return false;
	}

	bool DisallowWeaponDrawWhenAIGrabbing(FormID idle, Actor* performer) {
		switch (idle) {
			case NonMountedDraw:
			case DrawMagic:
			case DefaultDrawWeapon: {
				if (!performer->IsPlayerRef() && Grab::GetHeldActor(performer) != nullptr) {
					return true;
				}
				break;
			}
			default: {}
		}
		return false;
	}

	bool PreventNPCSprinint(FormID idle, Actor* performer) {

		switch (idle) {
			case SprintStart:
			case SprintRootStart:
			case SprintRootStop:
			{
				//Fully prevent sprinting for npcs at the animation level if they are over the clamp start threshold
				if (!performer->IsPlayerRef()){
					if (get_visual_scale(performer) >= Config::General.fNPCMaxSpeedMultClampStartAt) {
						return true;
					}
				}

			} break;

			default: break;
		}
		return false;
	}

	bool BlockAnimation(TESIdleForm* idle, ConditionCheckParams* params) {
		if (!idle) {
			return false;
		}

		const FormID Form = idle->formID;
		Actor* performer = params->actionRef->As<RE::Actor>();

		if (performer) {

			if (PreventNPCSprinint(Form, performer)) {
				return true;
			}

			if (PreventKillMove(Form, params, performer, params->targetRef)) {
				return true;
			}

			if (AnimationVars::General::IsTransitioning(performer)) {
				return IsDisallowed(Form);
			}

			if (AnimationVars::Action::IsThighSandwiching(performer)) { // Disallow anims in these 2 cases 
				//log::info("Block IsThighSandwiching");
				return IsDisallowed(Form);
			}

			if (IsBetweenBreasts(performer)) {
				//log::info("Block IsBetweenBreasts");
				return IsDisallowed(Form);
			}
			
			if (PreventJumpFall(Form, performer)) {
				//log::info("Block PreventJumpFall");
				return true; // Disable fall down anim for GTS so it won't look off/annoying at large scales
			}

			if (DisallowWeaponDrawWhenAIGrabbing(Form, performer)) {
				return true;
			}

			if (!performer->IsPlayerRef() && PreventSprinting(Form, performer)) {
				return true;
			}

			if (performer->IsPlayerRef() && AnimationVars::General::IsGTSBusy(performer) && IsFreeCameraEnabled()) {
				return IsDisallowed(Form); 							// One of cases when we alter anims for Player. 
				// Needed because it's problematic to disallow specific controls through controls.hpp
			}

			if (!AnimationVars::General::IsGTSBusy(performer) && !AnimationVars::Prone::IsProne(performer)) {
                // Do not affect non-gts-busy actors!
				return false;
			}

			return IsDisallowed(Form);
			//log::info("Blocking anims for {}", performer->GetDisplayFullName());
		}

		return false;

	}

}

namespace Hooks {

	struct IdleForm {

		// 24067 = sub_140358150 (SE)
		// 24570 = FUN_14036ec80 (AE)

		// Return nullptr When we don't want specific anims to happen
		// This hook prevents them from playing (KillMoves and Sheathe/Unsheathe/Jump/Jump Root anims)

		static TESIdleForm* thunk(TESIdleForm* a_this, ConditionCheckParams* a_params, void* a_unk03) {

			TESIdleForm* result = func(a_this, a_params, a_unk03);

			{
				GTS_PROFILE_ENTRYPOINT("AnimationPrevent::IdleForm");

				if (a_this) {
					if (BlockAnimation(a_this, a_params)) {
						/*auto* EventName = a_this->GetFormEditorID();
						Actor* performer = params->actionRef->As<RE::Actor>();
						if (performer) {
							logger::info("Blocking anim: {} of {}", EventName, performer->GetDisplayFullName());
						}*/
						result = nullptr; // cancel anim
					}
				}
			}

			return result;
		}

		FUNCTYPE_DETOUR func;
	};

	void Hook_PreventAnimations::Install() {
		logger::info("Installing");
		stl::write_detour<IdleForm>(REL::RelocationID(24067, 24570, NULL));
	}
}